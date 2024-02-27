#ifndef SRC_DEPOSIT_CALC_H_
#define SRC_DEPOSIT_CALC_H_

#define KEY_BID_CB 7.5  // ключевая ставка ЦБ
#define AMOUNT_WITHOUT_TAX 75000  // сумма дохода, необлагаемая налогом

#define ONE_TIME 0
#define EVERY_DAY 1
#define EVERY_WEEK 7
#define EVERY_MONTH 30
#define EVERY_TWO_MONTH 60
#define EVERY_THREE_MONTH 90
#define EVERY_SIX_MONTHS 180
#define EVERY_YEAR 360
#define THE_END_DATE 8

#define SIMPLE_PERCENT 1
#define COMPLEX_PERCENT 2

typedef struct {
  double sum;                  // сумма вклада
  int num_months;              // срок размещения
  double percent_rate;         // процентная ставка
  double tax_rate;             // налоговая ставка
  int period_pay;              // периодичность выплат
  int capitalization_percent;  // капитализация процентов
  int *list_deposits_date;     // список дней пополнений
  double *list_deposits_sum;   // список суммы пополнений
  int *list_withdrawals_date;  // список дней снятий
  double *list_withdrawals_sum;  // список cуммы снятий
} Input_data_dc;

typedef struct {
  double accrued_percent;  // начисленные проценты
  double tax_amount;       // сумма налога
  double sum_output;  // сумма на вкладе к концу срока
} Output_data_dc;

typedef struct {
  double *accrued_percent;  // начисленные проценты
  double *add_deposit;      // вклад пополнен
  double *remains_deposit;  // остаток на вкладе
  int size;                 // размер массивов
} Log_data_dc;

void deposit_calc(Input_data_dc input, Output_data_dc *output);
void initial_output_dc(Output_data_dc *output);
int create_log_data(Log_data_dc *log_data, int size);
void fill_array_zero(double *array, int size);
void free_log_data(Log_data_dc *log_data);
int check_input_dc(Input_data_dc *input);
void calc_deposit_on_remain(Input_data_dc *input, Output_data_dc *output,
                            Log_data_dc *log_data);
void calc_tax_amount(Input_data_dc input, Output_data_dc *output,
                     double sum_percent_for_year);
double round_num(double num);
void form_log_file_dc(Output_data_dc *output, Log_data_dc log_data);

#endif  // SRC_DEPOSIT_CALC_H_