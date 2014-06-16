#include "decls.h"
#include "aux.h"
#include "handlers.h"
int main(int argc, char **argv){
	
	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "knife editor 1.0");
	gtk_window_set_default_size (GTK_WINDOW (window), 600, 600);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_signal), NULL);
	g_signal_connect (G_OBJECT (window), "key_press_event", G_CALLBACK (on_key_press), NULL);
	
	box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add (GTK_CONTAINER (window), box);

	tool_bar = gtk_toolbar_new();
	gtk_box_pack_start ((GtkBox *)box, tool_bar, FALSE, FALSE, 0);
	
	gstack=gtk_stack_new ();
	switcher=gtk_stack_switcher_new ();
	gtk_box_pack_start ((GtkBox *)box, switcher, FALSE, FALSE, 0);
	gtk_box_pack_start ((GtkBox *)box, gstack, TRUE, TRUE, 0);
	gtk_stack_switcher_set_stack ((GtkStackSwitcher *)switcher,(GtkStack *)gstack);
	gtk_stack_set_transition_type ((GtkStack *)gstack, GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
	gtk_stack_set_transition_duration ((GtkStack *)gstack, 1000);
	
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

	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}
