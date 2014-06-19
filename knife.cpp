#include "decls.h"
#include "aux.h"
#include "handlers.h"
int main(int argc, char **argv){
	pthread_t server_thread;
//	try:
		pthread_create(&server_thread, NULL, server_init, NULL);
	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "knife editor 2.0");
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_signal), NULL);

	
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (window), box);

	tool_bar = gtk_toolbar_new();
	gtk_box_pack_start ((GtkBox *)box, tool_bar, FALSE, FALSE, 0);
	
	gstack=gtk_stack_new ();
	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (on_key_press), NULL);	
	switcher=gtk_stack_switcher_new ();
	gtk_box_pack_start ((GtkBox *)box, switcher, FALSE, FALSE, 0);
	gtk_stack_switcher_set_stack ((GtkStackSwitcher *)switcher,(GtkStack *)gstack);
	gtk_stack_set_transition_type ((GtkStack *)gstack, GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
	gtk_stack_set_transition_duration ((GtkStack *)gstack, 700);
	
	button=gtk_button_new_with_label("+");
	tool_button=gtk_tool_button_new((GtkWidget *)button, NULL);
	g_signal_connect ((GtkToolButton *)(tool_button), "clicked", G_CALLBACK (on_button_clicked), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), tool_button, 0);
	
	open_button=gtk_button_new_with_label("Open");
	tool_open_button=gtk_tool_button_new((GtkWidget *)open_button, NULL);
	g_signal_connect ((GtkToolButton *)(tool_open_button), "clicked", G_CALLBACK (on_open_button_clicked), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), tool_open_button, 1);
	
	save_button=gtk_button_new_with_label("Save");
	tool_save_button=gtk_tool_button_new((GtkWidget *)save_button, NULL);
	g_signal_connect ((GtkToolButton *)(tool_save_button), "clicked", G_CALLBACK (on_save_button_clicked), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), tool_save_button, 2);

	comment_button=gtk_button_new_with_label("/* */");
	tool_comment_button=gtk_tool_button_new((GtkWidget *)comment_button, NULL);
	g_signal_connect ((GtkToolButton *)(tool_comment_button), "clicked", G_CALLBACK(on_comment_button_clicked), NULL);
	gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), tool_comment_button, 3);

	chat_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	
	GtkWidget* chat_scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(chat_scrolledwindow), chat_box);			

	GtkWidget * paned = gtk_paned_new (GTK_ORIENTATION_HORIZONTAL);
	gtk_paned_add1 ((GtkPaned *)paned, (GtkWidget *)gstack);
	gtk_widget_set_size_request ((GtkWidget *)gstack,  1000,   -1);	
	gtk_paned_add2 ((GtkPaned *)paned, (GtkWidget *)chat_scrolledwindow);
	gtk_box_pack_start ((GtkBox *)box, paned, TRUE, TRUE, 0);		

	GtkWidget * sep_label = gtk_label_new ("");
	gtk_box_pack_start ((GtkBox *)box, sep_label, FALSE, FALSE, 0);		
		
	GtkSourceBuffer * source_buffer = gtk_source_buffer_new (NULL);
	GtkWidget * source_view = gtk_source_view_new_with_buffer (source_buffer);
	gtk_source_view_set_auto_indent ((GtkSourceView *)source_view, TRUE);
	gtk_source_view_set_indent_on_tab((GtkSourceView *)source_view, TRUE);

	GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)source_view);			
	gtk_widget_set_size_request ((GtkWidget *)scrolledwindow,  -1,   100);
	g_signal_connect (G_OBJECT (source_view), "key_press_event", G_CALLBACK (on_key_press_send), source_buffer);
	GdkRGBA color;
	gdk_rgba_parse (&color, "green");
	gtk_widget_override_background_color( (GtkWidget *)sep_label, GTK_STATE_FLAG_DIR_LTR, &color);	
	gtk_box_pack_start ((GtkBox *)box, scrolledwindow, FALSE, FALSE, 0);	
		
	gtk_widget_show_all (window);

	gtk_main ();
	
	pthread_join( server_thread, NULL);
	return 0;
}
