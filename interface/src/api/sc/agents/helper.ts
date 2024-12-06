import { ScAddr, ScTemplate, ScType } from 'ts-sc-client';
import { client } from '@api/sc/client';
import { ScConstruction, ScLinkContent, ScLinkContentType, ScEventParams, ScEventType } from 'ts-sc-client';
import { makeAgent } from '@api/sc/agents/makeAgent';
import { createLinkText } from './newMessageAgent';
import React from "react";

const nrelSystemIdentifier = 'nrel_system_identifier';
const question = 'question';
const actionCreateMessageClassAndPhraseTemplate = 'action_create_message_class_and_phrase_template';
const actionCreateClassInstance = 'action_create_class_instance';
const actionCreateClass = 'action_create_class';
const actionCreateRelation = 'action_create_relation';
const rrel1 = 'rrel_1';
const rrel2 = 'rrel_2';
const conceptTextFile = 'concept_text_file';
const langRu = 'lang_ru';
const message = '_message';


let data : string[] | undefined;
let Chips : string[];
let relation : {entity:string; relation:string;}[] = [];
let answers : string[];



const baseKeynodes = [
    { id: nrelSystemIdentifier, type: ScType.NodeConstNoRole},
    { id: question, type: ScType.NodeConstClass },
    { id: actionCreateMessageClassAndPhraseTemplate, type: ScType.NodeConstClass },
    { id: actionCreateClassInstance, type: ScType.NodeConstClass },
    { id: actionCreateClass, type: ScType.NodeConstClass },
    { id: actionCreateRelation, type: ScType.NodeConstClass },
    { id: rrel1, type: ScType.NodeConstRole },
    { id: rrel2, type: ScType.NodeConstRole },
    { id: conceptTextFile, type: ScType.NodeConstClass },
    { id: langRu, type: ScType.NodeConstClass },
    { id: message, type: ScType.NodeVar},
];
//Функции для вызова агентов по нажатию кнопки
export const handleSave = async (
    phraseSystemIdentifierRef: React.RefObject<HTMLInputElement>,
    phraseRussianIdentifierRef: React.RefObject<HTMLInputElement>,
    form : string,
    chipsValues: string[]
) => {
        const inputValues = {
            phraseSystemIdentifier: phraseSystemIdentifierRef.current?.value || '',
            phraseRussianIdentifier: phraseRussianIdentifierRef.current?.value || '',
            };
        data = Object.values(inputValues);
        answers = Object.values(chipsValues);
        const record : string[] = form.split('\n');

        data = [...data, ...record];
        const result : string =  "done";

        const resultLinkAddr = await createLinkText(result);
        
        if (resultLinkAddr !== null) {
            await createAgent(resultLinkAddr, actionCreateMessageClassAndPhraseTemplate);
        }
};

export const handleSaveClassInstance = async (
    classInstanceSystemIdentifierRef: React.RefObject<HTMLInputElement>,
    classInstanceRussianIdentifierRef: React.RefObject<HTMLInputElement>,
    classInstanceNoteRef: React.RefObject<HTMLInputElement>,
    classInstanceClass: string,
    
) => {
        const inputValues = {
            classInstanceSystemIdentifier: classInstanceSystemIdentifierRef.current?.value || '',
            classInstanceRussianIdentifier: classInstanceRussianIdentifierRef.current?.value || '',
            classInstanceNote: classInstanceNoteRef.current?.value || '',
            classInstanceClass: classInstanceClass
            };
        //console.log(inputValues);
        data = Object.values(inputValues);
        const result : string = "done";//Object.values(inputValues).join('\n');
        const resultLinkAddr = await createLinkText(result);
        if (resultLinkAddr !== null) {
            await createAgent(resultLinkAddr, actionCreateClassInstance);
        }
};

export const handleSaveClassInstanceWithRelations = async (
    firstForm: string[],
    form: {entity:string; relation:string;}[]
) => {
        const result : string = firstForm.join('\n') + "\n" + form.map(item => `${item.entity} - ${item.relation}`).join(', ');
        const resultLinkAddr = await createLinkText(result);
        data = Object.values(firstForm);
        relation = form;
        //console.log(relation);
        if (resultLinkAddr !== null) {
            await createAgent(resultLinkAddr, actionCreateClassInstance);
        }
};

export const handleRelationInstance = async (
    relationSystemIdentifierRef: React.RefObject<HTMLInputElement>,
    relationRussianIdentifierRef: React.RefObject<HTMLInputElement>,
    relationNoteRef: React.RefObject<HTMLInputElement>,
    relationFirstDomain: string,
    relationSecondDomain: string,
    isBinary: boolean,
    isOriented: boolean,
    isAntireflexive: boolean,
    isAsymmetric: boolean,
    isAntitransitive: boolean,
) => {
        const inputValues = {
            SystemIdentifier: relationSystemIdentifierRef.current?.value || '',
            RussianIdentifier: relationRussianIdentifierRef.current?.value || '',
            Note: relationNoteRef.current?.value || '',
            FirstDomain: relationFirstDomain,
            SecondDomain: relationSecondDomain
            };
        const relationClassificationList = getRelationClassificationList(isBinary, isOriented, isAntireflexive, isAsymmetric, isAntitransitive);
        data = Object.values(inputValues);
        Chips = relationClassificationList;
        console.log(data);
        const result : string = "done";//Object.values(inputValues).join('\n') + '\n' + relationClassificationList.join(', ');
        const resultLinkAddr = await createLinkText(result);
        if (resultLinkAddr !== null) {
            await createAgent(resultLinkAddr, actionCreateRelation);
        }
};

export const handleSaveToCreateClass = async (
    classSystemIdentifierRef: React.RefObject<HTMLInputElement>,
    classRussianIdentifierRef: React.RefObject<HTMLInputElement>,
    classNoteRef: React.RefObject<HTMLInputElement>,
    classSuperClass: string,
    chipsValues: string[]
) => {
        const inputValues = {
            phraseSystemIdentifier: classSystemIdentifierRef.current?.value || '',
            phraseRussianIdentifier: classRussianIdentifierRef.current?.value || '',
            classNote: classNoteRef.current?.value || '',
            classSuperClass: classSuperClass,
            };

        data = Object.values(inputValues);

        Chips = chipsValues;

        const result : string = "done";//string = Object.values(inputValues).join('\n') + '\n' + phrases;

        const resultLinkAddr = await createLinkText(result);
        
        if (resultLinkAddr !== null) {
            await createAgent(resultLinkAddr, actionCreateClass);
        }
};

export const userClose = async () => {
    const result : string = "User close";

    const resultLinkAddr = await createLinkText(result);
    data = undefined;
    if (resultLinkAddr !== null) {
        await createAgent(resultLinkAddr, actionCreateMessageClassAndPhraseTemplate);
    }    
}
//Функция для открытия компонентов
export const createPopupCheck = async (
    setCreatePopup,
    conceptName: string
)  => {

    const baseKeynodes = [
        { id: conceptName, type: ScType.NodeConstClass},
    ];

    const keynodes = await client.resolveKeynodes(baseKeynodes);
    const eventParams = new ScEventParams(keynodes[conceptName], ScEventType.AddOutgoingEdge, () => {setCreatePopup(true)});
    await client.eventsCreate([eventParams])
}

//Функции добавления агента в программу обработки
const describeAgent = async (
    keynodes: Record<string, ScAddr>,
    linkAddr: ScAddr,
    action: string
) => {
    const actionNodeAlias = '_action_node';
    console.log(actionNodeAlias)

    const template = new ScTemplate();

    template.triple(keynodes[question], ScType.EdgeAccessVarPosPerm, [ScType.NodeVar, actionNodeAlias]);
    template.triple(keynodes[action], ScType.EdgeAccessVarPosPerm, actionNodeAlias);

    template.tripleWithRelation(
        actionNodeAlias,
        ScType.EdgeAccessVarPosPerm,
        keynodes[message],
        ScType.EdgeAccessVarPosPerm,
        keynodes[rrel1],
    );
    template.tripleWithRelation(
        actionNodeAlias,
        ScType.EdgeAccessVarPosPerm,
        linkAddr,
        ScType.EdgeAccessVarPosPerm,
        keynodes[rrel2],
    );
    template.triple(keynodes[conceptTextFile], ScType.EdgeAccessVarPosPerm, linkAddr);
    template.triple(keynodes[langRu], ScType.EdgeAccessVarPosPerm, linkAddr);
    
    console.log(data);
    if(data !== undefined){
    if(action == "action_create_class") {   
        const keys = [
            {id: data[0], type: ScType.NodeVar},
            {id: data[3], type: ScType.NodeVar},
            {id: "rrel_system_idtf", type: ScType.NodeConstRole},
            {id: "rrel_ru", type: ScType.NodeConstRole},
            {id: "rrel_note", type: ScType.NodeConstRole},
            {id: "rrel_super_class", type: ScType.NodeConstRole},
            {id: "rrel_decomposition", type: ScType.NodeConstRole}
        ]
    
        const res = await client.resolveKeynodes(keys);
        if(data[1] == "") data[1] = "error_no_data";
        if(data[2] == "") data[2] = "error_no_data";
        const link1 =  await createLinkText(data[1]);
        const link2 = await createLinkText(data[2]);
    
        template.tripleWithRelation(
            actionNodeAlias,
            ScType.EdgeAccessVarPosPerm,
            res[data[0]],
            ScType.EdgeAccessVarPosPerm,
            res["rrel_system_idtf"],
        );
        
        if(link1 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link1,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_ru"],
            );
        }
        if(link2 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link2,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_note"],
            );
        }
        template.tripleWithRelation(
            actionNodeAlias,
            ScType.EdgeAccessVarPosPerm,
            res[data[3]],
            ScType.EdgeAccessVarPosPerm,
            res["rrel_super_class"],
        );
    
    
        for(const value of Chips){
            const key_chips = [
                {id : value, type : ScType.NodeVar}
            ];
    
            const res_chips = await client.resolveKeynodes(key_chips);
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                res_chips[value],
                ScType.EdgeAccessVarPosPerm,
                res["rrel_decomposition"],
            );
        }
    }

    if(action == "action_create_relation"){
        const keys = [
            {id: "rrel_system_idtf", type: ScType.NodeConstRole},
            {id: "rrel_ru", type: ScType.NodeConstRole},
            {id: "rrel_note", type: ScType.NodeConstRole},
            {id: "rrel_first_domain", type: ScType.NodeConstRole},
            {id: "rrel_second_domain", type: ScType.NodeConstRole},
            {id: "rrel_include", type: ScType.NodeConstRole},
            {id: data[0], type: ScType.NodeVar},
            {id: data[3], type: ScType.NodeVar},
            {id: data[4], type: ScType.NodeVar}
        ]

        const res = await client.resolveKeynodes(keys);

        if(data[1] == "") data[1] = "error_no_data";
        if(data[2] == "") data[2] = "error_no_data";
        const link1 =  await createLinkText(data[1]);
        const link2 = await createLinkText(data[2]);

        template.tripleWithRelation(
            actionNodeAlias,
            ScType.EdgeAccessVarPosPerm,
            res[data[0]],
            ScType.EdgeAccessVarPosPerm,
            res["rrel_system_idtf"],
        );

        if(link1 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link1,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_ru"],
            );
        }

        if(link2 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link2,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_note"],
            );
        }

        template.tripleWithRelation(
            actionNodeAlias,
            ScType.EdgeAccessVarPosPerm,
            res[data[3]],
            ScType.EdgeAccessVarPosPerm,
            res["rrel_first_domain"],
        );

        template.tripleWithRelation(
            actionNodeAlias,
            ScType.EdgeAccessVarPosPerm,
            res[data[4]],
            ScType.EdgeAccessVarPosPerm,
            res["rrel_second_domain"],
        );

        for(const value of Chips){
            const key = [
                {id: value, type: ScType.NodeVar}
            ]
            const ch = await client.resolveKeynodes(key);
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                ch[value],
                ScType.EdgeAccessVarPosPerm,
                res["rrel_include"],
            );
            
        }
    }

    if(action == "action_create_class_instance"){
        const keys = [
            {id: "rrel_system_idtf", type: ScType.NodeConstRole},
            {id: "rrel_ru", type: ScType.NodeConstRole},
            {id: "rrel_note", type: ScType.NodeConstRole},
            {id: "rrel_class", type: ScType.NodeConstRole},
            {id: "rrel_relation", type: ScType.NodeConstRole},
            {id: "rrel_concept_relation", type: ScType.NodeConstRole},
            {id: "rrel_nrel_relation", type: ScType.NodeConstRole},
            {id: data[0], type: ScType.NodeVar},
            {id: data[3], type: ScType.NodeVar}
        ]
        const res = await client.resolveKeynodes(keys);

        if(data[1] == "") data[1] = "error_no_data";
        if(data[2] == "") data[2] = "error_no_data";
        const link1 =  await createLinkText(data[1]);
        const link2 = await createLinkText(data[2]);

        
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                res[data[0]],
                ScType.EdgeAccessVarPosPerm,
                res["rrel_system_idtf"],
            );
        
        if(link1 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link1,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_ru"],
            );
        }

        if(link2 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link2,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_note"],
            );
        }

        template.tripleWithRelation(
            actionNodeAlias,
            ScType.EdgeAccessVarPosPerm,
            res[data[3]],
            ScType.EdgeAccessVarPosPerm,
            res["rrel_class"],
        );

        console.log(relation);

        if (!relation || Object.keys(relation).length === 0) {
            relation.push({entity: 'none', relation: 'none'});
        }
        for(let value of relation){
            console.log(relation);
            if(value.entity == 'none' && value.relation == 'none') break;
            const node = "node";
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                [ScType.NodeVar, node],
                ScType.EdgeAccessVarPosPerm,
                res["rrel_relation"],
            );

            const keys_v = [
                {id: value.entity, type: ScType.NodeVar},
                {id: value.relation, type: ScType.NodeVar}
            ];

            const res_v = await client.resolveKeynodes(keys_v);

            template.tripleWithRelation(
                node,
                ScType.EdgeAccessVarPosPerm,
                res_v[value.entity],
                ScType.EdgeAccessVarPosPerm,
                res["rrel_concept_relation"],
            )

            template.tripleWithRelation(
                node,
                ScType.EdgeAccessVarPosPerm,
                res_v[value.relation],
                ScType.EdgeAccessVarPosPerm,
                res["rrel_nrel_relation"],
            )

        }
    }

    if(action == "action_create_message_class_and_phrase_template"){
        const keys = [
            {id: data[0], type: ScType.NodeVar},
            {id: data[2], type: ScType.NodeVar},
            {id: "rrel_phrase", type: ScType.NodeConstRole},
            {id: "rrel_phrase_ru", type: ScType.NodeConstRole},
            {id: "rrel_answer", type: ScType.NodeConstRole},
            {id: "rrel_answer_ru", type: ScType.NodeConstRole},
            {id: "rrel_wit", type: ScType.NodeConstRole},
            {id: "rrel_answers", type: ScType.NodeConstRole}
        ];

        const res = await client.resolveKeynodes(keys);

        const link1 = await createLinkText(data[1]);
        const link2 = await createLinkText(data[3]);
        if(data[4] == "") data[4] = "error_no_data";
        const link3 = await createLinkText(data[4]);

        template.tripleWithRelation(
            actionNodeAlias,
            ScType.EdgeAccessVarPosPerm,
            res[data[0]],
            ScType.EdgeAccessVarPosPerm,
            res["rrel_phrase"]
        );

        if(link1 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link1,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_phrase_ru"]
            );
        }
        
        template.tripleWithRelation(
            actionNodeAlias,
            ScType.EdgeAccessVarPosPerm,
            res[data[2]],
            ScType.EdgeAccessVarPosPerm,
            res["rrel_answer"]
        );

        if(link2 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link2,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_answer_ru"]
            );
        }

        if(link3 !== null){
            template.tripleWithRelation(
                actionNodeAlias,
                ScType.EdgeAccessVarPosPerm,
                link3,
                ScType.EdgeAccessVarPosPerm,
                res["rrel_wit"]
            );
        }

        if(answers.length == 0) answers.push("none");

        for(let value of answers){
            const link = await createLinkText(value);
            if(link !== null){
                template.tripleWithRelation(
                    actionNodeAlias,
                    ScType.EdgeAccessVarPosPerm,
                    link,
                    ScType.EdgeAccessVarPosPerm,
                    res["rrel_answers"]
                );
            }
        }
    }
}
    return [template, actionNodeAlias] as const;
};

const createAgent = async (linkAddr: ScAddr, action: string) => {
    const keynodes = await client.resolveKeynodes(baseKeynodes);
    const [template, userActionNodeAlias] = await describeAgent(keynodes, linkAddr, action);
    await makeAgent(template, userActionNodeAlias);
}

//Вспомагательные функции
export const getRelationClassificationList = (binary: boolean, oriented: boolean, antireflexive: boolean, asymmetric: boolean, antitransitive: boolean) => {
    const classificationList : string[] = [];

    if (binary)
    { classificationList.push("binary_relation")}

    if (oriented)
    { classificationList.push("oriented_relation")}

    if (antireflexive)
    { classificationList.push("antireflexive_relation")}

    if (asymmetric)
    { classificationList.push("asymmetric_relation")}

    if (antitransitive)
    { classificationList.push("antitransitive_relation")}

    return classificationList
}

export const findAnyInKb = async (setList: (options: { label: string; value: string }[]) => void, param: string): Promise<void> => {
    const list = await client.getLinksContentsByContentSubstrings([param]);
    const options = list[0]
      .filter((word) => word.startsWith(param))
      .map((word, index) => ({
        label: word,
        value: (index + 1).toString(),
      }));
    setList(options);
  };

  export const findRelationsInKb = async (setList: (options: { label: string; value: string }[]) => void): Promise<void> => {
    const list = await client.getLinksContentsByContentSubstrings(["nrel_", "rrel_"]);
    const options = list[0]
      .map((word, index) => ({
        label: word,
        value: (index + 1).toString(),
      }));
    setList(options);
  };

  const setSystemIdtf = async (addr: ScAddr, systemIdtf: string) => {
    const keynodes = await client.resolveKeynodes(baseKeynodes);

    const template = new ScTemplate();
    const linkAlias = "_link";

    const sysIdtfLinkConstruction = new ScConstruction();
    sysIdtfLinkConstruction.createLink(
        ScType.LinkConst,
        new ScLinkContent(systemIdtf, ScLinkContentType.String)
    );
    const [sysIdtfLinkNode] = await client.createElements(sysIdtfLinkConstruction);

    template.tripleWithRelation(
      addr,
      ScType.EdgeDCommonVar,
      sysIdtfLinkNode,
      ScType.EdgeAccessVarPosPerm,
      keynodes[nrelSystemIdentifier]
    );
    const result = await client.templateGenerate(template, {});
};

  
