#include "clist.h"
#include "interpreter.h"
#include "globals.h"

#ifndef _clistinterface
#define _clistinterface

bool key_is_member( clist L, char *key ) ;
lisp_expression get_value_at_key( clist L, char *key ) ;
status set_value_at_key( clist *p_L, char *key, lisp_expression new_value ) ;
status append_lisp_var( clist *p_L, lisp_var *var ) ;

#endif
