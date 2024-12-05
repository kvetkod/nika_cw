# Create message class and phrase template agent

This agent creates a message class, phrase templates and a logical response rule.

**Action class:**

`action_create_message_class_and_phrase_template`

**Parameters:**

1. `messageAddr` -- an element of `concept_message`.
2. `formLinkAddr` -- link with information from popup component("done" or "user close").
3. `nodeVarPhrase` -- class about questions.
4. `linkPhraseRu` -- ru identifier of the class about questions.
5. `nodeVarAnswer` -- class about answers.
6. `linkAnswerRu` -- ru identifier or the class about answers.
7. `structAnswers` -- array of answers.

### Examples

**Example of an input structure:**

<img src="../images/createMessageClassAndPhraseTemplateInput.jpg"></img>

**Examples of an output structure:**

Output about the successful creation of structures.

<img src="../images/createMessageClassAndPhraseTemplateAgentOutput1.png"></img>

Error output about invalid parameters.

<img src="../images/createMessageClassAndPhraseTemplateAgentOutput2.png"></img>

Error ouput about the existence of params.

<img src="../images/createMessageClassAndPhraseTemplateAgentOutput3.png"></img>

Output to notify when the user has completed an action.

<img src="../images/createMessageClassAndPhraseTemplateAgentOutput4.png"></img>

The result of the agent's work depends on the data received from the component.If the received data is correct, the structures will be created. Otherwise, a corresponding error message will be displayed.

### Agent implementation language
C++

### Result

Possible result codes:

* `SC_RESULT_OK` - the constructs (message class, phrase templates and logical response rule) were successfully created or formLinkAddr contains incorrect data.
* `SC_RESULT_ERROR`- internal error.
