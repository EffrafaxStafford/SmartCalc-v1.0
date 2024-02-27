#ifndef SRC_STACK_H_
#define SRC_STACK_H_

#define EPS 1e-07

typedef struct stack_char {
  struct stack_char *next;
  char symbol;
} Stack_char;

typedef struct stack_int {
  struct stack_int *next;
  double number;
} Stack_int;

Stack_char *init_stack_char();
void push_char(Stack_char *head, char symbol);
void pop_char(Stack_char *head);
void destroy_char(Stack_char *head);
void output_Stack_char(Stack_char *head);

Stack_int *init_stack_int();
void push_int(Stack_int *head, double number);
void pop_int(Stack_int *head);
void destroy_int(Stack_int *head);
void output_Stack_int(Stack_int *head);
#endif  //  SRC_STACK_H_
