/***************************************************/
/* clist.h                                         */
/*                                                 */
/***************************************************/

#ifndef _list
#define _list

#include "globals.h"
typedef struct node node, *clist ;
struct node { generic_ptr datapointer ; clist next ; } ;

extern status allocate_node( clist *p_l, generic_ptr data ) ;
extern void free_node( clist *p_L ) ;

extern status init_circ_list( clist *p_L ) ;

extern bool empty_circ_list( clist L ) ;
extern status circ_insert( clist *p_L, generic_ptr data ) ;
extern status circ_append( clist *p_L, generic_ptr data ) ;
extern status circ_delete( clist *p_L, generic_ptr *p_data ) ;
extern status circ_delete_node( clist *p_L, clist node ) ;

extern status circ_traverse( clist L, status (*p_func_f) () ) ;
extern clist circ_list_iterator( clist L, clist lastreturn ) ;
extern int  circ_length( clist L ) ;
extern clist nth_node( clist L, int number ) ;

#endif
