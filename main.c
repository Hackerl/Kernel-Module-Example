#include <linux/module.h>
#include "data_pipe.h"

int init_module() {
    init_data_pipe("lkm_demo");
    return 0;
}

void cleanup_module() {
    cleanup_data_pipe("lkm_demo");
}