#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include<string.h>
GtkWidget   *note_book, *pane, *button, *open_button, *comment_button, *tool_bar, *window, * save_button;

GtkToolItem * tool_button, *tool_open_button, *tool_comment_button, *tool_save_button;

void destroy_signal(GtkWidget * widget, gpointer data);
void on_comment_button_clicked (GtkToolButton * tool_button, GtkNotebook  * notebook);
void on_open_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book);
void on_save_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book);
void on_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book);

gchar * get_only_name(gchar * dir_name, gchar * file_name);
