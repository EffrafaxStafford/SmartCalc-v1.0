// Copyright 2022 effrafax
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

Stack_char *init_stack_char() {
  Stack_char *head = (Stack_char *)malloc(sizeof(Stack_char));
  if (head) {
    head->next = NULL;
    head->symbol = '\0';
  }
  return head;
}
Stack_int *init_stack_int() {
  Stack_int *head = (Stack_int *)malloc(sizeof(Stack_int));
  if (head) {
    head->next = NULL;
    head->number = 0.;
  }
  return head;
}

void push_char(Stack_char *head, char symbol) {
  Stack_char *node = (Stack_char *)malloc(sizeof(Stack_char));
  if (node) {
    node->next = head->next;
    node->symbol = head->symbol;
    head->next = node;
    head->symbol = symbol;
  } /* else {
     printf("The function push_char returned: NULL\n");
   }*/
}
void push_int(Stack_int *head, double number) {
  Stack_int *node = (Stack_int *)malloc(sizeof(Stack_int));
  if (node) {
    node->next = head->next;
    node->number = head->number;
    head->next = node;
    head->number = number;
  } /* else {
     printf("The function push_int returned: NULL\n");
   }*/
}

void pop_char(Stack_char *head) {
  if (head->next) {
    Stack_char *p_temp = head->next;
    head->symbol = head->next->symbol;
    head->next = head->next->next;
    free(p_temp);
  } /*else {
    free(head);
    head = NULL;
    // printf("The function pop_char returned: Stack is empty\n");
  }*/
}

void pop_int(Stack_int *head) {
  if (head->next) {
    Stack_int *p_temp = head->next;
    head->number = head->next->number;
    head->next = head->next->next;
    free(p_temp);
  } /*else {
    free(head);
    head = NULL;
    // printf("The function pop_int returned: Stack is empty\n");
  }*/
}

void destroy_char(Stack_char *head) {
  while (head->next) pop_char(head);
  free(head);
  head = NULL;
}

void destroy_int(Stack_int *head) {
  while (head->next) pop_int(head);
  free(head);
  head = NULL;
}

// void output_Stack_char(Stack_char *head) {
//   if (head) {
//     printf("'%c' ", head->symbol);
//     output_Stack_char(head->next);
//   } else {
//     printf("\n");
//   }
//   return;
// }

// void output_Stack_int(Stack_int *head) {
//   if (head) {
//     printf("%.1lf ", head->number);
//     output_Stack_int(head->next);
//   } else {
//     printf("\n");
//   }
//   return;
// }
