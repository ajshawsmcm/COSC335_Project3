#ifndef _XSIMULATOR_H_
#define _XSIMULATOR_H_

#include <time.h>

/* Create resources needed by simulator */
void xSimulator_initialise(void (*process)(void*), void (*free)(void*));

/* Run the simulator */
void xSimulator_run(time_t end_time);

/* Free resources needed by simulator */
void xSimulator_finalise(void);

/*
 * Simulated system call backs
 */

/* Obtain the current simulation time */
time_t xSimulator_time(void);

/* Schedule a future event on the queue */
void xSimulator_schedule(time_t time, void* event);

/* Cancel a future event on the queue */
void xSimulator_cancel(void* event);

/* Print a message to stdout */
void xSimulator_print(char* message);

/* Print a message to stderr */
void xSimulator_print_error(char* message);

#endif
