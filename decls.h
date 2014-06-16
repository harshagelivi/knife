#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include<string.h>
#include <gdk/gdkkeysyms.h>
GtkWidget   *search_bar,*search_entry, *button, *open_button, *comment_button, *tool_bar, *window, * save_button, *box, *gstack, *switcher;

GtkToolItem * tool_button, *tool_open_button, *tool_comment_button, *tool_save_button;
static gint tab_counter=1;
void destroy_signal(GtkWidget * widget, gpointer data);
void on_comment_button_clicked (GtkToolButton * tool_button, gpointer data);
void on_open_button_clicked (GtkToolButton * tool_button, gpointer data);
void on_save_button_clicked (GtkToolButton * tool_button, gpointer data);
void on_button_clicked (GtkToolButton * tool_button, gpointer data);
void on_key_press(GtkWidget *widget, GdkEventKey  *event, gpointer   user_data);

gchar * get_only_name(gchar * dir_name, gchar * file_name);
