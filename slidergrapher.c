#include <gtk/gtk.h>
#include <math.h>
#include <pffft.h>
#include <stdio.h>
#include <stdlib.h>

#include "loader.h"
#include "windowfunction.h"

static gchar *flag_txtfft_file = NULL;
static gint flag_fft_window_size = 2048;
static gdouble flag_plot_size = 200;
static gdouble flag_top_val = 100.0;
static gboolean flag_auto = FALSE;
static gint flag_auto_speed = 4;
static gboolean flag_log_scale = TRUE;
static gboolean flag_show_windowed = TRUE;
static gboolean flag_show_nonwindowed = FALSE;
static gboolean flag_show_nonsummed = FALSE;
static GOptionEntry slidergrapher_option_entries[] = {
    {"txtfft_file", 'f', 0, G_OPTION_ARG_FILENAME, &flag_txtfft_file,
     "Name of the input file.", "<txtfft file>"},
    {"fft_window_size", 's', 0, G_OPTION_ARG_INT, &flag_fft_window_size,
     "Size of the fft window", "<int>"},
    {"plot_size", 'p', 0, G_OPTION_ARG_DOUBLE, &flag_plot_size,
     "Part of the fft window to be drawn", "<double>"},
    {"top_val", 't', 0, G_OPTION_ARG_DOUBLE, &flag_top_val,
     "Highest displayed value", "<double>"},
    {"auto", 'a', 0, G_OPTION_ARG_NONE, &flag_auto, "Autoplay", ""},
    {"auto_speed", 'r', 0, G_OPTION_ARG_INT, &flag_auto_speed,
     "Speed of the autoplay in number of frames per tick", "<int>"},
    {"log_scale", 'l', 0, G_OPTION_ARG_NONE, &flag_log_scale,
     "If y-axis should be logarithmic", "<bool>"},
    {"show_windowed", 'w', 0, G_OPTION_ARG_NONE, &flag_show_windowed,
     "Show the Fourier-transform of the windowed function", ""},
    {"show_nonwindowed", 'o', 0, G_OPTION_ARG_NONE, &flag_show_nonwindowed,
     "Show the Fourier-transform of the original function", ""},
    {"show_nonsummed", 'u', 0, G_OPTION_ARG_NONE, &flag_show_nonsummed,
     "Show the Fourier-transform of the non-summed vectors of original "
     "function if log_scale is false",
     ""},
    G_OPTION_ENTRY_NULL};

static GdkRGBA green_color;
static GdkRGBA black_color;
static GdkRGBA blue_color;
static GdkRGBA red_color;

typedef struct {
  float *data;
  int data_len;

  int fft_window_size;
  double plot_size;
  double top_val;

  int cursor;
  gboolean playing;
  int auto_speed;

  float *abs_buf;
  float *abs_buf_w;

  WindowFunction wf;

  PFFFT_Setup *setup;
  float *input_buf;
  float *output_buf;
  float *work_buf;

  GtkWidget *area;
  GtkWidget *slider;
  GtkWidget *play_control;

  gdouble mouse_x;
  gdouble plot_begin;
} GameState;

GameState *game_state_new() {
  GameState *game_state = (GameState *)malloc(sizeof(GameState));
  if (!game_state) {
    fprintf(stderr, "GameState malloc in activate failed\n");
    exit(1);
  }

  return game_state;
}

void set_play_button_label(GameState *game_state) {
  gtk_button_set_label(GTK_BUTTON(game_state->play_control),
                       game_state->playing ? "⏸" : "▶");
}

void game_state_init(GameState *game_state, char *file_name,
                     int fft_window_size, double plot_size, double top_val) {
  int rc = read_txtfft(file_name, &game_state->data, &game_state->data_len);
  if (rc < 0) {
    fprintf(stderr, "read_txtfft failed\n");
    exit(1);
  }

  game_state->fft_window_size = fft_window_size;
  game_state->plot_size = flag_plot_size;
  game_state->top_val = flag_top_val;

  game_state->cursor = 0;

  game_state->setup = pffft_new_setup(fft_window_size, PFFFT_REAL);

  window_function_setup(&game_state->wf, game_state->fft_window_size);

  game_state->input_buf =
      (float *)pffft_aligned_malloc(sizeof(float) * fft_window_size);
  if (!game_state->input_buf) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  game_state->output_buf =
      (float *)pffft_aligned_malloc(sizeof(float) * fft_window_size);
  if (!game_state->output_buf) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  game_state->work_buf =
      (float *)pffft_aligned_malloc(sizeof(float) * fft_window_size * 2);
  if (!game_state->work_buf) {
    fprintf(stderr, "pffft_aligned_malloc failed to allocate memory\n");
    exit(1);
  }

  game_state->abs_buf = (float *)malloc(fft_window_size / 2 * sizeof(float));
  if (!game_state->abs_buf) {
    fprintf(stderr, "malloc failed to allocate memory\n");
    exit(1);
  }

  game_state->abs_buf_w = (float *)malloc(fft_window_size / 2 * sizeof(float));
  if (!game_state->abs_buf_w) {
    fprintf(stderr, "malloc failed to allocate memory\n");
    exit(1);
  }

  game_state->mouse_x = 0;
  game_state->plot_begin = 0;
}

void game_state_fft(GameState *game_state, int cursor) {
  memcpy(game_state->input_buf, game_state->data + cursor,
         game_state->fft_window_size * sizeof(float));
  if (flag_show_nonwindowed) {
    pffft_transform_ordered(game_state->setup, game_state->input_buf,
                            game_state->output_buf, game_state->work_buf,
                            PFFFT_FORWARD);
    for (int i = 0; i < game_state->fft_window_size / 2; ++i) {
      game_state->abs_buf[i] =
          sqrt(pow(game_state->output_buf[i * 2], 2.0) +
               pow(game_state->output_buf[i * 2 + 1], 2.0));
    }
  }

  if (flag_show_windowed) {
    window_function_apply_hann(&game_state->wf, game_state->input_buf,
                               game_state->fft_window_size);
    pffft_transform_ordered(game_state->setup, game_state->input_buf,
                            game_state->output_buf, game_state->work_buf,
                            PFFFT_FORWARD);
    for (int i = 0; i < game_state->fft_window_size / 2; ++i) {
      game_state->abs_buf_w[i] =
          sqrt(pow(game_state->output_buf[i * 2], 2.0) +
               pow(game_state->output_buf[i * 2 + 1], 2.0));
    }
  }
}

static void fine_step(GameState *game_state, int delta) {
  game_state->cursor += delta;

  if (game_state->cursor < 0) {
    game_state->cursor = 0;
  } else if (game_state->cursor >
             game_state->data_len - game_state->fft_window_size) {
    game_state->cursor = game_state->data_len - game_state->fft_window_size;
  }

  double slider_val = 100.0 * (double)game_state->cursor /
                      (game_state->data_len - game_state->fft_window_size);
  gtk_range_set_value(GTK_RANGE(game_state->slider), slider_val);
  gtk_widget_queue_draw(game_state->area);
}

static gboolean auto_step(GtkWidget *widget, GdkFrameClock *frame_clock,
                          gpointer user_data) {
  GameState *game_state = (GameState *)user_data;

  if (!game_state->playing) {
    return G_SOURCE_REMOVE;
  }

  if (game_state->cursor >=
      game_state->data_len - game_state->fft_window_size) {
    game_state->playing = FALSE;
    set_play_button_label(game_state);
    return G_SOURCE_REMOVE;
  }

  fine_step(game_state, game_state->auto_speed);
  return G_SOURCE_CONTINUE;
}

static void left_button_clicked(GtkButton *button, gpointer user_data) {
  fine_step((GameState *)user_data, -1);
}

static void right_button_clicked(GtkButton *button, gpointer user_data) {
  fine_step((GameState *)user_data, 1);
}

static void play_control_clicked(GtkButton *button, gpointer user_data) {
  GameState *game_state = (GameState *)user_data;

  game_state->playing = !game_state->playing;

  if (game_state->playing) {
    gtk_widget_add_tick_callback(game_state->slider, auto_step, game_state,
                                 NULL);
  }

  set_play_button_label(game_state);
}

static gboolean slider_user_input(GtkRange *slider, GtkScrollType *scroll,
                                  gdouble value, gpointer user_data) {
  GameState *game_state = (GameState *)user_data;

  game_state->cursor =
      (int)floor((double)(game_state->data_len - game_state->fft_window_size) *
                 value / 100.0);
  gtk_widget_queue_draw(game_state->area);

  return FALSE;
}

static void constrain_double(double *x, double min_v, double max_v) {
  if (*x < min_v) {
    *x = min_v;
    return;
  }
  if (*x > max_v) {
    *x = max_v;
    return;
  }
}

static gboolean scroll_input(GtkEventControllerScroll *scroll_input, gdouble dx,
                             gdouble dy, gpointer user_data) {
  GameState *game_state = (GameState *)user_data;

  // fprintf(stderr, "%f\n", dy);

  GdkModifierType modifiers = gtk_event_controller_get_current_event_state(
      GTK_EVENT_CONTROLLER(scroll_input));

  if (modifiers & GDK_SHIFT_MASK) {
    int width = gtk_widget_get_width(game_state->area);

    double xm = (double)game_state->fft_window_size *
                (double)game_state->mouse_x / (double)width;
    double z =
        game_state->plot_size * xm / (double)game_state->fft_window_size +
        game_state->plot_begin;
    double c = 1.0 + 0.1 * dy;
    double t = (c - 1) * (double)game_state->fft_window_size /
                   game_state->plot_size * z +
               game_state->plot_begin * (double)game_state->fft_window_size /
                   game_state->plot_size;

    game_state->plot_size = game_state->plot_size / c;
    constrain_double(&game_state->plot_size, 10.0, game_state->fft_window_size);

    double b2 = t * game_state->plot_size / (double)game_state->fft_window_size;
    constrain_double(&b2, 0.0,
                     game_state->fft_window_size - game_state->plot_size);

    game_state->plot_begin = b2;

    gtk_widget_queue_draw(game_state->area);
  } else {
    game_state->top_val = game_state->top_val * (1.0 + 0.1 * dy);
    gtk_widget_queue_draw(game_state->area);
  }

  return TRUE;
}

static void mouse_moved_in_area(GtkEventControllerMotion *mouse_movement,
                                gdouble x, gdouble y, gpointer user_data) {
  GameState *game_state = (GameState *)user_data;

  game_state->mouse_x = x;
}

typedef struct {
  GtkDrawingArea *area;
  cairo_t *cr;
  int width;
  int height;
  GameState *game_state;
  float plot_begin_x;
  float column_size;
} DrawContext;

static void draw_single_graph(DrawContext* dc, float* buf, GdkRGBA* color) {
  gdk_cairo_set_source_rgba(dc->cr, color);
  cairo_set_line_width(dc->cr, 1.0);

  double prev_x, prev_y;

  for (int i = 0; i < (int)ceil(dc->game_state->plot_size / 2); ++i) {
    double x =
        dc->plot_begin_x +
        dc->column_size * 2.0 * (i + (int)floor(dc->game_state->plot_begin / 2.0));
    double val =
        buf[i + (int)floor(dc->game_state->plot_begin / 2.0)];
    if (flag_log_scale) {
      val = -log(val + 1.0);
    }
    double y = val * (double)dc->height / (2.0 * dc->game_state->top_val) +
               (double)dc->height / 2.0;

    cairo_arc(dc->cr, x, y, 3.0, 0, 2 * G_PI);
    cairo_stroke(dc->cr);

    if (i != 0) {
      cairo_move_to(dc->cr, prev_x, prev_y);
      cairo_line_to(dc->cr, x, y);
      cairo_stroke(dc->cr);
    }
    prev_x = x;
    prev_y = y;
  }
}

static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width,
                          int height, gpointer data) {
  GameState *game_state = (GameState *)data;

  game_state_fft(game_state, game_state->cursor);

  double column_size = (double)width / (double)game_state->plot_size;

  // cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
  // cairo_paint(cr);

  gdk_cairo_set_source_rgba(cr, &black_color);
  cairo_set_line_width(cr, 1.0);

  cairo_move_to(cr, 0.0, height / 2);
  cairo_line_to(cr, width, height / 2);
  cairo_stroke(cr);

  double prev_x, prev_y;
  double plot_begin_x = column_size / 2 - game_state->plot_begin * column_size;

  if (!flag_log_scale && flag_show_nonsummed) {
    gdk_cairo_set_source_rgba(cr, &red_color);
    cairo_set_line_width(cr, 1.0);

    for (int i = 0; i < (int)ceil(game_state->plot_size); ++i) {
      double x =
          plot_begin_x + column_size * (i + (int)floor(game_state->plot_begin));
      double val =
          -game_state->output_buf[(int)floor(game_state->plot_begin) + i];
      double y = val * (double)height / (2.0 * game_state->top_val) +
                 (double)height / 2.0;
      cairo_arc(cr, x, y, 3.0, 0, 2 * G_PI);
      cairo_stroke(cr);

      if (i != 0) {
        cairo_move_to(cr, prev_x, prev_y);
        cairo_line_to(cr, x, y);
        cairo_stroke(cr);
      }
      prev_x = x;
      prev_y = y;
    }
  }

  DrawContext dc;
  dc.area = area;
  dc.cr = cr;
  dc.width = width;
  dc.height = height;
  dc.game_state = game_state;
  dc.plot_begin_x = plot_begin_x;
  dc.column_size = column_size;

  if (flag_show_nonwindowed) {
    draw_single_graph(&dc, game_state->abs_buf, &blue_color);
  }

  if (flag_show_windowed) {
    draw_single_graph(&dc, game_state->abs_buf_w, &green_color);
  }
}

void init_color_palette() {
  red_color.red = 1.0f;
  red_color.blue = 0.0f;
  red_color.green = 0.0f;
  red_color.alpha = 1.0f;

  blue_color.red = 0.0f;
  blue_color.blue = 1.0f;
  blue_color.green = 0.0f;
  blue_color.alpha = 0.3f;

  black_color.red = 0.0f;
  black_color.blue = 0.0f;
  black_color.green = 0.0f;
  black_color.alpha = 1.0f;

  green_color.red = 0.0f;
  green_color.blue = 0.0f;
  green_color.green = 1.0f;
  green_color.alpha = 1.0f;
}

static void activate(GtkApplication *app, gpointer user_data) {
  fprintf(stderr, "flag_log_scale: %d\n", flag_log_scale);

  GameState *game_state = (GameState *)user_data;
  game_state_init(game_state, flag_txtfft_file, flag_fft_window_size,
                  flag_plot_size, flag_top_val);

  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Slidergrapher");

  GtkWidget *area = gtk_drawing_area_new();
  gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area), 1920);
  gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area), 1200);
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), draw_function,
                                 game_state, NULL);

  GtkEventController *scroll_controller =
      gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
  gtk_widget_add_controller(area, scroll_controller);
  g_signal_connect(GTK_EVENT_CONTROLLER_SCROLL(scroll_controller), "scroll",
                   G_CALLBACK(scroll_input), game_state);

  GtkEventController *pointer_movement_controller =
      gtk_event_controller_motion_new();
  gtk_widget_add_controller(area, pointer_movement_controller);
  g_signal_connect(GTK_EVENT_CONTROLLER_MOTION(pointer_movement_controller),
                   "motion", G_CALLBACK(mouse_moved_in_area), game_state);

  game_state->area = area;

  GtkWidget *slider =
      gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0);
  g_signal_connect(slider, "change-value", G_CALLBACK(slider_user_input),
                   game_state);
  gtk_range_set_value(GTK_RANGE(slider), 0.0);
  game_state->slider = slider;
  game_state->playing = flag_auto;
  if (flag_auto) {
    gtk_widget_add_tick_callback(slider, auto_step, game_state, NULL);
  }
  gtk_widget_set_hexpand(slider, TRUE);

  game_state->auto_speed = flag_auto_speed;

  GtkWidget *left_step_button = gtk_button_new_with_label("<");
  g_signal_connect(GTK_BUTTON(left_step_button), "clicked",
                   G_CALLBACK(left_button_clicked), game_state);

  GtkWidget *right_step_button = gtk_button_new_with_label(">");
  g_signal_connect(GTK_BUTTON(right_step_button), "clicked",
                   G_CALLBACK(right_button_clicked), game_state);

  GtkWidget *play_control = gtk_button_new();
  game_state->play_control = play_control;
  set_play_button_label(game_state);
  g_signal_connect(GTK_BUTTON(play_control), "clicked",
                   G_CALLBACK(play_control_clicked), game_state);

  GtkWidget *control_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  gtk_box_append(GTK_BOX(control_box), slider);
  gtk_box_append(GTK_BOX(control_box), left_step_button);
  gtk_box_append(GTK_BOX(control_box), right_step_button);
  gtk_box_append(GTK_BOX(control_box), play_control);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_append(GTK_BOX(box), area);
  gtk_box_append(GTK_BOX(box), control_box);

  gtk_window_set_child(GTK_WINDOW(window), box);
  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
  init_color_palette();
  GameState *game_state = game_state_new();
  GOptionGroup *options = g_option_group_new(
      "slidergrapher", "slidergrapher options:", "Show slidergrapher options",
      NULL, NULL);
  g_option_group_add_entries(options, slidergrapher_option_entries);

  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

  g_application_add_option_group(G_APPLICATION(app), options);

  g_signal_connect(app, "activate", G_CALLBACK(activate), game_state);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}