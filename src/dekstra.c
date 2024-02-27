// Copyright 2022 effrafax
#include "dekstra.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"

char *dekstra(char *input_function) {
  int result = SUCCESS;
  char *ptr_output_string = NULL;
  char *fun_for_dekstra = function_for_dekstra(input_function);
  if (fun_for_dekstra && check_input_function(fun_for_dekstra)) {
    Stack_char *stack = init_stack_char();
    if (stack) {
      char *output_string = memory_allocation(2 * strlen(input_function));
      if (output_string) {
        ptr_output_string = output_string;
        char *pfun_for_dekstra = fun_for_dekstra;
        while (*pfun_for_dekstra) {
          pfun_for_dekstra = is_open_parenthesis(pfun_for_dekstra, stack);
          pfun_for_dekstra = is_number(pfun_for_dekstra, output_string);
          output_string = end_str(output_string);
          pfun_for_dekstra = is_function(pfun_for_dekstra, stack);
          pfun_for_dekstra =
              is_operator(pfun_for_dekstra, output_string, stack);
          output_string = end_str(output_string);
          pfun_for_dekstra =
              is_closing_parenthesis(pfun_for_dekstra, output_string, stack);
          output_string = end_str(output_string);
          if (pfun_for_dekstra == fun_for_dekstra) {
            result = FAILURE;
            break;
          }
        }
        if (output_string != ptr_output_string)
          from_stack_to_str(output_string, stack);
      }
    }
    destroy_char(stack);
  }
  free(fun_for_dekstra);
  if (result == FAILURE) {
    free(ptr_output_string);
    ptr_output_string = NULL;
  }
  // printf("ptr_output_string = %s\n", ptr_output_string);
  return ptr_output_string;
}

int check_input_function(char *function) {
  int result = SUCCESS;
  while (*function && result) {
    if (!(check_number(*function) || check_exponent_number(*function) ||
          check_function(*function) || check_operator(*function) ||
          *function == '(' || *function == ')')) {
      result = FAILURE;
    }
    function++;
  }
  return result;
}

char *function_for_dekstra(char *input_function) {
  char *fun_for_dekstra = NULL;
  if (!check_input_functuon(input_function)) {
    fun_for_dekstra = memory_allocation(2 * strlen(input_function));
    if (fun_for_dekstra) {
      removing_spaces(fun_for_dekstra, input_function);
      corrected_function(fun_for_dekstra);
      corrected_unary_operator(fun_for_dekstra);
      corrected_mult_operator(fun_for_dekstra);
    }
  }
  return fun_for_dekstra;
}

char *memory_allocation(int len) {
  char *output_string = (char *)malloc(len * sizeof(char));
  for (int i = 0; i < len; i++) {
    *(output_string + i) = '\0';
  }
  return output_string;
}
int check_input_functuon(char *input_function) {
  int result = 0;
  while (*input_function) {
    if (*input_function == '(') result++;
    if (*input_function == ')') result--;
    input_function++;
  }
  return result;
}
void removing_spaces(char *fun_for_dekstra, char *input_function) {
  while (*input_function) {
    while (*input_function == ' ') input_function++;
    if (*input_function) *fun_for_dekstra++ = *input_function++;
  }
  *fun_for_dekstra = '\0';
}

void corrected_function(char *str) {
  swap_substr_to_symbol(str, "acos", 'C');
  swap_substr_to_symbol(str, "cos", 'c');
  swap_substr_to_symbol(str, "asin", 'S');
  swap_substr_to_symbol(str, "sin", 's');
  swap_substr_to_symbol(str, "atan", 'T');
  swap_substr_to_symbol(str, "tan", 't');
  swap_substr_to_symbol(str, "sqrt", 'q');
  swap_substr_to_symbol(str, "ln", 'l');
  swap_substr_to_symbol(str, "log", 'L');
  swap_substr_to_symbol(str, "mod", 'm');
}
void swap_substr_to_symbol(char *str, const char *substr, char symbol) {
  char *ptr = strstr(str, substr);
  while (ptr) {
    int len_substr = strlen(substr);
    char *ptr_end_substr = ptr + len_substr;
    *ptr++ = symbol;
    while (*ptr_end_substr) {
      *ptr++ = *ptr_end_substr++;
    }
    *ptr = '\0';
    ptr = strstr(str, substr);
  }
}
void corrected_unary_operator(char *str) {
  unary_operator(str);  // проверяем первый символ функции
  while (*str) {
    if (check_operator(*str) || *str == '(')
      unary_operator(++str);
    else
      str++;
  }
}
void unary_operator(char *str) {
  if (*str == '-')
    *str = '~';
  else if (*str == '+')
    unary_operator_plus(str);
}
void unary_operator_plus(char *str) {
  while (*str) {
    *str = *(str + 1);
    str++;
  }
}

void corrected_mult_operator(char *fun_for_dekstra) {
  int flag_start = 0;
  if (*(fun_for_dekstra + 1) && *fun_for_dekstra == 'x' &&
      (check_number(*(fun_for_dekstra + 1)) || *(fun_for_dekstra + 1) == '(')) {
    fun_for_dekstra++;
    flag_start = 1;
    add_symbol(fun_for_dekstra, '*');
  }
  while (*fun_for_dekstra) {
    while (check_number(*fun_for_dekstra) && *fun_for_dekstra != 'x') {
      fun_for_dekstra++;
      flag_start = 1;
    }

    if (*(fun_for_dekstra + 1) &&
        (*fun_for_dekstra == ')' || *fun_for_dekstra == 'x' ||
         check_number(*fun_for_dekstra)) &&
        (check_function(*(fun_for_dekstra + 1)) ||
         check_number(*(fun_for_dekstra + 1)) ||
         *(fun_for_dekstra + 1) == '(')) {
      add_symbol(fun_for_dekstra + 1, '*');
      flag_start = 1;
    }

    if (flag_start &&
        (*fun_for_dekstra == '(' || *fun_for_dekstra == 'x' ||
         check_function(*fun_for_dekstra)) &&
        (check_number(*(fun_for_dekstra - 1)) || *(fun_for_dekstra - 1) == ')'))
      add_symbol(fun_for_dekstra, '*');
    fun_for_dekstra++;
  }
}
void add_symbol(char *ptr, char symbol) {
  size_t len = strlen(ptr);
  while (len) {
    *(ptr + len + 1) = *(ptr + len);
    len--;
  }
  *(ptr + 1) = *ptr;
  *ptr = symbol;
}

char *end_str(char *str) {
  while (*str) str++;
  return str;
}

void from_stack_to_str(char *str, Stack_char *stack) {
  while (stack->symbol) {
    *str++ = stack->symbol;
    pop_char(stack);
  }
  *str = '\0';
}

int check_number(char symbol) {
  return ((symbol >= '0' && symbol <= '9') || symbol == '.' || symbol == 'x')
             ? SUCCESS
             : FAILURE;
}
int check_exponent_number(char symbol) {
  return (symbol == 'e' || symbol == 'E' || symbol == '+' || symbol == '-' ||
          (symbol >= '0' && symbol <= '9'))
             ? SUCCESS
             : FAILURE;
}

char *is_number(char *function, char *output_string) {
  if (check_number(*function)) {
    while (check_number(*function)) *output_string++ = *function++;
    if (*function == 'e' || *function == 'E') {
      while (check_exponent_number(*function)) *output_string++ = *function++;
    }
    *output_string++ = ' ';  // разделитель между числами
  }
  *output_string = '\0';
  return function;
}

int check_function(char symbol) {
  return (symbol == 'c' || symbol == 's' || symbol == 't' || symbol == 'l' ||
          symbol == 'C' || symbol == 'S' || symbol == 'T' || symbol == 'q' ||
          symbol == 'L')
             ? SUCCESS
             : FAILURE;
}

char *is_function(char *function, Stack_char *stack) {
  if (check_function(*function)) push_char(stack, *function++);
  return function;
}

int check_operator(char symbol) {
  return (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/' ||
          symbol == '^' || symbol == 'm' || symbol == '~')
             ? SUCCESS
             : FAILURE;
}

char *is_operator(char *function, char *output_string, Stack_char *stack) {
  if (check_operator(*function)) {
    while (check_operator(stack->symbol) &&
           priority(stack->symbol) >= priority(*function)) {
      *output_string++ = stack->symbol;
      pop_char(stack);
    }
    *output_string = '\0';
    push_char(stack, *function);
    function++;
  }
  return function;
}

int priority(char operator_O) {
  int result = 0;
  switch (operator_O) {
    case ('+'):
    case ('-'):
      result = 3;
      break;
    case ('*'):
    case ('/'):
      result = 5;
      break;
    case ('^'):
      result = 6;
      break;
    case ('~'):
      result = 8;
      break;
    case ('m'):
      result = 4;
      break;
  }
  return result;
}

char *is_open_parenthesis(char *function, Stack_char *stack) {
  if (*function == '(') {
    function++;
    push_char(stack, '(');
  }
  return function;
}

char *is_closing_parenthesis(char *function, char *output_string,
                             Stack_char *stack) {
  if (*function == ')') {
    function++;
    while (stack->symbol != '(') {
      *output_string++ = stack->symbol;
      pop_char(stack);
    }
    if (stack->symbol == '(') pop_char(stack);
    while (function && stack->symbol && check_function(stack->symbol) &&
           stack->symbol != '(') {
      *output_string++ = stack->symbol;
      pop_char(stack);
    }
    *output_string = '\0';
  }
  return function;
}
