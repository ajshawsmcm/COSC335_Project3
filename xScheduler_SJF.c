#include "xScheduler.h"

#include <assert.h>
#include <stdlib.h>
/* TODO Remove after debugging */
#include <stdio.h>

#include "xList.h"
#include "xProcess.h"
#include "xSystem.h"


static xList_Ptr ready_queue;
static xList_Ptr waiting_queue;

static xProcess_PCB_Ptr current;

static int currentStartTime;

static void select_process(void){
  if(xProcess_terminated == current->state){
    xProcess_destroy(current);
  }else if(xProcess_ready == current->state){//only expect this once
    current->expAvg = 10000;
    xList_insert(ready_queue,xList_last(ready_queue),current);
  }else{
    xList_insert(waiting_queue, xList_last(waiting_queue), current);
    current->expAvg = (current->expAvg + (double)xSimulator_time() - (double)currentStartTime) / 2.0;
  }
  currentStartTime = xSimulator_time();
  current = (xProcess_PCB_Ptr) xList_data(xList_first(ready_queue));
  xList_remove(queue, NULL);
  current->state = xProcess_running;
  xSystem_dispatch(current);
}
static void addToReadyQueue(xProcess_PCB_Ptr process){
  process->state = xProcess_ready;

  if(process->expAvg < xList_first(ready_queue)->data->expAvg){
    xList_insert(ready_queue,NULL,process);
    return;
  }
  xList_Element_Ptr currentNode = xList_first(ready_queue);
  while(xList_next(currentNode) != NULL && process->expAvg > xList_next(currentNode)->data->expAvg){
    currentNode = xList_next(currentNode);
  }
  xList_insert(ready_queue,currentNode,process);
}

void xScheduler_initialise(int raw_quantum){
  ready_queue = xList_create();
  waiting_queue = xList_create();
  current = xProcess_create(0);

  currentStartTime = xSystem_time();
}

void xScheduler_finalise(void)
{
  current->state = xProcess_terminated;
  xProcess_destroy(current);

  while (NULL != xList_first(ready_queue)) {
    current = (xProcess_PCB_Ptr) xList_data(xList_first(ready_queue));
    xList_remove(ready_queue, NULL);
    current->state = xProcess_terminated;
    xProcess_destroy(current);
  }
  xList_destroy(ready_queue);
  //probably better to concat the lists first
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
  addToReadyQueue(process);
}

void xScheduler_process_end(void)
{
  if(0 != current->number){
    current ->state = xProcess_terminated;
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
  addToReadyQueue(process);
  if(xList_first(waiting_queue)->data->number == process->number){
    xList_remove(waiting_queue,NULL);
    return;
  }
  xList_Element_Ptr currentNode = xList_first(waiting_queue);
  while(xList_next(currentNode)->data->number != process->number){
    currentNode = xList_next(currentNode);
  }
  xList_remove(waiting_queue,currentNode);
}
//I don't think I'll require this method, no? If it runs, the code won't work.
void xScheduler_timer_event(void)
{
  current->state = xProcess_ready;
  select_process();
}
