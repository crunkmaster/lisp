/* a simple testdriver to see if we can check 
	 the list of variables for membership after pushing
	 some expressions to it 
*/


#include <stdio.h>
#include <string.h>
#include "clist_interface.h"
#include "interpreter.h"

static clist variables ;

int main( int argc, char *argv[] ) {
	
	lisp_var temp_var ;
	lisp_expression temp_expression ;
	char *name = "hotdog" ;

	/* instatiate the lisp_var struct */
	temp_var.key = name ;
	temp_var.value = temp_expression ;
	
	/* append the variable to the list */
	append_lisp_var( &variables, &temp_var ) ;

	/* test for membership in the new list */
	if ( key_is_member( variables, name ) )
		printf( "this test is passing!!!\n" ) ;
	else 
		printf( "you are doing this wrong...\n" ) ;

	return 0 ;

}
