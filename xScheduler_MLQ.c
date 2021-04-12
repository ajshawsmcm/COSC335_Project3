#include "xScheduler.h"

#include <assert.h>
#include <stdlib.h>
/* TODO Remove after debugging */
#include <stdio.h>

#include "xList.h"
#include "xProcess.h"
#include "xSystem.h"

void unimplemented(char* algorithm, char* function)
{
  fprintf(stderr, "xScheduler_%s:xScheduler_%s unimplemented.", algorithm, function);
}

void xScheduler_initialise(int raw_quantum)
{
  unimplemented("MLQ", "initialise");
}

void xScheduler_finalise(void)
{
  unimplemented("MLQ", "finalise");
}

void xScheduler_process_start(xProcess_PCB_Ptr process)
{
  unimplemented("MLQ", "process_start");
}

void xScheduler_process_end(void)
{
  unimplemented("MLQ", "end");
}

void xScheduler_io_start(void)
{
  unimplemented("MLQ", "io_start");
}

void xScheduler_io_end(xProcess_PCB_Ptr process)
{
  unimplemented("MLQ", "io_end");
}

void xScheduler_timer_event(void)
{
  unimplemented("MLQ", "timer_event");
}
