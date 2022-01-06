#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>

#include "utils.h"
#include "database.h"
#include "sql.h"
#include "table.h"
#include "check.h"

#define SQL_COMMAND_MAX_SIZE 1500

int main(int argc, char *argv[]) {

     // Verifier les paramètres
    int option_index = 0;
    char *name = NULL;
    char *path = NULL;
    while((option_index = getopt(argc, argv, "d:l:")) != -1){
        switch (option_index) {
            case 'd':
                name = optarg;
                break;
            case 'l':
                path = optarg;
                break;
            default:
                return 1;
        }
    }

  char buffer[SQL_COMMAND_MAX_SIZE];
    do {
        printf(">");
        fflush(stdin);
        if (fgets(buffer, SQL_COMMAND_MAX_SIZE, stdin) == NULL)
            continue;
        buffer[strlen(buffer)-1] = '\0';
        if (strcmp(buffer, "exit") == 0)
            break;
        query_result_t *result = malloc(sizeof(query_result_t));
        // on copie le nom de la base de donnée dans la structure resulta :

        parse(buffer, result);
        
        if (check_query(buffer, SQL_COMMAND_MAX_SIZE) == false){
            printf("Unsupported query code \n");
        }

        //Je test ici si ce que j'ai fait fonctionne
        if(result->query_type != 0){
            printf("query_result_t : type = %u, nom_table = %s, bdd = %s\n", result->query_type, result->query_content.table_name, result->query_content.database_name);
        }else{
            printf("ERROR\n");
        }

        //Create
        if(result->query_type == 1){
            printf("Create : nom_table = %s, nombre de champs : %d\n", result->query_content.create_query.table_name, result->query_content.create_query.table_definition.fields_count);
            for(int i = 0; i < result->query_content.create_query.table_definition.fields_count; i++){
                printf("nom = %s, type = %u\n", result->query_content.create_query.table_definition.definitions[i].column_name, result->query_content.create_query.table_definition.definitions[i].column_type);
            }
        }

        //Drop table
        if(result->query_type == 2){
            printf("drop table.\n");
        }

        //Select
        if(result->query_type == 3){
            printf("select : nom_table = %s, nb.valeurs demandés = %d\n", result->query_content.select_query.table_name, result->query_content.select_query.set_clause.fields_count);
            for(int j=0; j < result->query_content.select_query.set_clause.fields_count; j++){
                printf("%s\n", result->query_content.select_query.set_clause.fields[j].column_name);
            }
            printf("nb de clause where : %d\n, opérateur : %u\n", result->query_content.select_query.where_clause.values.fields_count, result->query_content.select_query.where_clause.logic_operator);
            for(int l=0; l < result->query_content.select_query.where_clause.values.fields_count; l++){
                printf("nom = %s, veleur = %s\n", result->query_content.select_query.where_clause.values.fields[l].column_name, result->query_content.select_query.where_clause.values.fields[l].field_value.text_value);
            }
        }

        //update
        if(result->query_type == 4){
            printf("update : nom_table = %s\n, nb champs : %d\n", result->query_content.update_query.table_name, result->query_content.update_query.set_clause.fields_count);
            for(int h=0; h < result->query_content.update_query.set_clause.fields_count; h++){
                printf("nom = %s, valeur = %s\n", result->query_content.update_query.set_clause.fields[h].column_name, result->query_content.update_query.set_clause.fields[h].field_value.text_value);
            }
            printf("nb de clause where : %d\n, opérateur : %u\n", result->query_content.update_query.where_clause.values.fields_count, result->query_content.update_query.where_clause.logic_operator);
            for(int l=0; l < result->query_content.update_query.where_clause.values.fields_count; l++){
                printf("nom = %s, veleur = %s\n", result->query_content.update_query.where_clause.values.fields[l].column_name, result->query_content.update_query.where_clause.values.fields[l].field_value.text_value);
            }
        }

        //delete
        if(result->query_type == 5){
            printf("delete :nom_table = %s\n", result->query_content.delete_query.table_name);
            printf("nb de clause where : %d\n, opérateur : %u\n", result->query_content.delete_query.where_clause.values.fields_count, result->query_content.delete_query.where_clause.logic_operator);
            for(int l=0; l < result->query_content.delete_query.where_clause.values.fields_count; l++){
                printf("nom = %s, veleur = %s\n", result->query_content.delete_query.where_clause.values.fields[l].column_name, result->query_content.delete_query.where_clause.values.fields[l].field_value.text_value);
            }
        }

        //insert
        if(result->query_type == 6){
            printf("insert : nom_table = %s\n", result->query_content.insert_query.table_name);
            printf("nom = %s\n, nb valeurs : %d %d\n", result->query_content.insert_query.table_name, result->query_content.insert_query.fields_names.fields_count, result->query_content.insert_query.fields_values.fields_count);
            for(int p=0; p < result->query_content.insert_query.fields_names.fields_count; p++){
                printf("nom = %s valeur = %s\n", result->query_content.insert_query.fields_names.fields[p].column_name, result->query_content.insert_query.fields_values.fields[p].column_name);
            }
        }

        //Drop db
        if(result->query_type == 7){
            printf("drop db.\n");
        }

        execute(result, namen, path);

        free(result);
    } while (true);
    return 0;
