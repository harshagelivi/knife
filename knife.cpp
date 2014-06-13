#include "decls.h"
#include "handlers.h"
int main(int argc, char **argv){
	
	gtk_init (&argc, &argv);

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "knife editor 1.0");
	gtk_window_set_default_size (GTK_WINDOW (window), 600, 600);
	g_signal_connect (G_OBJECT (window), "destroy", G_CALLBACK (destroy_signal), NULL);
	
	pane = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
	gtk_container_add (GTK_CONTAINER (window), pane);
		
	tool_bar = gtk_toolbar_new();
	gtk_paned_add1 (GTK_PANED (pane), tool_bar);
	
	note_book = gtk_notebook_new ();
	gtk_paned_add2 (GTK_PANED (pane), note_book);
	
	text_view = gtk_text_view_new ();
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
	gtk_text_buffer_set_text (buffer, "lets cut!!!", -1);
	GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), text_view);
	gtk_notebook_append_page(GTK_NOTEBOOK (note_book), scrolledwindow, NULL);
	
	button=gtk_button_new_with_label("+");
	tool_button=gtk_tool_button_new((GtkWidget *)button, "qqqqq");
	g_signal_connect ((GtkToolButton *)(tool_button), "clicked", G_CALLBACK (on_button_clicked), note_book);
	gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), tool_button, 0);
	
	open_button=gtk_button_new_with_label("Open");
	tool_open_button=gtk_tool_button_new((GtkWidget *)open_button, "qqqqq");
	g_signal_connect ((GtkToolButton *)(tool_open_button), "clicked", G_CALLBACK (on_open_button_clicked), note_book);
	gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), tool_open_button, 1);
	

	comment_button=gtk_button_new_with_label("/* */");
	tool_comment_button=gtk_tool_button_new((GtkWidget *)comment_button, NULL);
	g_signal_connect ((GtkToolButton *)(tool_comment_button), "clicked", G_CALLBACK(on_comment_button_clicked),note_book);
	gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), tool_comment_button, 1);

	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}
