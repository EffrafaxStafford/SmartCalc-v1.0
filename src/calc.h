#ifndef SRC_CALC_H_
#define SRC_CALC_H_

#include "stack.h"
double calc(char *function, double x);
char *str_to_int(char *str, Stack_int *stack, double x);
void point_to_comma(char *str);
char *exe_operator(char *polish, Stack_int *stack);
char *exe_function(char *polish, Stack_int *stack);
#endif  // SRC_CALC_H_