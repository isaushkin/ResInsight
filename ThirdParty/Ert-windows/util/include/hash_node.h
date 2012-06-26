/*
   Copyright (C) 2011  Statoil ASA, Norway. 
    
   The file 'hash_node.h' is part of ERT - Ensemble based Reservoir Tool. 
    
   ERT is free software: you can redistribute it and/or modify 
   it under the terms of the GNU General Public License as published by 
   the Free Software Foundation, either version 3 of the License, or 
   (at your option) any later version. 
    
   ERT is distributed in the hope that it will be useful, but WITHOUT ANY 
   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
   FITNESS FOR A PARTICULAR PURPOSE.   
    
   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
   for more details. 
*/

#ifndef __HASH_NODE_H__
#define __HASH_NODE_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <inttypes.h>

#include <node_data.h>

typedef struct hash_node_struct hash_node_type;
typedef uint32_t      (hashf_type) (const char *key, size_t len);
typedef enum          {hash_ref_data , hash_int_data , hash_double_data , hash_string_data} hash_data_type;


bool             hash_node_key_eq(const hash_node_type * , uint32_t  , const char *);
hash_node_type * hash_node_get_next(const hash_node_type * );
uint32_t         hash_node_get_insert_nr(const hash_node_type * );
void             hash_node_set_next(hash_node_type * , const hash_node_type * );
hash_node_type * hash_node_alloc_new(const char *, node_data_type * , hashf_type *, uint32_t);
void             hash_node_set_insert_nr(hash_node_type *, uint32_t );
uint32_t         hash_node_get_table_index(const hash_node_type * );
uint32_t         hash_node_get_global_index(const hash_node_type * );
const char *     hash_node_get_key(const hash_node_type * );
node_data_type * hash_node_get_data(const hash_node_type *); 
void             hash_node_free(hash_node_type *);
uint32_t         hash_node_set_table_index(hash_node_type *, uint32_t );
#ifdef __cplusplus
}
#endif
#endif
