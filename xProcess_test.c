#include <stdio.h>

#include "xProcess.h"

char* process_state_name(xProcess_state state)
{
  char* result;

  switch (state) {
  case xProcess_new:
    result = "New";
    break;
  case xProcess_ready: 
    result = "Ready";
    break;
  case xProcess_running:
    result = "Running";
    break;
  case xProcess_waiting: 
    result = "Waiting";
    break;
  case xProcess_terminated:
    result = "Terminated";
    break;
  default:
    result = "Undefined";
    break;
  }

  return result;
}

void print_process(xProcess_PCB_Ptr process)
{
  printf("Process %d has priority %d in state %s.\n", 
	 process->number,
	 process->priority,
	 process_state_name(process->state));
}

int main()
{
  xProcess_PCB_Ptr process_0;
  xProcess_PCB_Ptr process_1;

  printf("Initialise the process module.\n");
  xProcess_initialise();

  printf("Create process 0.\n");
  process_0 = xProcess_create(10);
  print_process(process_0);

  printf("Change state of process 0.\n");
  process_0->state = xProcess_running;
  print_process(process_0);
  
  printf("Create process 1.\n");
  process_1 = xProcess_create(5);
  print_process(process_1);  

  /*
   * Note: The module should complain if you try to destroy an
   * unterminated process.
   */
  printf("Change state of process 1.\n");
  process_1->state = xProcess_terminated;
  print_process(process_1);

  printf("Destroy process 1.\n");
  xProcess_destroy(process_1);

  printf("Destroy process 0.\n");
  process_0->state = xProcess_terminated;
  xProcess_destroy(process_0);

  printf("Finalise the process module.\n");
  xProcess_finalise();

  return 0;
}
