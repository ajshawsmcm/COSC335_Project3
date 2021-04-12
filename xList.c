#include "xList.h"

#include <stdlib.h>

typedef struct xList_Element {
  void* data;
  xList_Element_Ptr next;
} xList_Element;

typedef struct xList {
  int size;
  xList_Element_Ptr first;
  xList_Element_Ptr last;
} xList;

/* Create a list */
xList_Ptr xList_create()
{
  xList_Ptr result;

  result = malloc(sizeof(xList));
  result->size = 0;
  result->first = NULL;
  result->last = NULL;

  return result;
}

/* Destroy a list */
void xList_destroy(xList_Ptr list)
{

  /* Remove all the elements */
  while (NULL != list->first) {
    xList_remove(list, NULL);
  }

  /* Free the xList data structure */
  free(list);
}

/* Note: In inserting and removing elements from the list we're taking
 * it on faith  that element is an element of list  and not some other
 * xList.  Tracking the element down would be expensive.
 */

/* Insert a value after element, or at head of list if element is NULL */
void xList_insert(xList_Ptr list, xList_Element_Ptr element, void* data)
{
  xList_Element_Ptr new_element;

  new_element = malloc(sizeof(xList_Element));
  new_element->data = data;

  if (NULL == element) { /* Inserting element at head of list */
    new_element->next = list->first;
    list->first = new_element;
  } else { /* Inserting in the middle of a list */
    new_element->next = element->next;
    element->next = new_element;
  }

  if (NULL == new_element->next) { /* We just inserted the last element */
    list->last = new_element;
  }

  list->size = list->size + 1;
}

/* Remove the value after element, or at head of list if element is NULL */
void xList_remove(xList_Ptr list, xList_Element_Ptr element)
{
  xList_Element_Ptr target;

  /* Can't remove from an empty list */
  if (NULL == list->first) { 
    /* This should be an exception, but this is C */
    return;
  }

  /* Can't remove after the end of the list */
  /* NOTE: Short-cutting */
  if ((NULL != element) && (NULL == element->next)) { 
    /* This should be an exception, but this is C */
    return;
  }

  /* Adjust the list to remove the requested element */
  if (NULL == element) { /* element is the first in the list */
    target = list->first;
    list->first = target->next;
  } else { /* element is in the middle of the list */
    target = element->next;
    element->next = target->next;
  }

  /* Adjust the last pointer if necessary */
  if (list->last == target) { /* about to remove the last element */
    list->last = element;
  }

  /* Clean up */
  free(target);
  list->size = list->size - 1;
}

/* Determine the number of elements in the list */
int xList_size(xList_Ptr list)
{
  return list->size;
}

/* Obtain the first element in the list */
xList_Element_Ptr xList_first(xList_Ptr list)
{
  return list->first;
}

/* Obtain the last element in the list */
xList_Element_Ptr xList_last(xList_Ptr list)
{
  return list->last;
}

/* Obtain the data associated with the element */
void* xList_data(xList_Element_Ptr element)
{
  return element->data;
}

/* Obtain the next element in the list */
xList_Element_Ptr xList_next(xList_Element_Ptr element)
{
  return element->next;
}
