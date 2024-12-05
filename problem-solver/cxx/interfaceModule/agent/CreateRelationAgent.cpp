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

#include "CreateRelationAgent.hpp"

#include "generator/MessageConstructionsGenerator.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>


using namespace interfaceModule;
using namespace scAgentsCommon;
using namespace dialogControlModule;
using namespace commonModule;

SC_AGENT_IMPLEMENTATION(CreateRelationAgent)
{

  ScAddr const & questionNode = otherAddr;
  if (!checkActionClass(questionNode))
  {
    return SC_RESULT_OK;
  }
  SC_LOG_DEBUG("CreateRelationAgent started");

  ScAddr const & messageAddr = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_1);
  if (!messageAddr.IsValid())
  {
     SC_LOG_ERROR("Message Addr not found.");
     utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
     SC_LOG_DEBUG("CreateRelationAgent finished ");
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

    SC_LOG_DEBUG("CreateRelationAgent finished : user close");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  

  //rrel nodes

  ScAddr const & _rrel_system_idtf = m_memoryCtx.HelperFindBySystemIdtf("rrel_system_idtf");
  ScAddr const & _rrel_ru = m_memoryCtx.HelperFindBySystemIdtf("rrel_ru");
  ScAddr const & _rrel_note = m_memoryCtx.HelperFindBySystemIdtf("rrel_note");
  ScAddr const & _rrel_first_domain = m_memoryCtx.HelperFindBySystemIdtf("rrel_first_domain");
  ScAddr const & _rrel_second_domain= m_memoryCtx.HelperFindBySystemIdtf("rrel_second_domain");
  
  

  ScAddr const & _system_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_system_idtf);
  ScAddr const & _system_ru_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_ru);
  ScAddr const & _note = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_note);
  ScAddr const & _first_domain = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_first_domain);
  ScAddr const & _second_domain = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_second_domain);

  std::string idtf, ru_idtf, note, first_domain, second_domain;

  idtf = m_memoryCtx.HelperGetSystemIdtf(_system_idtf);
  m_memoryCtx.GetLinkContent(_system_ru_idtf, ru_idtf);
  m_memoryCtx.GetLinkContent(_note, note);
  first_domain = m_memoryCtx.HelperGetSystemIdtf(_first_domain);
  second_domain = m_memoryCtx.HelperGetSystemIdtf(_second_domain);


  SC_LOG_DEBUG(" Create relation data:");

  SC_LOG_DEBUG("\t system idtf : " + idtf);
  SC_LOG_DEBUG("\t ru idtf : " + ru_idtf);
  SC_LOG_DEBUG("\t note : " + note);
  SC_LOG_DEBUG("\t first_domain : " + first_domain);
  SC_LOG_DEBUG("\t second_domain : " + second_domain);
  SC_LOG_DEBUG("\t relation classification : ");



  // if idtf contains non-english symbols
  if(!containsOnlyEnglish(idtf)){
    createAnswerMessageAndStructure("concept_message_about_error_in_system_idtf_non_latin_symbol", answerStructure);
    SC_LOG_DEBUG("CreateRelationAgent finished with error: invalid idtf input");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }
  SC_LOG_DEBUG(1);

  // if idtf already exists
  ScAddr new_concept;
  if(m_memoryCtx.HelperFindBySystemIdtf(idtf, new_concept)){
		if((m_memoryCtx.GetElementType(new_concept) != ScType::NodeVar))
  		{ 
			createAnswerMessageAndStructure("concept_message_about_error_system_idtf_already_exists", answerStructure);
			SC_LOG_DEBUG("CreateRelationAgent finished with error: idtf node already exists");
			utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
			return SC_RESULT_OK;
  		}
	}
SC_LOG_DEBUG(2);
  // both first and second domain should already exist
   if(m_memoryCtx.HelperFindBySystemIdtf(first_domain, new_concept)){
		if(m_memoryCtx.GetElementType(new_concept) == ScType::NodeVar)
  		{ 
			createAnswerMessageAndStructure("concept_message_about_error_first_domain_does_not_exist", answerStructure);
			SC_LOG_DEBUG("CreateRelationAgent finished with error: first domain does not exist");
			utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
			return SC_RESULT_OK;
  		}
	}

SC_LOG_DEBUG(3);
   if(m_memoryCtx.HelperFindBySystemIdtf(second_domain, new_concept)){
		if(m_memoryCtx.GetElementType(new_concept) == ScType::NodeVar)
  		{ 
			createAnswerMessageAndStructure("concept_message_about_error_second_domain_does_not_exist", answerStructure);
			SC_LOG_DEBUG("CreateRelationAgent finished with error: second domain does not exist");
			utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
			return SC_RESULT_OK;
  		}
	}

SC_LOG_DEBUG(4);

  //relation classification list 
	ScAddr const & _rrel_include= m_memoryCtx.HelperFindBySystemIdtf("rrel_include");

	ScTemplate search_include;
	
	search_include.TripleWithRelation(
    questionNode,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar >> "x",
    ScType::EdgeAccessVarPosPerm,
    _rrel_include
  );

	ScTemplateSearchResult result_include;
  bool _is_success = m_memoryCtx.HelperSearchTemplate(search_include, result_include);
	std::vector<std::string>classification;
	
	if(_is_success)
	{
		for (size_t i = 0; i < result_include.Size(); ++i)
		{
			std::string clas = m_memoryCtx.HelperGetSystemIdtf(result_include[i]["x"]);
			classification.push_back(clas);
      SC_LOG_DEBUG("\t\t"+i+clas);
		}
	}





  SC_LOG_DEBUG("All checks passed");


	//class creating



 ScTemplate result_struct;

  ScAddr const & system_idtf = m_memoryCtx.CreateNode(ScType::NodeConstNoRole);
  m_memoryCtx.HelperSetSystemIdtf(idtf, system_idtf);

  ScAddr const& ru_idtf_ = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(ru_idtf_, ru_idtf);

  ScAddr const& note_node = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(note_node, note);


  ScAddr const& first_domain_ =  m_memoryCtx.HelperFindBySystemIdtf(first_domain);
  ScAddr const& second_domain_ =  m_memoryCtx.HelperFindBySystemIdtf(second_domain);



  vector<ScAddr> classification_addr;
  for(string value : classification){
    ScAddr const& addr = m_memoryCtx.HelperFindBySystemIdtf(value);
    classification_addr.push_back(addr);
  }

  result_struct.TripleWithRelation(
    system_idtf,
    ScType::EdgeDCommonVar,
    ru_idtf_,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_main_idtf
  );

  result_struct.Triple(
    InterfaceKeynodes::lang_ru, 
    ScType::EdgeAccessVarPosPerm,
    ru_idtf_
  );

  result_struct.TripleWithRelation(
    system_idtf,
    ScType::EdgeDCommonVar,
    note_node,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_note
  );

   result_struct.TripleWithRelation(
    system_idtf,
    ScType::EdgeDCommonVar,
    first_domain_,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_first_domain
  );

   result_struct.TripleWithRelation(
    system_idtf,
    ScType::EdgeDCommonVar,
    second_domain_,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_second_domain
  );

  
  for(const auto& element : classification_addr){
    ScAddr dec = m_memoryCtx.HelperFindBySystemIdtf(m_memoryCtx.HelperGetSystemIdtf(element));
    result_struct.Triple(
    dec, 
    ScType::EdgeAccessVarPosPerm,
    system_idtf
  );
  }




  ScTemplateGenResult genRelationConstruction;
  m_memoryCtx.HelperGenTemplate(result_struct, genRelationConstruction);


  createAnswerMessageAndStructure("concept_phrase_about_successful_creating_relation", answerStructure);

  for (size_t i = 0; i < genRelationConstruction.Size(); ++i)
  {
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStructure, genRelationConstruction[i]);
  }


  SC_LOG_DEBUG("CreateRelationAgent finished ");
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
  return SC_RESULT_OK;









/*
  std::vector<std::string> formItems = split(formLinkContent, "\n");
  
  ScTemplate relationConstruction;

  ScAddr const & relationNode = m_memoryCtx.CreateNode(ScType::NodeConstNoRole);
  m_memoryCtx.HelperSetSystemIdtf(formItems[0], relationNode);

  ScAddr const & mainIdtf = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(mainIdtf, formItems[1]);

  ScAddr const & definitionLink = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(definitionLink, formItems[2]);

  ScAddr const & firstDomainNode = m_memoryCtx.HelperFindBySystemIdtf(formItems[3]);

  ScAddr const & secondDomainNode = m_memoryCtx.HelperFindBySystemIdtf(formItems[4]);

  relationConstruction.Triple(
    InterfaceKeynodes::sc_node_norole_relation,
    ScType::EdgeAccessVarPosPerm,
    relationNode
  );

  relationConstruction.Triple(
    InterfaceKeynodes::relation,
    ScType::EdgeAccessVarPosPerm,
    relationNode
  );

  relationConstruction.TripleWithRelation(
    relationNode,
    ScType::EdgeDCommonVar,
    mainIdtf,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_main_idtf
  );

  relationConstruction.Triple(
    InterfaceKeynodes::lang_ru,
    ScType::EdgeAccessVarPosPerm,
    mainIdtf
  );

  relationConstruction.TripleWithRelation(
    relationNode,
    ScType::EdgeDCommonVar,
    definitionLink,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_definition
  );

  relationConstruction.Triple(
    InterfaceKeynodes::lang_ru,
    ScType::EdgeAccessVarPosPerm,
    definitionLink
  );

  relationConstruction.TripleWithRelation(
    relationNode,
    ScType::EdgeDCommonVar,
    firstDomainNode,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_first_domain
  );

  relationConstruction.TripleWithRelation(
    relationNode,
    ScType::EdgeDCommonVar,
    secondDomainNode,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_second_domain
  );

  relationConstruction.Triple(
    ScType::NodeVarTuple >> "_tuple",
    ScType::EdgeAccessVarPosPerm,
    firstDomainNode
  );

  relationConstruction.Triple(
    "_tuple",
    ScType::EdgeAccessVarPosPerm,
    secondDomainNode
  );

  relationConstruction.TripleWithRelation(
    relationNode,
    ScType::EdgeDCommonVar,
    ScType::NodeVar >> "_node",
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_definitional_domain
  );

  relationConstruction.TripleWithRelation(
    "_tuple",
    ScType::EdgeDCommonVar,
    "_node",
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_combination
  );

  std::vector<std::string> classification = split(formItems[5], ", ");

  for(size_t i = 0; i < classification.size(); ++i)
  {
    ScAddr const & classificationConceptNode = m_memoryCtx.HelperFindBySystemIdtf(classification[i]);
    relationConstruction.Triple(
      classificationConceptNode,
      ScType::EdgeAccessVarPosPerm,
      relationNode
    );
  }

  ScTemplateGenResult full_construction;
  m_memoryCtx.HelperGenTemplate(relationConstruction, full_construction);

  

  for (size_t i = 0; i < full_construction.Size(); ++i)
  {
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStructure, full_construction[i]);
  }
*/
}



bool CreateRelationAgent::checkActionClass(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(
      InterfaceKeynodes::action_create_relation, actionAddr, ScType::EdgeAccessConstPosPerm);
}

void CreateRelationAgent::createAnswer(std::string message)
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
  m_memoryCtx.HelperSearchTemplate(findTarg, result);
  ScAddr const & targAddr = result[0]["target"];
  
  ScAddr const &replyAddr = m_memoryCtx.CreateNode(ScType::NodeConst);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, MessageKeynodes::concept_message, replyAddr);
  messageConstructionGenerator.generateTextTranslationConstruction(replyAddr, InterfaceKeynodes::lang_ru, message);
  utils::GenerationUtils::generateRelationBetween(&m_memoryCtx, replyAddr, author, InterfaceKeynodes::nrel_authors);
  m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, targAddr, replyAddr);
}

void CreateRelationAgent::createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
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


bool CreateRelationAgent::containsOnlyEnglish(const std::string& str){
    for (char ch : str)
        if (!std::isalpha(ch) && ch != '_') 
            return false;
    return true; 
}

