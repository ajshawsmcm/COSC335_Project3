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

static void addToReadyQueue(xProcess_PCB_Ptr process);

static void select_process(void){
  //printf("%s\n",xProcess_state_name(current->state));
  if(xProcess_terminated == current->state){
    xProcess_destroy(current);
  }else if(0 == current->number){
    current->expAvg = 99999;
    addToReadyQueue(current);
  }else if(xProcess_ready == current->state){
    current->state = xProcess_running;
    xSystem_set_timer(xSystem_time()+2000);
    xSystem_dispatch(current);
    return;
  }else{
    xList_insert(waiting_queue, xList_last(waiting_queue), current);
    current->expAvg = (current->expAvg + (double)xSystem_time() - (double)currentStartTime) / 2.0;
  }
  currentStartTime = xSystem_time();
  current = (xProcess_PCB_Ptr) xList_data(xList_first(ready_queue));
  xList_remove(ready_queue, NULL);
  current->state = xProcess_running;
  xSystem_set_timer(xSystem_time()+2000);
  xSystem_dispatch(current);
}
static void addToReadyQueue(xProcess_PCB_Ptr process){
  process->state = xProcess_ready;
  if(0 == xList_size(ready_queue)){
    xList_insert(ready_queue,NULL,process);
    return;
  }
  xList_Element_Ptr currentNode = (xList_Element_Ptr) xList_first(ready_queue);
  //printf("hey\n");
  xProcess_PCB_Ptr nextData = (xProcess_PCB_Ptr) xList_data(currentNode);
  //printf("hey\n");
  if(process->expAvg < nextData->expAvg){
    //printf("if\n");
    xList_insert(ready_queue,NULL,process);
  }else{
    //printf("else\n");
    if(xList_next(currentNode) != NULL){
      nextData = (xProcess_PCB_Ptr) xList_data(xList_next(currentNode));
    }
    //printf("next\n");
    while(xList_next(currentNode) != NULL && process->expAvg > nextData->expAvg){
      currentNode = (xList_Element_Ptr) xList_next(currentNode);
      if(xList_next(currentNode) != NULL){
        nextData = (xProcess_PCB_Ptr) xList_data(xList_next(currentNode));
      }
    }
    xList_insert(ready_queue,currentNode,process);
  }
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
    //printf("Process %d Exponentional Average: %lf\n",current->number,current->expAvg);
    xProcess_destroy(current);
  }
  xList_destroy(ready_queue);
  //probably better to concat the lists first
  while (NULL != xList_first(waiting_queue)) {
    current = (xProcess_PCB_Ptr) xList_data(xList_first(waiting_queue));
    xList_remove(waiting_queue, NULL);
    current->state = xProcess_terminated;
    //printf("Process %d Exponentional Average: %lf\n",current->number,current->expAvg);
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
  process->expAvg = 1000;
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
  xList_Element_Ptr currentNode = (xList_Element_Ptr) xList_first(waiting_queue);
  xProcess_PCB_Ptr nextData = (xProcess_PCB_Ptr) xList_data(currentNode);
  if(nextData->number == process->number){
    xList_remove(waiting_queue,NULL);
    return;
  }
  nextData = (xProcess_PCB_Ptr) xList_data(xList_next(currentNode));
  while(nextData->number != process->number){
    currentNode = (xList_Element_Ptr) xList_next(currentNode);
    nextData = (xProcess_PCB_Ptr) xList_data(xList_next(currentNode));
  }
  xList_remove(waiting_queue,currentNode);
}

void xScheduler_timer_event(void)
{

  current->state = xProcess_ready;
  select_process();
}
