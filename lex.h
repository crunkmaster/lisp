/***********************************************************************/
/* lex.h                                                               */
/***********************************************************************/

#ifndef _lex
#define _lex

#define MAXTOKENLENGTH 32
typedef enum { QUOTE_T, RIGHTPAREN_T, LEFTPAREN_T, STRING_T, EOF_T } inputtype ;

typedef struct tokendata { inputtype tokentype ; char tokenvalue[MAXTOKENLENGTH] ; }
  tokendata ;

extern tokendata *gettoken( void ) ;
extern tokendata *lookahead( void ) ;

#endif
