import React from 'react';
import { useEffect, useState, useRef } from 'react';
import {
    handleSave,
    userClose,
    handleSaveClassInstance,
    findAnyInKb,
    findRelationsInKb,
    handleSaveToCreateClass,
    handleRelationInstance,
    handleSaveClassInstanceWithRelations,
} from '@api/sc/agents/helper';
import { AutoComplete } from 'antd';
import { FormInput } from './styled';

export const CreateMessageClassPopup = (setCreatePopup, setCreatePhraseTemplatePopup, setForm) => {
    const messageSystemIdentifierRef = useRef<HTMLInputElement>(null);
    const messageRussianIdentifierRef = useRef<HTMLInputElement>(null);
    const messageWitAiRef = useRef<HTMLInputElement>(null);

    useEffect(() => {
        messageSystemIdentifierRef.current!.value = 'concept_message_about_';
        messageRussianIdentifierRef.current!.value = 'Класс сообщений о';
    }, [messageSystemIdentifierRef, messageRussianIdentifierRef]);

    const handleClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        const idtf_prefix = 'concept_message_about_';
        const idtf_currentValue = messageSystemIdentifierRef.current?.value ?? '';
        if (!idtf_currentValue.startsWith(idtf_prefix) || idtf_currentValue.length <= idtf_prefix.length) {
            alert('Некорректный системный идентификатор');
            return;
        }

        const ru_prefix = 'Класс сообщений о';
        const ru_currentValue = messageRussianIdentifierRef.current?.value ?? '';
        if (!ru_currentValue.startsWith(ru_prefix) || ru_currentValue.length <= ru_prefix.length) {
            alert('Некорректный идентификатор на русском языке');
            return;
        }

        if (!messageWitAiRef.current?.value) {
            alert("Поле 'Название интента в Wit.ai' не может быть пустым");
            return;
        }
        setCreatePopup(false);
        setCreatePhraseTemplatePopup(true);
        setForm(
            messageSystemIdentifierRef.current?.value +
                '\n' +
                messageRussianIdentifierRef.current?.value +
                '\n' +
                messageWitAiRef.current?.value,
        );
    };

    const closeClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        await userClose();
        setCreatePopup(false);
    };

    const systemIdtfHandleKeyDown = (event) => {
        const inputValue = messageSystemIdentifierRef.current?.value;
        const fixedText = 'concept_message_about_';

        if (inputValue === fixedText && (event.key === 'Backspace' || event.key === 'Delete')) {
            event.preventDefault();
            messageSystemIdentifierRef.current!.value = fixedText;
        }
    };

    const russianIdtfHandleKeyDown = (event) => {
        const inputValue = messageRussianIdentifierRef.current?.value;
        const fixedText = 'Класс сообщений о';

        if (inputValue === fixedText && (event.key === 'Backspace' || event.key === 'Delete')) {
            event.preventDefault();
            messageRussianIdentifierRef.current!.value = fixedText;
        }
    };

    return (
        <div className="popup">
            <h3>Создание класса сообщений и класса ответных фраз</h3>
            <button className="close_button" onClick={closeClick}>
                ×
            </button>
            <div className="numdiv">
                <p className="state-3">1</p>
                <p className="state-4">2</p>
            </div>
            <div className="form">
                <h4>Системный индетификатор*</h4>
                <input
                    type="text"
                    className="input"
                    ref={messageSystemIdentifierRef}
                    onKeyDown={systemIdtfHandleKeyDown}
                    required
                />
                <h4>Индетификатор на русском языке*</h4>
                <input
                    type="text"
                    className="input"
                    ref={messageRussianIdentifierRef}
                    onKeyDown={russianIdtfHandleKeyDown}
                    required
                />
                <h4>Название интента в Wit.ai*</h4>
                <input type="text" className="input" ref={messageWitAiRef} placeholder="Ваш Wit.ai intent" required />
            </div>
            <button className="button next" onClick={handleClick}>
                Далее
            </button>
        </div>
    );
};

export const CreatePhraseTemplatePopup = (setCreatePhraseTemplatePopup, form) => {
    const phraseSystemIdentifierRef = useRef<HTMLInputElement>(null);
    const phraseRussianIdentifierRef = useRef<HTMLInputElement>(null);

    const [chipsValues, setChipsValues] = useState<string[]>([]);
    const [editingIndex, setEditingIndex] = useState(null);
    const handleChipAdd = (value: string) => {
        if (!value) {
            alert('Ответ не может быть пустым');
            return;
        }
        setChipsValues([...chipsValues, value]);
    };

    useEffect(() => {
        phraseSystemIdentifierRef.current!.value = 'concept_phrase_about_';
        phraseRussianIdentifierRef.current!.value = 'Класс ответных фраз о';
    }, [phraseSystemIdentifierRef, phraseRussianIdentifierRef]);

    const handleChipDelete = (index: number) => {
        const newChipsValues = [...chipsValues];
        newChipsValues.splice(index, 1);
        setChipsValues(newChipsValues);
    };

    const handleChipEdit = (index) => {
        setEditingIndex(index);
    };

    const handleChipChange = (event, index) => {
        const updatedChipsValues = [...chipsValues];
        updatedChipsValues[index] = event.target.value;
        setChipsValues(updatedChipsValues);
    };

    const handleClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        if (chipsValues.length == 0) {
            alert('Вы не добавили ни одного ответа');
            return;
        }

        const idtf_prefix = 'concept_phrase_about_';
        const idtf_currentValue = phraseSystemIdentifierRef.current?.value ?? '';
        if (!idtf_currentValue.startsWith(idtf_prefix) || idtf_currentValue.length <= idtf_prefix.length) {
            alert('Некорректный системный идентификатор');
            return;
        }

        const ru_prefix = 'Класс ответных фраз о';
        const ru_currentValue = phraseRussianIdentifierRef.current?.value ?? '';
        if (!ru_currentValue.startsWith(ru_prefix) || ru_currentValue.length <= ru_prefix.length) {
            alert('Некорректный идентификатор на русском языке');
            return;
        }

        await handleSave(phraseSystemIdentifierRef, phraseRussianIdentifierRef, form, chipsValues);
        setCreatePhraseTemplatePopup(false);
    };

    const closeClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        await userClose();
        setCreatePhraseTemplatePopup(false);
    };

    const systemIdtfHandleKeyDown = (event) => {
        const inputValue = phraseSystemIdentifierRef.current?.value;
        const fixedText = 'concept_phrase_about_';

        if (inputValue === fixedText && (event.key === 'Backspace' || event.key === 'Delete')) {
            event.preventDefault();
            phraseSystemIdentifierRef.current!.value = fixedText;
        }
    };

    const russianIdtfHandleKeyDown = (event) => {
        const inputValue = phraseRussianIdentifierRef.current?.value;
        const fixedText = 'Класс ответных фраз о';

        if (inputValue === fixedText && (event.key === 'Backspace' || event.key === 'Delete')) {
            event.preventDefault();
            phraseRussianIdentifierRef.current!.value = fixedText;
        }
    };

    return (
        <div className="popup">
            <h3>Создание класса сообщений и класса ответных фраз</h3>
            <button className="close_button" onClick={closeClick}>
                ×
            </button>
            <div className="numdiv">
                <p className="state-1">1</p>
                <p className="state-2">2</p>
            </div>
            <div className="form">
                <h4>Системный индетификатор*</h4>
                <input
                    type="text"
                    className="input"
                    ref={phraseSystemIdentifierRef}
                    onKeyDown={systemIdtfHandleKeyDown}
                    required
                />
                <h4>Индетификатор на русском языке*</h4>
                <input
                    type="text"
                    className="input"
                    ref={phraseRussianIdentifierRef}
                    onKeyDown={russianIdtfHandleKeyDown}
                    required
                />
            </div>
            <h3>Ответные фразы</h3>
            <div className="chips">
                <input
                    type="text"
                    className="chip-input"
                    placeholder="Напишите фразу и нажмите enter"
                    onKeyDown={(e: React.KeyboardEvent<HTMLInputElement>) => {
                        if (e.key === 'Enter') {
                            handleChipAdd(e.currentTarget.value);
                            e.currentTarget.value = '';
                        }
                    }}
                />
                {chipsValues.map((value, index) => (
                    <div className="chip-div" key={index}>
                        {editingIndex === index ? (
                            <input
                                type="text"
                                className="chip"
                                value={value}
                                onChange={(e) => handleChipChange(e, index)}
                                onBlur={() => setEditingIndex(null)}
                                autoFocus
                            />
                        ) : (
                            <div className="chip" onClick={() => handleChipEdit(index)}>
                                {value}
                            </div>
                        )}
                        <span className="chip-delete" onClick={() => handleChipDelete(index)}>
                            ×
                        </span>
                    </div>
                ))}
            </div>
            <button className="button save" onClick={handleClick}>
                Сохранить
            </button>
        </div>
    );
};

export const CreateClassInstancePopup = (setCreateClassInstancePopup, setCreateRelationToEntityPopup, setFirstForm) => {
    const classInstanceSystemIdentifierRef = useRef<HTMLInputElement>(null);
    const classInstanceRussianIdentifierRef = useRef<HTMLInputElement>(null);
    const classInstanceNoteRef = useRef<HTMLInputElement>(null);
    const [selectedOption, setSelectedOption] = useState('');
    const [inputValue, setInputValue] = useState('concept_');
    const [availableClasses, setAvailableClasses] = useState<{ label: string; value: string }[]>([]);

    const onSelect = (data, option) => {
        setSelectedOption(option);
        setInputValue(option.label);
    };

    const onChange = (data, option) => {
        setInputValue(data);
        setSelectedOption(option);
    };

    useEffect(() => {
        findAnyInKb(setAvailableClasses, 'concept_');
    }, []);

    const handleClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        if (!classInstanceRussianIdentifierRef.current?.value || !classInstanceSystemIdentifierRef.current?.value) {
            alert('Вы не заполнили все обязательные поля');
            return;
        }

        const class_prefix = 'concept_';
        const class_currentValue = inputValue ?? '';
        if (!class_currentValue.startsWith(class_prefix) || class_currentValue.length <= class_prefix.length) {
            alert('Некорректный класс экземпляра');
            return;
        }

        await handleSaveClassInstance(
            classInstanceSystemIdentifierRef,
            classInstanceRussianIdentifierRef,
            classInstanceNoteRef,
            inputValue,
        );
        setCreateClassInstancePopup(false);
    };

    const handleAddRelationClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        if (!classInstanceRussianIdentifierRef.current?.value || !classInstanceSystemIdentifierRef.current?.value) {
            alert('Вы не заполнили все обязательные поля');
            return;
        }

        const class_prefix = 'concept_';
        const class_currentValue = inputValue ?? '';
        if (!class_currentValue.startsWith(class_prefix) || class_currentValue.length <= class_prefix.length) {
            alert('Некорректный класс экземпляра');
            return;
        }

        setFirstForm([
            classInstanceSystemIdentifierRef.current?.value,
            classInstanceRussianIdentifierRef.current?.value,
            classInstanceNoteRef.current?.value,
            inputValue,
        ]);
        setCreateClassInstancePopup(false);
        setCreateRelationToEntityPopup(true);
    };

    const closeClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        await userClose();
        setCreateClassInstancePopup(false);
    };

    return (
        <div className="popup">
            <h2>Создание экземпляра класса</h2>
            <button className="close_button" onClick={closeClick}>
                ×
            </button>
            <div className="form">
                <h4>Системный индетификатор*</h4>
                <input
                    type="text"
                    className="input"
                    placeholder="Системный идентификатор вашего экземпляра"
                    ref={classInstanceSystemIdentifierRef}
                    required
                />
                <h4>Индетификатор на русском языке*</h4>
                <input
                    type="text"
                    className="input"
                    placeholder="Название вашего экземпляра"
                    ref={classInstanceRussianIdentifierRef}
                    required
                />
                <h4>Примечание(определение)</h4>
                <input
                    type="text"
                    className="input"
                    placeholder="Описание вашего экземпляра"
                    ref={classInstanceNoteRef}
                />
                <h4>Класс*</h4>
                <AutoComplete
                    value={inputValue}
                    options={availableClasses}
                    autoFocus={true}
                    style={{ width: '95%' }}
                    filterOption={(inputValue, option) =>
                        option?.label.toUpperCase().indexOf(inputValue.toUpperCase()) !== -1
                    }
                    onSelect={onSelect}
                    onChange={onChange}
                    dropdownStyle={{ zIndex: 10000 }}
                >
                    <FormInput />
                </AutoComplete>
                <button className="button next" onClick={handleAddRelationClick}>
                    Добавить отношения
                </button>
            </div>
            <button className="button save" onClick={handleClick}>
                Сохранить
            </button>
        </div>
    );
};

export const AddRelationToEntityPopup = (setCreateRelationToEntityPopup, firstForm: string[]) => {
    const [selectedEntityOption, setSelectedEntityOption] = useState('');
    const [entity, setEntity] = useState('concept_');
    const [selectedRelationOption, setSelectedRelationOption] = useState('');
    const [relation, setRelation] = useState('nrel_');
    const [availableEntity, setAvailableEntity] = useState<{ label: string; value: string }[]>([]);
    const [availableRelations, setAvailableRelations] = useState<{ label: string; value: string }[]>([]);
    const [form, setForm] = useState<{ entity: string; relation: string }[]>([]);

    const onSelectEntity = (data, option) => {
        setSelectedEntityOption(option);
        setEntity(option.label);
    };

    const onChangeEntity = (data, option) => {
        setEntity(data);
        setSelectedEntityOption(option);
    };

    const onSelectRelation = (data, option) => {
        setSelectedRelationOption(option);
        setRelation(option.label);
    };

    const onChangeRelation = (data, option) => {
        setRelation(data);
        setSelectedRelationOption(option);
    };

    useEffect(() => {
        findAnyInKb(setAvailableEntity, 'concept_');
        findRelationsInKb(setAvailableRelations);
    }, []);

    const handleClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        handleSaveClassInstanceWithRelations(firstForm, form);
        setCreateRelationToEntityPopup(false);
        console.log(form.map((item) => `${item.entity} - ${item.relation}`).join(', '));
    };

    const handleSaveClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        const concept_prefix = 'concept_';
        const concept_currentValue = entity ?? '';
        if (!concept_currentValue.startsWith(concept_prefix) || concept_currentValue.length <= concept_prefix.length) {
            alert('Некорректная сущность');
            return;
        }

        const nrel_prefix = 'nrel_';
        const nrel_currentValue = relation ?? '';
        if (!nrel_currentValue.startsWith(nrel_prefix) || nrel_currentValue.length <= nrel_prefix.length) {
            alert('Некорректное отношение');
            return;
        }

        const newForm = [...form, { entity: entity, relation: relation }];
        setForm(newForm);
        setEntity('concept_');
        setRelation('nrel_');
    };

    const closeClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        await userClose();
        setCreateRelationToEntityPopup(false);
    };

    const handleChipDelete = (index: number) => {
        const newChipsValues = [...form];
        newChipsValues.splice(index, 1);
        setForm(newChipsValues);
    };

    return (
        <div className="popup">
            <h2>Создание экземпляра класса</h2>
            <button className="close_button" onClick={closeClick}>
                ×
            </button>
            <div className="form">
                <h4>Сущность</h4>
                <AutoComplete
                    value={entity}
                    options={availableEntity}
                    style={{ width: '98%' }}
                    filterOption={(inputValue, option) =>
                        option?.label.toUpperCase().indexOf(inputValue.toUpperCase()) !== -1
                    }
                    onSelect={onSelectEntity}
                    onChange={onChangeEntity}
                    dropdownStyle={{ zIndex: 10000 }}
                >
                    <FormInput />
                </AutoComplete>
                <h4>Отношение</h4>
                <AutoComplete
                    value={relation}
                    options={availableRelations}
                    style={{ width: '98%' }}
                    filterOption={(inputValue, option) =>
                        option?.label.toUpperCase().indexOf(inputValue.toUpperCase()) !== -1
                    }
                    onSelect={onSelectRelation}
                    onChange={onChangeRelation}
                    dropdownStyle={{ zIndex: 10000 }}
                >
                    <FormInput />
                </AutoComplete>

                {form.map((value, index) => (
                    <div className="chip-div" key={index}>
                        <div className="chip">
                            <span>{value.entity}</span>
                            <span>—</span>
                            <span>{value.relation}</span>
                        </div>
                        <span className="chip-delete" onClick={() => handleChipDelete(index)}>
                            ×
                        </span>
                    </div>
                ))}
                <button className="button next" onClick={handleSaveClick}>
                    Добавить
                </button>
            </div>
            <button className="button save" onClick={handleClick}>
                Сохранить
            </button>
        </div>
    );
};

export const CreateClassPopup = (setCreateClassPopup) => {
    const classSystemIdentifierRef = useRef<HTMLInputElement>(null);
    const classRussianIdentifierRef = useRef<HTMLInputElement>(null);
    const classNoteIdentifierRef = useRef<HTMLInputElement>(null);

    const [chipsValues, setChipsValues] = useState<string[]>([]);
    const [editingIndex, setEditingIndex] = useState(null);

    const [selectedOption, setSelectedOption] = useState('');
    const [inputValue, setInputValue] = useState('concept_');
    const [inputValueDecomposition, setInputValueDecomposition] = useState('concept_');
    const [availableClasses, setAvailableClasses] = useState<{ label: string; value: string }[]>([]);

    const onSelect = (data, option) => {
        setSelectedOption(option);
        setInputValue(option.label);
    };

    const onChange = (data, option) => {
        setInputValue(data);
        setSelectedOption(option);
    };

    const onSelectDecomposition = (data, option) => {
        setSelectedOption(option);
        setInputValueDecomposition(option.label);
    };

    const onChangeDecomposition = (data, option) => {
        setInputValueDecomposition(data);
        setSelectedOption(option);
    };

    const handleChipAdd = (value: string) => {
        if (chipsValues.includes(value)) {
            alert('Компонент не может быть добавлен дважды');
            return;
        }

        const prefix = 'concept_';
        const currentValue = value ?? '';
        if (!currentValue.startsWith(prefix) || currentValue.length <= prefix.length) {
            alert('Некорректный элемент декомпозиции');
            return;
        }

        setChipsValues([...chipsValues, value]);
    };

    useEffect(() => {
        classSystemIdentifierRef.current!.value = 'concept_';
        findAnyInKb(setAvailableClasses, 'concept_');
    }, [classSystemIdentifierRef]);

    const handleChipDelete = (index: number) => {
        const newChipsValues = [...chipsValues];
        newChipsValues.splice(index, 1);
        setChipsValues(newChipsValues);
    };

    const handleChipEdit = (index) => {
        setEditingIndex(index);
    };

    const handleChipChange = (event, index) => {
        const updatedChipsValues = [...chipsValues];
        updatedChipsValues[index] = event.target.value;
        setChipsValues(updatedChipsValues);
    };

    const handleClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        if (!classRussianIdentifierRef.current?.value || !classNoteIdentifierRef.current?.value) {
            alert('Вы не заполнили все обязательные поля');
            return;
        }

        const prefix = 'concept_';
        const idtf_currentValue = classSystemIdentifierRef.current?.value ?? '';
        if (!idtf_currentValue.startsWith(prefix) || idtf_currentValue.length <= prefix.length) {
            alert('Некорректный системный идентификатор');
            return;
        }

        const idtf_prefix = 'concept_';
        const innput_currentValue = inputValue ?? '';
        if (innput_currentValue != '') {
            if (!innput_currentValue.startsWith(idtf_prefix) || innput_currentValue.length < idtf_prefix.length) {
                alert('Некорректный системный идентификатор');
                return;
            }
        }

        await handleSaveToCreateClass(
            classSystemIdentifierRef,
            classRussianIdentifierRef,
            classNoteIdentifierRef,
            inputValue,
            chipsValues,
        );
        setCreateClassPopup(false);
    };

    const closeClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        await userClose();
        setCreateClassPopup(false);
    };

    const systemIdtfHandleKeyDown = (event) => {
        const inputValue = classSystemIdentifierRef.current?.value;
        const fixedText = 'concept_';

        if (inputValue === fixedText && (event.key === 'Backspace' || event.key === 'Delete')) {
            event.preventDefault();
            classSystemIdentifierRef.current!.value = fixedText;
        }
    };

    return (
        <div className="popup">
            <h2>Добавление понятий</h2>
            <button className="close_button" onClick={closeClick}>
                ×
            </button>
            <div className="form">
                <h4>Системный индетификатор*</h4>
                <input
                    type="text"
                    className="input"
                    ref={classSystemIdentifierRef}
                    onKeyDown={systemIdtfHandleKeyDown}
                    required
                />
                <h4>Индетификатор на русском языке*</h4>
                <input
                    type="text"
                    className="input"
                    ref={classRussianIdentifierRef}
                    placeholder="Название вашего класса"
                    required
                />
                <h4>Примечание*</h4>
                <input
                    type="text"
                    className="input"
                    ref={classNoteIdentifierRef}
                    placeholder="Примечание(определение)"
                    required
                />
                <h4>Надкласс</h4>
                <AutoComplete
                    value={inputValue}
                    options={availableClasses}
                    style={{ width: '96%' }}
                    autoFocus={true}
                    filterOption={(inputValue, option) =>
                        option?.label.toUpperCase().indexOf(inputValue.toUpperCase()) !== -1
                    }
                    onSelect={onSelect}
                    onChange={onChange}
                    dropdownStyle={{ zIndex: 10000 }}
                >
                    <FormInput />
                </AutoComplete>
            </div>

            <h2>Разбиение на компоненты</h2>

            <div className="chips">
                <AutoComplete
                    value={inputValueDecomposition}
                    options={availableClasses}
                    autoFocus={true}
                    style={{ width: '98%' }}
                    filterOption={(inputValue, option) =>
                        option?.label.toUpperCase().indexOf(inputValue.toUpperCase()) !== -1
                    }
                    onSelect={onSelectDecomposition}
                    onChange={onChangeDecomposition}
                    dropdownStyle={{ zIndex: 10000 }}
                >
                    <FormInput
                        placeholder="Декомпозиция"
                        onKeyDown={(e: React.KeyboardEvent<HTMLInputElement>) => {
                            if (e.key === 'Enter') {
                                handleChipAdd(e.currentTarget.value);
                                e.currentTarget.value = '';
                            }
                        }}
                    ></FormInput>
                </AutoComplete>

                {chipsValues.map((value, index) => (
                    <div className="chip-div" key={index}>
                        {editingIndex === index ? (
                            <input
                                type="text"
                                className="chip"
                                value={value}
                                onChange={(e) => handleChipChange(e, index)}
                                onBlur={() => setEditingIndex(null)}
                                autoFocus
                            />
                        ) : (
                            <div className="chip" onClick={() => handleChipEdit(index)}>
                                {value}
                            </div>
                        )}
                        <span className="chip-delete" onClick={() => handleChipDelete(index)}>
                            ×
                        </span>
                    </div>
                ))}
            </div>
            <button className="button save" onClick={handleClick}>
                Сохранить
            </button>
        </div>
    );
};

export const CreateRelationPopup = (setCreateRelationPopup) => {
    const relationSystemIdentifierRef = useRef<HTMLInputElement>(null);
    const relationRussianIdentifierRef = useRef<HTMLInputElement>(null);
    const relationNoteRef = useRef<HTMLInputElement>(null);
    const [firstDomain, setFirstDomain] = useState('concept_');
    const [secondDomain, setSecondDomain] = useState('concept_');
    const [availableClasses, setAvailableClasses] = useState<{ label: string; value: string }[]>([]);
    const [isBinaryChecked, setIsBinaryChecked] = useState(false);
    const [isOrientedChecked, setIsOrientedChecked] = useState(false);
    const [isAntireflexiveChecked, setIsAntireflexiveChecked] = useState(false);
    const [isAsymmetricChecked, setIsAsymmetricChecked] = useState(false);
    const [isAntitransitiveChecked, setIsAntitransitiveChecked] = useState(false);

    const handleBinaryCheckboxChange = () => {
        setIsBinaryChecked(!isBinaryChecked);
    };

    const handleOrientedCheckboxChange = () => {
        setIsOrientedChecked(!isOrientedChecked);
    };

    const handleAntireflexiveCheckboxChange = () => {
        setIsAntireflexiveChecked(!isAntireflexiveChecked);
    };

    const handleAsymmetricCheckboxChange = () => {
        setIsAsymmetricChecked(!isAsymmetricChecked);
    };

    const handleAntitransitiveCheckboxChange = () => {
        setIsAntitransitiveChecked(!isAntitransitiveChecked);
    };

    const onSelectFirstDomain = (data, option) => {
        setFirstDomain(option.label);
    };

    const onChangeFirstDomain = (data, option) => {
        setFirstDomain(data);
    };

    const onSelectSecondDomain = (data, option) => {
        setSecondDomain(option.label);
    };

    const onChangeSecondDomain = (data, option) => {
        setSecondDomain(data);
    };

    useEffect(() => {
        findAnyInKb(setAvailableClasses, 'concept_');
        relationSystemIdentifierRef.current!.value = 'nrel_';
    }, [relationSystemIdentifierRef]);

    const systemIdtfHandleKeyDown = (event) => {
        const inputValue = relationSystemIdentifierRef.current?.value;
        const fixedText = 'nrel_';

        if (inputValue === fixedText && (event.key === 'Backspace' || event.key === 'Delete')) {
            event.preventDefault();
            relationSystemIdentifierRef.current!.value = fixedText;
        }
    };

    const handleClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        const idtf_prefix = 'nrel_';
        const idtf_currentValue = relationSystemIdentifierRef.current?.value ?? '';
        if (!idtf_currentValue.startsWith(idtf_prefix) || idtf_currentValue.length <= idtf_prefix.length) {
            alert('Некорректный системный идентификатор');
            return;
        }

        if (!relationRussianIdentifierRef.current?.value) {
            alert('Пустой идентификатор на русском языке');
            return;
        }

        if (!firstDomain || !secondDomain) {
            alert('Пустой домен');
            return;
        }

        if (firstDomain == secondDomain) {
            alert('Первый домен не может быть равен второму');
            return;
        }

        await handleRelationInstance(
            relationSystemIdentifierRef,
            relationRussianIdentifierRef,
            relationNoteRef,
            firstDomain,
            secondDomain,
            isBinaryChecked,
            isOrientedChecked,
            isAntireflexiveChecked,
            isAsymmetricChecked,
            isAntitransitiveChecked,
        );
        setCreateRelationPopup(false);
    };

    const closeClick: React.MouseEventHandler<HTMLButtonElement> = async (event) => {
        await userClose();
        setCreateRelationPopup(false);
    };

    return (
        <div className="popup">
            <h2>Создание неролевого отношения</h2>
            <button className="close_button" onClick={closeClick}>
                ×
            </button>
            <div className="form">
                <h4>Системный индетификатор*</h4>
                <input
                    type="text"
                    className="input"
                    ref={relationSystemIdentifierRef}
                    onKeyDown={systemIdtfHandleKeyDown}
                    required
                />
                <h4>Индетификатор на русском языке*</h4>
                <input
                    type="text"
                    className="input"
                    placeholder="Название вашего отношения"
                    ref={relationRussianIdentifierRef}
                    required
                />
                <h4>Примечание(определение)*</h4>
                <input
                    type="text"
                    className="input"
                    placeholder="Описание вашего отношения"
                    ref={relationNoteRef}
                    required
                />
                <h4>Первый домен*</h4>
                <AutoComplete
                    value={firstDomain}
                    options={availableClasses}
                    style={{ width: '96%' }}
                    filterOption={(inputValue, option) =>
                        option?.label.toUpperCase().indexOf(inputValue.toUpperCase()) !== -1
                    }
                    onSelect={onSelectFirstDomain}
                    onChange={onChangeFirstDomain}
                    dropdownStyle={{ zIndex: 10000 }}
                >
                    <FormInput />
                </AutoComplete>

                <h4>Второй домен*</h4>
                <AutoComplete
                    value={secondDomain}
                    options={availableClasses}
                    style={{ width: '96%' }}
                    filterOption={(inputValue, option) =>
                        option?.label.toUpperCase().indexOf(inputValue.toUpperCase()) !== -1
                    }
                    onSelect={onSelectSecondDomain}
                    onChange={onChangeSecondDomain}
                    dropdownStyle={{ zIndex: 10000 }}
                >
                    <FormInput />
                </AutoComplete>

                <h4>Классификация отношения</h4>
                <div style={{ display: 'flex', flexDirection: 'column' }}>
                    <div style={{ display: 'flex' }}>
                        <input
                            type="checkbox"
                            checked={isBinaryChecked}
                            onChange={handleBinaryCheckboxChange}
                            style={{ width: '20px', height: '20px', marginRight: '15px', marginLeft: '15px' }}
                        />
                        <label>Бинарное отношение</label>
                    </div>
                    <div style={{ display: 'flex' }}>
                        <input
                            type="checkbox"
                            checked={isOrientedChecked}
                            onChange={handleOrientedCheckboxChange}
                            style={{ width: '20px', height: '20px', marginRight: '15px', marginLeft: '15px' }}
                        />
                        <label>Ориентированное отношение</label>
                    </div>
                    <div style={{ display: 'flex' }}>
                        <input
                            type="checkbox"
                            checked={isAntireflexiveChecked}
                            onChange={handleAntireflexiveCheckboxChange}
                            style={{ width: '20px', height: '20px', marginRight: '15px', marginLeft: '15px' }}
                        />
                        <label>Антирефлексивное отношение</label>
                    </div>
                    <div style={{ display: 'flex' }}>
                        <input
                            type="checkbox"
                            checked={isAsymmetricChecked}
                            onChange={handleAsymmetricCheckboxChange}
                            style={{ width: '20px', height: '20px', marginRight: '15px', marginLeft: '15px' }}
                        />
                        <label>Асиметрическое отношение</label>
                    </div>
                    <div style={{ display: 'flex' }}>
                        <input
                            type="checkbox"
                            checked={isAntitransitiveChecked}
                            onChange={handleAntitransitiveCheckboxChange}
                            style={{ width: '20px', height: '20px', marginRight: '15px', marginLeft: '15px' }}
                        />
                        <label>Антитранзитивное отношение</label>
                    </div>
                </div>
            </div>
            <button className="button save" onClick={handleClick}>
                Сохранить
            </button>
        </div>
    );
};
