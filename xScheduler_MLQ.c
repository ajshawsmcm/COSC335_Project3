#include "xScheduler.h"

#include <assert.h>
#include <stdlib.h>
/* TODO Remove after debugging */
#include <stdio.h>

#include "xList.h"
#include "xProcess.h"
#include "xSystem.h"

static xList_Ptr queue[10];
static xList_Ptr waiting_queue;

static xProcess_PCB_Ptr current;

static void select_process(void){
  //printf("%s\n",xProcess_state_name(current->state));
  //Put the current process somewhere
  switch(current->state){
    case xProcess_terminated:
    xProcess_destroy(current);
    break;
    case xProcess_ready:
    xList_insert(queue[current->priority],xList_last(queue[current->priority]),current);
    break;
    default:
    xList_insert(waiting_queue,xList_last(waiting_queue),current);
  }
  //printf("%s\n",xProcess_state_name(current->state));
  //Find next ready process according to the MLQ line of succession
  int i;
  for(i = 9; i > -1; i--){
    //printf("%d\n",i);
    if(xList_size(queue[i])){
        current = (xProcess_PCB_Ptr) xList_data(xList_first(queue[i]));
        xList_remove(queue[i], NULL);
        break;
    }
  }
  //printf("%s\n",xProcess_state_name(current->state));
  current->state = xProcess_running;
  xSystem_set_timer(xSystem_time() + 1500);
  xSystem_dispatch(current);
}

void xScheduler_initialise(int raw_quantum)
{
  int i;
  for(i = 0; i < 10; i++){
    queue[i] = xList_create();
  }
  waiting_queue = xList_create();
  current = xProcess_create(0);
  current->state = xProcess_running;

}

void xScheduler_finalise(void)
{
  current->state = xProcess_terminated;
  xProcess_destroy(current);
  int i;
  for(i = 0; i < 10; i++){
    while (NULL != xList_first(queue[i])) {
      current = (xProcess_PCB_Ptr) xList_data(xList_first(queue[i]));
      xList_remove(queue[i], NULL);
      current->state = xProcess_terminated;
      xProcess_destroy(current);
    }
    xList_destroy(queue[i]);
  }
  while (NULL != xList_first(waiting_queue)) {
    current = (xProcess_PCB_Ptr) xList_data(xList_first(waiting_queue));
    xList_remove(waiting_queue, NULL);
    current->state = xProcess_terminated;
    xProcess_destroy(current);
  }
  xList_destroy(waiting_queue);

}

void xScheduler_process_start(xProcess_PCB_Ptr process)
{
  char buffer[1000];
  snprintf(buffer, 1000, "Setting Process %d to Ready", process->number);
  xSystem_kernel_message(buffer);

  process->state = xProcess_ready;
  xList_insert(queue[process->priority],NULL,process);

  if(process->priority > current->priority){

    xScheduler_timer_event();
  }
}

void xScheduler_process_end(void)
{
  if(0 != current->number){
    current->state = xProcess_terminated;
  }
  select_process();
}

void xScheduler_io_start(void)
{
  current->state = xProcess_waiting;
  select_process();
}

void xScheduler_io_end(xProcess_PCB_Ptr process)
{
  process->state = xProcess_ready;
  xList_insert(queue[process->priority],NULL,process);
  xList_Element_Ptr currentNode = (xList_Element_Ptr) xList_first(waiting_queue);
  xProcess_PCB_Ptr nextData = (xProcess_PCB_Ptr) xList_data(currentNode);

  if(nextData->number == process->number){
    xList_remove(waiting_queue,NULL);
  }else{
    nextData = (xProcess_PCB_Ptr) xList_data(xList_next(currentNode));
    while(nextData->number != process->number){
      currentNode =  (xList_Element_Ptr) xList_next(currentNode);
      nextData = (xProcess_PCB_Ptr) xList_data(xList_next(currentNode));
    }
    xList_remove(waiting_queue,currentNode);
  }
  xScheduler_timer_event();
}

void xScheduler_timer_event(void)
{
  current->state = xProcess_ready;
  select_process();
}
