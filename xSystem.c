#include "xSystem.h"

#include <stdlib.h>
#include <stdio.h>

#include "xSimulator.h"
#include "xScheduler.h"

#define BUFFER_SIZE 1000

static char buffer[BUFFER_SIZE];

/* Likelihood of a new process being created during a burst */
#define NEW_PROCESS_PROBABILITY 10
#define END_PROCESS_PROBABILITY 10

/* Probabilities for CPU and IO burst lengths */
#define CPU_BURST_MEDIAN 1000
#define CPU_BURST_SIGMA 100
#define IO_BURST_MEDIAN 10000
#define IO_BURST_SIGMA 1000

#define PRIORITY_MAXIMUM 10

/* A enumeration of possible events */
typedef enum { 
   xSystem_process_start, 
   xSystem_process_end, 
   xSystem_timer_event, 
   xSystem_io_start,
   xSystem_io_end 
 } xSystem_type;

typedef struct xSystem_Event* xSystem_Event_Ptr;

typedef struct xSystem_Event {
  xSystem_type type;
  xProcess_PCB_Ptr process;
} xSystem_Event;

/* Details of the current settings for the timer */
static time_t timer_time;
static xSystem_Event_Ptr timer_event;

/* Details of a current io_burst */
static time_t io_start_time;
static xSystem_Event_Ptr io_start_event;
static time_t io_end_time;
static xSystem_Event_Ptr io_end_event;

/* Details of the current process_end event */
static time_t process_end_time;
static xSystem_Event_Ptr process_end_event;

/* Print a message using xSimulator */
static void print(xProcess_PCB_Ptr process, char* message)
{
  if (NULL != process) {
    snprintf(buffer, BUFFER_SIZE, "Process %03d:%s ", process->number, message);
  } else {
    snprintf(buffer, BUFFER_SIZE, "%s", message);
  }
  xSimulator_print(buffer);
}

/* Print an error message using xSimulator */
static void print_error(xProcess_PCB_Ptr process, char* message)
{
  if (NULL != process) {
    snprintf(buffer, BUFFER_SIZE, "Process %03d:%s ", process->number, message);
  } else {
    snprintf(buffer, BUFFER_SIZE, "%s", message);
  }
  xSimulator_print_error(buffer);
}

/*
 * TODO  This  should be  properly  implemented  using the  Box-Muller
 * method.   For now  any  value between  mean -  2*sigma  and mean  +
 * 2*sigma is generated with even probability distribution.
 */
static int gaussian_random(int median, int sigma)
{
  int deviation;

  deviation = rand() % (2 * sigma);

  /* On some platforms % always returns a positive value */
  /* This solution is platform agnostic */
  if (0 == rand() % 2) {
    deviation = -deviation;
  }

  return median - 2 * sigma + deviation;
}

/* Schedule an xSystem event */
static xSystem_Event_Ptr schedule_event(time_t time, xSystem_type type,
				   xProcess_PCB_Ptr process)
{
  xSystem_Event_Ptr event;

  event = malloc(sizeof(xSystem_Event));
  event->type = type;
  event->process = process;

  xSimulator_schedule(time, event);

  return event;
}

static void create_burst(xProcess_PCB_Ptr process)
{
  time_t burst_length;
  time_t io_service_time;

  /* Determine the length of the CPU burst */
  burst_length = gaussian_random(CPU_BURST_MEDIAN, CPU_BURST_SIGMA);
  
  /* 
   * Determine the manner in which the quantum will end.  The idle
   * process always runs to the end of its quantum
   */

  if (0 == process->number) {
    /* Just let the timer event stand */
  } else {
  
    if (xSimulator_time() + burst_length > timer_time ) { /* CPU Burst */
      /* Determine if the process will end on this CPU burst */
      if (0 == (rand() % END_PROCESS_PROBABILITY)) {
	burst_length = rand() % (timer_time - xSimulator_time());
	process_end_time = xSimulator_time() + burst_length;
	process_end_event = schedule_event(process_end_time,
					   xSystem_process_end, process);
      } else {
	/* Just allow the timer to handle everything */
      }
    } else { /* The slice ends with blocking I/O */
      
      io_start_time = xSimulator_time() + burst_length;
      io_service_time = gaussian_random(IO_BURST_MEDIAN, IO_BURST_SIGMA);
      io_end_time = io_start_time + io_service_time;

      io_start_event = schedule_event(io_start_time, xSystem_io_start, process);
      io_end_event = schedule_event(io_end_time, xSystem_io_end, process);
      
    }
  }
}

static void create_process(void)
{
  xProcess_PCB_Ptr process;
  time_t time;

  process = xProcess_create(rand() % (PRIORITY_MAXIMUM + 1));
  time = xSimulator_time() + 
    gaussian_random(CPU_BURST_MEDIAN, CPU_BURST_SIGMA);

  schedule_event(time, xSystem_process_start, process);
}

/* Create all data structures needed by the simulator */
void xSystem_initialise(time_t time)
{
  xSimulator_print("Simulated system initialised.");

  xProcess_initialise();
  xScheduler_initialise((CPU_BURST_MEDIAN * 80) / 100);

  /* Start the simulation with the most benign call possible */
  schedule_event(10, xSystem_timer_event, NULL);
}

/* Free resources used by the simulator */
void xSystem_finalise(void)
{
  xSimulator_print("Simulated system ended.");
  xScheduler_finalise();
  xProcess_finalise();
}

/* Process the next event from the xSimulator */
void xSystem_process(void* raw)
{
  /* NOTE: Not being able to do an implicit cast through the parameter
   * type is annoying 
   */

  xSystem_Event_Ptr event;
  event = (xSystem_Event_Ptr) raw;

  switch (event->type) {
  case xSystem_process_start:
    print(event->process, "Process started.");
    xScheduler_process_start(event->process);
    break;
  case xSystem_process_end:
    print(event->process, "Process ended.");
    xScheduler_process_end();
    break;
  case xSystem_timer_event:
    print(event->process, "Timer event occurred.");
    xScheduler_timer_event();
    break;
  case xSystem_io_start:
    print(event->process, "Blocking I/O started.");
    xScheduler_io_start();
    break;
  case xSystem_io_end:
    print(event->process, "Blocking I/O ended.");
    xScheduler_io_end(event->process);
    break;
  default:
    print_error(NULL, "Undefined event type.");
    break;
  }
}

void xSystem_free(void* event)
{
  free(event);
}

time_t xSystem_time(void)
{
  return xSimulator_time();
}

void xSystem_set_timer(time_t time)
{
  snprintf(buffer, BUFFER_SIZE, "Timer set for %08ld microseconds.", time);
  xSimulator_print(buffer);

  /* Cancel the previous timer event */
  xSimulator_cancel(timer_event);

  /* Record the time for use when cancelling timer event. */
  timer_time = time;

  /* Schedule a new timer event */
  timer_event = schedule_event(time, xSystem_timer_event, NULL);
}

void xSystem_dispatch(xProcess_PCB_Ptr process)
{
  char buffer[BUFFER_SIZE];

  if (io_start_time > xSimulator_time()) {
    /* The process has been pre-empted before input/output started */

    xSimulator_cancel(io_start_event);
    xSimulator_cancel(io_end_event);
    snprintf(buffer, BUFFER_SIZE, "Cancelling I/O burst end at %08ld.", io_end_time);
    print(process, buffer);
  }

  if (process_end_time > xSimulator_time()) {
    /* The process has been pre-empted before it ended */

    xSimulator_cancel(process_end_event);
  }

  if (process_end_time > xSimulator_time()) {
    /* The process has been pre-empted before it ended */

    xSimulator_cancel(process_end_event);
  }

  /* We don't need  to cancel the timer_event because  this is already
     done in xSystem_set_timer() */

  print(process, "Dispatched.");

  create_burst(process);
    
  /* Determine if a new process should be created */
  if (0 == rand() % NEW_PROCESS_PROBABILITY) {
    create_process();
  }
}

void xSystem_kernel_message(char* message)
{
  char buffer[BUFFER_SIZE];

  snprintf(buffer, BUFFER_SIZE, "Kernel: %s", message);
  xSimulator_print(buffer);
}
