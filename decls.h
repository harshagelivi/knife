#include <gtk/gtk.h>
GtkWidget * window;
GtkWidget  * text_view, *note_book, *pane, *button, *open_button, *comment_button;
GtkToolItem * tool_button, *tool_open_button, *tool_comment_button;
GtkWidget * tool_bar;
GtkTextBuffer * buffer;
void destroy_signal(GtkWidget * widget, gpointer data);
void on_comment_button_clicked (GtkToolButton * tool_button, GtkNotebook  * notebook);
void on_open_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book);
void on_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book);
