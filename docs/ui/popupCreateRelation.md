# User interface component (pop-up window) for creating a relation

This component is used to create a relation.

**Phrase for calling the component**

`Создай отношение`

**Action Class:**

`action_create_relation`

### Description

<img src="../images/createRelation.png"></img>

This state of the component is a form, by filling in which data is stored about the system identifier of the new relation, about the identifier of the relation in Russian, note about class, first and second domains of the new relation and classification. To save the form data and create structures, click the "Save" button.

### Component implementation language

TypeScript

### Result

Possible results:

* `Отношение добавлено.` - construct has been successfully created.
* `Ошибка. Введённые вами системные индетификаторы уже существуют.`- the data entered by the user already exists in the knowledge base.
* `Дейстиве прервано пользователем.` - the user has closed the component.