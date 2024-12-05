# Create relation agent 

This agent creates a relation.

**Action class:**

`action_create_relation`

**Parameters:**

1. `messageAddr` -- an element of `concept_message`.
2. `linkStateAddr` -- link with information from popup component("done" or "user close").
3. `nodeVarSystemIdtf` -- system identifier of the new relation.
4. `linkSystemRuIdtf` -- system ru identifier.
5. `linkNote` -- note about class.
6. `nodeVarFirstDomain` -- first domain of the relation.
7. `nodeVarSecondDomain` -- second domain of the relation.
8. `structClassificators` -- what kind of relation(binary and etc.).


### Examples

**Example of an input structure:**

<img src="../images/createRelationAgentInput.jpg"></img>

**Examples of an output structure:**

Output about the successful creation of structures.

<img src="../images/createRelationAgentOutput1.jpg"></img>

Error ouput about the existence of params.

<img src="../images/createRelationAgentOutput2.jpg"></img>

Output to notify when the user has completed an action.

<img src="../images/createRelationAgentOutput3.jpg"></img>

The result of the agent's work depends on the data received from the component.If the received data is correct, the structures will be created. Otherwise, a corresponding error message will be displayed.

### Agent implementation language
C++

### Result

Possible result codes:

* `SC_RESULT_OK` - the construct was successfully created or form contains incorrect data.
* `SC_RESULT_ERROR`- internal error.
