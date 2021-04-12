#ifndef _XSCHEDULER_H_
#define _XSCHEDULER_H_

#include "xProcess.h"

/* 
  Set up all data structures needed by the scheduler, particularly the
  process queue(s).
 */
void xScheduler_initialise(int quantum);

/*
  Clean up all the data structures maintained by the scheduler.
*/
void xScheduler_finalise(void);

/*
  Call-back  for a  process starting.   This function  should set  the
  process to be executing, initialise  scheduler related fields in the
  PCB and put the process in the appropriate queue.
*/
void xScheduler_process_start(xProcess_PCB_Ptr process);

/*
  Call-back for  the process ending (terminating).   It should release
  all of  the data  structures related  to this  process and  mark the
  process as terminated.  Remove it from the appropriate queue.
*/
void xScheduler_process_end(void);

/*
  Call-back for a blocking I/O  request starting.  Mark the process as
  waiting  for  I/O,  move  it into  the  appropriate  queue.   Update
  appropriate process scheduler specific data structures.
*/
void xScheduler_io_start(void);

/*
  Call back  for a blocking I/O  request ending.  Mark the  process as
  ready,  place  it  in  the appropriate  queue.   Update  appropriate
  process scheduler specific data structures.
 */
void xScheduler_io_end(xProcess_PCB_Ptr process);

/*
  Call  back  for the  process'  time  quantum completing.   Move  the
  process  to  the  appropriate  queue.   Update  appropriate  process
  scheduler specific data structures.
*/
void xScheduler_timer_event(void);

#endif
