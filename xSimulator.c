#include "xSimulator.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "xList.h"

typedef struct Event* Event_Ptr;

typedef struct Event {
  time_t time;
  void* event_data;
} Event;

/* The current time in the simulation */
static time_t simulator_time;

/* A list of future events kept in time order */
static xList_Ptr events;

/* The function to process an event */
void (*process_event)(void*);

/* The function to free an event */
void (*free_event)(void*); 

/* This could be a macro */
static Event* get_event(xList_Element_Ptr element)
{
  return (Event*) xList_data(element);
}

static int same_event(void* event, void* data)
{
  return data == ((Event_Ptr) event)->event_data;
}


void xSimulator_initialise(void (*p)(void*), void (*f)(void*))
{
  simulator_time = 0;
  xSimulator_print("Initialising simulator.");
  events = xList_create();
  process_event = p;
  free_event = f;
}

void xSimulator_run(time_t end_time)
{
  Event_Ptr next_event;

  /* If there are events and we haven't run out of time */
  while (NULL != xList_first(events) && simulator_time < end_time) {

    /* Obtain the next event in the event queue */
    next_event = (Event_Ptr) xList_data(xList_first(events));
    xList_remove(events, NULL);
    
    /* Process the event */
    simulator_time = next_event->time;
    process_event(next_event->event_data);

    /* Free both resources associated with the event */
    free_event(next_event->event_data);
    free(next_event);
  }
}

void xSimulator_finalise(void)
{
  Event_Ptr next_event;

  xSimulator_print("Finalising simulator.");

  /* This will run down the event queue */
  while (NULL != xList_first(events)) {

    /* Obtain the next event in the event queue */
    next_event = (Event_Ptr) xList_data(xList_first(events));
    xList_remove(events, NULL);
    
    /* Free both resources associated with the event */
    free_event(next_event->event_data);
    free(next_event);
  }

  /* Destroy the actual event queue */
  xList_destroy(events);
}

time_t xSimulator_time(void)
{
  return simulator_time;
}

void xSimulator_schedule(time_t time, void* event_data)
{
  xList_Element_Ptr current;
  xList_Element_Ptr previous;
  Event_Ptr new_event;

  /* Assertion */
  if (time < simulator_time) {
    fprintf(stderr, "%08ld:xSimulator: Cannot schedule event for time %08ld.\n", simulator_time, time);
    return;
  }

  /* Create event */
  new_event = malloc(sizeof(Event));
  new_event->time = time;
  new_event->event_data = event_data;

  /* Insert event into schedule */
  previous = NULL;
  current = xList_first(events);
  
  /* Move along the list until we get to the point where previous is
     before the time of this event and current is after that time */
  while ((NULL != current) && get_event(current)->time < time) {
    previous = current;
    current = xList_next(current);
  }

  xList_insert(events, previous, new_event);
}

void xSimulator_cancel(void* event_data)
{
  xList_Element_Ptr current;
  xList_Element_Ptr previous;
  Event_Ptr event;

  /* Find event in schedule */
  previous = NULL;
  current = xList_first(events);
  
  /* Move along the list until we get to the event we're looking for */
  while ((NULL != current) && ! same_event(xList_data(current), event_data)) {
    previous = current;
    current = xList_next(current);
  }
  
  /* If the event is found, remove it */
  if (NULL != current) {
    event = (Event_Ptr) xList_data(current);
    free_event(event->event_data);
    free(event);
    xList_remove(events, previous);
  }
}

/* TODO Make this take var_args like printf and message be a format string? */
void xSimulator_print(char* message)
{
  printf("%08ld:%s\n", simulator_time, message);
}

void xSimulator_print_error(char* message)
{
  fprintf(stderr, "%08ld:%s\n", simulator_time, message);
}
