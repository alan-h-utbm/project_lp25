
#include "query_exec.h"

#include <dirent.h>
#include <unistd.h>

#include "database.h"
#include "table.h"
#include "utils.h"


void execute(query_result_t *query, char *name, char *path) {  // ( +récuperation de name et path à partir de project.c)
    create_db_directory(query->query_content.database_name, path);  // permet également de concatener path et name sur path
    switch (query->query_type) {
        case QUERY_CREATE_TABLE:
            execute_create(&query->query_content.create_query, char path);
            break;
        case QUERY_INSERT:
            execute_insert(&query->query_content.insert_query, char path);
            break;
        case QUERY_DROP_DB:
            execute_drop_database(char path); // à ce stade "path" contien le chemin jusqu'à BDD (comprise)
            break;
        case QUERY_DROP_TABLE:
            execute_drop_table(query->query_content.table_name, char path)
            break;
        case QUERY_SELECT:
            break;
        case QUERY_UPDATE:
            break;
        case QUERY_DELETE:
            break;
        case QUERY_NONE:
            break;
    }
}

void execute_create(create_query_t *query, char *path) {
    create_table(query->query_content.create_query, path);
}

void execute_insert(insert_query_t *query, char *path) {

    if ( query->fields_names.fields_count <= 16 ) {  // s il y a 16 champs ou moins :
        for ( int i=0; i < query->fields_values.fields_count; i++ ) {  // pour chaque champ, faire :
            int buffer_binaire[], active_number, ligne=0;
            DIR *my_table = opendir(path + query->table_name);
            FILE* fichier = NULL;
            fichier = fopen(path + table_definition->table_name + ".idx", "r+");
            fscanf(fichier, "%d", &active_number);  // on récupere la 1ère valeur d'active
            fclose(fichier);

            if ( *active_number != NULL ) {  // si 1ère valeur existe

                switch (query->fields_values.fields[i].field_type) {  // on récupere le type de champ
                    case TYPE_INTEGER:  // c'est un INTEGER, alors : On l'écrit et on complète avec des '0'

                        fichier = fopen(path + table_definition->table_name + ".data", "a+");  // on ouvre le fichier data lié à la table
                        fprintf(fichier, "%ld", Reverse(decimalToBinary(query->fields_values.fields[i].field_value.int_value)));  // on affiche l INT convertis en binaire et inversé dans le fichier data
                        for ( int j=0; j < 64; j++ ) {  // à partir du 1er caractère d'écriture,
                                                        // pour les 8 premiers octets (INT) on complète avec des 0 si besoin
                            int nbr;
                            fscanf(fichier, "%1d", &nbr);
                            if ( nbr != 0 && nbr != 1 ) {  // si le caractère est différent de 0 et 1, on ajoute 0.
                                fprintf(fichier, "0");
                        }

                        fclose(fichier);

                        break;
                    case TYPE_FLOAT:
                        fichier = fopen(path + table_definition->table_name + ".data", "a+");  // on ouvre le fichier data lié à la table
                            fprintf(fichier, "%ld", Reverse(floatToBinary(query->fields_values.fields[i].field_value.int_value)));  // on affiche l INT convertis en binaire et inversé dans le fichier data
                            for ( int j=0; j < 64; j++ ) {  // à partir du 1er caractère d'écriture,
                                // pour les 8 premiers octets (INT) on complète avec des 0 si besoin
                                int nbr;
                                fscanf(fichier, "%1d", &nbr);
                                if ( nbr != 0 && nbr != 1 ) {  // si le caractère est différent de 0 et 1, on ajoute 0.
                                    fprintf(fichier, "0");
                                }

                                fclose(fichier);
                        break;
                    case TYPE_TEXT:

                        break;
                    case TYPE_PRIMARY_KEY:

                        break;
                    case TYPE_UNKNOWN:

                        break;
                }
            }
            else if ( *active_number == NULL ) {  // si 1ère valeur n existe pas

            }
        }
    }
    else {  // si plus de 16 champs saisis, message d'erreur
        printf("Plus de 16 champs ont été saisis. Veuillez réessayer.\n");
    }

}

void execute_select(update_or_select_query_t *query) {}

void execute_update(update_or_select_query_t *query) {}

void execute_delete(delete_query_t *query) {
}

void execute_drop_table(char *table_name, char *path) {
    make_full_path(path, table_name);
    drop_table(path);
}

void execute_drop_database(char *path) {
    recursive_rmdir(path);
}
