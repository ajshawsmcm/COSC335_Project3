/*
 * This is a linked list modified from code on pp. 52-64 of "Mastering
 * Algorithms with C" by Kyle Loudon, published by O'Reilly.
 */

#ifndef _XLIST_H_
#define _XLIST_H_

typedef struct xList * xList_Ptr;
typedef struct xList_Element * xList_Element_Ptr;
 
/* Create a list */
xList_Ptr xList_create();

/* Destroy a list */
void xList_destroy(xList_Ptr list);

/* Insert a value after element, or at head of list if element is NULL */
void xList_insert(xList_Ptr list, xList_Element_Ptr element, void* data);

/* Remove the value after element, or at head of list if element is NULL */
void xList_remove(xList_Ptr list, xList_Element_Ptr element);

/* Determine the number of elements in the list */
int xList_size(xList_Ptr list);

/* Obtain the first element in the list */
xList_Element_Ptr xList_first(xList_Ptr list);

/* Obtain the last element in the list */
xList_Element_Ptr xList_last(xList_Ptr list);

/* Obtain the data associated with the element */
void* xList_data(xList_Element_Ptr element);

/* Obtain the next element in the list */
xList_Element_Ptr xList_next(xList_Element_Ptr element);

#endif
