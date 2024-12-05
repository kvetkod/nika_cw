# Create class instance agent

This agent creates a class instance.

**Action class:**

`action_create_class_instance`

**Parameters:**

1. `messageAddr` -- an element of `concept_message`.
2. `linkStateAddr` -- link with information from popup component("done" or "user close").
3. `nodeVarSystemIdtf` -- system identifier of the new class instance.
4. `linkSystemRuIdtf` -- system ru identifier.
5. `linkNote` -- note about class instance.
6. `nodeVarClass` -- class of the class instance.
7. {`nodeVarConcept`: `nodeVarRelation`} -- a map with relations.


### Examples

**Example of an input structure:**

<img src="../images/createClassInstanceInput.jpg"></img>

**Examples of an output structure:**

Output about the successful creation of structures.

<img src="../images/createClassInstanceOutput1.jpg"></img>

Error ouput about the existence of params.

<img src="../images/createClassInstanceOutput2.jpg"></img>

Output to notify when the user has completed an action.

<img src="../images/createClassInstanceOutput3.jpg"></img>

The result of the agent's work depends on the data received from the component. If the received data is correct, the structures will be created. Otherwise, a corresponding error message will be displayed.

### Agent implementation language
C++

### Result

Possible result codes:

* `SC_RESULT_OK` - the construct was successfully created or form contains incorrect data.
* `SC_RESULT_ERROR`- internal error.
