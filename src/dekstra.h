#ifndef SRC_DEKSTRA_H_
#define SRC_DEKSTRA_H_
#include "stack.h"
#define SUCCESS 1
#define FAILURE 0

int check_input_function(char *function);
// преобразует входную функцию в постфиксную форму
char *dekstra(char *input_function);
// выделяет память и заполняет элементы массива '\0'
char *memory_allocation(int len);
// изменяет входную строку под алгоритм dekstra()
char *function_for_dekstra(char *input_function);
// проверяет корректность входной функции
int check_input_functuon(char *input_function);
// удаляет пробелы в input_function
void removing_spaces(char *fun_for_dekstra, char *input_function);
// меняет подстроку substr на символ symbol в str
void swap_substr_to_symbol(char *str, const char *substr, char symbol);
// меняет функции в fun_for_dekstra на символы
void corrected_function(char *str);
// обработка унарных операторов [+] и [-]
void corrected_unary_operator(char *str);
// унарный [-] меняем на [~]
// унарный [+] удаляем
void unary_operator(char *str);
void unary_operator_plus(char *str);
// добавляет [*] если оператор [*] опущен
void corrected_mult_operator(char *fun_for_dekstra);
// добавляет symbol по указателю *ptr
void add_symbol(char *ptr, char symbol);

char *end_str(char *str);
void from_stack_to_str(char *str, Stack_char *stack);
int check_number(char symbol);
int check_exponent_number(char symbol);
char *is_number(char *function, char *output_string);
int check_function(char symbol);
char *is_function(char *function, Stack_char *stack);
int check_operator(char symbol);
char *is_operator(char *function, char *output_string, Stack_char *stack);
int priority(char operator_O);
char *is_open_parenthesis(char *function, Stack_char *stack);
char *is_closing_parenthesis(char *function, char *output_string,
                             Stack_char *stack);
#endif  //  SRC_DEKSTRA_H_

// cos - c
// sin - s
// tan - t
// acos - C
// asin - S
// atan - T
// sqrt - q
// ln - l
// log - L
// mod - m