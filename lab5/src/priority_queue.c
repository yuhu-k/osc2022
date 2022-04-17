#include "allocator.h"
#include "priority_queue.h"
#include "uint.h"

struct node *nodes = NULL;
int num_of_nodes=0;

int add_node(void (*callback_f)(),void* arguments,int times,int time_gap){
    
    struct node *node = simple_malloc(sizeof(struct node));
    node->time_to_ring = times;
    node->todo = callback_f;
    node->next = NULL;
    node->arguments = arguments;

    if(nodes == NULL){
        nodes = node;
    }else if( time_gap >= times){
        time_gap = nodes->time_to_ring-time_gap;
        nodes->time_to_ring -= time_gap;
        node->next = nodes;
        nodes = node;
        struct node *temp=nodes->next->next;
        while(temp != NULL){
            temp->time_to_ring -= time_gap;
            temp = temp->next;
        }
    }else{
        struct node* temp1 = nodes;
        temp1->time_to_ring -= time_gap;
        struct node* temp2 = nodes->next;
        while(temp2 != NULL){
            temp2->time_to_ring -= time_gap;
            if(temp2->time_to_ring >= times){
                temp1->next = node;
                node->next = temp2;
            }else{
                temp1 = temp2;
                temp2 = temp2->next;
            }
        }
        if(temp2 == NULL){
            temp1->next = node;
        }else{
            while(temp2 != NULL){
                temp2->time_to_ring -= time_gap;
                temp2 = temp2->next;
            }
        }
    }
    num_of_nodes++;
    return nodes->time_to_ring;
}

struct node* delete_first_node(){
    int times=nodes->time_to_ring;
    struct node *t = nodes;
    nodes = nodes->next;
    struct node *temp = nodes;
    while(temp != NULL){
        temp->time_to_ring -= times;
        temp = temp->next;
    }
    num_of_nodes--;
    return t;
}