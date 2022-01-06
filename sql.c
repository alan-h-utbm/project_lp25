//
// Created by flassabe on 16/11/2021.
//
#include "sql.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


#define SIZE 30

bool has_reached_sql_end(const char *sql);

char *get_sep_space(char *sql) {
    int i = 0;

    while (sql[i] == ' ') { // Tant que c'est un espace on continue
        if (sql[i] == '\0') { // Si la requête est null, on renvoie null(le cas de l'erreur
            return NULL;
        } else {
            i++; // On incrémente
        }
    }
    return &sql[i];
}

char *get_sep_space_and_char(char *sql, char c) {
    int i = 0;

    while (!has_reached_sql_end(&sql[i]) &&
           sql[i] != c) { //tant que l'on n'est pas à la fin de la requette et que l'on n'a pas trouver le caractère
        i++;
    }
    if (!has_reached_sql_end(
            &sql[i])) { // si on n'est pas à la fin de la requête, alors on a trouver la caractère au ième caractère
        sql = &sql[i];
        i = 1;
        while (sql[i] == ' ') // tant qu'il y a des espaces on passe au caractère suivant
            i++;
        return &sql[i];
    } else { // sinon nous n'avons pas trouver le caractère, on renvoi NULL
        return NULL;
    }

}

char *get_keyword(char *sql, char *keyword) {
    if (sql == NULL || keyword == NULL) return NULL;

    char cara;
    char maj_req[SIZE] = {0};
    char *dup_sql = strdup(sql); // pour utiliser strtok
    char *req = NULL;

    /* récupérer le premier element de la requete*/
    req = strtok(dup_sql, " ");
    //tout mettre en majuscule
    for (int i = 0; req[i] != '\0'; i++) {
        /* si les caractères sont en minuscules, convertissez-les
           en majuscules en soustrayant 32 de leur valeur ASCII. */
        if (req[i] >= 'a' && req[i] <= 'z') {
            cara = req[i];
            cara -= 32;
            maj_req[i] = cara;
        } else if (req[i] >= 'A' && req[i] <= 'Z')
            maj_req[i] = req[i];
    }

    // comparer
    if (strcmp(maj_req, keyword) == 0) sql = &sql[strlen(keyword) + 1];
    else return NULL; //renvoyer un pointeur nul si la comparaison est fausse

    return sql;
}

char *get_field_name(char *sql, char *field_name) {
    char *dup_sql = strdup(sql); // pour utiliser strtok
    char *test = NULL;

    if (get_sep_space_and_char(sql, '\'') !=
        NULL) { // si le nom est entre côtes simples, alors les espaces sont pris en compte.

        sql = get_sep_space_and_char(sql, '\'');

        if (get_sep_space_and_char(sql, '\'') != NULL) { // on vérifie qu'il y a bien une deuxième côte simple

            test = strtok(dup_sql, "\'");

            strcpy(field_name, test);

            sql = &sql[strlen(field_name) + 1];
        } else {
            return NULL; //Erreur
        }
    } else { // Il n'y a pas de côtes simples, le nom est séparer du reste de la requête par un espace

        test = strtok(dup_sql, " ");

        strcpy(field_name, test);
        sql = get_sep_space(sql);
    }

    return sql;
}

bool has_reached_sql_end(const char *sql) {
    int i = 0;
    while (sql[i] != '\0') {
        if (sql[i] == ' ')
            i++;
        else
            return false; // ce n'est ni un espace, ni un caractère de fin de requête, on a donc pas atteind la fin de la requette
    }
    return true;
}

char *parse_fields_or_values_list(char *sql, table_record_t *result) {
    char *dup_sql = strdup(sql); // pour utiliser strtok
    char *token = NULL;
    int i = 0;
    int taille = 0;

    /* premier token */
    token = strtok(dup_sql, ",");

    /* prendre les suivants */
    while (token != NULL || get_keyword(token, "FROM") != NULL || get_keyword(token, "VALUES") != NULL) {
        strcpy(result->fields[i].column_name, token);
        //Dans le cas où l'utilisateur n'a pas rentrer de valeur,
        if (has_reached_sql_end(result->fields[i - 1].column_name)) {
            strcpy(result->fields[i - 1].column_name, "*");
        }
        token = strtok(NULL, ",");

        i++;
    }

    strcpy(result->fields[i - 1].column_name, strtok(result->fields[i - 1].column_name, " "));

    //Dans le cas où l'utilisateur n'a pas rentrer de valeur,
    if (has_reached_sql_end(result->fields[i - 1].column_name)) {
        strcpy(result->fields[i - 1].column_name, "*");
    }

    result->fields_count = i;

    for (int k = 0; k < result->fields_count; k++) {
        taille += (int) strlen(result->fields[k].column_name);
    }

    sql = &sql[taille + result->fields_count];
    return sql;
}

char *parse_create_fields_list(char *sql, table_definition_t *result) {
    // On considère qu'une ligne est constituée d'un nom de colone et de son type, on créer donc 3 variables : on récupère dabord la ligne,
    // on récupère ensuite à partir de la ligne les variables nom et type de colone que l'on affect à la structure.
    int i = 0;
    char *dup_sql = NULL, *ligne = NULL, *dup_ligne = NULL, nom_colone[100] = {0}, *type_colone = NULL;

    do { // do while car il y a au moins un champ
        dup_sql = strdup(sql); // pour utiliser strtok
        ligne = strtok(dup_sql, ",");
        get_field_name(ligne, nom_colone);
        type_colone = &ligne[strlen(nom_colone) + 1];
        if (get_sep_space_and_char(type_colone, '\'') != NULL) {
            strcpy(type_colone, get_sep_space_and_char(type_colone, '\''));
        }

        // On insère le nom et le type dans la structure
        strcpy(result->definitions[i].column_name, nom_colone);
        //On affect le type en conséquence
        if (get_keyword(type_colone, "INT"))
            result->definitions[i].column_type = TYPE_INTEGER;
        else if (get_keyword(type_colone, "TEXT"))
            result->definitions[i].column_type = TYPE_TEXT;
        else if (get_keyword(type_colone, "FLOAT"))
            result->definitions[i].column_type = TYPE_FLOAT;
        else if (get_keyword(type_colone, "PRIMARY")) {
            type_colone = &type_colone[8/**taille de PRIMARY + l'espace*/];
            if (get_keyword(type_colone, "KEY"))
                result->definitions[i].column_type = TYPE_PRIMARY_KEY;
            else
                return NULL; //erreur
        } else
            return NULL; //erreur

        //incrémentation
        if (get_sep_space_and_char(sql, ',') != NULL) {
            sql = get_sep_space_and_char(sql, ',');
        } else {
            sql = " ";
        }

        i++;
    } while (!has_reached_sql_end(sql)); // onrécupère le nom et le type tant que la requette n'est pas fini

    result->fields_count = i;
    return sql;
}

char *parse_equality(char *sql, field_record_t *equality) {
    char *dup_sql = strdup(sql); //pour utiliser strtok
    char *token = strtok(dup_sql, " "); // On considère que l'égalitée n'es pas constitué d'espaces
    int taille = (int)strlen(token);
    if (token == NULL) {
        return NULL; //erreur
    } else {
        char *dup_token = strdup(token); //pour utiliser strtok, desavantage : beaucoup de chaine de caractères = pas optimal
        //On va séparer token en deux parties : la partie avant le égale est le nom, la partie après est la valeur
        if (get_sep_space_and_char(dup_token, '=') != NULL) { // une égalitée doi comporter un égale
            char *nom = strtok(dup_token, "=");
            char *valeur = get_sep_space_and_char(token, '=');

            equality->field_type = TYPE_UNKNOWN;
            strcpy(equality->column_name, nom);
            strcpy(equality->field_value.text_value,
                   valeur); // Ne connaisant pas son type, on le considère pour l'instant comme un char
        } else {
            return NULL; //erreur
        }
    }

    return &sql[taille + 1];
}

char *parse_set_clause(char *sql, table_record_t *result) {
    char *dup_sql = strdup(sql); // pour utiliser strtok
    char *token = NULL;
    int i = 0;
    char *test = NULL;
    /* premier token */
    token = strtok(dup_sql, ","); // On sépare en fonction des virgules
    // Il y a au moins une égalitée
    parse_equality(token, &result->fields[i]);

    //incrémentation
    i++;

    while (!has_reached_sql_end(sql) && get_keyword(sql, "WHERE") ==
                                        NULL) { // La clause set s'arrête sois lorsque la requête est fini sois lorsque la clause WHERE est rencontrée
        // On passe au mot suivant, sois la nouvelle condition
        sql = parse_equality(get_sep_space_and_char(sql, ','), &result->fields[i]);
        //incrémentation
        i++;
    }
    result->fields_count = i;

    return sql;
}

char *parse_where_clause(char *sql, filter_t *filter) {
    char *dup_sql = strdup(sql); // pour utiliser strtok
    int i = 0;
    // Il y a au moins une égalitée
    sql = parse_equality(sql, &filter->values.fields[i]);
    //incrémentation
    i++;

    while (!has_reached_sql_end(sql)) { // La clause where dans notre langage sql simplifié est toujours à la fin
        // on récupère l'operateur

        if (get_keyword(sql, "OR") != NULL) {
            filter->logic_operator = OP_OR;
            sql = get_keyword(sql, "OR");
        } else if (get_keyword(sql, "AND") != NULL) {
            filter->logic_operator = OP_AND;
            sql = get_keyword(sql, "AND");
        } else {

            return NULL; //erreur
        }

        sql = parse_equality(sql, &filter->values.fields[i]);
        //incrementation
        i++;
    }
    filter->values.fields_count = i;
    return sql;
}

query_result_t *parse(char *sql, query_result_t *result) {
    //Si le caractère de fin de chaine ';' n'existe pas, il y a erreur, sinon on récupère tout ce qu'il y a avant ce caractère.
    if(get_sep_space_and_char(sql, ';') == NULL){
        return NULL;
    }else{
        strtok(sql, ";");
    }

    /*Appelle de la fonction spécialisée en fonction du type de requete*/
    if (get_keyword(sql, "SELECT") != NULL)
        parse_select(get_keyword(sql, "SELECT"), result);
    if (get_keyword(sql, "UPDATE") != NULL)
        parse_update(get_keyword(sql, "UPDATE"), result);
    if (get_keyword(sql, "DELETE") != NULL) {
        sql = get_keyword(sql, "DELETE");
        if (sql != NULL) {
            if (get_keyword(sql, "FROM") != NULL)parse_delete(get_keyword(sql, "FROM"), result);
        }
    }

    if (get_keyword(sql, "DROP") != NULL) { // Drop ne peut pas être seul, il est sois suivit de table sois de DB
        sql = get_keyword(sql, "DROP");
        if (sql != NULL) {
            if (get_keyword(sql, "TABLE") != NULL) parse_drop_table(get_keyword(sql, "TABLE"), result);
            if (get_keyword(sql, "DATABASE") != NULL) parse_drop_db(get_keyword(sql, "DATABASE"), result);
        }
    }
    if (get_keyword(sql, "CREATE") != NULL) {
        sql = get_keyword(sql, "CREATE");
        if (sql != NULL) {
            if (get_keyword(sql, "TABLE") != NULL) parse_create(get_keyword(sql, "TABLE"), result);
        }
    }


    if (get_keyword(sql, "INSERT") != NULL) {
        sql = get_keyword(sql, "INSERT");
        if (sql != NULL) {
            if (get_keyword(sql, "INTO") != NULL) parse_insert(get_keyword(sql, "INTO"), result);
        }
    }
    return NULL;
}

query_result_t *parse_select(char *sql, query_result_t *result) {
    // On informe que la requette est de type SELECT
    result->query_type = QUERY_SELECT;

    //On récupère les champs demandés
    sql = parse_fields_or_values_list(sql, &result->query_content.select_query.set_clause);
    sql = get_keyword(sql, "FROM");

    //on copie le premier mot après "from" dans la partie table_name de result
    char *dup_sql = strdup(sql); // pour utiliser strtok
    const char *nom = strtok(dup_sql, " ");
    strcpy(result->query_content.table_name, nom);
    strcpy(result->query_content.select_query.table_name, nom);

    // Sql pointe sur le caractère après le nom de la table + l'espace
    sql = &sql[strlen(nom) +1];

    if(!has_reached_sql_end(sql)){
        sql = get_keyword(sql, "WHERE");
        parse_where_clause(sql, &result->query_content.select_query.where_clause);
    }

    return NULL;
}

query_result_t *parse_create(char *sql, query_result_t *result) {
    char *dup_sql = strdup(sql); // pour utiliser strtok

    // On informe que la requette est de type CREATE_TABLE
    result->query_type = QUERY_CREATE_TABLE;

    //on copie le premier mot après "create table" dans la partie table_name de result
    const char *nom = strtok(dup_sql, " ");
    strcpy(result->query_content.table_name, nom);
    strcpy(result->query_content.create_query.table_name, nom);
    sql = &sql[strlen(nom) + 1];

    //On récupère les valeurs à créer dans la nouvelle table qui est entre parenthèses.
    sql = get_sep_space_and_char(sql, '(');
    strtok(sql, ")");

    parse_create_fields_list(sql, &result->query_content.create_query.table_definition);
    return NULL;
}

query_result_t *parse_insert(char *sql, query_result_t *result) {
    char *dup_sql = strdup(sql); // pour utiliser strtok
    // On séparera la partie sql contenant les champs de celle contenant les valeurs pour ensuite utiliser la fonction parse_fields_or_values_list
    char *fields = NULL;
    char *values = NULL;

    // On informe que la requette est de type INSERT
    result->query_type = QUERY_INSERT;

    //on copie le premier mot après "INSERT INTO" dans la partie table_name de result
    const char *nom = strtok(dup_sql, " ");
    strcpy(result->query_content.table_name, nom);
    strcpy(result->query_content.insert_query.table_name, nom);

    sql = &sql[strlen(nom) + 1];

    // On séparera la partie sql contenant les champs de celle contenant les valeurs (compris entre parenthèses)
    fields = strtok(sql, ")");
    values = strtok(NULL, ")");
    fields = get_sep_space_and_char(fields, '(');
    values = get_sep_space_and_char(values, '(');
    //pour ensuite utiliser la fonction parse_fields_or_values_list
    parse_fields_or_values_list(fields, &result->query_content.insert_query.fields_names);
    parse_fields_or_values_list(values, &result->query_content.insert_query.fields_values);
    return NULL;
}

query_result_t *parse_update(char *sql, query_result_t *result) {
    char *dup_sql = strdup(sql); // pour utiliser strtok

    // On informe que la requête est de type UPDATE
    result->query_type = QUERY_UPDATE;

    //on copie le premier mot après "UPDATE" dans la partie table_name de result
    const char *nom = strtok(dup_sql, " ");
    strcpy(result->query_content.table_name, nom);
    strcpy(result->query_content.update_query.table_name, nom);

    sql = &sql[strlen(nom) + 1];

    // On recupère SET et envoi ce qui suit dans la fonction dédier
    sql = get_keyword(sql, "SET");
    sql = parse_set_clause(sql, &result->query_content.update_query.set_clause);

    // Si il y a une clause WHERE on appelle la fonction dédier
    if (!has_reached_sql_end(sql)) {
        if (get_keyword(sql, "WHERE") != NULL) {
            sql = get_keyword(sql, "WHERE");
            parse_where_clause(sql, &result->query_content.select_query.where_clause);
        }
    }
    return NULL;
}

query_result_t *parse_delete(char *sql, query_result_t *result) {
    char *dup_sql = strdup(sql); // pour utiliser strtok
    // On informe que la requette est de type DELETE
    result->query_type = QUERY_DELETE;

    //on copie le premier mot après "DELETE FROM" dans la partie table_name de result
    const char *nom = strtok(dup_sql, " ");
    strcpy(result->query_content.table_name, nom);
    strcpy(result->query_content.delete_query.table_name, nom);

    sql = &sql[strlen(nom) + 1];
    if (!has_reached_sql_end(sql)) {
        if (get_keyword(sql, "WHERE") != NULL) {
            sql = get_keyword(sql, "WHERE");
            parse_where_clause(sql, &result->query_content.delete_query.where_clause);
        }
    }
    return NULL;
}

query_result_t *parse_drop_db(char *sql, query_result_t *result) {
    char *dup_sql = strdup(sql); // pour utiliser strtok

    // On informe que la requette est de type DROP_DB
    result->query_type = QUERY_DROP_DB;

    //on copie le premier mot après "DROP DATABASE" dans la partie database_name de result
    const char *nom = strtok(dup_sql, " ");

    strcpy(result->query_content.database_name, nom);
    return NULL;
}

query_result_t *parse_drop_table(char *sql, query_result_t *result) {
    char *dup_sql = strdup(sql); // pour utiliser strtok

    // On informe que la requette est de type DROP_TABLE
    result->query_type = QUERY_DROP_TABLE;

    //on copie le premier mot après "DROP TABLE" dans la partie table_name de result
    const char *nom = strtok(dup_sql, " ");
    strcpy(result->query_content.table_name, nom);
    return NULL;
}
