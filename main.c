#include "interpreter.h"
#include "globals.h"
#include <stdio.h>

void printerror( int errnum ) ;

int main( int argc, char *argv[] ) {

  lisp_expression expression, value ;
  int rc ;

  do {

    printf( "lisp: " ) ;
    rc = read_expression( &expression ) ;

    if ( rc ) {
      printerror( rc ) ;
      continue ;
    }

    rc = eval_expression( expression, &value ) ;

    if ( rc != E_EXIT ) {

      if ( rc ) {
        printerror( rc ) ;
        continue ;
      }

      print_expression( value ) ;
      printf( "\n" ) ;

    }
  } while ( rc != E_EXIT ) ;

  printf( " Normal termination.\n" ) ;
  return 0 ;

}

void printerror( int errnum ) {

  static char *errmsg[] = {
    "unexpected end of file",
    "out of memory space",
    "syntax error",
    "error evaluating expression"
  } ;

  if ( errnum < 0 || errnum >= MAXERROR ) {

    printf( " System Error. INvalid error number: %d\n", errnum ) ;
    return ;

  }

  printf( "%s\n", errmsg[errnum-1] ) ;

}
