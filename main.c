#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

struct Node
{
    int data;
    struct Node *next;
};

int hashCycler(struct Node *head){
    if(head == NULL) return 1;

    struct Node *slow = head;
    struct Node *fast = head;

    while (fast != NULL && fast -> next != NULL)
    {
        slow = slow -> next;
        fast = fast -> next -> next;

        if (slow == fast)
        {
            return 1;
        }
        
    }
    
    return 0;
}

struct Node* newNode(int data){
    struct Node *temp = (struct Node*)malloc(sizeof(struct Node));
    
    temp -> data = data;
    temp -> next = NULL;
    return temp;
}


int main(){
    
    struct Node *head = newNode(1);
    head -> next = newNode(2);
    head -> next -> next = newNode(3);
    head -> next -> next -> next = newNode(4);

    head->next->next->next->next = head->next;

    if(hashCycler(head)){
        printf("Цикл обнаружен!\n");
    }else{
        printf("Цикла нет.\n");
    }

    return 0;
    
}