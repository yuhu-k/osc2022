#pragma once
#include "thread.h"
int schedule();
void push2run_queue(struct thread* thread);
void wakeup_queue(struct thread *t);
void* push2waiting_queue();