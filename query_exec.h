
#ifndef _QUERY_EXEC_H
#define _QUERY_EXEC_H

#include "sql.h"
#include "utils.h"

void execute(query_result_t *query, char *name, char *path);

void execute_create(create_query_t *query, char *path);
void execute_insert(insert_query_t *query), char *path;
void execute_select(update_or_select_query_t *query);
void execute_update(update_or_select_query_t *query);
void execute_delete(delete_query_t *query);
void execute_drop_table(char *table_name, char path);
void execute_drop_database(char *path);
