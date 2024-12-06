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

#include "CreateClassAgent.hpp"

#include "generator/MessageConstructionsGenerator.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <cctype>
#include<set>

using namespace interfaceModule;
using namespace scAgentsCommon;
using namespace dialogControlModule;
using namespace commonModule;


SC_AGENT_IMPLEMENTATION(CreateClassAgent)
{
  ScAddr const & questionNode = otherAddr;
  if (!checkActionClass(questionNode))
  {
    return SC_RESULT_OK;
  }
  SC_LOG_DEBUG("CreateClassAgent started");

  ScAddr const & messageAddr = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_1);
  if (!messageAddr.IsValid())
  {
     SC_LOG_ERROR("Message Addr not found.");
     utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
     SC_LOG_DEBUG("CreateClassAgent finished");
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

  ScAddr const & formLinkAddr = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_2);

  std::string formLinkContent;
  m_memoryCtx.GetLinkContent(formLinkAddr, formLinkContent);

  m_memoryCtx.EraseElement(formLinkAddr);
//user close
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

    SC_LOG_DEBUG("CreateClassAgent finished : user close");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }
//create result
//rrel nodes
  ScAddr const & _rrel_idtf = m_memoryCtx.HelperFindBySystemIdtf("rrel_system_idtf");
  ScAddr const & _rrel_ru = m_memoryCtx.HelperFindBySystemIdtf("rrel_ru");
  ScAddr const & _rrel_note = m_memoryCtx.HelperFindBySystemIdtf("rrel_note");
  ScAddr const & _rrel_super_class = m_memoryCtx.HelperFindBySystemIdtf("rrel_super_class");
  ScAddr const & _rrel_decomposition= m_memoryCtx.HelperFindBySystemIdtf("rrel_decomposition");

//find nodes
  ScAddr const & _system_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_idtf);
  ScAddr const & _system_ru_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_ru);
  ScAddr const & _note = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_note);
  ScAddr const & _super_class = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_super_class);

//проверка существующих нод на правильность заполнения
  bool s_class = true;
  if(m_memoryCtx.HelperGetSystemIdtf(_super_class) == "concept_"){
    s_class = false;
  }
  string name;
  name = m_memoryCtx.HelperGetSystemIdtf(_system_idtf);
  if(m_memoryCtx.HelperGetSystemIdtf(_system_idtf) == "concept_"){
    createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params_for_creating_class", answerStructure);
    
    SC_LOG_DEBUG("CreateClassAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }
  std::string ru;
  m_memoryCtx.GetLinkContent(_system_ru_idtf, ru);
  if(ru == "error_no_data"){
    createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params_for_creating_class", answerStructure);
    
    SC_LOG_DEBUG("CreateClassAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }
  
  string note;
  m_memoryCtx.GetLinkContent(_note, note);
  if(note== "error_no_data"){
    createAnswerMessageAndStructure("concept_phrase_about_error_invalid_params_for_creating_class", answerStructure);
    
    SC_LOG_DEBUG("CreateClassAgent finished with error: invalid inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  if(!containsOnlyEnglish(name)){
    createAnswerMessageAndStructure("concept_errors_in_names", answerStructure);
    
    SC_LOG_DEBUG("CreateClassAgent finished with error: nodes exists");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  ScAddr new_concept;
  if(m_memoryCtx.HelperFindBySystemIdtf(name, new_concept))
  { 
    if(m_memoryCtx.GetElementType(new_concept) != ScType::NodeVar){
      createAnswerMessageAndStructure("concept_phrase_about_error_of_existing_class", answerStructure);
    
      SC_LOG_DEBUG("CreateClassAgent finished with error: nodes exists");
      utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
      return SC_RESULT_OK;
    }
  }
//decomposition

  ScAddr const & rrel_decomposition = m_memoryCtx.HelperFindBySystemIdtf("rrel_decomposition");

  ScTemplate search_d;

  search_d.TripleWithRelation(
    questionNode,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar >> "d",
    ScType::EdgeAccessVarPosPerm,
    rrel_decomposition
  );
  
  ScTemplateSearchResult result_d;
  bool const is_success_d = m_memoryCtx.HelperSearchTemplate(search_d, result_d);
  set<string>decomposition;

  if (is_success_d)
  {
    for (size_t i = 0; i < result_d.Size(); ++i)
    {
      string decomp = m_memoryCtx.HelperGetSystemIdtf(result_d[i]["d"]);
      decomposition.insert(decomp);
    }
  }

//create class
  ScTemplate result_struct;


  ScAddr const & system_idtf = m_memoryCtx.CreateNode(ScType::NodeConstClass);
  m_memoryCtx.HelperSetSystemIdtf(name, system_idtf);

  ScAddr const& ru_idtf = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(ru_idtf, ru);

  ScAddr const& note_node = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(note_node, note);


  vector<ScAddr> decomposition_addr;
  for(string value : decomposition){
    ScAddr const& addr = m_memoryCtx.HelperFindBySystemIdtf(value);
    decomposition_addr.push_back(addr);
  }

  
  if(s_class){
    ScAddr super_class = m_memoryCtx.CreateNode(ScType::NodeConstClass);
    string name_super_class = m_memoryCtx.HelperGetSystemIdtf(_super_class);
    super_class = m_memoryCtx.HelperFindBySystemIdtf(name_super_class);

  result_struct.TripleWithRelation(
    super_class,
    ScType::EdgeDCommonVar,
    system_idtf,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_inclusion
  );
  }

  result_struct.TripleWithRelation(
    system_idtf,
    ScType::EdgeDCommonVar,
    ru_idtf,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_main_idtf
  );

  result_struct.Triple(
    InterfaceKeynodes::lang_ru, 
    ScType::EdgeAccessVarPosPerm,
    ru_idtf
  );

  result_struct.TripleWithRelation(
    system_idtf,
    ScType::EdgeDCommonVar,
    note_node,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_note
  );

  result_struct.Triple(
    InterfaceKeynodes::lang_ru, 
    ScType::EdgeAccessVarPosPerm,
    note_node
  );

  for(int i = 0; i < decomposition_addr.size(); i++){
    //= m_memoryCtx.CreateNode(ScType::NodeConstClass);
    ScAddr dec = m_memoryCtx.HelperFindBySystemIdtf(m_memoryCtx.HelperGetSystemIdtf(decomposition_addr[i]));
    //dec = m_memoryCtx.CreateNode(ScType::NodeConstClass);
    SC_LOG_DEBUG(m_memoryCtx.GetElementType(dec));
    result_struct.TripleWithRelation(
      system_idtf,
      ScType::EdgeDCommonVar,
      dec,
      ScType::EdgeAccessVarPosPerm,
      InterfaceKeynodes::nrel_subdividing
    );
  }
  ScTemplateGenResult genClassConstruction;
  m_memoryCtx.HelperGenTemplate(result_struct, genClassConstruction);

  createAnswerMessageAndStructure("concept_phrase_about_successful_creating_class", answerStructure);


  for (size_t i = 0; i < genClassConstruction.Size(); ++i)
  {
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStructure, genClassConstruction[i]);
  }

  SC_LOG_DEBUG("CreateClassAgent finished");
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
  return SC_RESULT_OK;

  
}

bool CreateClassAgent::containsOnlyEnglish(const string name){
    for (char ch : name) {
        if (!std::isalpha(ch) && ch != '_') {
            return false;
        }
    }
    return true; 
}

bool CreateClassAgent::checkActionClass(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(
      InterfaceKeynodes::action_create_class, actionAddr, ScType::EdgeAccessConstPosPerm);
}

void CreateClassAgent::createAnswer(std::string message)
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

void CreateClassAgent::createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
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