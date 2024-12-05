# User interface component (pop-up window) for creating a class

This component is used to create a class.

**Phrase for calling the component**

`Создай класс`

**Action Class:**

`action_create_class`

### Description

<img src="../images/createClass.png"></img>

This state of the component is a form, by filling in which data is stored about the system identifier of the new class, about the identifier of the class in Russian, note about class, superclass and decomposition of the new class. To save the form data and create structures, click the "Save" button.

### Component implementation language

TypeScript

### Result

Possible results:

* `Новый класс создан!` - construct has been successfully created.
* `Ошибка. Введённые вами системные индетификаторы уже существуют.`- the data entered by the user already exists in the knowledge base.
* `Дейстиве прервано пользователем.` - the user has closed the component.