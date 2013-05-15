#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "clist_interface.h"
#include "clist.h"
#include "globals.h"

bool key_is_member( clist L, char *key ) {
	
	clist temp = NULL ;
	bool rc = FALSE ;

	lisp_var *temp_var ;

	if ( empty_circ_list( L ) == TRUE )
		return ERROR ;

	while ( ( temp = circ_list_iterator( L, temp )) != NULL ) {
		
		temp_var = (lisp_var *)DATA( temp ) ;

		if ( strcmp( temp_var -> key, key ) == 0 )
			rc = TRUE ;
	}
		
	return rc ;
	
}

status get_value_at_key( clist L, char *key, lisp_expression *value ) {
	
	clist temp = NULL ;
	lisp_var *temp_var ;
	lisp_expression temp_value ;

	if ( key_is_member( L, key ) == FALSE )
		return ERROR ;

	while ( ( temp = circ_list_iterator( L, temp )) != NULL ) {
		temp_var = (lisp_var *)DATA( temp ) ;
		
		if ( strcmp( temp_var -> key, key ) == 0 ) {
			temp_value = temp_var -> value ;
			*value = temp_value ;
			return OK ;
		}
		else
			return ERROR ;

	}

	return OK ;

}

status append_lisp_var( clist *p_L, lisp_var *var ) {
	
	lisp_var *temp = malloc( sizeof( lisp_var ) ) ;
	
	if ( !temp ) return ERROR ;

	*temp = *var ;

	circ_append( p_L, (generic_ptr)temp ) ;
	return OK ;
	
}
