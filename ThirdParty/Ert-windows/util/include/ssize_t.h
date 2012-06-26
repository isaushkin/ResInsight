/*
   Copyright (C) 2011  Statoil ASA, Norway. 
    
   The file 'ssize_t.h' is part of ERT - Ensemble based Reservoir Tool. 
    
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

#ifndef __SSIZE_T_H__
#define __SSIZE_T_H__

#ifdef _MSC_VER
// This should of course check 32/64/...
typedef long ssize_t;
#endif

#endif
