//
// Created by flassabe on 19/11/2021.
//

#ifndef _CHECK_H
#define _CHECK_H

#include "sql.h"

bool check_query(char *buffer,int SQL_COMMAND_MAX_SIZE);

bool check_query_select(int nb_arguments,char **nom_val);
bool check_query_update(int nb_arguments,char **nom_val);
bool check_query_create(int nb_arguments,char **nom_val);
bool check_query_insert(int nb_arguments,char **nom_val);
bool check_query_delete(int nb_arguments,char **nom_val);
bool check_query_drop_table(int nb_arguments,char **nom_val);
bool check_query_drop_db(int nb_arguments,char **nom_val);

bool check_fields_list(table_record_t *fields_list, table_definition_t *table_definition);
bool check_value_types(table_record_t *fields_list, table_definition_t *table_definition);

field_definition_t *find_field_definition(char *field_name, table_definition_t *table_definition);
bool is_value_valid(field_record_t *value, field_definition_t *field_definition);
bool is_int(char *value);
bool is_float(char *value);
bool is_key(char *value);

int find_word_in_file(char *filename, char *strsearch);

#endif //_CHECK_H
