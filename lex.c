#include "lex.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static tokendata *p_lookahead = NULL ;

tokendata *gettoken() {

  static tokendata token[2] ;
  static int tokenindex = 0 ;
  tokendata *p_token ;
  char *p_value ;
  int c ;

  if ( p_lookahead != NULL ) {

    p_token = p_lookahead ;
    p_lookahead = NULL ;
    return p_token ;

  }

  tokenindex = ( tokenindex + 1 ) % 2 ;
  p_token = &token[tokenindex] ;
  p_value = p_token -> tokenvalue ;

  do {

    if (( *p_value = getchar() ) == EOF ) {

      p_token -> tokentype = EOF_T ;
      return p_token ;

    }

  } while ( isspace( *p_value )) ;

  p_value[1] = 0 ;
  switch ( *p_value ) {

  case '\'':
    p_token -> tokentype = QUOTE_T ;
    break ;

  case '(':
    p_token -> tokentype = LEFTPAREN_T ;
    break ;

  case ')':
    p_token -> tokentype = RIGHTPAREN_T ;
    break ;

  default:
    *p_value = toupper( *p_value ) ;
    p_value++ ;

    while (( c = getchar() ) != EOF && isalpha( c ))
      *p_value++ = toupper( c ) ;

    *p_value = '\0' ;
    if ( c != EOF )
      ungetc( c, stdin ) ;

    p_token -> tokentype = STRING_T ;

  }

  return p_token ;

}

tokendata *lookahead() {

  if ( p_lookahead == NULL )
    p_lookahead = gettoken() ;

  return p_lookahead ;

}
