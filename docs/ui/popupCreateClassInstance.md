# User interface component (pop-up window) for creating a class instance

This component is used to create a class instance.

**Phrase for calling the component**

`Создай экземпляр класса`

**Action Class:**

`action_create_class_instance`

### Description

<img src="../images/createClassInstance1.png"></img>

This state of the component is a form, by filling in which data is stored about the system identifier of the new relation, about the identifier of the relation in Russian, note about class and class of the new instance . To save the form data and create structures, click the "Save" button. To add some relations with the new instance, click "Add relations" button.

<img src="../images/createClassInstance2.png"></img>

This state of the component is a form, by fillind in which data is stored about the relation with some concept. To save relation click "Save" button. To add another one relation click "Add" button.

### Component implementation language

TypeScript

### Result

Possible results:

* `Экземпляр класса создан` - construct has been successfully created.
* `Ошибка. Введённые вами системные индетификаторы уже существуют.`- the data entered by the user already exists in the knowledge base.
* `Дейстиве прервано пользователем.` - the user has closed the component.