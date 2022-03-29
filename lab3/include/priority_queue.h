#pragma once
int add_node(void (*callback_f)(),void* arguments,float times, float time_gap);
struct node* delete_first_node();
struct node{
    float time_to_ring;
    void (*todo)();
    void *arguments;
    struct node* next;
};