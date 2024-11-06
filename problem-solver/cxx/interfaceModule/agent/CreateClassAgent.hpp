#pragma once

#include "sc-memory/kpm/sc_agent.hpp"
#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "CreateClassAgent.generated.hpp"

namespace interfaceModule
{
class CreateClassAgent : public ScAgent
{
  SC_CLASS(Agent, Event(scAgentsCommon::CoreKeynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

private:
  bool containsOnlyEnglish(const std::string);
  bool checkActionClass(ScAddr const & actionAddr);
  void createAnswer(std::string message);
  void createAnswerMessageAndStructure(std::string conceptName, ScAddr const & answerStructure);
  ScAddr Search(ScAddr rrel_node, ScAddr node);
};
}  // namespace interfaceModule
