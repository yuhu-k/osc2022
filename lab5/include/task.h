void UserScheduler();
void* ReadyListPop();
void UserExit();
void PushToReadyList(unsigned int tid);
void InitUserTaskScheduler();
int UserThread(void* func, void* arg);