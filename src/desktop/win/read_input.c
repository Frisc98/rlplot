#include "../../plotter.h"
#include <processthreadsapi.h>

static void* thandle;
unsigned long read_input_main_worker(void* gv);

void read_input_main(graph_values_t* gv) {
  thandle = CreateThread(NULL, 0, read_input_main_worker, gv, 0, NULL);
}

void read_input_stop() {
  TerminateThread(thandle, 69);
}

