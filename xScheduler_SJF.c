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
  unimplemented("SJF", "initialise");
}

void xScheduler_finalise(void)
{
  unimplemented("SJF", "finalise");
}

void xScheduler_process_start(xProcess_PCB_Ptr process)
{
  unimplemented("SJF", "process_start");
}

void xScheduler_process_end(void)
{
  unimplemented("SJF", "end");
}

void xScheduler_io_start(void)
{
  unimplemented("SJF", "io_start");
}

void xScheduler_io_end(xProcess_PCB_Ptr process)
{
  unimplemented("SJF", "io_end");
}

void xScheduler_timer_event(void)
{
  unimplemented("SJF", "timer_event");
}
