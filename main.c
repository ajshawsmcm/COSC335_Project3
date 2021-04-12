#include "xSimulator.h"
#include "xSystem.h"

#include <stdlib.h>

int main() {
  int end_time;

  // Original
  srand(37648726);
  // Works
  //  srand(34567);

  end_time = 100000;
  xSimulator_initialise(xSystem_process, xSystem_free);
  xSystem_initialise();
  
  xSimulator_run(end_time);

  xSystem_finalise();
  xSimulator_finalise();

  return EXIT_SUCCESS;
}
