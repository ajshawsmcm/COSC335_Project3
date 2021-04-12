#ifndef _XSYSTEM_H_
#define _XSYSTEM_H_

#include <time.h>

#include "xProcess.h"

/* Initialise the operating system emulator */
void xSystem_initialise();

/* Finalise the operating system emulator */
void xSystem_finalise();

/*
 * Simulator Calls
 */

/* Process an event given to us by the simulator */
void xSystem_process(void* event);

/* Free resources associated with an event */
void xSystem_free(void* event);

/*
 * Scheduler Call-backs
 */

/* Obtain the current time */
time_t xSystem_time(void);

/* Set a timer event for the future */
void xSystem_set_timer(time_t time);

/* Dispatch a process to run */
void xSystem_dispatch(xProcess_PCB_Ptr process);

/* Print a kernel message */
void xSystem_kernel_message(char* message);

#endif
