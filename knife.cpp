#include <gtk/gtk.h>
GtkWidget * window;
void destroy_signal(GtkWidget * widget, gpointer data){
	gtk_main_quit();
}
void on_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book){
	GtkTextView * text_view;
	text_view=g_new(GtkTextView, 1);
	GtkTextBuffer * buffer;
	buffer=g_new(GtkTextBuffer, 1);	
	text_view = (GtkTextView *)gtk_text_view_new ();
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
	gtk_text_buffer_set_text (buffer, "new page!!!", -1);

	g_print("%d\n",gtk_notebook_append_page((GtkNotebook *) (note_book), (GtkWidget *) (text_view), NULL));
	gtk_widget_show_all (window);		
	
}

int main(int argc, char **argv){
	GtkWidget  * text_view, *note_book, *pane, *button;
	GtkToolItem * tool_button;
	GtkWidget * tool_bar;
	GtkTextBuffer * buffer;
	
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
	
	gtk_notebook_append_page(GTK_NOTEBOOK (note_book), text_view, NULL);
	
	button=gtk_button_new_with_label("+");

	tool_button=gtk_tool_button_new((GtkWidget *)button, "qqqqq");
	g_signal_connect ((GtkToolButton *)(tool_button), "clicked", G_CALLBACK (on_button_clicked), note_book);
	gtk_toolbar_insert(GTK_TOOLBAR(tool_bar), tool_button, 0);
	
	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}
