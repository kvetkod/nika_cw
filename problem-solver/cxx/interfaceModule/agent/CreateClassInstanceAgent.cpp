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

#include "CreateClassInstanceAgent.hpp"

#include "generator/MessageConstructionsGenerator.hpp"

#include <vector>
#include <utility>
#include <string>
#include <unordered_map>
#include <algorithm>


using namespace interfaceModule;
using namespace scAgentsCommon;
using namespace dialogControlModule;
using namespace commonModule;

SC_AGENT_IMPLEMENTATION(CreateClassInstanceAgent)
{
  ScAddr const & questionNode = otherAddr;
  if (!checkActionClass(questionNode))
  {
    return SC_RESULT_OK;
  }
  SC_LOG_DEBUG("CreateClassInstanceAgent started");

  ScAddr const & messageAddr = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, scAgentsCommon::CoreKeynodes::rrel_1);
  if (!messageAddr.IsValid())
  {
     SC_LOG_ERROR("Message Addr not found.");
     utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, false);
     SC_LOG_DEBUG("CreateClassInstanceAgent finished");
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
    //SC_LOG_ERROR("size of answer structure");
    SC_LOG_ERROR(resultAnswerStructure.Size());
    for (size_t i = 0; i < resultAnswerStructure.Size(); ++i)
    {
      //SC_LOG_ERROR("For step");
      //SC_LOG_ERROR(i);
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

    SC_LOG_DEBUG("CreateClassInstanceAgent finished : user close");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }

  


  ScAddr const & _rrel_system_idtf = m_memoryCtx.HelperFindBySystemIdtf("rrel_system_idtf");
  ScAddr const & _rrel_ru = m_memoryCtx.HelperFindBySystemIdtf("rrel_ru");
  ScAddr const & _rrel_note = m_memoryCtx.HelperFindBySystemIdtf("rrel_note");
  ScAddr const & _rrel_clas = m_memoryCtx.HelperFindBySystemIdtf("rrel_class");
  


  ScAddr const & _system_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_system_idtf);
  ScAddr const & _system_ru_idtf = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_ru);
  ScAddr const & _note = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_note);
  ScAddr const & _clas = utils::IteratorUtils::getAnyByOutRelation(&m_memoryCtx, questionNode, _rrel_clas);


  string idtf, ru_idtf, note, clas;


  idtf = m_memoryCtx.HelperGetSystemIdtf(_system_idtf);
  m_memoryCtx.GetLinkContent(_system_ru_idtf, ru_idtf);
  m_memoryCtx.GetLinkContent(_note, note);
  clas = m_memoryCtx.HelperGetSystemIdtf(_clas);



  SC_LOG_DEBUG(" Create class example data:");

  SC_LOG_DEBUG("\t system idtf : " + idtf);
  SC_LOG_DEBUG("\t ru idtf : " + ru_idtf);
  SC_LOG_DEBUG("\t note : " + note);
  SC_LOG_DEBUG("\t class : " + clas);
  SC_LOG_DEBUG("\t relations data : ");



  //relation list 
	ScAddr const & _rrel_relation= m_memoryCtx.HelperFindBySystemIdtf("rrel_relation");

	ScTemplate search_relation;
	
	search_relation.TripleWithRelation(
    questionNode,
    ScType::EdgeAccessVarPosPerm,
    ScType::NodeVar >> "x",
    ScType::EdgeAccessVarPosPerm,
    _rrel_relation
  );



	ScTemplateSearchResult result_relation;
  bool _is_success = m_memoryCtx.HelperSearchTemplate(search_relation, result_relation);
	
  vector<pair<string, string>> relation_list;

	if(_is_success)
	{
		for (size_t i = 0; i < result_relation.Size(); ++i)
		{
      ScAddr const & _rrel_concept_relation= m_memoryCtx.HelperFindBySystemIdtf("rrel_concept_relation");
      ScAddr const & _rrel_nrel_relation= m_memoryCtx.HelperFindBySystemIdtf("rrel_nrel_relation");


      ScTemplate search_concept_and_nrel_relation;
	
      search_concept_and_nrel_relation.TripleWithRelation(
        result_relation[i]["x"],
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> "c",
        ScType::EdgeAccessVarPosPerm,
        _rrel_concept_relation
      );

      search_concept_and_nrel_relation.TripleWithRelation(
        result_relation[i]["x"],
        ScType::EdgeAccessVarPosPerm,
        ScType::NodeVar >> "n",
        ScType::EdgeAccessVarPosPerm,
        _rrel_nrel_relation
      );


      ScTemplateSearchResult result_concept_and_nrel_relation;
      bool _is_success_find = m_memoryCtx.HelperSearchTemplate(search_concept_and_nrel_relation, result_concept_and_nrel_relation);


      if(_is_success_find)
      {
        for (size_t j = 0; j < result_concept_and_nrel_relation.Size(); j++)
        {
          std::string rel = m_memoryCtx.HelperGetSystemIdtf(result_concept_and_nrel_relation[j]["n"]);
          std::string conc = m_memoryCtx.HelperGetSystemIdtf(result_concept_and_nrel_relation[j]["c"]);
          
          string out = "\t\t"+std::to_string(j)+" :  "+rel+"  -  "+conc;
          SC_LOG_DEBUG(out);

          relation_list.push_back(make_pair(rel, conc));
        }
          
      }
		}
	}





  // if idtf contains non-english symbols
  if(!containsOnlyEnglish(idtf)){
    createAnswerMessageAndStructure("concept_message_about_error_in_system_idtf_non_latin_symbol", answerStructure);
    SC_LOG_DEBUG("CreateClassInstanceAgent finished with error: invalid idtf inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }



  // if class contains non-english symbols
  if(!containsOnlyEnglish(clas)){
    createAnswerMessageAndStructure("concept_message_about_error_in_class_non_latin_symbol", answerStructure);
    SC_LOG_DEBUG("CreateClassInstanceAgent finished with error: invalid class inputs");
    utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
    return SC_RESULT_OK;
  }


  //if idtf already exists
  ScAddr new_concept;
  if(m_memoryCtx.HelperFindBySystemIdtf(idtf, new_concept)){
		if((m_memoryCtx.GetElementType(new_concept) != ScType::NodeVar))
  		{ 
			createAnswerMessageAndStructure("concept_message_about_error_system_idtf_already_exists", answerStructure);
			SC_LOG_DEBUG("CreateClassInstanceAgent finished with error: idtf node already exists");
			utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
			return SC_RESULT_OK;
  		}
	}


  // class should already exist
  if(m_memoryCtx.HelperFindBySystemIdtf(clas, new_concept)){
		if(m_memoryCtx.GetElementType(new_concept) == ScType::NodeVar)
  		{ 
			createAnswerMessageAndStructure("concept_message_about_error_class_idtf_does_not_exist", answerStructure);
			SC_LOG_DEBUG("CreateRelationAgent finished with error: class idtf does not exist");
			utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
			return SC_RESULT_OK;
  		}
	}


  for(const auto& element : relation_list)
  {
    if(clas == element.second)
    {
      createAnswerMessageAndStructure("concept_message_about_error_relation_second_domain_is_class", answerStructure);
			SC_LOG_DEBUG("CreateRelationAgent finished with error: added relation can not connect instance and its class ");
			utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
			return SC_RESULT_OK;
    }
  }


  SC_LOG_DEBUG("All checks passed");


  //creating structure
 ScTemplate result_struct;

  ScAddr const & system_idtf = m_memoryCtx.CreateNode(ScType::NodeConst);
  m_memoryCtx.HelperSetSystemIdtf(idtf, system_idtf);

  ScAddr const& ru_idtf_ = m_memoryCtx.CreateLink(ScType::LinkConst);
  m_memoryCtx.SetLinkContent(ru_idtf_, ru_idtf);



  if(note!="error_no_data")
  {
    ScAddr const& note_node = m_memoryCtx.CreateLink(ScType::LinkConst);
    m_memoryCtx.SetLinkContent(note_node, note);
    result_struct.TripleWithRelation(
      system_idtf,
      ScType::EdgeDCommonVar,
      note_node,
      ScType::EdgeAccessVarPosPerm,
      InterfaceKeynodes::nrel_note
    );
  }

  ScAddr const & clas_ = m_memoryCtx.HelperFindBySystemIdtf(clas);
 


  for (const auto& p : relation_list)
  {
    ScAddr rel_addr = m_memoryCtx.HelperFindBySystemIdtf(p.first);
    ScAddr class_addr =m_memoryCtx.HelperFindBySystemIdtf(p.second);
    

    result_struct.TripleWithRelation(
    system_idtf,
    ScType::EdgeDCommonVar,
    class_addr,
    ScType::EdgeAccessVarPosPerm,
    rel_addr
  );
  }
  
  result_struct.TripleWithRelation(
    system_idtf,
    ScType::EdgeDCommonVar,
    ru_idtf_,
    ScType::EdgeAccessVarPosPerm,
    InterfaceKeynodes::nrel_main_idtf
  );

  
  result_struct.Triple(
    clas_, 
    ScType::EdgeAccessVarPosPerm,
    system_idtf
  );

  result_struct.Triple(
    InterfaceKeynodes::lang_ru, 
    ScType::EdgeAccessVarPosPerm,
    ru_idtf_
  );


  ScTemplateGenResult genInstanceConstruction;
  m_memoryCtx.HelperGenTemplate(result_struct, genInstanceConstruction);


  createAnswerMessageAndStructure("concept_phrase_about_success_creating_class_instance", answerStructure);

  for (size_t i = 0; i < genInstanceConstruction.Size(); ++i)
  {
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, answerStructure, genInstanceConstruction[i]);
  }


  SC_LOG_DEBUG("CreateClassInstanceAgent finished ");
  utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
  return SC_RESULT_OK;



  // std::vector<std::string> formItems = split(formLinkContent, "\n");

  // if (m_memoryCtx.HelperFindBySystemIdtf(formItems[0]).IsValid() || formItems[1] == "" || formItems[2] == "")
  // {
  //   createAnswerMessageAndStructure("concept_phrase_about_create_instance_error", answerStructure);
  //   SC_LOG_DEBUG("CreateClassInstanceAgent finished with error");
  //   utils::AgentUtils::finishAgentWork(&m_memoryCtx, questionNode, true);
  //   return SC_RESULT_OK;
  // }

  
  // ScTemplate instanceConstruction;

  // ScAddr const & instanceNode = m_memoryCtx.CreateNode(ScType::NodeConst);
  // m_memoryCtx.HelperSetSystemIdtf(formItems[0], instanceNode);

  // ScAddr const & mainIdtf = m_memoryCtx.CreateLink(ScType::LinkConst);
  // m_memoryCtx.SetLinkContent(mainIdtf, formItems[1]);

  // ScAddr const & noteLink = m_memoryCtx.CreateLink(ScType::LinkConst);
  // m_memoryCtx.SetLinkContent(noteLink, formItems[2]);

  // ScAddr const & instanceClassNode = m_memoryCtx.HelperFindBySystemIdtf(formItems[3]);

  // if (formItems.size() > 4)
  // {
  //   std::vector<std::string> relationsWithEntities = split(formItems[4], ", ");


  //   for (size_t i = 0; i < relationsWithEntities.size(); ++i)
  //   {
  //     std::vector<std::string> entityAndRelation = split(relationsWithEntities[i], " - ");

  //     ScAddr const & entity = m_memoryCtx.HelperFindBySystemIdtf(entityAndRelation[0]);
  //     ScAddr const & relation = m_memoryCtx.HelperFindBySystemIdtf(entityAndRelation[1]);

  //     instanceConstruction.TripleWithRelation(
  //       instanceNode,
  //       ScType::EdgeDCommonVar,
  //       entity,
  //       ScType::EdgeAccessVarPosPerm,
  //       relation
  //     );
  //   } 
  // }

  // instanceConstruction.Triple(
  //   InterfaceKeynodes::concept_wit_entity,
  //   ScType::EdgeAccessVarPosPerm,
  //   instanceNode
  // );
  // instanceConstruction.Triple(
  //   instanceClassNode,
  //   ScType::EdgeAccessVarPosPerm,
  //   instanceNode
  // );
  // instanceConstruction.Triple(
  //   InterfaceKeynodes::lang_ru,
  //   ScType::EdgeAccessVarPosPerm,
  //   mainIdtf
  // );
  // instanceConstruction.Triple(
  //   InterfaceKeynodes::lang_ru,
  //   ScType::EdgeAccessVarPosPerm,
  //   noteLink
  // );
  // instanceConstruction.TripleWithRelation(
  //   instanceNode,
  //   ScType::EdgeDCommonVar,
  //   mainIdtf,
  //   ScType::EdgeAccessVarPosPerm,
  //   InterfaceKeynodes::nrel_main_idtf
  // );
  // instanceConstruction.TripleWithRelation(
  //   instanceNode,
  //   ScType::EdgeDCommonVar,
  //   noteLink,
  //   ScType::EdgeAccessVarPosPerm,
  //   InterfaceKeynodes::nrel_note
  // );

  // ScTemplateGenResult full_construction;
  // m_memoryCtx.HelperGenTemplate(instanceConstruction, full_construction);



}



std::vector<std::string> CreateClassInstanceAgent::split(const string & s, const string & delimiter)
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

bool CreateClassInstanceAgent::checkActionClass(ScAddr const & actionAddr)
{
  return m_memoryCtx.HelperCheckEdge(
      InterfaceKeynodes::action_create_class_instance, actionAddr, ScType::EdgeAccessConstPosPerm);
}

void CreateClassInstanceAgent::createAnswer(std::string message)
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

void CreateClassInstanceAgent::createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure)
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



bool CreateClassInstanceAgent::containsOnlyEnglish(const std::string& str){
    for (char ch : str)
        if (!std::isalpha(ch) && ch != '_') 
            return false;
    return true; 
}