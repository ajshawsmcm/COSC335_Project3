#include <stdio.h>
#include <stdlib.h>

#include "xList.h"

static void check_list(xList_Ptr list)
{
  xList_Element_Ptr current;

  printf("[%02d] ", xList_size(list));

  printf("{");
  current = xList_first(list);
  while (NULL != current) {
    printf("%02d", (int) *((int*) xList_data(current)));
    if (current != xList_last(list)) {
      printf(", ");
    }
    current = xList_next(current);
  }
  printf("}\n\n");
}

/* Create a new list element */
static int* new_node(int value)
{
  int* node;

  node = malloc(sizeof(int));
  *node = value;

  return node;
}

int main(int argc, char** argv)
{
  xList_Ptr test;
  xList_Element_Ptr element;
  int count;
  int* node;

  printf("Create a list.\n");
  test = xList_create();
  check_list(test);
  
  printf("Try to remove the first element of an empty list.\n");
  xList_remove(test, NULL);
  check_list(test);

  printf("Try to remove the last element of an empty list.\n");
  xList_remove(test, xList_last(test));
  check_list(test);

  printf("Insert 15 integers.\n");
  for (count = 0; count < 15; count++) {
    xList_insert(test, xList_last(test), new_node(count));
    
  }
  check_list(test);

  printf("Insert an element at the end of the list.\n");
  xList_insert(test, xList_last(test), new_node(15));
  check_list(test);

  printf("Insert an element in the middle of a list.\n");
  element = xList_first(test);
  for (count = 0; count < 4; count++) {
    element = xList_next(element);
  }
  xList_insert(test, element, new_node(50));
  check_list(test);
  
  printf("Remove an element in the middle of a list.\n");
  element = xList_first(test);
  for (count = 0; count < 4; count++) {
    element = xList_next(element);
  }
  free(xList_data(xList_next(element)));
  xList_remove(test, element);
  check_list(test);

  printf("Remove the first element from a normal list.\n");
  free(xList_data(xList_first(test)));
  xList_remove(test, NULL);
  check_list(test);

  printf("Try to remove the element after the last one in a normal list.\n");
  xList_remove(test, xList_last(test));
  check_list(test);

  printf("Empty the list.\n");
  while (NULL != xList_first(test)) {
    free(xList_data(xList_first(test)));
    xList_remove(test, NULL);
  }
  check_list(test);

  printf("Done testing.\n");

  return 0;
}
