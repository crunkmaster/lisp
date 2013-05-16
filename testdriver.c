/* a simple testdriver to see if we can check 
	 the list of variables for membership after pushing
	 some expressions to it 
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "clist_interface.h"
#include "interpreter.h"

static clist variables ;

int main( int argc, char *argv[] ) {
	
	lisp_var temp_var ;
	lisp_expression temp_expression ;
	lisp_expression new_expression ;

	char *name = "hotdog" ;

	/* create a new lisp expression with some pointless data */

	temp_expression = ( lisp_expression ) malloc( sizeof( lisp_node )) ;
	LISP_TYPE( temp_expression ) = ATOM ;
	ATOM_VALUE( temp_expression ) = new_atom( "test_value" ) ;

	/* instatiate the lisp_var struct */
	temp_var.key = name ;
	temp_var.value = temp_expression ;

	/* append the variable to the list */
	append_lisp_var( &variables, &temp_var ) ;

	/* test for membership in the new list */
	printf ("key is member test\n");
	if ( key_is_member( variables, name ) )
		printf( "this test is passing!!!\n" ) ;
	else 
		printf ("this test doesn't pass\n");

	printf ("key is not member test\n");

	if ( key_is_member( variables, "something" ) == TRUE )
		printf ("this test doesn't pass\n");
	else
		printf ("this test is passing!!!\n");

	printf ("print previously stored value\n");
	new_expression = get_value_at_key( variables, name ) ;
	
	print_expression( new_expression ) ;

	return 0 ;

}
