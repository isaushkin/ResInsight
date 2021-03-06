/*
   Copyright (C) 2012  Statoil ASA, Norway. 
    
   The file 'well_branch.h' is part of ERT - Ensemble based Reservoir Tool. 
    
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

#ifndef __WELL_BRANCH_H__
#define __WELL_BRANCH_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <well_conn.h>

  typedef struct well_branch_struct well_branch_type;

  well_branch_type       *  well_branch_alloc(int branch_nr);
  void                      well_branch_free( well_branch_type * branch );
  void                      well_branch_add_conn( well_branch_type * branch , well_conn_type * connection );
  int                       well_branch_get_length( const well_branch_type * branch );
  const well_conn_type   ** well_branch_get_connections( const well_branch_type * branch );
  int                       well_branch_get_nr( const well_branch_type * branch );

#ifdef __cplusplus
}
#endif
#endif

