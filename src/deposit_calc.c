#include "deposit_calc.h"

#include <stdio.h>
#include <stdlib.h>

void deposit_calc(Input_data_dc input, Output_data_dc *output) {
  if (check_input_dc(&input)) {
    initial_output_dc(output);
    // срок вклада в днях
    int days = input.num_months * 30;
    if (input.period_pay == THE_END_DATE) input.period_pay = days;
    // сумма процента за заданный период
    double pp = (input.percent_rate / 100. / 365.) * input.period_pay;
    double sum_percent_for_year = 0.;
    int size = days / input.period_pay + 1;
    if (days % input.period_pay != 0) size++;
    Log_data_dc log_data;
    int check_log = create_log_data(&log_data, size);
    if (check_log) {
      log_data.add_deposit[0] = input.sum;
      log_data.remains_deposit[0] = input.sum;
    }
    for (int i = 0, add = 0, delete = 0, log = 1; i < days && check_log; i++) {
      if (input.list_deposits_date && i + 1 == input.list_deposits_date[add]) {
        input.sum += input.list_deposits_sum[add];
        log_data.add_deposit[log] += input.list_deposits_sum[add++];
      }
      if (input.list_withdrawals_date &&
          (i + 1 == input.list_withdrawals_date[delete]) &&
          (input.sum >= input.list_withdrawals_date[delete])) {
        input.sum -= input.list_withdrawals_sum[delete];
        log_data.add_deposit[log] -= input.list_withdrawals_sum[delete ++];
      }
      if ((i + 1) % input.period_pay == 0) {
        double delta_tax = round_num(input.sum * pp);
        output->accrued_percent += delta_tax;
        if (i > 359) sum_percent_for_year += delta_tax;
        if (input.capitalization_percent == COMPLEX_PERCENT) {
          input.sum += delta_tax;
          log_data.add_deposit[log] += delta_tax;
        }
        log_data.accrued_percent[log] = delta_tax;
        log_data.remains_deposit[log++] = input.sum;
      }
      if (((i + 1) % 360 == 0 || (i + 1) == days) && i > 360) {
        calc_tax_amount(input, output, sum_percent_for_year);
        sum_percent_for_year = 0.;
      }
    }
    calc_deposit_on_remain(&input, output, &log_data);
    output->sum_output = input.sum;

    form_log_file_dc(output, log_data);
    free_log_data(&log_data);
  }
}

void initial_output_dc(Output_data_dc *output) {
  output->accrued_percent = 0.;
  output->tax_amount = 0.;
  output->sum_output = 0.;
}

int create_log_data(Log_data_dc *log_data, int size) {
  log_data->accrued_percent = (double *)malloc(size * sizeof(double));
  log_data->add_deposit = (double *)malloc(size * sizeof(double));
  log_data->remains_deposit = (double *)malloc(size * sizeof(double));
  log_data->size = size;
  fill_array_zero(log_data->accrued_percent, size);
  fill_array_zero(log_data->add_deposit, size);
  fill_array_zero(log_data->remains_deposit, size);
  return (log_data->accrued_percent && log_data->add_deposit &&
          log_data->remains_deposit);
}

void fill_array_zero(double *array, int size) {
  if (array) {
    for (int i = 0; i < size; i++) {
      array[i] = 0.;
    }
  }
}

void free_log_data(Log_data_dc *log_data) {
  free(log_data->accrued_percent);
  free(log_data->add_deposit);
  free(log_data->remains_deposit);
}

int check_input_dc(Input_data_dc *input) {
  return (input->sum > 0 && input->num_months > 0 &&
          input->percent_rate >= 0.01 && input->tax_rate >= 0.01);
}

void calc_deposit_on_remain(Input_data_dc *input, Output_data_dc *output,
                            Log_data_dc *log_data) {
  // остаток дней для выплаты процентов
  int remains_day = (input->num_months * 30) % input->period_pay;
  if (remains_day) {
    double pp = (input->percent_rate / 100. / 365.) * remains_day;
    double delta_tax = round_num(input->sum * pp);
    output->accrued_percent += delta_tax;
    if (input->capitalization_percent == COMPLEX_PERCENT) {
      input->sum += delta_tax;
      log_data->add_deposit[log_data->size - 1] = delta_tax;
    }
    log_data->accrued_percent[log_data->size - 1] = delta_tax;
    log_data->remains_deposit[log_data->size - 1] = input->sum;
  }
}

void calc_tax_amount(Input_data_dc input, Output_data_dc *output,
                     double sum_percent_for_year) {
  if (sum_percent_for_year > AMOUNT_WITHOUT_TAX) {
    double delta = sum_percent_for_year - AMOUNT_WITHOUT_TAX;
    output->tax_amount += round_num(delta * input.tax_rate / 100.);
  }
}

double round_num(double num) {
  char num_str[255] = {'\0'};
  sprintf(num_str, "%.2lf", num);
  return atof(num_str);
}

void form_log_file_dc(Output_data_dc *output, Log_data_dc log_data) {
  FILE *file = fopen("./bin/logfiles/deposit_calc.log", "w");
  fprintf(file,
          " № | Начислено процентов | Вклад пополнен | Остаток на вкладе\n");
  fprintf(file,
          "--------------------------------------------------------------\n");
  for (int i = 0; i < log_data.size; i++) {
    fprintf(file, "%.3d| %.2lf | %.2lf | %.2lf\n", i,
            log_data.accrued_percent[i], log_data.add_deposit[i],
            log_data.remains_deposit[i]);
  }
  fprintf(file,
          "--------------------------------------------------------------\n");
  fprintf(file,
          "Начисленные проценты %.2lf руб.\nCумма налога %.2lf руб.\nCумма на "
          "вкладе к концу срока %.2lf руб.\n",
          output->accrued_percent, output->tax_amount, output->sum_output);
  fclose(file);
}
