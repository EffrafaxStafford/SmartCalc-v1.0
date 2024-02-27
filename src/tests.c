// Copyright 2022 effrafax
#include <check.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "calc.h"
#include "credit_calc.h"
#include "dekstra.h"
#include "deposit_calc.h"
#include "stack.h"
//*
//---------------test_stack---------------
START_TEST(test_stack_char) {
  Stack_char *st = init_stack_char();
  push_char(st, '1');
  push_char(st, '2');
  push_char(st, '3');
  ck_assert_int_eq(st->symbol, '3');
  ck_assert_int_eq(st->next->symbol, '2');
  ck_assert_int_eq(st->next->next->symbol, '1');
  pop_char(st);
  ck_assert_int_eq(st->symbol, '2');
  ck_assert_int_eq(st->next->symbol, '1');
  pop_char(st);
  ck_assert_int_eq(st->symbol, '1');
  destroy_char(st);
}
END_TEST
START_TEST(test_stack_int) {
  Stack_int *st = init_stack_int();
  push_int(st, 1.);
  push_int(st, 2.);
  push_int(st, 3.);
  ck_assert_double_le(fabs(st->number - 3.), EPS);
  ck_assert_double_le(fabs(st->next->number - 2.), EPS);
  ck_assert_double_le(fabs(st->next->next->number - 1.), EPS);
  pop_int(st);
  ck_assert_double_le(fabs(st->number - 2.), EPS);
  ck_assert_double_le(fabs(st->next->number - 1.), EPS);
  pop_int(st);
  ck_assert_double_le(fabs(st->number - 1.), EPS);
  destroy_int(st);
}
END_TEST
//---------------test_dekstra--------------
START_TEST(test_dekstra_0) {
  char *str = dekstra("(1+2)*4+3");
  int res = strcmp(str, "1 2 +4 *3 +");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_1) {
  char *str = dekstra("(10-15)*3");
  int res = strcmp(str, "10 15 -3 *");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_2) {
  char *str = dekstra("3.7+4.56*2/(1.4-5)^2");
  int res = strcmp(str, "3.7 4.56 2 *1.4 5 -2 ^/+");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_3) {
  char *str = dekstra("sin(1/2*5)");
  int res = strcmp(str, "1 2 /5 *s");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_4) {
  char *str = dekstra("acos(1/2*5)");
  int res = strcmp(str, "1 2 /5 *C");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_5) {
  char *str = dekstra("ln(1/2*5)");
  int res = strcmp(str, "1 2 /5 *l");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_6) {
  char *str = dekstra("log(1/2*5)");
  int res = strcmp(str, "1 2 /5 *L");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_7) {
  char *str = dekstra("2 / ( 3 + 2 )  * 5");
  int res = strcmp(str, "2 3 2 +/5 *");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_8) {
  char function[20] = "2 / ( 3 + 2 )  * 5";
  char *str = dekstra(function);
  int res = strcmp(str, "2 3 2 +/5 *");
  free(str);
  ck_assert_int_eq(res, 0);
}
END_TEST
START_TEST(test_dekstra_error_0) {
  char *str = dekstra("(1+2))*3");
  int res = 0;
  if (str == NULL) res = 1;
  ck_assert_int_eq(res, 1);
}
END_TEST
START_TEST(test_dekstra_error_1) {
  char *str = dekstra("((((1+2))*3");
  int res = 0;
  if (str == NULL) res = 1;
  ck_assert_int_eq(res, 1);
}
END_TEST
//---------------test_calc--------------
START_TEST(test_calc_0) {
  double res = calc("(1+2)*4+3", 0.);
  ck_assert_double_lt(fabs(res - 15.), EPS);
}
END_TEST
START_TEST(test_calc_1) {
  double res = calc("3.7+4.56*2/(1.4-5)^2", 0.);
  ck_assert_double_lt(fabs(res - 4.4037037037), EPS);
}
END_TEST
START_TEST(test_calc_2) {
  double res = calc("sin(1/2*5)", 0.);
  ck_assert_double_lt(fabs(res - 0.5984721441), EPS);
}
END_TEST
START_TEST(test_calc_3) {
  double res = calc("cos(1/2*5)", 0.);
  ck_assert_double_lt(fabs(res + 0.80114361554), EPS);
}
END_TEST
START_TEST(test_calc_4) {
  double res = calc("tan(1/2*5)", 0.);
  ck_assert_double_lt(fabs(res + 0.74702229723), EPS);
}
END_TEST
START_TEST(test_calc_5) {
  double res = calc("asin(1/2)", 0.);
  ck_assert_double_lt(fabs(res - 0.523598776), EPS);
}
END_TEST
START_TEST(test_calc_6) {
  double res = calc("acos(1/2)", 0.);
  ck_assert_double_lt(fabs(res - 1.04719755), EPS);
}
END_TEST
START_TEST(test_calc_7) {
  double res = calc("atan(1/2)", 0.);
  ck_assert_double_lt(fabs(res - 0.463647609), EPS);
}
END_TEST
START_TEST(test_calc_8) {
  double res = calc("sqrt(1/2*5)", 0.);
  ck_assert_double_lt(fabs(res - 1.5811388), EPS);
}
END_TEST
START_TEST(test_calc_9) {
  double res = calc("ln(1/2*5)", 0.);
  ck_assert_double_lt(fabs(res - 0.91629073187), EPS);
}
END_TEST
START_TEST(test_calc_10) {
  double res = calc("log(2)", 0.);
  ck_assert_double_lt(fabs(res - 0.30102999566), EPS);
}
END_TEST
START_TEST(test_calc_11) {
  double res = calc("12.4 mod 2.354", 0.);
  ck_assert_double_lt(fabs(res - 0.63), EPS);
}
END_TEST
START_TEST(test_calc_12) {
  double res = calc("12345679 * 9", 0.);
  ck_assert_double_lt(fabs(res - 111111111), EPS);
}
END_TEST
START_TEST(test_calc_13) {
  double res = calc(
      "sqrt(log(ln(sin(7)+cos(4)*tan(2)+acos(1)+asin(1)*atan(7))))^7.5", 0.);
  ck_assert_double_lt(fabs(res - 0.0011892791057943808), EPS);
}
END_TEST
START_TEST(test_calc_14) {
  double res = calc("2ln(ln(ln((-1)(-1)x)))", 18);
  ck_assert_double_lt(fabs(res - 0.11914956265), EPS);
}
END_TEST
START_TEST(test_calc_15) {
  double res = calc("-xcos(0)sqrt(4)log(10)(-x)x", 2);
  ck_assert_double_lt(fabs(res - 16), EPS);
}
END_TEST
START_TEST(test_calc_16) {
  double res = calc("2*1e+2", 0.);
  ck_assert_double_lt(fabs(res - 200), EPS);
}
END_TEST
START_TEST(test_calc_17) {
  double res = calc("sin(cos(12e-1^2x))", 2);
  ck_assert_double_lt(fabs(res + 0.82260615913), EPS);
}
END_TEST
START_TEST(test_calc_18) {
  double res = calc("log(1e+28)", 0.);
  ck_assert_double_lt(fabs(res - 28), EPS);
}
END_TEST
START_TEST(test_calc_19) {
  double res = calc("3xxx", 2);
  ck_assert_double_lt(fabs(res - 24), EPS);
}
END_TEST
START_TEST(test_calc_20) {
  double res = calc("5.34 mod 0.29", 2);
  ck_assert_double_lt(fabs(res - 0.12), EPS);
}
END_TEST
START_TEST(test_calc_21) {
  double res = calc("(7)()", 2);
  ck_assert_double_lt(fabs(res - 7), EPS);
}
END_TEST
START_TEST(test_calc_22) {
  double res = calc("()(6)", 2);
  ck_assert_double_lt(fabs(res - 6), EPS);
}
END_TEST
START_TEST(test_calc_23) {
  double res = calc("((()()))", 2);
  ck_assert_double_lt(fabs(res - 0), EPS);
}
END_TEST
START_TEST(test_calc_24) {
  double res = calc("((()(9)))", 2);
  ck_assert_double_lt(fabs(res - 9), EPS);
}
END_TEST

//---------------test_credit_calc--------------
START_TEST(test_credit_calc_0) {
  Input_data input = {100000., 12, 18, ANNUITY};
  Output_data output;
  credit_calc(input, &output);
  free(output.month_pay);
  ck_assert_double_lt(fabs(output.total_payout - 110016.00), CREDIT_EPC);
  ck_assert_double_lt(fabs(output.over_pay - 10016.00), CREDIT_EPC);
}
END_TEST
START_TEST(test_credit_calc_1) {
  Input_data input = {100000., 12, 18, DIFFERENTIATED};
  Output_data output;
  credit_calc(input, &output);
  free(output.month_pay);
  ck_assert_double_lt(fabs(output.total_payout - 109749.96), CREDIT_EPC);
  ck_assert_double_lt(fabs(output.over_pay - 9749.96), CREDIT_EPC);
}
END_TEST
//---------------test_deposit_calc--------------
START_TEST(test_deposit_calc_0) {
  int list_deposits_date[4] = {1, 5, 10, -1};
  double list_deposits_sum[3] = {100., 200., 300.};
  int list_withdrawals_date[4] = {2, 6, 7, -1};
  double list_withdrawals_sum[3] = {15., 520., 100.};
  Input_data_dc input = {100000,
                         3,
                         5,
                         13,
                         EVERY_DAY,
                         SIMPLE_PERCENT,
                         list_deposits_date,
                         list_deposits_sum,
                         list_withdrawals_date,
                         list_withdrawals_sum};
  Output_data_dc output;
  deposit_calc(input, &output);
  ck_assert_double_lt(fabs(output.accrued_percent - 1232.1), CREDIT_EPC);
  ck_assert_double_eq(output.tax_amount, 0.);
  ck_assert_double_lt(fabs(output.sum_output - 99965.), CREDIT_EPC);
}
END_TEST
START_TEST(test_deposit_calc_1) {
  int list_deposits_date[4] = {0};
  double list_deposits_sum[3] = {0};
  int list_withdrawals_date[4] = {0};
  double list_withdrawals_sum[3] = {0};
  Input_data_dc input = {123456789,
                         12,
                         5,
                         13,
                         EVERY_WEEK,
                         SIMPLE_PERCENT,
                         list_deposits_date,
                         list_deposits_sum,
                         list_withdrawals_date,
                         list_withdrawals_sum};
  Output_data_dc output;
  deposit_calc(input, &output);
  ck_assert_double_lt(fabs(output.accrued_percent - 6088279.89), CREDIT_EPC);
  ck_assert_double_lt(fabs(output.sum_output - 123456789.), CREDIT_EPC);
  ck_assert_double_eq(output.tax_amount, 0.);
}
END_TEST
START_TEST(test_deposit_calc_2) {
  int list_deposits_date[4] = {0};
  double list_deposits_sum[3] = {0};
  int list_withdrawals_date[4] = {0};
  double list_withdrawals_sum[3] = {0};
  Input_data_dc input = {100000,
                         12,
                         5,
                         13,
                         EVERY_WEEK,
                         COMPLEX_PERCENT,
                         list_deposits_date,
                         list_deposits_sum,
                         list_withdrawals_date,
                         list_withdrawals_sum};
  Output_data_dc output;
  deposit_calc(input, &output);
  ck_assert_double_lt(fabs(output.accrued_percent - 5052.65), CREDIT_EPC);
  ck_assert_double_eq(output.tax_amount, 0.);
  ck_assert_double_lt(fabs(output.sum_output - 105052.65), CREDIT_EPC);
}
END_TEST

Suite *stack(void) {
  Suite *s = suite_create("stack");
  TCase *tc_stack = tcase_create("stack");
  //---------------test_stack---------------
  tcase_add_test(tc_stack, test_stack_char);
  tcase_add_test(tc_stack, test_stack_int);
  //---------------test_dekstra--------------
  tcase_add_test(tc_stack, test_dekstra_0);
  tcase_add_test(tc_stack, test_dekstra_1);
  tcase_add_test(tc_stack, test_dekstra_2);
  tcase_add_test(tc_stack, test_dekstra_3);
  tcase_add_test(tc_stack, test_dekstra_4);
  tcase_add_test(tc_stack, test_dekstra_5);
  tcase_add_test(tc_stack, test_dekstra_6);
  tcase_add_test(tc_stack, test_dekstra_7);
  tcase_add_test(tc_stack, test_dekstra_8);
  tcase_add_test(tc_stack, test_dekstra_error_0);
  tcase_add_test(tc_stack, test_dekstra_error_1);
  //---------------test_calc--------------
  tcase_add_test(tc_stack, test_calc_0);
  tcase_add_test(tc_stack, test_calc_1);
  tcase_add_test(tc_stack, test_calc_2);
  tcase_add_test(tc_stack, test_calc_3);
  tcase_add_test(tc_stack, test_calc_4);
  tcase_add_test(tc_stack, test_calc_5);
  tcase_add_test(tc_stack, test_calc_6);
  tcase_add_test(tc_stack, test_calc_7);
  tcase_add_test(tc_stack, test_calc_8);
  tcase_add_test(tc_stack, test_calc_9);
  tcase_add_test(tc_stack, test_calc_10);
  tcase_add_test(tc_stack, test_calc_11);
  tcase_add_test(tc_stack, test_calc_12);
  tcase_add_test(tc_stack, test_calc_13);
  tcase_add_test(tc_stack, test_calc_14);
  tcase_add_test(tc_stack, test_calc_15);
  tcase_add_test(tc_stack, test_calc_16);
  tcase_add_test(tc_stack, test_calc_17);
  tcase_add_test(tc_stack, test_calc_18);
  tcase_add_test(tc_stack, test_calc_19);
  tcase_add_test(tc_stack, test_calc_20);
  tcase_add_test(tc_stack, test_calc_21);
  tcase_add_test(tc_stack, test_calc_22);
  tcase_add_test(tc_stack, test_calc_23);
  tcase_add_test(tc_stack, test_calc_24);
  //-----------test_credit_calc------------
  tcase_add_test(tc_stack, test_credit_calc_0);
  tcase_add_test(tc_stack, test_credit_calc_1);
  //-----------test_deposit_calc------------
  tcase_add_test(tc_stack, test_deposit_calc_0);
  tcase_add_test(tc_stack, test_deposit_calc_1);
  tcase_add_test(tc_stack, test_deposit_calc_2);

  suite_add_tcase(s, tc_stack);
  return s;
}
//*/
int main() {
  Suite *s = stack();
  SRunner *sr = srunner_create(s);
  // srunner_set_fork_status(sr, CK_NOFORK);
  srunner_run_all(sr, CK_NORMAL);  //  CK_VERBOSE
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (!number_failed) ? EXIT_SUCCESS : EXIT_FAILURE;
}
