#ifndef SRC_GRAPH_H_
#define SRC_GRAPH_H_
#include <gtk/gtk.h>

#include "calc.h"
#include "credit_calc.h"
#include "deposit_calc.h"
#define MAX_LEN_FUNCTION 255

typedef struct {
  GtkText *input;
  GtkText *arg_x;
  GtkText *output;
  GtkWidget *grid_sc;
  GtkWidget *grid_cc;
  GtkWidget *grid_dc;
  GtkWidget *area;
  char text_min_y[MAX_LEN_FUNCTION];
  char text_max_y[MAX_LEN_FUNCTION];
  char text_min_x[MAX_LEN_FUNCTION];
  char text_max_x[MAX_LEN_FUNCTION];
  cairo_t *cr;
} User_data;

typedef struct {
  double min_y;
  double max_y;
  double min_x;
  double max_x;
} Border_values;

typedef struct {
  GtkText *sum;
  GtkText *num_months;
  GtkText *percent;
  GtkWidget *type;
  GtkWidget *grid_cc;
} User_data_cc;

typedef struct {
  GtkText *sum;
  GtkText *num_months;
  GtkText *percent_rate;
  GtkText *tax_rate;
  GtkWidget *period_pay;
  GtkWidget *capitalization_percent;
  GtkWidget *grid_dc;
  int num_line;  // номер строки последнего пополнения
  int num_deposit;          // количество пополнении
  int num_line_withdrawal;  // номер строки последнего снятия
  int num_withdrawal;  // количество снятий
} User_data_dc;

// static void destroy(GtkWidget *, gpointer);
//  Создаем окно SmartCalc v1.0
static void activate(GtkApplication *app, gpointer data);
const char *GtkText_get_const_char_str(GtkText *text);
void add_label_GtkText_label_in_grid(GtkWidget *grid, const char *label1,
                                     GtkWidget *widget, const char *label2,
                                     int num_line);
//--------------------------smart_calc----------------------------------
void fill_grid_sc(GtkWidget *grid_sc);
static void button_equal(GtkWidget *widget, gpointer data);
static void button_show_graph(GtkWidget *widget, gpointer data);
static void button_update(GtkWidget *widget, gpointer data);
static void button_remove_graph(GtkWidget *widget, gpointer data);

static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width,
                          int height, gpointer data);
void draw_mesh(cairo_t *cr, int width, int height, char *function,
               Border_values border);
void clear_grid(cairo_t *cr, int width, int height);
void corrected_range(double *min_y, double *max_y, double *min_x,
                     double *max_x);
void write_value_grid(cairo_t *cr, double value, int width, int height);
//--------------------------credit_calc----------------------------------
void fill_grid_cc(GtkWidget *grid_cc);
static void button_сс(GtkWidget *widget, gpointer data);
void button_show_log_file_cc();
void show_result_in_grid_cc(User_data_cc *data, Output_data od, int num_months);
void show_month_pay_in_grid_cc(User_data_cc *data, Output_data od,
                               int num_months);
void show_over_pay_in_grid_cc(User_data_cc *data, Output_data od);
void show_total_payout_in_grid_cc(User_data_cc *data, Output_data od);
void clear_result_in_grid_cc(User_data_cc *data);
//--------------------------deposit_calc----------------------------------
void fill_grid_dc(GtkWidget *grid_dc);
static void button_add_deposit(GtkWidget *widget, gpointer data);
static void button_delete_deposit(GtkWidget *widget, gpointer data);
static void button_add_withdrawal(GtkWidget *widget, gpointer data);
static void button_delete_withdrawal(GtkWidget *widget, gpointer data);
static void button_dc(GtkWidget *widget, gpointer data);
int *get_list_deposits_date(User_data_dc *data);
double *get_list_deposits_sum(User_data_dc *data);
int *get_list_withdrawals_date(User_data_dc *data);
double *get_list_withdrawals_sum(User_data_dc *data);
void show_result_in_grid_dc(User_data_dc *data, Output_data_dc od);
void button_show_log_file_dc();
void clear_result_in_grid_dc(User_data_dc *data, int num_line);
void show_accrued_percent_in_grid_dc(User_data_dc *data, Output_data_dc od,
                                     int num_line);
void show_tax_amount_in_grid_dc(User_data_dc *data, Output_data_dc od,
                                int num_line);
void show_sum_output_in_grid_dc(User_data_dc *data, Output_data_dc od,
                                int num_line);
void correct_period_pay(int *period_pay);

#endif  // SRC_GRAPH_H_