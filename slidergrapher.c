#include <gtk/gtk.h>
#include <math.h>
#include <pffft.h>
#include <stdio.h>
#include <stdlib.h>

#include "loader.h"

static gchar *flag_txtfft_file = NULL;
static gint flag_fft_window_size = 2048;
static gdouble flag_plot_size = 200;
static gdouble flag_top_val = 100.0;
static gboolean flag_auto = FALSE;
static gint flag_auto_speed = 4;
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
    G_OPTION_ENTRY_NULL};

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

  game_state->mouse_x = 0;
  game_state->plot_begin = 0;
}

void game_state_fft(GameState *game_state, int cursor) {
  memcpy(game_state->input_buf, game_state->data + cursor,
         game_state->fft_window_size * sizeof(float));
  pffft_transform_ordered(game_state->setup, game_state->input_buf,
                          game_state->output_buf, game_state->work_buf,
                          PFFFT_FORWARD);
  for (int i = 0; i < game_state->fft_window_size / 2; ++i) {
    game_state->abs_buf[i] = sqrt(pow(game_state->output_buf[i * 2], 2.0) +
                                  pow(game_state->output_buf[i * 2 + 1], 2.0));
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

static gboolean scroll_input(GtkEventControllerScroll *scroll_input, gdouble dx,
                             gdouble dy, gpointer user_data) {
  GameState *game_state = (GameState *)user_data;

  // fprintf(stderr, "%f\n", dy);

  GdkModifierType modifiers = gtk_event_controller_get_current_event_state(GTK_EVENT_CONTROLLER(scroll_input));

  if (modifiers & GDK_SHIFT_MASK) {
    game_state->plot_size = game_state->plot_size * (1.0 + 0.1 * dy);
    if (game_state->plot_size >= game_state->fft_window_size) {
      game_state->plot_size = game_state->fft_window_size;
    }
    game_state->plot_begin = game_state->mouse_x - game_state->plot_size / 2;
    if (game_state->plot_begin >
        game_state->fft_window_size - game_state->plot_size) {
      game_state->plot_begin =
          game_state->fft_window_size - game_state->plot_size;
    }
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

static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width,
                          int height, gpointer data) {
  GameState *game_state = (GameState *)data;

  game_state_fft(game_state, game_state->cursor);

  double column_size = (double)width / (double)game_state->plot_size;

  GdkRGBA color;
  color.red = 1.0f;
  color.blue = 0.0f;
  color.green = 0.0f;
  color.alpha = 1.0f;

  GdkRGBA blue_color;
  blue_color.red = 0.0f;
  blue_color.blue = 1.0f;
  blue_color.green = 0.0f;
  blue_color.alpha = 1.0f;

  // cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
  // cairo_paint(cr);

  double prev_x, prev_y;

  gdk_cairo_set_source_rgba(cr, &color);
  cairo_set_line_width(cr, 1.0);

  for (int i = game_state->plot_begin;
       i < (int)game_state->plot_size + game_state->plot_begin; ++i) {
    double x = column_size * i + column_size / 2.0;
    double val = -game_state->output_buf[i];
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

  gdk_cairo_set_source_rgba(cr, &blue_color);
  cairo_set_line_width(cr, 1.0);

  for (int i = 0; i < (int)game_state->plot_size / 2; ++i) {
    double x = column_size * 2.0 * i + column_size;
    double val = -game_state->abs_buf[i];
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

static void activate(GtkApplication *app, gpointer user_data) {
  GameState *game_state = (GameState *)user_data;
  game_state_init(game_state, flag_txtfft_file, flag_fft_window_size,
                  flag_plot_size, flag_top_val);

  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Hello");

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