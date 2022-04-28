#pragma once

void* switch_to(void *prev,void *next);
void* get_current();
int store_and_jump(void* addr, void (*call_fun)());
void set_current(void* addr);
void set_proc(void* addr);