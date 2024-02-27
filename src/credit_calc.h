#ifndef SRC_CREDIT_CALC_H_
#define SRC_CREDIT_CALC_H_

#define ANNUITY 1
#define DIFFERENTIATED 2
#define CREDIT_EPC 1e-2

typedef struct {
  double sum;
  int num_months;
  double percent;
  int type;
} Input_data;

typedef struct {
  double *month_pay;
  double over_pay;
  double total_payout;
} Output_data;

void credit_calc(Input_data input, Output_data *output);
void calc_annuity(Input_data input, Output_data *output);
void calc_differentiated(Input_data input, Output_data *output);
void initial_output(Output_data *output, int num_months);
int check_input_data(Input_data *input);
void form_log_file(Input_data input, Output_data *output);

#endif  // SRC_CREDIT_CALC_H_