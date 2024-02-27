// Copyright 2022 effrafax
#include "graph.h"

#include <gtk/gtk.h>
#include <math.h>
#include <string.h>

User_data user_data;
User_data_cc user_data_cc;
User_data_dc user_data_dc;

int main(int argc, char *argv[]) {
  GtkApplication *app;
  app = gtk_application_new("org.gtk.smartcalc", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}

// Создаем окно SmartCalc v1.0
static void activate(GtkApplication *app, gpointer data) {
  GtkWidget *window;
  GtkWidget *notebook;
  GtkWidget *grid_sc, *grid_cc, *grid_dc;
  GtkWidget *label;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "SmartCalc_v1.0");
  // // задаем размер окна
  // gtk_window_set_default_size(GTK_WINDOW(window), 200, 100);
  //  запрещаем изменение размеров окна
  gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
  // // уничтожение окна после закрытия
  // gtk_window_set_hide_on_close(GTK_WINDOW(window), FALSE);

  notebook = gtk_notebook_new();
  gtk_window_set_child(GTK_WINDOW(window), notebook);
  grid_sc = gtk_grid_new();
  label = gtk_label_new("Smart calc");
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), grid_sc, label);
  grid_cc = gtk_grid_new();
  label = gtk_label_new("Credit calc");
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), grid_cc, label);
  grid_dc = gtk_grid_new();
  label = gtk_label_new("Deposit calc");
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), grid_dc, label);

  fill_grid_sc(grid_sc);
  fill_grid_cc(grid_cc);
  fill_grid_dc(grid_dc);

  gtk_widget_show(window);
}

void fill_grid_sc(GtkWidget *grid_sc) {
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *input, *arg_x, *output;
  // устанавливает одинаковую высоту строк
  gtk_grid_set_row_homogeneous(GTK_GRID(grid_sc), TRUE);

  label = gtk_label_new("f(x) = ");
  gtk_grid_attach(GTK_GRID(grid_sc), label, 0, 0, 1, 1);
  input = gtk_text_new();
  gtk_text_set_max_length((GtkText *)input, 255);
  gtk_text_set_placeholder_text((GtkText *)input, "Введите функцию");
  gtk_grid_attach(GTK_GRID(grid_sc), input, 1, 0, 1, 1);

  label = gtk_label_new("x = ");
  gtk_grid_attach(GTK_GRID(grid_sc), label, 0, 1, 1, 1);
  arg_x = gtk_text_new();
  gtk_text_set_placeholder_text((GtkText *)arg_x, "Введите x (x = 0)");
  gtk_grid_attach(GTK_GRID(grid_sc), arg_x, 1, 1, 1, 1);

  output = gtk_text_new();
  gtk_text_set_placeholder_text((GtkText *)output, "Решение");
  gtk_grid_attach(GTK_GRID(grid_sc), output, 1, 3, 1, 1);

  user_data.input = (GtkText *)input;
  user_data.arg_x = (GtkText *)arg_x;
  user_data.output = (GtkText *)output;
  user_data.grid_sc = grid_sc;
  strcpy(user_data.text_min_y, "-1000000");
  strcpy(user_data.text_max_y, "+1000000");
  strcpy(user_data.text_min_x, "-1000000");
  strcpy(user_data.text_max_x, "+1000000");

  button = gtk_button_new_with_label("=");
  g_signal_connect(button, "clicked", G_CALLBACK(button_equal), &user_data);
  gtk_grid_attach(GTK_GRID(grid_sc), button, 1, 2, 1, 1);

  button = gtk_button_new_with_label("Показать график");
  gtk_grid_attach(GTK_GRID(grid_sc), button, 1, 4, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_show_graph),
                   &user_data);
}

static void button_equal(GtkWidget *widget, gpointer data) {
  const char *function = GtkText_get_const_char_str(((User_data *)data)->input);
  const char *arg_x = GtkText_get_const_char_str(((User_data *)data)->arg_x);
  double x = atof(arg_x);
  double res = calc((char *)function, x);
  char res_str[255] = {'\0'};
  sprintf(res_str, "%.7lf", res);
  GtkEntryBuffer *buffer_output = gtk_entry_buffer_new(res_str, -1);
  gtk_text_set_buffer(((User_data *)data)->output, buffer_output);
}

static void button_show_graph(GtkWidget *widget, gpointer data) {
  GtkWidget *area = gtk_drawing_area_new();
  ((User_data *)data)->area = area;
  GtkWidget *grid = ((User_data *)data)->grid_sc;
  gtk_grid_attach(GTK_GRID(grid), area, 0, 0, 5, 5);
  gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area), 600);
  gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area), 300);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_function, data,
                                 NULL);

  gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);

  GtkWidget *button_graph2 = gtk_button_new_with_label("Обновить");
  gtk_grid_attach(GTK_GRID(grid), button_graph2, 0, 5, 1, 1);
  g_signal_connect(button_graph2, "clicked", G_CALLBACK(button_update),
                   &user_data);

  GtkWidget *button_graph1 = gtk_button_new_with_label("Скрыть график");
  gtk_grid_attach(GTK_GRID(grid), button_graph1, 0, 6, 1, 1);
  g_signal_connect(button_graph1, "clicked", G_CALLBACK(button_remove_graph),
                   &user_data);

  GtkWidget *label_min_y, *label_max_y, *label_min_x, *label_max_x;
  GtkWidget *min_y, *max_y, *min_x, *max_x;
  GtkEntryBuffer *buffer_min_y, *buffer_max_y, *buffer_min_x, *buffer_max_x;

  buffer_min_y = gtk_entry_buffer_new(((User_data *)data)->text_min_y, -1);
  buffer_max_y = gtk_entry_buffer_new(((User_data *)data)->text_max_y, -1);
  buffer_min_x = gtk_entry_buffer_new(((User_data *)data)->text_min_x, -1);
  buffer_max_x = gtk_entry_buffer_new(((User_data *)data)->text_max_x, -1);

  label_min_y = gtk_label_new("min_y = ");
  gtk_grid_attach(GTK_GRID(grid), label_min_y, 1, 5, 1, 1);
  min_y = gtk_text_new_with_buffer(buffer_min_y);
  gtk_grid_attach(GTK_GRID(grid), min_y, 2, 5, 1, 1);

  label_max_y = gtk_label_new("max_y = ");
  gtk_grid_attach(GTK_GRID(grid), label_max_y, 1, 6, 1, 1);
  max_y = gtk_text_new_with_buffer(buffer_max_y);
  gtk_grid_attach(GTK_GRID(grid), max_y, 2, 6, 1, 1);

  label_min_x = gtk_label_new("min_x = ");
  gtk_grid_attach(GTK_GRID(grid), label_min_x, 3, 5, 1, 1);
  min_x = gtk_text_new_with_buffer(buffer_min_x);
  gtk_grid_attach(GTK_GRID(grid), min_x, 4, 5, 1, 1);

  label_max_x = gtk_label_new("max_x = ");
  gtk_grid_attach(GTK_GRID(grid), label_max_x, 3, 6, 1, 1);
  max_x = gtk_text_new_with_buffer(buffer_max_x);
  gtk_grid_attach(GTK_GRID(grid), max_x, 4, 6, 1, 1);
}

static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width,
                          int height, gpointer data) {
  // границы отрисовки
  int w = width - width / 5, h = height - height / 5;
  char *function =
      (char *)GtkText_get_const_char_str(((User_data *)data)->input);
  double min_y = atof(((User_data *)data)->text_min_y);
  double max_y = atof(((User_data *)data)->text_max_y);
  double min_x = atof(((User_data *)data)->text_min_x);
  double max_x = atof(((User_data *)data)->text_max_x);
  corrected_range(&min_y, &max_y, &min_x, &max_x);
  Border_values border = {min_y, max_y, min_x, max_x};
  double range_y = fabs(max_y - min_y);
  double range_x = fabs(max_x - min_x);
  double delta_x = range_x / w;
  double x = min_x;
  double y = calc(function, x);
  int i = width / 10;  // счетчик для цикла for (начало отрисовки графика)
  // начальные точки отрисовки
  while ((isnan(y) || y < min_y || y > max_y) && x <= max_x) {
    x += delta_x;
    y = calc(function, x);
    i++;
  }

  clear_grid(cr, width, height);
  draw_mesh(cr, width, height, function, border);

  cairo_set_line_width(cr, 2);
  cairo_set_source_rgb(cr, 0, 1, 0);  //зеленый цвет
  double koef = h / range_y;
  double j = koef * y;
  double j_pred = j;
  double shift_h = h - h * fabs(max_y) / range_y;
  h = range_y;
  for (i += 1; i <= w + width / 10; i++) {
    x += delta_x;
    y = calc(function, x);
    j = koef * y;
    if (!isnan(y) && y >= min_y && y <= max_y) {
      cairo_move_to(cr, (i - 1), (height - height / 10) - (j_pred + shift_h));
      cairo_line_to(cr, i, (height - height / 10) - (j + shift_h));
      cairo_stroke(cr);
    }
    j_pred = j;
  }
  cairo_fill(cr);
}

void clear_grid(cairo_t *cr, int width, int height) {
  cairo_set_source_rgb(cr, 255, 255, 255);  //  белый цвет
  cairo_set_line_width(cr, height);
  cairo_move_to(cr, 0, height / 2);
  cairo_line_to(cr, width, height / 2);
  cairo_stroke(cr);
}

void draw_mesh(cairo_t *cr, int width, int height, char *function,
               Border_values border) {
  int delta = 6;
  int shift_y = width / 10;
  int shift_x = height / 10;

  cairo_set_source_rgb(cr, 0, 0, 0);  // черный цвет
  cairo_set_line_width(cr, 0.1);
  // рисуем линии оси Y
  double tmp_x = border.min_x;
  double delta_x = (border.max_x - border.min_x) / delta;
  for (int i = shift_y; i <= width - shift_y;
       i += (width - 2 * shift_y) / delta) {
    if (i == shift_y) cairo_set_line_width(cr, 2);
    cairo_move_to(cr, i, shift_x);
    cairo_line_to(cr, i, height - shift_x);
    cairo_stroke(cr);
    if (i == shift_y) cairo_set_line_width(cr, 0.1);
    write_value_grid(cr, tmp_x, i, height - 10);
    tmp_x += delta_x;
  }
  // рисуем линии оси X
  double tmp_y = border.max_y;
  double delta_y = (border.max_y - border.min_y) / delta;
  for (int i = shift_x; i <= height - shift_x;
       i += (height - 2 * shift_x) / delta) {
    if (i == height - shift_x) cairo_set_line_width(cr, 2);
    cairo_move_to(cr, shift_y, i);
    cairo_line_to(cr, width - shift_y, i);
    cairo_stroke(cr);
    if (i == height - shift_x) cairo_set_line_width(cr, 0.1);
    write_value_grid(cr, tmp_y, 0, i);
    tmp_y -= delta_y;
  }
  // подписываем оси
  cairo_move_to(cr, shift_y, shift_x / 2);
  cairo_show_text(cr, "Y");
  cairo_move_to(cr, width - shift_y / 2, height - shift_x);
  cairo_show_text(cr, "X");
  // функция
  cairo_move_to(cr, width / 2 - 25, shift_x / 2);
  cairo_show_text(cr, "f(x) = ");
  cairo_move_to(cr, width / 2, shift_x / 2);
  cairo_show_text(cr, function);
}

static void button_update(GtkWidget *widget, gpointer data) {
  GtkWidget *grid = ((User_data *)data)->grid_sc;
  GtkText *min_y = (GtkText *)gtk_grid_get_child_at(GTK_GRID(grid), 2, 5);
  GtkText *max_y = (GtkText *)gtk_grid_get_child_at(GTK_GRID(grid), 2, 6);
  GtkText *min_x = (GtkText *)gtk_grid_get_child_at(GTK_GRID(grid), 4, 5);
  GtkText *max_x = (GtkText *)gtk_grid_get_child_at(GTK_GRID(grid), 4, 6);
  strcpy(((User_data *)data)->text_min_y, GtkText_get_const_char_str(min_y));
  strcpy(((User_data *)data)->text_max_y, GtkText_get_const_char_str(max_y));
  strcpy(((User_data *)data)->text_min_x, GtkText_get_const_char_str(min_x));
  strcpy(((User_data *)data)->text_max_x, GtkText_get_const_char_str(max_x));

  button_remove_graph(widget, data);
  button_show_graph(widget, data);
}

static void button_remove_graph(GtkWidget *widget, gpointer data) {
  GtkWidget *grid = ((User_data *)data)->grid_sc;
  gtk_grid_remove(GTK_GRID(grid), ((User_data *)data)->area);
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 0, 5));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 0, 6));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 1, 5));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 1, 6));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 2, 5));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 2, 6));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 3, 5));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 3, 6));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 4, 5));
  gtk_grid_remove(GTK_GRID(grid), gtk_grid_get_child_at(GTK_GRID(grid), 4, 6));

  gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
}

void corrected_range(double *min_y, double *max_y, double *min_x,
                     double *max_x) {
  if (*min_y < -1000000.) *min_y = -1000000.;
  if (*max_y > 1000000.) *max_y = 1000000.;
  if (*min_x < -1000000.) *min_x = -1000000.;
  if (*max_x > 1000000.) *max_x = 1000000.;
}

void write_value_grid(cairo_t *cr, double value, int width, int height) {
  char str[MAX_LEN_FUNCTION] = {'\0'};
  sprintf(str, "%.2lf", value);
  cairo_move_to(cr, width, height);
  cairo_show_text(cr, str);
}

const char *GtkText_get_const_char_str(GtkText *text) {
  GtkEntryBuffer *buffer = gtk_text_get_buffer(text);
  return gtk_entry_buffer_get_text(buffer);
}

//--------------------------credit_calc----------------------------------
void fill_grid_cc(GtkWidget *grid_cc) {
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *sum, *num_months, *percent;
  // устанавливает одинаковую высоту строк
  gtk_grid_set_row_homogeneous(GTK_GRID(grid_cc), TRUE);

  label = gtk_label_new("Сумма кредита");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 0, 0, 1, 1);
  sum = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_cc), sum, 1, 0, 1, 1);
  label = gtk_label_new("руб.");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 2, 0, 1, 1);

  label = gtk_label_new("Срок кредита");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 0, 1, 1, 1);
  num_months = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_cc), num_months, 1, 1, 1, 1);
  label = gtk_label_new("месяцев");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 2, 1, 1, 1);

  label = gtk_label_new("Процентная\nставка");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 0, 2, 1, 1);
  percent = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_cc), percent, 1, 2, 1, 1);
  label = gtk_label_new("%");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 2, 2, 1, 1);

  label = gtk_label_new("Тип ежемесячных\nплатежей");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 0, 3, 1, 2);
  button = gtk_check_button_new_with_label("Аннуитетные");
  gtk_grid_attach(GTK_GRID(grid_cc), button, 1, 3, 1, 1);
  GtkWidget *group = gtk_check_button_new_with_label("Дифференцированные");
  gtk_grid_attach(GTK_GRID(grid_cc), group, 1, 4, 1, 1);
  gtk_check_button_set_group(GTK_CHECK_BUTTON(button), GTK_CHECK_BUTTON(group));
  gtk_check_button_set_active(GTK_CHECK_BUTTON(button), TRUE);

  user_data_cc.sum = (GtkText *)sum;
  user_data_cc.num_months = (GtkText *)num_months;
  user_data_cc.percent = (GtkText *)percent;
  user_data_cc.type = button;  // тип - Аннуитетный
  user_data_cc.grid_cc = grid_cc;

  button = gtk_button_new_with_label("Рассчитать");
  gtk_grid_attach(GTK_GRID(grid_cc), button, 1, 5, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_сс), &user_data_cc);

  Output_data od = {NULL, 0, 0};
  show_result_in_grid_cc(&user_data_cc, od, 0);
}

static void button_сс(GtkWidget *widget, gpointer data) {
  double sum = atof(GtkText_get_const_char_str(((User_data_cc *)data)->sum));
  int num_months =
      atoi(GtkText_get_const_char_str(((User_data_cc *)data)->num_months));
  double percent =
      atof(GtkText_get_const_char_str(((User_data_cc *)data)->percent));
  int type = gtk_check_button_get_active(
                 GTK_CHECK_BUTTON(((User_data_cc *)data)->type))
                 ? ANNUITY
                 : DIFFERENTIATED;
  if (isnan(sum) || isnan(percent)) {
    sum = 0.;
    num_months = 0;
    percent = 0.01;
  }
  Input_data id = {sum, num_months, percent, type};
  Output_data od = {NULL, 0, 0};
  credit_calc(id, &od);
  show_result_in_grid_cc((User_data_cc *)data, od, num_months);
}

void button_show_log_file_cc() {
  FILE *file = fopen("./bin/logfiles/credit_calc.log", "r");
  if (file) {
    fclose(file);
    system("open ./bin/logfiles/credit_calc.log");
  }
}

void show_result_in_grid_cc(User_data_cc *data, Output_data od,
                            int num_months) {
  clear_result_in_grid_cc(data);
  show_month_pay_in_grid_cc((User_data_cc *)data, od, num_months);
  show_over_pay_in_grid_cc((User_data_cc *)data, od);
  show_total_payout_in_grid_cc((User_data_cc *)data, od);

  GtkWidget *button = gtk_button_new_with_label(".log");
  gtk_grid_attach(GTK_GRID(data->grid_cc), button, 2, 5, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_show_log_file_cc),
                   NULL);
}

void show_month_pay_in_grid_cc(User_data_cc *data, Output_data od,
                               int num_months) {
  GtkWidget *grid_cc = ((User_data_cc *)data)->grid_cc;
  GtkWidget *label = gtk_label_new("Ежемесячный платеж: ");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 0, 6, 1, 1);
  GtkWidget *month_pay = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_cc), month_pay, 1, 6, 1, 1);
  label = gtk_label_new("руб.");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 2, 6, 1, 1);
  if (num_months <= 0) num_months = 1;
  char month_pay_str_first[255] = {'\0'};
  char month_pay_str_last[255] = {'\0'};
  if (od.month_pay) {
    sprintf(month_pay_str_first, "%.2lf", od.month_pay[0]);
    sprintf(month_pay_str_last, "%.2lf", od.month_pay[num_months - 1]);
  } else {
    sprintf(month_pay_str_first, "%.2lf", 0.);
    sprintf(month_pay_str_last, "%.2lf", 0.);
  }
  strcat(month_pay_str_first, " ... ");
  strcat(month_pay_str_first, month_pay_str_last);
  GtkEntryBuffer *buffer_output = gtk_entry_buffer_new(month_pay_str_first, -1);
  gtk_text_set_buffer((GtkText *)month_pay, buffer_output);
  // free(od.month_pay);
}

void show_over_pay_in_grid_cc(User_data_cc *data, Output_data od) {
  GtkWidget *grid_cc = ((User_data_cc *)data)->grid_cc;
  GtkWidget *label = gtk_label_new("Переплата по кредиту: ");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 0, 7, 1, 1);
  char over_pay_str[255] = {'\0'};
  sprintf(over_pay_str, "%.2lf", od.over_pay);
  GtkWidget *over_pay = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_cc), over_pay, 1, 7, 1, 1);
  GtkEntryBuffer *buffer_output = gtk_entry_buffer_new(over_pay_str, -1);
  gtk_text_set_buffer((GtkText *)over_pay, buffer_output);
  label = gtk_label_new("руб.");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 2, 7, 1, 1);
}

void show_total_payout_in_grid_cc(User_data_cc *data, Output_data od) {
  GtkWidget *grid_cc = ((User_data_cc *)data)->grid_cc;
  GtkWidget *label = gtk_label_new("Общая выплата: ");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 0, 8, 1, 1);
  char total_payout_str[255] = {'\0'};
  sprintf(total_payout_str, "%.2lf", od.total_payout);
  GtkWidget *total_payout = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_cc), total_payout, 1, 8, 1, 1);
  GtkEntryBuffer *buffer_output = gtk_entry_buffer_new(total_payout_str, -1);
  gtk_text_set_buffer((GtkText *)total_payout, buffer_output);
  label = gtk_label_new("руб.");
  gtk_grid_attach(GTK_GRID(grid_cc), label, 2, 8, 1, 1);
}

void clear_result_in_grid_cc(User_data_cc *data) {
  GtkWidget *grid = ((User_data_cc *)data)->grid_cc;
  if (gtk_grid_get_child_at(GTK_GRID(grid), 0, 6)) {
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 0, 6));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 1, 6));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 2, 6));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 0, 7));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 1, 7));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 2, 7));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 0, 8));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 1, 8));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 2, 8));
  }
}

//--------------------------deposit_calc----------------------------------
void fill_grid_dc(GtkWidget *grid_dc) {
  GtkWidget *button;
  GtkWidget *label;
  GtkWidget *sum, *num_months, *percent_rate, *tax_rate,
      *capitalization_percent;
  GtkWidget *period_pay;
  int num_line = 0;
  // устанавливает одинаковую высоту строк
  gtk_grid_set_row_homogeneous(GTK_GRID(grid_dc), TRUE);

  label = gtk_label_new("Сумма вклада");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  sum = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_dc), sum, 1, num_line, 1, 1);
  label = gtk_label_new("руб.");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 2, num_line, 1, 1);
  num_line = 1;
  label = gtk_label_new("Срок размещения");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  num_months = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_dc), num_months, 1, num_line, 1, 1);
  label = gtk_label_new("месяцев");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 2, num_line, 1, 1);
  num_line = 2;
  label = gtk_label_new("Процентная ставка");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  percent_rate = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_dc), percent_rate, 1, num_line, 1, 1);
  label = gtk_label_new("%");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 2, num_line, 1, 1);
  num_line = 3;
  label = gtk_label_new("Налоговая ставка");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  tax_rate = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_dc), tax_rate, 1, num_line, 1, 1);
  label = gtk_label_new("%");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 2, num_line, 1, 1);
  num_line = 4;
  label = gtk_label_new("Периодичность выплат");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  const char *const strings[8] = {
      "каждый день",   "каждую неделю", "каждый месяц",  "раз в квартал",
      "раз в полгода", "раз в год",     "в конце срока", NULL};
  period_pay = gtk_drop_down_new_from_strings(strings);
  gtk_grid_attach(GTK_GRID(grid_dc), period_pay, 1, num_line, 1, 1);
  num_line = 5;
  label = gtk_label_new("Капитализация\nпроцентов");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  capitalization_percent = gtk_check_button_new();
  gtk_grid_attach(GTK_GRID(grid_dc), capitalization_percent, 1, num_line, 1, 1);
  num_line = 6;
  label = gtk_label_new("Пополнения");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  button = gtk_button_new_with_label("Добавить пополнение");
  gtk_grid_attach(GTK_GRID(grid_dc), button, 1, num_line, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_add_deposit),
                   &user_data_dc);

  user_data_dc.sum = (GtkText *)sum;
  user_data_dc.num_months = (GtkText *)num_months;
  user_data_dc.percent_rate = (GtkText *)percent_rate;
  user_data_dc.tax_rate = (GtkText *)tax_rate;
  user_data_dc.period_pay = period_pay;
  user_data_dc.capitalization_percent = capitalization_percent;
  user_data_dc.grid_dc = grid_dc;
  user_data_dc.num_deposit = 0;
  user_data_dc.num_line = num_line;
  user_data_dc.num_withdrawal = 0;
  user_data_dc.num_line_withdrawal = num_line + 1;

  label = gtk_label_new("Снятия");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, user_data_dc.num_line_withdrawal,
                  1, 1);
  button = gtk_button_new_with_label("Добавить снятие");
  gtk_grid_attach(GTK_GRID(grid_dc), button, 1,
                  user_data_dc.num_line_withdrawal, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_add_withdrawal),
                   &user_data_dc);

  button = gtk_button_new_with_label("Рассчитать");
  gtk_grid_attach(GTK_GRID(grid_dc), button, 1,
                  user_data_dc.num_line_withdrawal + 1, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_dc), &user_data_dc);

  button = gtk_button_new_with_label(".log");
  gtk_grid_attach(GTK_GRID(grid_dc), button, 2,
                  user_data_dc.num_line_withdrawal + 1, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_show_log_file_dc),
                   NULL);

  Output_data_dc od = {0, 0, 0};
  show_result_in_grid_dc(&user_data_dc, od);
}

static void button_add_deposit(GtkWidget *widget, gpointer data) {
  ((User_data_dc *)data)->num_deposit++;
  ((User_data_dc *)data)->num_line++;
  ((User_data_dc *)data)->num_line_withdrawal++;
  int num_line = ((User_data_dc *)data)->num_line;
  GtkWidget *grid_dc = ((User_data_dc *)data)->grid_dc;
  gtk_grid_insert_row(GTK_GRID(grid_dc), num_line);

  GtkWidget *button;
  GtkWidget *deposit_day, *deposit_sum;
  deposit_day = gtk_text_new();
  gtk_text_set_placeholder_text((GtkText *)deposit_day, "День");
  gtk_grid_attach(GTK_GRID(grid_dc), deposit_day, 1, num_line, 1, 1);
  deposit_sum = gtk_text_new();
  gtk_text_set_placeholder_text((GtkText *)deposit_sum, "Сумма");
  gtk_grid_attach(GTK_GRID(grid_dc), deposit_sum, 2, num_line, 1, 1);
  button = gtk_button_new_with_label("-");
  gtk_grid_attach(GTK_GRID(grid_dc), button, 3, num_line, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_delete_deposit),
                   &user_data_dc);
}

static void button_delete_deposit(GtkWidget *widget, gpointer data) {
  if (((User_data_dc *)data)->num_deposit > 0) {
    ((User_data_dc *)data)->num_deposit--;
    int num_line = ((User_data_dc *)data)->num_line--;
    GtkWidget *grid_dc = ((User_data_dc *)data)->grid_dc;
    gtk_grid_remove_row(GTK_GRID(grid_dc), num_line);
    ((User_data_dc *)data)->num_line_withdrawal--;
  }
}

static void button_add_withdrawal(GtkWidget *widget, gpointer data) {
  ((User_data_dc *)data)->num_withdrawal++;
  ((User_data_dc *)data)->num_line_withdrawal++;
  int num_line = ((User_data_dc *)data)->num_line_withdrawal;
  GtkWidget *grid_dc = ((User_data_dc *)data)->grid_dc;
  gtk_grid_insert_row(GTK_GRID(grid_dc), num_line);

  GtkWidget *button;
  GtkWidget *withdrawal_day, *withdrawal_sum;
  withdrawal_day = gtk_text_new();
  gtk_text_set_placeholder_text((GtkText *)withdrawal_day, "День");
  gtk_grid_attach(GTK_GRID(grid_dc), withdrawal_day, 1, num_line, 1, 1);
  withdrawal_sum = gtk_text_new();
  gtk_text_set_placeholder_text((GtkText *)withdrawal_sum, "Сумма");
  gtk_grid_attach(GTK_GRID(grid_dc), withdrawal_sum, 2, num_line, 1, 1);
  button = gtk_button_new_with_label("-");
  gtk_grid_attach(GTK_GRID(grid_dc), button, 3, num_line, 1, 1);
  g_signal_connect(button, "clicked", G_CALLBACK(button_delete_withdrawal),
                   &user_data_dc);
}

static void button_delete_withdrawal(GtkWidget *widget, gpointer data) {
  if (((User_data_dc *)data)->num_withdrawal > 0) {
    ((User_data_dc *)data)->num_withdrawal--;
    int num_line = ((User_data_dc *)data)->num_line_withdrawal--;
    GtkWidget *grid_dc = ((User_data_dc *)data)->grid_dc;
    gtk_grid_remove_row(GTK_GRID(grid_dc), num_line);
  }
}

static void button_dc(GtkWidget *widget, gpointer data) {
  double sum = atof(GtkText_get_const_char_str(((User_data_dc *)data)->sum));
  int num_months =
      atoi(GtkText_get_const_char_str(((User_data_dc *)data)->num_months));
  double percent_rate =
      atof(GtkText_get_const_char_str(((User_data_dc *)data)->percent_rate));
  double tax_rate =
      atof(GtkText_get_const_char_str(((User_data_dc *)data)->tax_rate));
  int period_pay = (int)gtk_drop_down_get_selected(
      GTK_DROP_DOWN(((User_data_dc *)data)->period_pay));
  correct_period_pay(&period_pay);
  int capitalization_percent =
      gtk_check_button_get_active(
          GTK_CHECK_BUTTON(((User_data_dc *)data)->capitalization_percent))
          ? COMPLEX_PERCENT
          : SIMPLE_PERCENT;

  int *list_deposits_date = get_list_deposits_date(((User_data_dc *)data));
  double *list_deposits_sum = get_list_deposits_sum(((User_data_dc *)data));
  int *list_withdrawals_date =
      get_list_withdrawals_date(((User_data_dc *)data));
  double *list_withdrawals_sum =
      get_list_withdrawals_sum(((User_data_dc *)data));

  Input_data_dc id = {sum,
                      num_months,
                      percent_rate,
                      tax_rate,
                      period_pay,
                      capitalization_percent,
                      list_deposits_date,
                      list_deposits_sum,
                      list_withdrawals_date,
                      list_withdrawals_sum};

  Output_data_dc od = {0, 0, 0};
  deposit_calc(id, &od);
  free(list_deposits_date);
  free(list_deposits_sum);
  free(list_withdrawals_date);
  free(list_withdrawals_sum);
  show_result_in_grid_dc((User_data_dc *)data, od);
}

int *get_list_deposits_date(User_data_dc *data) {
  int *list_deposits_date = NULL;
  if (data->num_deposit > 0) {
    list_deposits_date = (int *)malloc((data->num_deposit + 1) * sizeof(int));
    if (list_deposits_date) {
      int num_deposit = data->num_deposit;
      int num_line = data->num_line;
      GtkWidget *grid_dc = data->grid_dc;
      while (num_deposit > 0) {
        GtkWidget *deposits_date =
            gtk_grid_get_child_at(GTK_GRID(grid_dc), 1, num_line);
        int deposits_date_int =
            atoi(GtkText_get_const_char_str(GTK_TEXT(deposits_date)));
        list_deposits_date[num_deposit - 1] = deposits_date_int;
        num_deposit--;
        num_line--;
      }
    }
  }
  return list_deposits_date;
}

double *get_list_deposits_sum(User_data_dc *data) {
  double *list_deposits_sum = NULL;
  if (data->num_deposit > 0) {
    list_deposits_sum =
        (double *)malloc((data->num_deposit + 1) * sizeof(double));
    if (list_deposits_sum) {
      int num_deposit = data->num_deposit;
      int num_line = data->num_line;
      GtkWidget *grid_dc = data->grid_dc;
      while (num_deposit > 0) {
        GtkWidget *deposits_sum =
            gtk_grid_get_child_at(GTK_GRID(grid_dc), 2, num_line);
        double deposits_sum_int =
            atof(GtkText_get_const_char_str(GTK_TEXT(deposits_sum)));
        list_deposits_sum[num_deposit - 1] = deposits_sum_int;
        num_deposit--;
        num_line--;
      }
    }
  }
  return list_deposits_sum;
}

int *get_list_withdrawals_date(User_data_dc *data) {
  int *list_withdrawals_date = NULL;
  if (data->num_withdrawal > 0) {
    list_withdrawals_date =
        (int *)malloc((data->num_withdrawal + 1) * sizeof(int));
    if (list_withdrawals_date) {
      int num_withdrawals = data->num_withdrawal;
      int num_line = data->num_line_withdrawal;
      GtkWidget *grid_dc = data->grid_dc;
      while (num_withdrawals > 0) {
        GtkWidget *withdrawals_date =
            gtk_grid_get_child_at(GTK_GRID(grid_dc), 1, num_line);
        int withdrawals_date_int =
            atoi(GtkText_get_const_char_str(GTK_TEXT(withdrawals_date)));
        list_withdrawals_date[num_withdrawals - 1] = withdrawals_date_int;
        num_withdrawals--;
        num_line--;
      }
    }
  }
  return list_withdrawals_date;
}

double *get_list_withdrawals_sum(User_data_dc *data) {
  double *list_withdrawals_sum = NULL;
  if (data->num_withdrawal > 0) {
    list_withdrawals_sum =
        (double *)malloc((data->num_withdrawal + 1) * sizeof(double));
    if (list_withdrawals_sum) {
      int num_withdrawals = data->num_withdrawal;
      int num_line = data->num_line_withdrawal;
      GtkWidget *grid_dc = data->grid_dc;
      while (num_withdrawals > 0) {
        GtkWidget *withdrawals_sum =
            gtk_grid_get_child_at(GTK_GRID(grid_dc), 2, num_line);
        double withdrawals_sum_int =
            atof(GtkText_get_const_char_str(GTK_TEXT(withdrawals_sum)));
        list_withdrawals_sum[num_withdrawals - 1] = withdrawals_sum_int;
        num_withdrawals--;
        num_line--;
      }
    }
  }
  return list_withdrawals_sum;
}

void show_result_in_grid_dc(User_data_dc *data, Output_data_dc od) {
  int num_line = ((User_data_dc *)data)->num_line_withdrawal + 3;
  clear_result_in_grid_dc(data, num_line);
  show_accrued_percent_in_grid_dc(data, od, num_line);
  show_tax_amount_in_grid_dc(data, od, num_line + 1);
  show_sum_output_in_grid_dc(data, od, num_line + 2);
}

void button_show_log_file_dc() {
  FILE *file = fopen("./bin/logfiles/deposit_calc.log", "r");
  if (file) {
    fclose(file);
    system("open ./bin/logfiles/deposit_calc.log");
  }
}

void clear_result_in_grid_dc(User_data_dc *data, int num_line) {
  GtkWidget *grid = data->grid_dc;
  if (gtk_grid_get_child_at(GTK_GRID(grid), 0, num_line)) {
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 0, num_line));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 1, num_line));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 2, num_line));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 0, num_line + 1));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 1, num_line + 1));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 2, num_line + 1));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 0, num_line + 2));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 1, num_line + 2));
    gtk_grid_remove(GTK_GRID(grid),
                    gtk_grid_get_child_at(GTK_GRID(grid), 2, num_line + 2));
  }
}

void show_accrued_percent_in_grid_dc(User_data_dc *data, Output_data_dc od,
                                     int num_line) {
  GtkWidget *grid_dc = data->grid_dc;
  GtkWidget *label = gtk_label_new("Начисленные проценты: ");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  char accrued_percent_str[255] = {'\0'};
  sprintf(accrued_percent_str, "%.2lf", od.accrued_percent);
  GtkWidget *accrued_percent = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_dc), accrued_percent, 1, num_line, 1, 1);
  GtkEntryBuffer *buffer_output = gtk_entry_buffer_new(accrued_percent_str, -1);
  gtk_text_set_buffer((GtkText *)accrued_percent, buffer_output);
  label = gtk_label_new("руб.");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 2, num_line, 1, 1);
}

void show_tax_amount_in_grid_dc(User_data_dc *data, Output_data_dc od,
                                int num_line) {
  GtkWidget *grid_dc = data->grid_dc;
  GtkWidget *label = gtk_label_new("Сумма налога: ");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  char tax_amount_str[255] = {'\0'};
  sprintf(tax_amount_str, "%.2lf", od.tax_amount);
  GtkWidget *tax_amount = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_dc), tax_amount, 1, num_line, 1, 1);
  GtkEntryBuffer *buffer_output = gtk_entry_buffer_new(tax_amount_str, -1);
  gtk_text_set_buffer((GtkText *)tax_amount, buffer_output);
  label = gtk_label_new("руб.");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 2, num_line, 1, 1);
}

void show_sum_output_in_grid_dc(User_data_dc *data, Output_data_dc od,
                                int num_line) {
  GtkWidget *grid_dc = data->grid_dc;
  GtkWidget *label = gtk_label_new("Сумма на вкладе\nк концу срока: ");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 0, num_line, 1, 1);
  char sum_output_str[255] = {'\0'};
  sprintf(sum_output_str, "%.2lf", od.sum_output);
  GtkWidget *sum_output = gtk_text_new();
  gtk_grid_attach(GTK_GRID(grid_dc), sum_output, 1, num_line, 1, 1);
  GtkEntryBuffer *buffer_output = gtk_entry_buffer_new(sum_output_str, -1);
  gtk_text_set_buffer((GtkText *)sum_output, buffer_output);
  label = gtk_label_new("руб.");
  gtk_grid_attach(GTK_GRID(grid_dc), label, 2, num_line, 1, 1);
}

void correct_period_pay(int *period_pay) {
  switch (*period_pay) {
    case (0):
      *period_pay = EVERY_DAY;
      break;
    case (1):
      *period_pay = EVERY_WEEK;
      break;
    case (2):
      *period_pay = EVERY_MONTH;
      break;
    case (3):
      *period_pay = EVERY_THREE_MONTH;
      break;
    case (4):
      *period_pay = EVERY_SIX_MONTHS;
      break;
    case (5):
      *period_pay = EVERY_YEAR;
      break;
    case (6):
      *period_pay = THE_END_DATE;
      break;
  }
}
