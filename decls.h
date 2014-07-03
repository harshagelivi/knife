#include <X11/Xlib.h>   
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h> 
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include<string.h>
#include <gdk/gdkkeysyms.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <iostream>
using namespace std;
#define MAXDATA 1000


gint server_port = 3490, friend_port = 3493;
gchar  server_ip[INET6_ADDRSTRLEN], friend_ip[INET6_ADDRSTRLEN];
gint client_sock_fd, bytesnum, yes=1, client_flag;
struct sockaddr_in friend_addr;
pthread_mutex_t mutex_chat_box = PTHREAD_MUTEX_INITIALIZER;
GtkWidget   *search_bar,*search_entry, *add_button, *open_button, *comment_button, *connect_switch, *window, * save_button, *box, *gstack, *switcher, *chat_box, *chat_source_view, *connect_button, * widget_box, *middle_box, *sep_label;
GtkSourceBuffer * chat_source_buffer;

//By Madhavi: start
GtkWidget  *connect_box, *connect_ip_label, *connect_port_label, *connect_box_button, *connect_ip_entry, *connect_port_entry;
GtkWidget  *menu_bar,*file_menu, *file_item, *view_menu, *view_item, *font_item, *quit_item, *edit_item, *edit_menu, *color_item, *bg_color_item;
PangoFontDescription * font_desc;
GdkRGBA color, bgcolor;
gchar  server_ip1[INET6_ADDRSTRLEN];
gchar prev_search_text[100];
//By Madhavi: end


static gint tab_counter=1;
void destroy_signal(GtkWidget * widget, gpointer data);
void on_comment_button_clicked (GtkToolButton * tool_button, GtkStack * gstack);
void on_open_button_clicked (GtkToolButton * tool_button, GtkStack * gstack);
void on_save_button_clicked (GtkToolButton * tool_button, GtkStack * gstack);
void on_connect_switch_activate(GtkSwitch *, gpointer data);
void on_button_clicked (GtkToolButton * tool_button, GtkStack * gstack);
gboolean on_key_press(GtkWidget *widget, GdkEventKey  *event, GtkStack *);
gboolean on_key_press_send(GtkWidget *widget, GdkEventKey  *event, GtkSourceBuffer * source_buffer);

gchar * get_only_name(gchar * dir_name, gchar * file_name);
void * server_init(void * ptr);
gboolean not_empty(gchar * buff);
int client_init();
void end_client_connection();
