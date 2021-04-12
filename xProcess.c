#include "xProcess.h"

#include <stdlib.h>
#include <stdio.h>

static int process_number;

void xProcess_initialise(void) {
  process_number = 0;
}

void xProcess_finalise(void) {
  /* Nothing to do */
}

xProcess_PCB_Ptr xProcess_create(int priority)
{
  xProcess_PCB_Ptr process;
  int count;

  /*
   * Create the PCB
   */
  process = malloc(sizeof(xProcess_PCB));

  /* 
   * Initialise all fields of the PCB
   */
  process->state = xProcess_new;
  process->number = process_number;

  /* Make sure the process number is unique */
  process_number = process_number + 1;

  /* This isn't necessary for our simulator and would be done
     differently by the loader in practice. */
  for (count = 0; count < 33; count++) {
    process->registers[count] = 0;
  }
  process->files = NULL;
  process->page_table = NULL;

  /* Scheduler specific data initialised here */
  process->priority = priority;  

  return process;
}

void xProcess_destroy(xProcess_PCB_Ptr process)
{
  if (xProcess_terminated != process->state) {
    fprintf(stderr, "Process should be terminated before it can be destroyed.\n");
  }

  free(process);

}

char* xProcess_state_name(xProcess_state state) {
  char* result;

  switch (state) {
  case xProcess_new:
    result = ("New");
    break;
  case xProcess_ready:
    result = ("Ready");
    break;
  case xProcess_running:
    result = ("Running");
    break;
  case xProcess_waiting:
    result = ("Waiting");
    break;
  case xProcess_terminated:
    result = ("Terminated");
    break;
  default:
    result = ("Undefined");
    break;
  }

  return result;
}
