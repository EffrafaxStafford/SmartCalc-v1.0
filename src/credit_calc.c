#include "credit_calc.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "deposit_calc.h"

void credit_calc(Input_data input, Output_data *output) {
  if (check_input_data(&input)) {
    if (input.num_months <= 0) input.num_months = 1;
    initial_output(output, input.num_months);
    if (input.type == ANNUITY && output->month_pay) calc_annuity(input, output);
    if (input.type == DIFFERENTIATED && output->month_pay)
      calc_differentiated(input, output);
    form_log_file(input, output);
  }
}

void calc_annuity(Input_data input, Output_data *output) {
  if (input.percent < 0.01) input.percent = 0.01;
  // percent_on_months
  double pm = input.percent / 100. / 12.;
  // Коэффициент аннуитета
  double koef_A = pm / (1 - pow(1 + pm, -1 * input.num_months));
  for (int i = 0; i < input.num_months; i++) {
    output->month_pay[i] = round_num(input.sum * koef_A);
    output->total_payout += output->month_pay[i];
  }
  output->over_pay = output->total_payout - input.sum;
}

void calc_differentiated(Input_data input, Output_data *output) {
  // остаток долга по кредиту
  double remains_sum = input.sum;
  // часть основного долга
  const double part_sum = round_num(input.sum / input.num_months);
  for (int i = 0; i < input.num_months; i++) {
    output->month_pay[i] =
        round_num(part_sum + remains_sum * input.percent / 100. / 12.);
    output->total_payout += output->month_pay[i];
    remains_sum -= part_sum;
  }
  output->over_pay = output->total_payout - input.sum;
}

void initial_output(Output_data *output, int num_months) {
  output->month_pay = (double *)malloc(num_months * sizeof(double));
  output->over_pay = 0.;
  output->total_payout = 0.;
}

int check_input_data(Input_data *input) {
  return (input->num_months > 0. && input->sum > 0. && input->percent >= 0.01);
}

void form_log_file(Input_data input, Output_data *output) {
  FILE *file = fopen("./bin/logfiles/credit_calc.log", "w");

  fprintf(file, " № | Eжемесячный платеж\n");
  fprintf(file, "--------------------------\n");
  for (int i = 0; i < input.num_months; i++) {
    fprintf(file, "%.2d |\t\t%.2lf\n", i + 1, output->month_pay[i]);
  }
  fprintf(file, "--------------------------\n");
  fprintf(file,
          "Сумма кредита = %.2lf руб.\nСрок кредита = %d мес.\nПроцентная "
          "ставка = %.2lf %%\n",
          input.sum, input.num_months, input.percent);
  fclose(file);
}