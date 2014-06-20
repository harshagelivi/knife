#include "decls.h"
#include "aux.h"
#include "handlers.h"
int main(int argc, char **argv){
	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "knife editor 2.0");
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_signal), NULL);
	
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_container_add (GTK_CONTAINER (window), box);

	widget_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
	gtk_box_pack_start ((GtkBox *)box, widget_box, FALSE, FALSE, 10);
	
	gstack=gtk_stack_new ();
	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (on_key_press), NULL);	
	switcher=gtk_stack_switcher_new ();
	gtk_box_pack_start ((GtkBox *)box, switcher, FALSE, FALSE, 0);
	gtk_stack_switcher_set_stack ((GtkStackSwitcher *)switcher,(GtkStack *)gstack);
	gtk_stack_set_transition_type ((GtkStack *)gstack, GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
	gtk_stack_set_transition_duration ((GtkStack *)gstack, 700);
	
	GtkWidget * add_button = gtk_button_new_from_icon_name ("list-add",GTK_ICON_SIZE_BUTTON);
	g_signal_connect ((GtkButton *)(add_button), "clicked", G_CALLBACK (on_button_clicked), NULL);
	gtk_box_pack_start ((GtkBox *)widget_box, add_button, FALSE, FALSE, 10);
		
	GtkWidget * open_button = gtk_button_new_from_icon_name ("document-open",GTK_ICON_SIZE_BUTTON);
	g_signal_connect ((GtkButton *)(open_button), "clicked", G_CALLBACK (on_open_button_clicked), NULL);
	gtk_box_pack_start ((GtkBox *)widget_box, open_button, FALSE, FALSE, 10);

	GtkWidget * save_button = gtk_button_new_from_icon_name ("document-save",GTK_ICON_SIZE_BUTTON);
	g_signal_connect ((GtkButton *)(save_button), "clicked", G_CALLBACK (on_save_button_clicked), NULL);
	gtk_box_pack_start ((GtkBox *)widget_box, save_button, FALSE, FALSE, 10);

	comment_button=gtk_button_new_with_label("/*  */");
	g_signal_connect ((GtkButton *)(comment_button), "clicked", G_CALLBACK (on_comment_button_clicked), NULL);
	gtk_box_pack_start ((GtkBox *)widget_box, comment_button, FALSE, FALSE, 10);

	connect_switch = gtk_switch_new ();
	g_signal_connect ((GtkSwitch *)(connect_switch), "notify::active", G_CALLBACK (on_connect_switch_activate), NULL);	
	gtk_box_pack_start ((GtkBox *)widget_box, connect_switch, FALSE, FALSE, 10);
	
	chat_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	
	GtkWidget* chat_scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(chat_scrolledwindow), chat_box);			

	GtkWidget * paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_add1 ((GtkPaned *)paned, (GtkWidget *)gstack);
	gtk_widget_set_size_request ((GtkWidget *)gstack,  1000,   -1);	
	gtk_paned_add2 ((GtkPaned *)paned, (GtkWidget *)chat_scrolledwindow);
	gtk_box_pack_start ((GtkBox *)box, paned, TRUE, TRUE, 0);		

	GtkWidget * sep_label = gtk_label_new ("");
	gtk_box_pack_start ((GtkBox *)box, sep_label, FALSE, FALSE, 0);		
		
	chat_source_buffer = gtk_source_buffer_new (NULL);
	chat_source_view = gtk_source_view_new_with_buffer (chat_source_buffer);
	gtk_source_view_set_auto_indent ((GtkSourceView *)chat_source_view, TRUE);
	gtk_source_view_set_indent_on_tab((GtkSourceView *)chat_source_view, TRUE);
	gtk_text_view_set_editable ((GtkTextView *)chat_source_view, FALSE);

	GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)chat_source_view);			
	gtk_widget_set_size_request ((GtkWidget *)scrolledwindow,  -1,   100);
	g_signal_connect (G_OBJECT (chat_source_view), "key_press_event", G_CALLBACK (on_key_press_send), chat_source_buffer);
	GdkRGBA color;
	gdk_rgba_parse (&color, "green");
	gtk_widget_override_background_color( (GtkWidget *)sep_label, GTK_STATE_FLAG_DIR_LTR, &color);	
	gtk_box_pack_start ((GtkBox *)box, scrolledwindow, FALSE, FALSE, 0);	


//By Madhavi: start
	connect_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 00);
	connect_ip_label = gtk_label_new(" IP Address: ");
	connect_port_label = gtk_label_new("  Port Number: ");
	connect_ip_entry=gtk_entry_new ();
	connect_port_entry=gtk_entry_new ();
	connect_box_button = gtk_button_new_with_label(" Connect ");
	gtk_entry_set_text ((GtkEntry *) connect_ip_entry, "localhost");
	gtk_entry_set_text ((GtkEntry *) connect_port_entry, "12345");
	gtk_box_pack_start((GtkBox *)connect_box, connect_ip_label, FALSE, FALSE, 0);
	gtk_box_pack_start((GtkBox *)connect_box, connect_ip_entry, FALSE, FALSE, 0);
	gtk_box_pack_start((GtkBox *)connect_box, connect_port_label, FALSE, FALSE,0);
	gtk_box_pack_start((GtkBox *)connect_box, connect_port_entry, FALSE, FALSE,0);
	gtk_box_pack_start((GtkBox *)connect_box, connect_box_button, FALSE, FALSE,0);
	gtk_box_set_spacing ((GtkBox *)connect_box,40);
	gtk_box_pack_start ((GtkBox *)box, connect_box, FALSE, FALSE, 0);	
	g_signal_connect ((GtkButton *)(connect_box_button), "clicked", G_CALLBACK (on_connect_box_button_clicked), connect_ip_entry);
//By Madhavi: end


	
	pthread_t server_thread;
	pthread_create(&server_thread, NULL, server_init, NULL);

	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}
