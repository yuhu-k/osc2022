#pragma once

void* switch_to(void *prev,void *next);
void* get_current();
void set_current(void* addr);