/****************************************************************/
/* interpreter.h                                                */
/****************************************************************/

#ifndef _interpreter
#define _interpreter

#include "clist.h"

typedef enum { LIST, ATOM } lisp_type ;
typedef clist lisp_list ;
typedef char *lisp_atom ;

typedef struct lisp_node {

  lisp_type type ;

  union {
    lisp_list lisplist ;
    lisp_atom atom ;
  } value ;

} lisp_node, *lisp_expression ;

typedef struct lisp_var {
		char *key ;
		lisp_expression value ;
} lisp_var ;

int read_expression( lisp_expression *p_expression ) ;
int eval_expression( lisp_expression expression, lisp_expression *p_value ) ;
status print_expression( lisp_expression expression ) ;

lisp_expression new_expression() ;
lisp_atom new_atom( char *s ) ;
int equal_atom( lisp_atom atom1, lisp_atom atom2 ) ;

int c_atomp( clist arglist, lisp_expression *p_return ) ;
int c_listp( clist arglist, lisp_expression *p_return ) ;
int c_nullp( clist arglist, lisp_expression *p_return ) ;
int c_setq( clist arglist, lisp_expression *p_return ) ;
int c_car( clist arglist, lisp_expression *p_return ) ;
int c_cdr( clist arglist, lisp_expression *p_return )  ;
int c_cons( clist arglist, lisp_expression *p_return ) ;
int c_eval( clist arglist, lisp_expression *p_return ) ;

#define LISP_TYPE(x)  (((lisp_expression) x) -> type)
#define ATOM_VALUE(x) (((lisp_expression) x) -> value.atom)
#define LIST_VALUE(x) (((lisp_expression) x) -> value.lisplist)

#define T_VALUE "T"
#define NIL_VALUE "NIL"
#define QUOTE_VALUE "QUOTE"

#define E_EXIT  -1
#define E_EOF    1
#define E_SPACE  2
#define E_SYNTAX 3
#define E_EVAL   4
#define MAXERROR 5

#endif
