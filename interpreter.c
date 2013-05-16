#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lex.h"
#include "interpreter.h"
#include "clist.h"
#include "clist_interface.h"
#include "globals.h"

#define MAXBUILTIN 10

static clist variables ;

lisp_expression new_expression() {
  return ( lisp_expression ) malloc( sizeof( lisp_node )) ;
}

lisp_atom new_atom( char *s ) {

  char *dest = malloc( sizeof(char) * (strlen(s) + 1 )) ;

  if ( dest )
    strcpy( dest, s ) ;
  return dest ;

}

int equal_atom( lisp_atom atom1, lisp_atom atom2 ) {

  return strcmp( atom1, atom2 ) == 0 ;

}

bool equal_lisp_list( clist list1, clist list2 ) {

  static clist temp1 = NULL ;
  static clist temp2 = NULL ;

  while ( 1 ) {

    temp1 = circ_list_iterator( list1, temp1 ) ;
    temp2 = circ_list_iterator( list2, temp2 ) ;

    /* try to check if lists are the same length */
    if( (temp1 != NULL) && (temp2 == NULL ) ) { return FALSE ; }
    if( (temp1 == NULL) && (temp2 != NULL ) ) { return FALSE ; }

    /* if we've hit the end of the list, then we're done! */
    if ( (temp1 == NULL) && (temp2 == NULL) ) return TRUE ;

    /* if the two lists contain different types, then we quit */
    if ( LISP_TYPE( DATA(temp1) ) != LISP_TYPE( DATA(temp2) )) {
      return FALSE ;
    }

    if ( LISP_TYPE( temp1 ) == ATOM ) {
      if ( equal_atom( ATOM_VALUE( temp1 ), ATOM_VALUE( temp2 )) == FALSE ) {
        return FALSE ;
      }
      else
        return equal_lisp_list( temp1, temp2 ) ;
    }
    if ( LISP_TYPE( temp1 ) == LIST ) return equal_lisp_list( temp1, temp2 ) ;
  }
}

int read_expression( lisp_expression *p_expression ) {

  clist expr_list ;
  lisp_expression expression, inner_expression ;
  tokendata *p_token, *p_next_token ;
  lisp_atom value ;
  status rc ;
  int readerror ;

  p_token = gettoken() ;
  if ( p_token -> tokentype == EOF_T )
    return E_EOF ;

  if (( expression = new_expression()) == NULL )
    return E_SPACE ;

  switch ( p_token -> tokentype ) {

  case RIGHTPAREN_T:
    return E_SYNTAX ;
  case EOF_T:
    return EOF_T ;
  case STRING_T:
    if (( value = new_atom( p_token -> tokenvalue )) == NULL )
      return E_SPACE ;
    LISP_TYPE( expression ) = ATOM ;
    ATOM_VALUE( expression ) = value ;
    break ;
  case LEFTPAREN_T :
    p_next_token = lookahead() ;
    if ( p_next_token -> tokentype == EOF_T )
      return E_EOF ;
    if ( p_next_token -> tokentype == RIGHTPAREN_T ) {

      if (( value = new_atom( NIL_VALUE )) == NULL )
        return E_SPACE ;
      LISP_TYPE( expression ) = ATOM ;
      ATOM_VALUE( expression ) = value ;
    }
    else {

      init_circ_list( &expr_list ) ;
      do {
        readerror = read_expression( &inner_expression ) ;
        if ( readerror != 0 )
          return readerror ;
        rc = circ_append( &expr_list,
                          (generic_ptr) inner_expression ) ;
        if ( rc == ERROR )
          return E_SPACE ;
        p_next_token = lookahead() ;
        if ( p_next_token -> tokentype == EOF_T )
          return E_EOF ;

      } while ( p_next_token -> tokentype != RIGHTPAREN_T ) ;
      LISP_TYPE( expression ) = LIST ;
      LIST_VALUE ( expression ) = expr_list ;

    }

    p_token = gettoken() ;
    break ;

  case QUOTE_T:
    p_next_token = lookahead() ;
    if ( p_next_token -> tokentype == EOF_T )
      return E_EOF ;

    if ( p_next_token -> tokentype == RIGHTPAREN_T ) {
      return E_SYNTAX ;
    } else {

      init_circ_list( &expr_list ) ;
      if (( inner_expression = new_expression()) == NULL )
        return E_SPACE ;
      if (( value = new_atom( QUOTE_VALUE )) == NULL )
        return E_SPACE ;
      LISP_TYPE( inner_expression ) = ATOM ;
      ATOM_VALUE( inner_expression ) = value ;

      rc = circ_append( &expr_list, (generic_ptr) inner_expression ) ;
      if ( rc == ERROR )
        return E_SPACE ;

      readerror = read_expression( &inner_expression ) ;
      if ( readerror != 0 )
        return readerror ;

      rc = circ_append( &expr_list, (generic_ptr) inner_expression ) ;
      if ( rc == ERROR )
        return E_SPACE ;

      LISP_TYPE( expression ) = LIST ;
      LIST_VALUE( expression ) = expr_list ;

    }
    break ;

  }

  *p_expression = expression ;
  return 0 ;

}

extern int evaluate_function( char *name, clist args, lisp_expression *p_value ) {

  int c_car(), c_cdr(), c_cons(), c_eval(), c_exit(), c_atomp(), c_nullp(),
    c_listp(), c_equal() ;
  static struct{ char *func_name ; int (*f)() ;
  } builtin[MAXBUILTIN] = {
    {"CAR", c_car}, {"CDR", c_cdr}, {"CONS", c_cons}, {"EVAL", c_eval},
    {"EXIT", c_exit}, {"ATOMP", c_atomp}, {"LISTP", c_listp}, {"NULLP", c_nullp},
    {"EQUAL", c_equal}, {"SETQ", c_setq}
  } ;

  int i ;

  for ( i = 0 ; i < MAXBUILTIN ; i++ ) {

    if ( strcmp( name, builtin[i].func_name ) == 0 ) return builtin[i].f( args, p_value ) ;

  }

  return E_EVAL ;

}

int eval_expression( lisp_expression expression, lisp_expression *p_value ) {

  clist L, arg ;
  lisp_atom function_name ;
  clist sublist ;
  lisp_expression subexpr ;
  lisp_expression return_value ;
  int rc ;

  if( LISP_TYPE(expression) == ATOM ) { *p_value = expression ; return 0 ; }

  L = LIST_VALUE( expression ) ;

  arg = circ_list_iterator( L, NULL ) ;

  if( LISP_TYPE( DATA(arg)) != ATOM ) return E_EVAL ;
  function_name = ATOM_VALUE(DATA(arg)) ;

  if( equal_atom( function_name, QUOTE_VALUE )) {

    if( circ_length(L) != 2 ) return E_EVAL ;
    *p_value = (lisp_expression) DATA( circ_list_iterator( L, arg )) ;
    return 0 ;

  }

  init_circ_list( &sublist );

  if( circ_length( L ) > 1 ) {

    while (( arg = circ_list_iterator( L, arg )) != NULL ) {

      rc = eval_expression( (lisp_expression) DATA( arg ), &subexpr ) ;
      if ( rc ) return rc ;
      if ( circ_append ( &sublist, (generic_ptr) subexpr ) == ERROR ) return E_EVAL ;

    }

  }

  rc = evaluate_function( function_name, sublist, &return_value ) ;
  if ( rc ) return rc ;
  *p_value = return_value ;

  return 0 ;

}

status print_expression( lisp_expression expression ) {

  lisp_expression variable_value ;

  if ( LISP_TYPE( expression ) == ATOM ) {

    /* if that atom is a variable name, print the value
       of that variable */
		
		/* need to make this so that it won't bother if there
			 is a quote in front of it. that will
			 probably require changing eval_expression */


		if ( key_is_member( variables, ATOM_VALUE( expression ) ) == TRUE ) {
			variable_value = get_value_at_key( variables, ATOM_VALUE( expression ) ) ;
			print_expression( variable_value ) ;
		}

		else
			printf( "%s", ATOM_VALUE( expression )) ;

  }

  else {

    printf( "(" ) ;
    circ_traverse( LIST_VALUE( expression ), print_expression ) ;
    printf( ")" ) ;

  }

  return OK ;
}

extern int c_exit( clist arglist, lisp_expression *p_return ) {

  return E_EXIT ;

}

int c_car( clist arglist, lisp_expression *p_return ) {

  lisp_expression car_list ;

  if ( circ_length( arglist ) != 1 )
    return E_EVAL ;

  car_list = ( lisp_expression ) DATA( nth_node( arglist, 1 )) ;
  if (LISP_TYPE( car_list ) != LIST )
    return E_EVAL ;

  *p_return = (lisp_expression) DATA( nth_node( LIST_VALUE( car_list ), 1)) ;
  return 0 ;

}

int c_atomp( clist arglist, lisp_expression *p_return ) {

  lisp_expression atom, expression ;

  if ( circ_length( arglist ) != 1 )
    return E_EVAL ;

  atom = ( lisp_expression ) DATA( nth_node( arglist, 1 )) ;

  if (( expression = new_expression() ) == NULL )
    return E_EVAL ;

  LISP_TYPE( expression ) = ATOM ;

  if ( LISP_TYPE( atom ) != ATOM )
    ATOM_VALUE( expression ) = new_atom( NIL_VALUE ) ;

  else
    ATOM_VALUE( expression ) = new_atom( T_VALUE ) ;

  *p_return = expression ;
  return 0 ;

}

int c_equal( clist arglist, lisp_expression *p_return ) {

  lisp_expression arg1, arg2, expression ;

  if ( circ_length( arglist ) != 2 )
    return E_EVAL ;

  arg1 = ( lisp_expression  ) DATA( nth_node( arglist, 1 )) ;
  arg2 = ( lisp_expression ) DATA( nth_node( arglist, 2 )) ;

  if (( expression = new_expression() ) == NULL )
    return E_EVAL ;

  LISP_TYPE( expression ) = ATOM ;

  /* if they aren't the same type give up immediately */
  if ( LISP_TYPE( arg1 ) != LISP_TYPE( arg2) )
    ATOM_VALUE( expression ) = new_atom( NIL_VALUE ) ;

  /* if they are both atoms, compare the atoms */
  if ( LISP_TYPE( arg1 ) == ATOM ) {
    if ( equal_atom( ATOM_VALUE(arg1), ATOM_VALUE(arg2) ))
      ATOM_VALUE( expression ) = new_atom( T_VALUE ) ;
    else
      ATOM_VALUE( expression ) = new_atom( NIL_VALUE ) ;
  }

  /* if they are both the same type
     and one is a list, they are both lists
     and should be compared as such
  */
  if ( LISP_TYPE( arg1 ) == LIST ) {
    if ( equal_lisp_list( LIST_VALUE( arg1 ), LIST_VALUE( arg2) ) == TRUE ) {
      ATOM_VALUE( expression ) = new_atom( T_VALUE ) ;
    }
    else
      ATOM_VALUE( expression ) = new_atom( NIL_VALUE ) ;
  }

  *p_return = expression ;
  return 0 ;

}


int c_nullp( clist arglist, lisp_expression *p_return ) {

  lisp_expression nullp, expression ;

  if ( circ_length( arglist ) != 1 )
    return E_EVAL ;

  nullp = ( lisp_expression ) DATA( nth_node( arglist, 1 ) ) ;
  if (( expression = new_expression() ) == NULL )
    return E_EVAL ;

  LISP_TYPE( expression ) = ATOM ;

  if ( LISP_TYPE( nullp) == ATOM && strcmp( ATOM_VALUE( nullp ), NIL_VALUE ) == 0 )
    ATOM_VALUE( expression ) = new_atom( T_VALUE ) ;

  else
    ATOM_VALUE( expression ) = new_atom( NIL_VALUE ) ;

  *p_return = expression ;
  return 0 ;

}

int c_listp( clist arglist, lisp_expression *p_return ) {

  lisp_expression list, expression ;

  if ( circ_length( arglist ) != 1 )
    return E_EVAL ;

  list = ( lisp_expression ) DATA( nth_node( arglist, 1 )) ;

  if (( expression = new_expression() ) == NULL )
    return E_EVAL ;

  LISP_TYPE( expression ) = ATOM ;

  if( LISP_TYPE( list ) != LIST )
    ATOM_VALUE( expression ) = new_atom( NIL_VALUE ) ;

  else
    ATOM_VALUE( expression ) = new_atom( T_VALUE ) ;

  *p_return = expression ;
  return 0 ;

}

int c_setq( clist arglist, lisp_expression *p_return ) {

  lisp_expression var_name ;
  lisp_expression var_value ;

  lisp_var temp ;

  if ( circ_length( arglist ) != 2 )
    return E_EVAL ;

  var_name = ( lisp_expression ) DATA( nth_node( arglist, 1 )) ;
  if ( LISP_TYPE( var_name ) != ATOM )
    return E_EVAL ;

  var_value = ( lisp_expression ) DATA( nth_node( arglist, 2 )) ;

  temp.key = ATOM_VALUE( var_name ) ;
  temp.value = var_value ;

	/* check if variable the variable name has already been used */
	if ( key_is_member( variables, temp.key ) == TRUE )
		set_value_at_key( &variables, temp.key, var_value ) ;
	else 
		append_lisp_var( &variables, &temp ) ;

  /* setq returns the data assigned*/
  *p_return = var_value ;
  return 0 ;

}

int c_cdr( clist arglist, lisp_expression *p_return ) {

  lisp_expression expression ;
  lisp_expression cdr_list ;
  clist car_node ;

  if ( circ_length(arglist) != 1 )
    return E_EVAL ;

  cdr_list = (lisp_expression) DATA( nth_node( arglist, 1 )) ;
  if ( LISP_TYPE( cdr_list ) != LIST )
    return E_EVAL ;

  if (( expression = new_expression() ) == NULL )
    return E_EVAL ;

  car_node = nth_node( LIST_VALUE( cdr_list ), 1 ) ;
  free( DATA( car_node )) ;

  if ( circ_delete_node( &LIST_VALUE( cdr_list ), car_node ) == ERROR )
    return E_EVAL ;

  if ( empty_circ_list( LIST_VALUE( cdr_list )) == TRUE ) {

    LISP_TYPE( expression ) = ATOM ;
    ATOM_VALUE( expression ) = new_atom( NIL_VALUE ) ;
    if ( ATOM_VALUE( expression ) == NULL )
      return E_SPACE ;

  }
  else {
    LISP_TYPE( expression ) = LIST ;
    LIST_VALUE( expression ) = LIST_VALUE( cdr_list ) ;
  }

  *p_return = expression ;
  return 0 ;

}

int c_cons( clist arglist, lisp_expression *p_return ) {

  lisp_expression arg1, arg2 ;

  if ( circ_length( arglist ) != 2 )
    return E_EVAL ;

  arg1 = (lisp_expression) DATA( nth_node( arglist, 1 )) ;
  arg2 = (lisp_expression) DATA( nth_node( arglist, 2 )) ;

  if ( LISP_TYPE( arg2 ) != LIST )
    return E_EVAL ;

  if( circ_insert( &LIST_VALUE( arg2 ), (generic_ptr) arg1) == ERROR )
    return E_EVAL ;

  *p_return = arg2 ;
  return 0 ;

}

int c_eval( clist arglist, lisp_expression *p_return ) {

  if( circ_length( arglist ) != 1 )
    return E_EVAL ;

  return eval_expression(( lisp_expression ) DATA( arglist ), p_return ) ;

}
