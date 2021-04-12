#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "xSimulator.h"

/* Create a new simulation event */
static int* new_event(int value)
{
  int* event;

  event = malloc(sizeof(int));
  *event = value;

  return event;
}

static void process_event(void* event)
{
  printf("%08ld: Event %03d occurred.\n", xSimulator_time(), * (int*) event);
}

static void free_event(void* event)
{
  free(event);
}

int main(int argc, char** argv)
{
  int* value;
  int count;
  void* cancellable_event;

  xSimulator_initialise(process_event, free_event);

  /* Schedule several events for the future */
  for (count = 0; count < 10; count++) {
    xSimulator_schedule(count*100, new_event(count));
  }

  /* Schedule some events out of order */
  xSimulator_schedule(450, new_event(40));
  xSimulator_schedule(550, new_event(50));
  xSimulator_schedule(650, new_event(60));

  /* Schedule two events for the same time */
  xSimulator_schedule(850, new_event(70));
  xSimulator_schedule(850, new_event(80));

  /* Schedule two events one for cancellation */
  cancellable_event = new_event(90);
  xSimulator_schedule(950, cancellable_event);
  xSimulator_schedule(980, new_event(95));

  printf("%08ld: Start simulation.\n", xSimulator_time());

  /* Run through half the events */
  xSimulator_run(700);

  /* Try to add an event for an earlier time */
  xSimulator_schedule(xSimulator_time() - 10, new_event(99));

  /* Schedule some more future events */
  for (count = 0; count < 5; count++) {
    xSimulator_schedule(xSimulator_time() + count*10, new_event(100 + count));
  }

  xSimulator_cancel(cancellable_event);

  /* Run simulator until there are no more events */
  xSimulator_run(10000);

  xSimulator_finalise();

  return 0;
}
