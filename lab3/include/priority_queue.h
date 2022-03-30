#pragma once
int add_node(void (*callback_f)(),void* arguments,int times, int time_gap);
struct node* delete_first_node();
struct node{
    int time_to_ring;
    void (*todo)();
    void *arguments;
    struct node* next;
};