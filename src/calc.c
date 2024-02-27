// Copyright 2022 effrafax
#include "calc.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "dekstra.h"
#include "stack.h"

double calc(char *function, double x) {
  double result = 0.;
  if (function) {
    char *polish = dekstra(function);
    char *ppolish = polish;
    Stack_int *stack = init_stack_int();
    if (stack && polish) {
      while (*polish) {
        polish = str_to_int(polish, stack, x);
        polish = exe_operator(polish, stack);
        polish = exe_function(polish, stack);
        if (stack->next && *polish == '~') {
          stack->number *= -1.;
          polish++;
        }
      }
      result = stack->number;
    }
    destroy_int(stack);
    free(ppolish);
  }
  return result;
}

char *str_to_int(char *str, Stack_int *stack, double x) {
  // point_to_comma(str);
  if (check_number(*str)) {
    if (*str == 'x')
      push_int(stack, x);
    else
      push_int(stack, atof(str));
    while (*str && *str != ' ') str++;
    str++;
  }
  return str;
}

void point_to_comma(char *str) {
  while (*str) {
    if (*str == '.') *str = ',';
    str++;
  }
}

char *exe_operator(char *polish, Stack_int *stack) {
  if (check_operator(*polish) && *polish != '~') {
    if (stack->next && stack->next->next) {
      switch (*polish) {
        case ('+'):
          stack->next->number += stack->number;
          break;
        case ('-'):
          stack->next->number -= stack->number;
          break;
        case ('*'):
          stack->next->number *= stack->number;
          break;
        case ('/'):
          stack->next->number /= stack->number;
          break;
        case ('^'):
          stack->next->number = pow(stack->next->number, stack->number);
          break;
        case ('m'):
          stack->next->number = fmod(stack->next->number, stack->number);
          break;
      }
      pop_int(stack);
    }
    polish++;
  }
  return polish;
}

char *exe_function(char *polish, Stack_int *stack) {
  if (check_function(*polish)) {
    switch (*polish) {
      case ('c'):
        stack->number = cos(stack->number);
        break;
      case ('s'):
        stack->number = sin(stack->number);
        break;
      case ('t'):
        stack->number = tan(stack->number);
        break;
      case ('C'):
        stack->number = acos(stack->number);
        break;
      case ('S'):
        stack->number = asin(stack->number);
        break;
      case ('T'):
        stack->number = atan(stack->number);
        break;
      case ('q'):
        stack->number = sqrt(stack->number);
        break;
      case ('l'):
        stack->number = log(stack->number);
        break;
      case ('L'):
        stack->number = log10(stack->number);
        break;
    }
    polish++;
  }
  return polish;
}