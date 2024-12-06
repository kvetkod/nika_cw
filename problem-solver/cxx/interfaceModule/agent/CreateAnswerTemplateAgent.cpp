#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"
#include "utils/ActionUtils.hpp"
#include "keynodes/InferenceKeynodes.hpp"

#include "keynodes/InterfaceKeynodes.hpp"
#include "keynodes/Keynodes.hpp"
#include "keynodes/MessageKeynodes.hpp"

#include "CreateAnswerTemplateAgent.hpp"

#include "generator/MessageConstructionsGenerator.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>


using namespace interfaceModule;
using namespace scAgentsCommon;
using namespace dialogControlModule;
using namespace commonModule;

SC_AGENT_IMPLEMENTATION(CreateAnswerTemplateAgent)
{
  ScAddr const & questionNode = otherAddr;
  if (!checkActionClass(questionNode))
  {
    return SC_RESULT_OK;
  }
  SC_LOG_DEBUG("CreateAnswerTemplateAgent started");

  ScAddr const & messageAddr = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_1);
  if (!messageAddr.IsValid())
  {
     SC_LOG_ERROR("Message Addr not found.");
     utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
     SC_LOG_DEBUG("CreateAnswerTemplateAgent finished");
     return SC_RESULT_ERROR;
  }

  //Clear answer structure
  ScAddr const & answerStructure = m_memoryCtx.HelperFindBySystemIdtf("answer_structure");
  
  ScTemplate findAnswerStructure;

  findAnswerStructure.Triple(
    answerStructure,
    ScType::EdgeAccessVarPosPerm >> "y",
    ScType::Unknown
  );

  ScTemplateSearchResult resultAnswerStructure;
  bool const is_success = m_memoryCtx.HelperSearchTemplate(findAnswerStructure, resultAnswerStructure);
  
 if (is_success)
  {
    for (size_t i = 0; i < resultAnswerStructure.Size(); ++i)
    {
      m_memoryCtx.EraseElement(resultAnswerStructure[i]["y"]);
    }
  }
  //------------------------------


  ScAddr const & formLinkAddr = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_2);

  std::string formLinkContent;
  m_memoryCtx.GetLinkContent(formLinkAddr, formLinkContent);

  m_memoryCtx.EraseElement(formLinkAddr);

  if (formLinkContent == "User close")
  {
    ScAddr const & phrase = m_memoryCtx.HelperFindBySystemIdtf("concept_phrase_about_user_close");
    ScTemplate searchPhraseConstruction;
    searchPhraseConstruction.TripleWithRelation(
      InterfaceKeynodes::concept_phrase,
      ScType::EdgeDCommonVar,
      phrase,
      ScType::EdgeAccessVarPosPerm,
      InterfaceKeynodes::nrel_inclusion
    );
    searchPhraseConstruction.Triple(
      phrase,
      ScType::EdgeAccessVarPosPerm,
      ScType::LinkVar >> "_link"
    );
    searchPhraseConstruction.Triple(
      InterfaceKeynodes::lang_ru,
      ScType::EdgeAccessVarPosPerm,
      "_link"
    );

    ScTemplateSearchResult phraseConstruction;
    m_memoryCtx.HelperSearchTemplate(searchPhraseConstruction, phraseConstruction);

    std::string answer;
    m_memoryCtx.GetLinkContent(phraseConstruction[0]["_link"], answer);
    createAnswer(answer);

    for (size_t i = 0; i < phraseConstruction[0].Size(); ++i)
    {
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStructure, phraseConstruction[0][i]);
    }

    SC_LOG_DEBUG("CreateAnswerTemplateAgent finished : user close");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

//create nodes

  ScAddr const& rrel_phrase = m_memoryCtx.HelperFindBySystemIdtf("rrel_phrase");
  ScAddr const& rrel_phrase_ru = m_memoryCtx.HelperFindBySystemIdtf("rrel_phrase_ru");
  ScAddr const& rrel_answer = m_memoryCtx.HelperFindBySystemIdtf("rrel_answer");
  ScAddr const& rrel_answer_ru = m_memoryCtx.HelperFindBySystemIdtf("rrel_answer_ru");
  ScAddr const& rrel_wit = m_memoryCtx.HelperFindBySystemIdtf("rrel_wit");

  ScAddr const& _phrase = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, rrel_phrase);
  ScAddr const& _phrase_ru = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, rrel_phrase_ru);
  ScAddr const& _answer = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, rrel_answer);
  ScAddr const& _answer_ru = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, rrel_answer_ru);
  ScAddr const& _wit = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, rrel_wit);

  string phrase_name = m_memoryCtx.HelperGetSystemIdtf(_phrase);
  string phrase_ru;
  m_memoryCtx.GetLinkContent(_phrase_ru, phrase_ru);
  string answer_name = m_memoryCtx.HelperGetSystemIdtf(_answer);
  string answer_ru;
  m_memoryCtx.GetLinkContent(_answer_ru, answer_ru);
  string wit;
  m_memoryCtx.GetLinkContent(_wit, wit);


  if(wit == "error_no_data") {
    createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
    
    SC_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  if(phrase_name == "concept_message_about_"){
     createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
    
    SC_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  if(answer_name == "concept_phrase_about_"){
     createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
    
    SC_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  if(!containsOnlyEnglish(phrase_name)){
    createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
    
    SC_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  if(!containsOnlyEnglish(answer_name)){
    createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
    
    SC_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  if(!containsOnlyEnglish(wit)){
    createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
    
    SC_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  ScAddr const& rrel_answers = m_memoryCtx.HelperFindBySystemIdtf("rrel_answers");
  ScTemplate search_answers;

  search_answers.TripleWithRelation(
    questionNode, 
    ScType::EdgeAccessVarPosPerm,
    ScType::Unknown >> "a",
    ScType::EdgeAccessVarPosPerm,
    rrel_answers
  );

  ScTemplateSearchResult results;
  bool const success = m_memoryCtx.HelperSearchTemplate(search_answers, results);
  set<string> answers;
  if(success){
    for(int i = 0; i < results.Size(); i++){
        string ans;
        m_memoryCtx.GetLinkContent(results[i]["a"], ans);
        answers.insert(ans);
    }
  } 

  if(answers.size() == 1){
    for(string value : answers){
      if(value == "none"){
        createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params", answerStructure);
        
        SC_LOG_DEBUG("CreateAnswerTemplateAgent finished with error: invalid inputs");
        utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
        return SC_RESULT_OK;
      }
    }
  }

  ScAddr new_concept;
  if(m_memoryCtx.HelperFindBySystemIdtf(phrase_name, new_concept))
  { 
    if(m_memoryCtx.GetElementType(new_concept) != ScType::NodeVar){
      createAnswerMessageAndStructure("concept_phrase_about_error_of_existing_class", answerStructure);
    
      SC_LOG_DEBUG("CreateClassAgent finished with error: nodes exists");
      utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
      return SC_RESULT_OK;
    }
  }
   ScAddr new_concept1;
  if(m_memoryCtx.HelperFindBySystemIdtf(answer_name, new_concept1))
  { 
    if(m_memoryCtx.GetElementType(new_concept1) != ScType::NodeVar){
      createAnswerMessageAndStructure("concept_phrase_about_error_of_existing_class", answerStructure);
    
      SC_LOG_DEBUG("CreateClassAgent finished with error: nodes exists");
      utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
      return SC_RESULT_OK;
    }
  }
  ScAddr new_concept2;
  if(m_memoryCtx.HelperFindBySystemIdtf(wit, new_concept2))
  { 
    if(m_memoryCtx.GetElementType(new_concept1) != ScType::NodeVar){
      createAnswerMessageAndStructure("concept_phrase_about_error_of_existing_class", answerStructure);
    
      SC_LOG_DEBUG("CreateClassAgent finished with error: nodes exists");
      utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
      return SC_RESULT_OK;
    }
  }


//create result

  ScAddr const& classNode = m_memoryCtx.CreateNode(ScType::NodeConstClass);
  m_memoryCtx.HelperSetSystemIdtf(phrase_name, classNode);

  ScAddr const& classMainIdtf = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(classMainIdtf, phrase_ru);

  ScAddr const& witIntent = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(witIntent, wit);

  ScTemplate messageClassConstruction;

  messageClassConstruction.Triple(
    InterfaceKeynodes::concept_intent_possible_class,
    ScType::EdgeAccessVarPosPerm,
    classNode
  );

  messageClassConstruction.TripleWithRelation(
    classNode,
    ScType::EdgeDCommonVar,
    classMainIdtf,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_main_idtf
  );

  messageClassConstruction.Triple(
    InterfaceKeynodes::lang_ru,
    ScType::EdgeAccessVarPosPerm,
    classMainIdtf
  );

  messageClassConstruction.TripleWithRelation(
    classNode,
    ScType::EdgeDCommonVar,
    witIntent,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_wit_ai_idtf
  );

  messageClassConstruction.Triple(
    InterfaceKeynodes::lang_ru,
    ScType::EdgeAccessVarPosPerm,
    witIntent
  );

  ScTemplateGenResult classConstruction;
  m_memoryCtx.HelperGenTemplate(messageClassConstruction, classConstruction);

  //create phrases
  ScAddr const& phraseTemplateNode = m_memoryCtx.CreateNode(ScType::NodeConstClass);
  m_memoryCtx.HelperSetSystemIdtf(answer_name, phraseTemplateNode);

  ScAddr const& phraseTemplateMainIdtf = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(phraseTemplateMainIdtf, answer_ru);

  ScTemplate phraseTemplate;

  phraseTemplate.TripleWithRelation(
    InterfaceKeynodes::concept_phrase,
    ScType::EdgeDCommonVar,
    phraseTemplateNode,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_inclusion
  );

  phraseTemplate.TripleWithRelation(
    phraseTemplateNode,
    ScType::EdgeDCommonVar,
    phraseTemplateMainIdtf,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_main_idtf
  );

  phraseTemplate.Triple(
    InterfaceKeynodes::lang_ru,
    ScType::EdgeAccessVarPosPerm,
    phraseTemplateMainIdtf
  );

  for(string phrase : answers){
    ScAddr const& phraseLink = m_memoryCtx.CreateLink(ScType::LinkConst);
    m_memoryCtx.SetLinkContent(phraseLink, phrase);

    phraseTemplate.Triple(
      phraseTemplateNode,
      ScType::EdgeAccessVarPosPerm,
      phraseLink
    );

    phraseTemplate.Triple(
      InterfaceKeynodes::lang_ru,
      ScType::EdgeAccessVarPosPerm,
      phraseLink
    );
  }

  ScTemplateGenResult genPhrase;
  m_memoryCtx.HelperGenTemplate(phraseTemplate, genPhrase);

  //create logic rule

  //first

  ScTemplate logicRule;
  ScAddr const & edgeFromConceptMessageToMessageVar = m_memoryCtx.CreateEdge(ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::concept_message, InterfaceKeynodes::_message);

  ScAddr const & edgeFromConceptMessageClassToMessageVar = m_memoryCtx.CreateEdge(ScType::EdgeAccessVarPosPerm,
    classNode, InterfaceKeynodes::_message);

  ScAddr const & structUp = m_memoryCtx.CreateNode(ScType::NodeConstStruct);

  logicRule.Triple(
    structUp,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::concept_message
  );

  logicRule.Triple(
    structUp,
    ScType::EdgeAccessVarPosPerm,
    edgeFromConceptMessageToMessageVar
  );

  logicRule.Triple(
    structUp,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::_message
  );

  logicRule.Triple(
    structUp,
    ScType::EdgeAccessVarPosPerm,
    classNode
  );

  logicRule.Triple(
    structUp,
    ScType::EdgeAccessVarPosPerm,
    edgeFromConceptMessageClassToMessageVar
  );

  //second
  ScAddr const & edgeFromConceptMessageClassToReplyMessage = m_memoryCtx.CreateEdge(ScType::EdgeAccessVarPosPerm,
    classNode, InterfaceKeynodes::_reply_message);

  ScAddr const & edgeFromConceptAtomicMessageToReplyMessage = m_memoryCtx.CreateEdge(ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::concept_atomic_message, InterfaceKeynodes::_reply_message);

  ScAddr const & edgeFromMessageToReplyMessage = m_memoryCtx.CreateEdge(ScType::EdgeDCommonVar,
    InterfaceKeynodes::_message, InterfaceKeynodes::_reply_message);

  ScAddr const & edgeFromNrelReplyToDCommonEdge = m_memoryCtx.CreateEdge(ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_reply, edgeFromMessageToReplyMessage);

  ScAddr const & structDown = m_memoryCtx.CreateNode(ScType::NodeConstStruct);

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    classNode
  );

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    edgeFromConceptMessageClassToReplyMessage
  );

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::_reply_message
  );

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::concept_atomic_message
  );

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    edgeFromConceptAtomicMessageToReplyMessage
  );

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::_message
  );

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_reply
  );

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    edgeFromMessageToReplyMessage
  );

  logicRule.Triple(
    structDown,
    ScType::EdgeAccessVarPosPerm,
    edgeFromNrelReplyToDCommonEdge
  );

  //create rule
  logicRule.Triple(
    InterfaceKeynodes::atomic_logical_formula,
    ScType::EdgeAccessVarPosPerm,
    structUp
  );

  logicRule.Triple(
    InterfaceKeynodes::atomic_logical_formula,
    ScType::EdgeAccessVarPosPerm,
    structDown
  );

  logicRule.TripleWithRelation(
    structUp,
    ScType::EdgeDCommonVar >> "key_sc_element",
    structDown,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_implication
  );

  ScAddr const & ruleNode = m_memoryCtx.CreateNode(ScType::NodeConst);

  std::string replace = "concept";
  size_t pos = phrase_name.find(replace);
    if (pos != std::string::npos) {
        phrase_name.replace(pos, replace.length(), "lr");
  }

  m_memoryCtx.HelperSetSystemIdtf(phrase_name, ruleNode);

  logicRule.TripleWithRelation(
    ruleNode,
    ScType::EdgeAccessVarPosPerm,
    "key_sc_element",
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::rrel_main_key_sc_element
  );

  logicRule.Triple(
    InterfaceKeynodes::concept_answer_on_standard_message_rule_priority_1,
    ScType::EdgeAccessVarPosPerm,
    ruleNode
  );

  logicRule.TripleWithRelation(
    ruleNode,
    ScType::EdgeDCommonVar,
    ScType::NodeVarTuple >> "tuple",
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_answer_pattern
  );

  logicRule.TripleWithRelation(
    "tuple",
    ScType::EdgeAccessVarPosPerm,
    phraseTemplateNode,
    ScType::EdgeAccessVarPosPerm,
    scAgentsCommon::CoreKeynodes::rrel_1
  );

  ScTemplateGenResult full_construction;
  m_memoryCtx.HelperGenTemplate(logicRule, full_construction);


  createAnswerMessageAndStructure("concept_phrase_about_successful_creating", answerStructure);

  for (size_t i = 0; i < genPhrase.Size(); ++i)
  {
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStructure, genPhrase[i]);
  }

  for (size_t i = 0; i < classConstruction.Size(); ++i)
  {
    if (std::find(genPhrase.begin(), genPhrase.end(), classConstruction[i]) == genPhrase.end())
    {
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStructure, classConstruction[i]);
    }
  }

  SC_LOG_DEBUG("CreateAnswerTemplateAgent finished");
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
  return SC_RESULT_OK;
}




std::vector<std::string> CreateAnswerTemplateAgent::split(const string & s, const string & delimiter)
{
  std::vector<std::string> tokens;
    std::size_t start = 0;
    std::size_t end = s.find(delimiter);
    
    while (end != std::string::npos)
    {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }
    
    tokens.push_back(s.substr(start));
    
    return tokens;
}

bool CreateAnswerTemplateAgent::checkActionClass(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(
      InterfaceKeynodes::action_create_message_class_and_phrase_template, actionAddr, ScType::EdgeAccessConstPosPerm);
}

void CreateAnswerTemplateAgent::createAnswer(std::string message)
{
  dialogControlModule::MessageConstructionsGenerator messageConstructionGenerator = MessageConstructionsGenerator(&m_memoryCtx);
  ScAddr const & author = m_memoryCtx.HelperFindBySystemIdtf("myself");
  ScAddr const & dialogue = m_memoryCtx.HelperFindBySystemIdtf("concept_dialogue");
  ScTemplate findTarg;
  findTarg.Triple(
    dialogue,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar >> "target"
  );
  ScTemplateSearchResult result;
  bool const isFoundByTemplate = m_memoryCtx.HelperSearchTemplate(findTarg, result);
  ScAddr const & targAddr = result[0]["target"];
  
  ScAddr const &replyAddr = m_memoryCtx.CreateNode(ScType::NodeConst);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, MessageKeynodes::concept_message, replyAddr);
  messageConstructionGenerator.generateTextTranslationConstruction(replyAddr, InterfaceKeynodes::lang_ru, message);
  utils::GenerationUtils::generateRelationBetween(&m_memoryCtx, replyAddr, author, InterfaceKeynodes::nrel_authors);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, targAddr, replyAddr);
}

void CreateAnswerTemplateAgent::createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
{
  ScAddr const & phrase = m_memoryCtx.HelperFindBySystemIdtf(conceptName);
  ScTemplate searchPhraseConstruction;
  searchPhraseConstruction.TripleWithRelation(
    InterfaceKeynodes::concept_phrase,
    ScType::EdgeDCommonVar,
    phrase,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_inclusion
  );
  searchPhraseConstruction.Triple(
    phrase,
    ScType::EdgeAccessVarPosPerm,
    ScType::LinkVar >> "_link"
  );
  searchPhraseConstruction.Triple(
    InterfaceKeynodes::lang_ru,
    ScType::EdgeAccessVarPosPerm,
    "_link"
  );

  ScTemplateSearchResult phraseConstruction;
  m_memoryCtx.HelperSearchTemplate(searchPhraseConstruction, phraseConstruction);
  std::string answer;
  m_memoryCtx.GetLinkContent(phraseConstruction[0]["_link"], answer);
  createAnswer(answer);

  for (size_t i = 0; i < phraseConstruction[0].Size(); ++i)
  {
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStructure, phraseConstruction[0][i]);
  }
}

bool CreateAnswerTemplateAgent::containsOnlyEnglish(const string name){
    for (char ch : name) {
        if (!std::isalpha(ch) && ch != '_') {
            return false;
        }
    }
    return true; 
}