#pragma once
#include <linux/types.h>

int init_data_pipe(const char *name);
int cleanup_data_pipe(const char *name);

unsigned long push_data(void *buffer, unsigned long length);
unsigned long pop_data(void *buffer, unsigned long length);