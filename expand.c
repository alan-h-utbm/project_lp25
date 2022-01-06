//
// Created by flassabe on 23/11/2021.
//

#include "expand.h"

#include <stdio.h>

void expand(query_result_t *query) {

    if (query->query_type == QUERY_SELECT) {
        strcpy(query->query_content.select_query.table_name, make_full_path(query->query_content.database_name, query->query_content.table_name));
        expand_select(&query->query_content.select_query);
    } else if (query->query_type == QUERY_INSERT) {
        strcpy(query->query_content.insert_query.table_name, make_full_path(query->query_content.database_name, query->query_content.table_name));
        expand_insert(&query->query_content.insert_query);
    }
}

void expand_select(update_or_select_query_t *query) {

     FILE *index_fichier = fopen(query->table_name, "r");


    char recup_index[TEXT_LENGTH + 2];


    if (query->set_clause.fields_count == 1 && query->set_clause.fields[0].column_name[0] == "*") {

        //on etudie condition
        //a completer
        //

        int parcours = 0;
        int parcours_copie = 0;

        while (fgets(recup_index, 152, index_fichier) != NULL) {

//on fait la recuperation du titre ici parce que c'est plus simple

            switch (recup_index[0]) {


                //recuperation du type
                case TYPE_TEXT:
                    query->set_clause.fields[parcours].field_type = TYPE_TEXT;
                    break;
                case TYPE_FLOAT:
                    query->set_clause.fields[parcours].field_type = TYPE_FLOAT;
                    break;
                case TYPE_INTEGER:
                    query->set_clause.fields[parcours].field_type == TYPE_INTEGER;
                    break;
                case TYPE_PRIMARY_KEY:
                    query->set_clause.fields[parcours].field_type = TYPE_PRIMARY_KEY;
                    break;
            }


            //recuperation du titre
            while (recup_index[parcours_copie + 2] != '\n' || recup_index[parcours_copie + 2] != '\0') {

                query->set_clause.fields[parcours].column_name[parcours_copie] = recup_index[parcours_copie + 2];
                parcours_copie++;
            }
            query->set_clause.fields[parcours].column_name[parcours_copie] = '\0';


            parcours++;
        }

        fclose(index_fichier);



    }
}
void expand_insert(insert_query_t *query) {
    char check[]="*";

//recupere les nom et le types de données
    get_table_definition(&query->fields_names, &query->fields_values, query->table_name);


    for (int i = 0; i < 1 ; ++i) {
        //si on a pas donné de valeurs pour le champ
        if (query->fields_values.fields[i].column_name[0] == '*'){
            if (is_field_in_record(&query->fields_names, query->fields_names.fields[i].column_name)){

                make_default_value(&query->fields_values.fields[i], query->table_name);

            } else{
                printf("erreur, le champ : %s , n'est pas compris dans la table : %s\n",query->fields_names.fields[i].column_name, query->table_name);
            }
        }
    }

    printf("valeur apres fonction :: %lld\n", query->fields_values.fields[0].field_value.int_value);

}

bool is_field_in_record(table_record_t *record, char *field_name) {

    //on parcours la definition de la table, si c'est bon on retourne
    for (int i = 0; i < record->fields_count; ++i) {

        if (strcmp(field_name, record->fields[i].field_value.text_value ) == 0){
            printf("%s est bien compris dans la table\n", field_name);
            return true;
        }
    }

    printf("%s n'est pas compris dans la table\n", field_name);

    return false;
}
void make_default_value(field_record_t *field, char *table_name) {

    switch (field->field_type) {
        case TYPE_INTEGER:
            field->field_value.int_value = 0;

            printf("case integer\n");
            break;
        case TYPE_TEXT:
            strcpy(field->field_value.text_value, "");
            printf("case type text\n");
            break;

        case TYPE_FLOAT:
            field->field_value.float_value = 0.0;
            printf("case type float\n");
            break;
        case TYPE_PRIMARY_KEY:
            //a completer
            //field->field_value.primary_key_value = get_next_key(table_name);
            printf("type primary key\n");
            break;
        case TYPE_UNKNOWN:
            printf("erreur TYPE_UNKNOWN\n");
            break;
    }
}

void get_table_definition(table_record_t *definition, table_record_t *type, char *path){

//on obtient le chemin complet
    char inedx_path[] = ".idx";
    strcat(path, inedx_path);

    //variable permettant d'enlever le caractere \n
    int longeur;

    FILE *index_fichier = fopen(inedx_path, "r");
    //si jamais fichier n'existe pas ou erreur
    if (index_fichier == NULL){
        perror("erreur");
    }


    char recup_nom[TEXT_LENGTH];
    char recup_type[TEXT_LENGTH];
    int valeur_type;
    int i=0;

    fseek(index_fichier, 2, SEEK_CUR);
    while (fgets(recup_nom, TEXT_LENGTH, index_fichier)!=0){


        strcpy(definition->fields[i].field_value.text_value, recup_nom);
        longeur = strlen(recup_nom) ;
        definition->fields[i].field_value.text_value[longeur -1]='\0';


//on cherche juste à recupérer le titre, on saute donc 2 caracteres
        fseek(index_fichier, 2, SEEK_CUR);
        i++;
    }
//on revient au début du fichier
    rewind(index_fichier);

    i=0;

    while (fgets(recup_type, TEXT_LENGTH, index_fichier)!=NULL){
        valeur_type= recup_type[0]-48;
        // printf("valeur type :: %d\n", valeur_type);
        type->fields[i].field_type=valeur_type;

        i++;
    }


    fclose(index_fichier);

}