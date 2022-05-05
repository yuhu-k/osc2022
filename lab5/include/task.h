void UserScheduler();
void* ReadyListPop();
void UserExit();
void PushToReadyList(unsigned int tid);
void InitUserTaskScheduler();
int UserThread(void* func, void* arg);

struct trapframe {
    uint64 x[31]; // general register from x0 ~ x30
    uint64 sp_el0;
    uint64 spsr_el1;
    uint64 elr_el1;
};