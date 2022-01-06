//
// Created by flassabe on 19/11/2021.
//

#include "check.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "table.h"



/*
 * @brief function check_query is the high level check function, which will call specialized check functions and
 * return their result.
 * @param buffer chaine de caractère donné par l'utilisateur (@see main.c)
 * @param SQL_COMMAND_MAX_SIZE taille maximale du buffer (@see main.c)
 * @return true if the query is valid, false else
 */
bool check_query(char *buffer,int SQL_COMMAND_MAX_SIZE) {

    //Variable booléene de retour de fonction
    bool check_fonction = false;

    //Conversion du buffer en buffer minuscule (buffer_m)
    char buffer_m[SQL_COMMAND_MAX_SIZE];
    for (int i = 0; i < strlen(buffer); ++i) {
        buffer_m[i]= tolower(buffer[i]);
    }

    //Séparation de chaque mot dans un tableau de chaine de caractère nom_val à l'aide de la fonction strok
    char **nom_val;
    nom_val = (char ** )malloc( SQL_COMMAND_MAX_SIZE* sizeof (char));
    int nb_arguments = 0;
    char *p = strtok(buffer, " ");
    while(p != NULL)
    {
        nom_val[nb_arguments] = p;
        p = strtok(NULL, " ");
        nb_arguments++;
    }
    //Suppression du ";" final
    int temp_str = 0;
    for (int i = 0; i < strlen(nom_val[nb_arguments-1]); ++i) {
        if (!isalnum(nom_val[nb_arguments-1][i])) {
            if (nom_val[nb_arguments-1][i] == '=' || nom_val[nb_arguments-1][i] == '/') {
                i--;
            }
            i++;
        }
        nom_val[nb_arguments-1][temp_str] = nom_val[nb_arguments-1][i];
        temp_str++;
    }
    nom_val[nb_arguments-1][temp_str] = '\0';


    //Appel des fonction check_querry si les première lettres du buffer en minuscules correspondent au bon type de query
    if (strncmp(buffer_m,"select",6) == 0){ //querry = select
        check_fonction = check_query_select(nb_arguments,nom_val);
    }
    else if (strncmp(buffer_m,"insert",6) == 0) { //querry = insert
        check_fonction = check_query_insert(nb_arguments,nom_val);
    }
    else if (strncmp(buffer_m,"create",6) == 0) { //querry = create
        check_fonction = check_query_create(nb_arguments,nom_val);
    }
    else if (strncmp(buffer_m,"update",6) == 0) { //querry = update
        check_fonction = check_query_update(nb_arguments,nom_val);
    }
    else if (strncmp(buffer_m,"delete",6) == 0) { //querry = delete
        check_fonction = check_query_delete(nb_arguments,nom_val);
    }
    else if (strncmp(buffer_m,"drop table",10) == 0) { //querry = drop table
        check_fonction = check_query_drop_table(nb_arguments,nom_val);
    }
    else if (strncmp(buffer_m,"drop database",13) == 0) { //querry = drop database
        check_fonction = check_query_drop_table(nb_arguments,nom_val);
    }
    else printf("Unsupported query code\n");

    free(nom_val);
    
    return check_fonction;
}

/*
 * @brief check_query_select checks a select query
 * @param nb_arguments entier nombre d'arguments extrait du buffer (@see check_query)
 * @param nom_val tableau de charactère a deux dimensions correspondant au mots extrait du buffer (@see check_query)
 * @return true if valid, false if invalid
 */
bool check_query_select(int nb_arguments,char **nom_val) {

    //Variable booléenne de retour de la fonction
    bool check = 1;

    //Variable répertoire courant dnné par la fonction getcwd
    char *repertoire_courant;
    repertoire_courant = getcwd(NULL,0);

    //Vérification existence Table et Field après "from"
    for (int i = 0; i < nb_arguments; ++i) {

        if (strcmp(nom_val[i], "from") == 0 || strcmp(nom_val[i], "FROM") == 0) { //identication de from

            if (i+1 < nb_arguments) { // Vérification existence de variable après from

                //Création de la chaine de caractère name_table = chemin complet de la table
                char *repertoire_table = repertoire_courant;
                char repertoire_table_imcomplet[strlen(nom_val[i+1])];
                strcpy(repertoire_table_imcomplet,nom_val[i + 1]);
                strcat(strcat(strcpy(repertoire_table, repertoire_courant), "/"), repertoire_table_imcomplet);

                //Est ce que le répertoire table existe
                if (!access(repertoire_table, F_OK)) {
                } else {
                    check = 0;
                    break;
                }

                //Création de la chaine de caractère deffinissant le chemin du fichier "table".def 
                char *file_def = NULL;
                int table_taille = 0;
                //Extraction du nom de la table
                for (int j = 1; j < strlen(nom_val[i+1]); ++j)
                {
                    if (nom_val[i+1][strlen(nom_val[i+1])-j] == '/') break;
                    table_taille++;
                }
                char name_table[table_taille];
                for (int j = 1; j < table_taille + 1; j++)
                {
                    name_table[table_taille-j] = nom_val[i+1][strlen(nom_val[i+1])-j];
                }
                name_table[table_taille] = '\0';
                //Concaténation pour le chemin complet de la table ("database/table/table.def")
                file_def =strcat(repertoire_table_imcomplet,"/");
                file_def =strcat(file_def,name_table);
                file_def =strcat(file_def,".def");

                //Vérification existence fields avant from
                for (int j = 1; j < nb_arguments; ++j) {
                    if (strcmp(nom_val[j], "from") == 0 || strcmp(nom_val[j], "FROM") == 0) break; //Detection fields jusqu'à "from"

                    //Pas de vérification pour * = l'ensemble des champs des enregistrements
                    if (strcmp(nom_val[j], "*") == 0){
                        break;
                    }


                    //Est ce que les fields existent dans le fichier de definition de la table
                    if (find_word_in_file(file_def, nom_val[j]) == 1) {
                    } else {
                        check = 0;
                        printf("Un ou plusieurs des champs n'existent pas\n");
                        break;
                    }

                    if (check == 0) break;
                }

                //Test Where
                for (int l = 0; l < nb_arguments; ++l) {


                    if (strcmp(nom_val[l], "where") == 0 || strcmp(nom_val[l], "WHERE") == 0) { //identication de where

                        if (l + 1 <= nb_arguments) { // Vérification existence de variables après WHERE
                            int nb_arg_supp_where = 1;

                            while(l + nb_arg_supp_where <= nb_arguments){ //Vérification existence éléments après where autre que connecteur logique AND et OR
                                
                                //Est ce que les fields existent dans le fichier de definition de la table
                                int temp=0;
                                //Detection du symbole "=" qui fait la séparation entre field et value
                                for (int k=0; k < strlen(nom_val[l+nb_arg_supp_where]); ++k)
                                {
                                    if(nom_val[l+nb_arg_supp_where][k] == '='){
                                        break;
                                    }
                                    temp++;
                                }
                                //Detection de champs jusqu'au symboble =
                                char field_where[temp];
                                for (int m = 0; m < temp; ++m)
                                {
                                    field_where[m] = nom_val[l+nb_arg_supp_where][m];
                                }
                                field_where[temp] = '\0';
                                //Detection de value jusqu'au symboble =
                                char value_where[strlen(nom_val[l+nb_arg_supp_where])-temp+1];
                                for (int n = temp+1; n < strlen(nom_val[l+nb_arg_supp_where]); ++n)
                                {
                                    value_where[n-temp-1] = nom_val[l+nb_arg_supp_where][n];
                                }
                                value_where[strlen(nom_val[l+nb_arg_supp_where])-temp-1] = '\0';

                                //Est ce que les fields existent dans le fichier de definition de la table
                                if (find_word_in_file(file_def, field_where) == 0) {
                                    check = 0;
                                    printf("Un ou plusieurs des champs n'existent pas apres 'where'\n");
                                    break;
                                }
                                nb_arg_supp_where = nb_arg_supp_where + 2;
                            }

                            if (check == 0) break;
                        }
                    }
                }

            } else {
                printf("Veuillez saisir le nom de la table sous la forme 'database/table' après 'from'\n");
            }

        }
    }
    return check;
}

/*
 * @brief check_query_update checks an update query
 * @param nb_arguments entier nombre d'arguments extrait du buffer (@see check_query)
 * @param nom_val tableau de charactère a deux dimensions correspondant au mots extrait du buffer (@see check_query)
 * @return true if valid, false if invalid
 */
bool check_query_update(int nb_arguments,char **nom_val) {
    
    //Variable booléenne de retour de la fonction
    bool check = 1;

    //Variable répertoire courant
    char *repertoire_courant;
    repertoire_courant = getcwd(NULL,0);

    //Création de la chaine de caractère name_table = chemin complet de la table
    char *repertoire_table = repertoire_courant;
    char repertoire_table_imcomplet[strlen(nom_val[1])];
    strcpy(repertoire_table_imcomplet,nom_val[1]);
    strcat(strcat(strcpy(repertoire_table, repertoire_courant), "/"), repertoire_table_imcomplet);

    //Est ce que le répertoire table existe
    if (!access(repertoire_table, F_OK)) {
    } else {
        check = 0;
    }

    //Création de la chaine de caractère entière du fichier de definition (database/table/table.def)
    char *file_def = NULL;
    int table_taille = 0;
    for (int j = 1; j < strlen(nom_val[1]); ++j) {
        if (nom_val[1][strlen(nom_val[1]) - j] == '/') break;
        table_taille++;
    }
    char name_table[table_taille];
    for (int j = 1; j < table_taille + 1; j++) {
        name_table[table_taille - j] = nom_val[1][strlen(nom_val[1]) - j];
    }
    name_table[table_taille] = '\0';
    file_def = strcat(repertoire_table_imcomplet, "/");
    file_def = strcat(file_def, name_table);
    file_def = strcat(file_def, ".def");
    char file_def_2[strlen(file_def)];
    strcpy(file_def_2,file_def);

    //Test Where
    for (int l = 0; l < nb_arguments; ++l) {
        if (strcmp(nom_val[l], "where") == 0 || strcmp(nom_val[l], "WHERE") == 0) { //identication de where

            //Test set
            int nb_arg_supp_set = 1;
            for (int i = 3; i < l; ++i) {
                if (strcmp(nom_val[2], "set") == 0 || strcmp(nom_val[2], "SET") == 0) { //identication de set

                    if (3 <= nb_arguments) { // Vérification existence de variables après set

                        int temp = 0;
                        for (int k = 0; k < strlen(nom_val[2 + nb_arg_supp_set]); ++k) {
                            if (nom_val[2 + nb_arg_supp_set][k] == '=') {
                                break;
                            }
                            temp++;
                        }
                        char field_set[temp];
                        for (int m = 0; m < temp; ++m) {
                            field_set[m] = nom_val[2 + nb_arg_supp_set][m];
                        }
                        field_set[temp] = '\0';
                        char value_set[strlen(nom_val[2 + nb_arg_supp_set]) - temp + 1];
                        for (int n = temp + 1; n < strlen(nom_val[2 + nb_arg_supp_set]); ++n) {
                            value_set[n - temp - 1] = nom_val[2 + nb_arg_supp_set][n];
                        }
                        value_set[strlen(nom_val[2 + nb_arg_supp_set]) - temp - 1] = '\0';

                        if (find_word_in_file(file_def_2, field_set) == 1) {
                        } else {
                            check = 0;
                            break;
                        }
                        nb_arg_supp_set = nb_arg_supp_set + 1;


                        if (check == 0) break;

                    }
                }
            }

            if (l + 1 <= nb_arguments) { // Vérification existence de variables après WHERE
                int nb_arg_supp_where = 1;

                while (l + nb_arg_supp_where <= nb_arguments) {
                    //Est ce que les fields existent dans le fichier de definition de la table

                    int temp2 = 0;
                    for (int k = 0; k < strlen(nom_val[l + nb_arg_supp_where]); ++k) {
                        if (nom_val[l + nb_arg_supp_where][k] == '=') {
                            break;
                        }
                        temp2++;
                    }
                    char field_where[temp2];
                    for (int m = 0; m < temp2; ++m) {
                        field_where[m] = nom_val[l + nb_arg_supp_where][m];
                    }
                    field_where[temp2] = '\0';

                    char value_where[strlen(nom_val[l + nb_arg_supp_where]) - temp2 + 1];
                    for (int n = temp2 + 1; n < strlen(nom_val[l + nb_arg_supp_where]); ++n) {
                        value_where[n - temp2 - 1] = nom_val[l + nb_arg_supp_where][n];
                    }
                    value_where[strlen(nom_val[l + nb_arg_supp_where]) - temp2 - 1] = '\0';

                    if (find_word_in_file(file_def_2, field_where) == 1) {
                    } else {
                        check = 0;
                        printf("Un ou plusieurs des champs n'existent pas apres 'where' \n");
                        break;
                    }
                    nb_arg_supp_where = nb_arg_supp_where + 2;
                }

                if (check == 0) break;
            }
        }
    }

    return check;
}

/*
 * @brief check_query_create checks a create query. It checks if the table doesn't already exist
 * @param nb_arguments entier nombre d'arguments extrait du buffer (@see check_query)
 * @param nom_val tableau de charactère a deux dimensions correspondant au mots extrait du buffer (@see check_query)
 * @return true if valid, false if invalid
 */
bool check_query_create(int nb_arguments,char **nom_val) {
    
    //Variable booléenne de retour de la fonction
    bool check = 1;

    //Variable répertoire courant
    char *repertoire_courant;
    repertoire_courant = getcwd(NULL,0);
    if (nb_arguments > 2){
        //Création de la chaine de caractère name_table = chemin complet de la table
        char *repertoire_table = repertoire_courant;
        char repertoire_table_imcomplet[strlen(nom_val[2])];
        strcpy(repertoire_table_imcomplet,nom_val[2]);
        strcat(strcat(strcpy(repertoire_table, repertoire_courant), "/"), repertoire_table_imcomplet);

        //Est ce que le répertoire table existe
        if (!access(repertoire_table, F_OK)) {
            check = 0;
        }
    }
    return check;
}

/*
 * @brief check_query_insert checks an insert query
 * @param nb_arguments entier nombre d'arguments extrait du buffer (@see check_query)
 * @param nom_val tableau de charactère a deux dimensions correspondant au mots extrait du buffer (@see check_query)
 * @return true if valid, false if invalid
 */
bool check_query_insert(int nb_arguments,char **nom_val) {

    //Variable booléenne de retour de la fonction
    bool check = 1;

    //Variable répertoire courant
    char *repertoire_courant;
    repertoire_courant = getcwd(NULL,0);

    //Création de la chaine de caractère name_table = chemin complet de la table
    char *repertoire_table = repertoire_courant;
    char repertoire_table_imcomplet[strlen(nom_val[2])];
    strcpy(repertoire_table_imcomplet,nom_val[2]);
    strcat(strcat(strcpy(repertoire_table, repertoire_courant), "/"), repertoire_table_imcomplet);

    //Est ce que le répertoire table existe
    if (!access(repertoire_table, F_OK)) {
    } else {
        check = 0;
    }

    if (nb_arguments > 3) {
        for (int i = 3; i < nb_arguments; ++i) {
            if (strcmp(nom_val[i], "set") == 0 || strcmp(nom_val[i], "SET") == 0) {
                if (nb_arguments - i - 1 == i - 3) {
                    //Création de la chaine de caractère entière du fichier de definition (database/table/table.def)
                    char *file_def = NULL;
                    int table_taille = 0;
                    for (int j = 1; j < strlen(nom_val[2]); ++j) {
                        if (nom_val[2][strlen(nom_val[2]) - j] == '/') break;
                        table_taille++;
                    }
                    char name_table[table_taille];
                    for (int j = 1; j < table_taille + 1; j++) {
                        name_table[table_taille - j] = nom_val[2][strlen(nom_val[2]) - j];
                    }
                    name_table[table_taille] = '\0';
                    file_def = strcat(repertoire_table_imcomplet, "/");
                    file_def = strcat(file_def, name_table);
                    file_def = strcat(file_def, ".def");

                    //Test field existent
                    int temp = 0;
                    for (int j = 3; j < i; ++j) {
                        //transformation en chaine de caractère uniquement alphanumérique
                        char nom_val_isalnum[strlen(nom_val[j])];
                        int temp2 = 0;
                        for (int k = 0; k < strlen(nom_val[j]); ++k) {
                            if (!isalnum(nom_val[j][k])) {
                                k = k + 1;
                            }
                            nom_val_isalnum[temp2] = nom_val[j][k];
                            temp2++;
                        }
                        nom_val_isalnum[temp2] = '\0';

                        //Est ce que la chaine de caractère field aphanumérique est contenue dans le fichier de definition
                        temp = find_word_in_file(file_def, nom_val_isalnum);
                        if (temp == 0) {
                            check = 0;
                            printf("Un ou plusieurs des champs n'existent pas \n");
                            break;
                        }
                    }
                    if (temp == 1) {
                        check = 1;
                        break;
                    }
                }
            }
            check = 0;
        }
    }

    return check;
}

/*
 * @brief check_query_delete checks a delete query
 * @param nb_arguments entier nombre d'arguments extrait du buffer (@see check_query)
 * @param nom_val tableau de charactère a deux dimensions correspondant au mots extrait du buffer (@see check_query)
 * @return true if valid, false if invalid
 */
bool check_query_delete(int nb_arguments,char **nom_val) {
    
    //Variable booléenne de retour de la fonction
    bool check = 1;

    //Variable répertoire courant
    char *repertoire_courant;
    repertoire_courant = getcwd(NULL,0);
    if (nb_arguments > 2){
        //Création de la chaine de caractère name_table = chemin complet de la table
        char *repertoire_table = repertoire_courant;
        char repertoire_table_imcomplet[strlen(nom_val[2])];
        strcpy(repertoire_table_imcomplet,nom_val[2]);
        strcat(strcat(strcpy(repertoire_table, repertoire_courant), "/"), repertoire_table_imcomplet);

        //Est ce que le répertoire table existe
        if (access(repertoire_table, F_OK)) {
            check = 0;
        } else {
        }

        //Création de la chaine de caractère entière du fichier de definition (database/table/table.def)
        char *file_def = NULL;
        int table_taille = 0;
        for (int j = 1; j < strlen(nom_val[2]); ++j) {
            if (nom_val[2][strlen(nom_val[2]) - j] == '/') break;
            table_taille++;
        }
        char name_table[table_taille];
        for (int j = 1; j < table_taille + 1; j++) {
            name_table[table_taille - j] = nom_val[2][strlen(nom_val[2]) - j];
        }
        name_table[table_taille] = '\0';
        file_def = strcat(repertoire_table_imcomplet, "/");
        file_def = strcat(file_def, name_table);
        file_def = strcat(file_def, ".def");

        //Test Where
        for (int l = 0; l < nb_arguments; ++l) {


            if (strcmp(nom_val[l], "where") == 0 || strcmp(nom_val[l], "WHERE") == 0) { //identication de from

                if (l + 1 <= nb_arguments) { // Vérification existence de variables après WHERE
                    int nb_arg_supp_where = 1;

                    while(l + nb_arg_supp_where <= nb_arguments){
                        //Est ce que les fields existent dans le fichier de definition de la table

                        int temp=0;
                        for (int k=0; k < strlen(nom_val[l+nb_arg_supp_where]); ++k)
                        {
                            if(nom_val[l+nb_arg_supp_where][k] == '='){
                                break;
                            }
                            temp++;
                        }
                        char field_where[temp];
                        for (int m = 0; m < temp; ++m)
                        {
                            field_where[m] = nom_val[l+nb_arg_supp_where][m];
                        }
                        field_where[temp] = '\0';
                        char value_where[strlen(nom_val[l+nb_arg_supp_where])-temp+1];
                        for (int n = temp+1; n < strlen(nom_val[l+nb_arg_supp_where]); ++n)
                        {
                            value_where[n-temp-1] = nom_val[l+nb_arg_supp_where][n];
                        }
                        value_where[strlen(nom_val[l+nb_arg_supp_where])-temp-1] = '\0';

                        if (find_word_in_file(file_def, field_where) == 1) {
                        } else {
                            check = 0;
                            break;
                        }
                        nb_arg_supp_where = nb_arg_supp_where + 2;
                    }

                    if (check == 0) break;
                }
            }
        }
    }
    return check;
}

/*
 * @brief check_query_drop_table checks a drop table query: checks that the table exists
 * @param nb_arguments entier nombre d'arguments extrait du buffer (@see check_query)
 * @param nom_val tableau de charactère a deux dimensions correspondant au mots extrait du buffer (@see check_query)
 * @return true if valid, false if invalid
 */
bool check_query_drop_table(int nb_arguments,char **nom_val) {
    
    //Variable booléenne de retour de la fonction
    bool check = 1;

    //Variable répertoire courant
    char *repertoire_courant;
    repertoire_courant = getcwd(NULL,0);
    if (nb_arguments > 2){
        //Création de la chaine de caractère name_table = chemin complet de la table
        char *repertoire_table = repertoire_courant;
        char repertoire_table_imcomplet[strlen(nom_val[2])];
        strcpy(repertoire_table_imcomplet,nom_val[2]);
        strcat(strcat(strcpy(repertoire_table, repertoire_courant), "/"), repertoire_table_imcomplet);

        //Est ce que le répertoire table existe
        if (access(repertoire_table, F_OK)) {
            check = 0;
        } else {
        }
    }
    return check;}

/*
 * @brief check_query_drop_db checks a drop database query: checks that the database exists
 * @param nb_arguments entier nombre d'arguments extrait du buffer (@see check_query)
 * @param nom_val tableau de charactère a deux dimensions correspondant au mots extrait du buffer (@see check_query)
 * @return true if valid, false if invalid
 */
bool check_query_drop_db(int nb_arguments,char **nom_val) {
    
    //Variable booléenne de retour de la fonction
    bool check = 1;

    //Variable répertoire courant
    char *repertoire_courant;
    repertoire_courant = getcwd(NULL,0);
    if (nb_arguments > 2){
        //Création de la chaine de caractère name_table = chemin complet de la table
        char *repertoire_table = repertoire_courant;
        char repertoire_table_imcomplet[strlen(nom_val[2])];
        strcpy(repertoire_table_imcomplet,nom_val[2]);
        strcat(strcat(strcpy(repertoire_table, repertoire_courant), "/"), repertoire_table_imcomplet);

        //Est ce que le répertoire table existe
        if (access(repertoire_table, F_OK)) {
            check = 0;
        } else {
        }
    }
    return check;}

/*
 * @brief function check_fields_list checks if all fields from a fields list exist in a table definition
 * @param nb_arguments entier nombre d'arguments extrait du buffer (@see check_query)
 * @param nom_val tableau de charactère a deux dimensions correspondant au mots extrait du buffer (@see check_query)
 * @return true if all fields belong to table, false else
 */
bool check_fields_list(table_record_t *fields_list, table_definition_t *table_definition) {
    return false;
}

/*
 * @brief function check_value_types checks if all fields in a record list belong to a table (through its definition)
 * AND if the corresponding value can be converted to the field type as specified in the table definition.
 * Uses find_field_definition and is_value_valid
 * @see find_field_definition
 * @see is_value_valid
 * @param fields_list list of the fields and their values (as text)
 * @param table_definition table definition
 * @return true if all fields belong to table and their value types are correct, false else
 */
bool check_value_types(table_record_t *fields_list, table_definition_t *table_definition) {
    return false;
}

/*
 * @brief function find_field_definition looks up for a field name in a table definition.
 * @param field_name the field name to lookup for.
 * @param table_definition the table definition in which to search.
 * @return a pointer to the field definition structure if the field name exists, NULL if it doesn't.
 */
field_definition_t *find_field_definition(char *field_name, table_definition_t *table_definition) {
    return NULL;
}

/*
 * @brief function is_value_valid checks if a field_record_t holding a field name and a text value matches a field definition:
 * it checks fields names, and tests if the text value can be converted into the table defined type.
 * /!\ If it succeeded, this function must modify the value in the field_record_t structure by converting and setting
 * the value to the proper type!
 * Uses is_int, is_float, is_key
 * @see is_int
 * @see is_float
 * @see is_key
 * @param value the field and value record
 * @param field_definition the field definition to test against
 * @return true if valid (and converted), false if invalid
 */
bool is_value_valid(field_record_t *value, field_definition_t *field_definition) {
    return false;
}

/*
 * @brief function is_int tests if the string value is a text representation of an integer value.
 * You may use strtoll for this test.
 * @param value the text representation to test
 * @return true if value can be converted into an integer, false if it cannot
 */
bool is_int(char *value) {

    for (int i = 0; i < strlen(value)-1; ++i)
    {
        if (isdigit(value[i]) == 0)
        {
            return false;
            break;
        }
    }
    return true;
}

/*
 * @brief function is_float tests if the string value is a text representation of a double value.
 * You may use strtod for this test.
 * @param value the text representation to test
 * @return true if value can be converted into a double, false if it cannot
 */
bool is_float(char *value) {
    for (int i = 0; i < strlen(value)-1; ++i)
    {
        if (isdigit(value[i]) == 0 || value[i] == ',')
        {
            return false;
            break;
        }
    }
    return true;
}

/*
 * @brief function is_int tests if the string value is a text representation of a key value.
 * You may use strtoull for this test.
 * @param value the text representation to test
 * @return true if value can be converted into a key, false if it cannot
 */
bool is_key(char *value) {
    if (strtoull(value, NULL, 10) == '0ULL')
    {
        return false;
    }
    return true;
}

/*
 * @brief function find_word_in_file tests if the string strsearch is in the text file filename.
 * @param *filename the file representation for test
 * @param *strsearch the text representation to test
 * @return true if the string is find in the text file
 */
int find_word_in_file(char *filename, char *strsearch) {

    FILE *f;
    char c;
    int found=0;
    int pos=0;
    unsigned long length;

    f = fopen(filename,"r");
    if( f == NULL )
        return 0;

    length=strlen(strsearch);

    while( fread(&c,1,1,f)>0 && !found ) {

        if( c==strsearch[pos] ) {
            pos++;
        } else {
            if(pos!=0) {
                // On doit rembobiner !
                fseek(f,-pos,SEEK_CUR);
                pos=0;
            }
        }

        found = (pos==length);

    }
    fclose(f);

    return found;

}