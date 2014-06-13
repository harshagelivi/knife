#include <gtk/gtk.h>
GtkWidget * window;
void destroy_signal(GtkWidget * widget, gpointer data){
	gtk_main_quit();
}
void on_open_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book){
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL,"Open", GTK_RESPONSE_ACCEPT, NULL);
gchar *filename;
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gint page_no=gtk_notebook_get_current_page(note_book);
		gtk_notebook_remove_page(note_book,page_no);	
		GError *err=NULL;
        	gchar *buff;
		if(g_file_get_contents(filename, &buff, NULL, &err)){
			GtkTextView * text_view;
			text_view=g_new(GtkTextView, 1);
			GtkTextBuffer * buffer;
			buffer=g_new(GtkTextBuffer, 1);	
			text_view = (GtkTextView *)gtk_text_view_new ();
			buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
			gtk_text_buffer_set_text (buffer, (const gchar *)(buff), -1);
			GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
			gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)text_view);

			gtk_notebook_insert_page((GtkNotebook *) (note_book), (scrolledwindow), NULL,page_no);
			gtk_widget_show_all (window);	
			gtk_notebook_set_current_page(note_book,page_no);
		}		
	}
	gtk_widget_destroy (dialog);
}
void on_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book){
	GtkTextView * text_view;
	text_view=g_new(GtkTextView, 1);
	GtkTextBuffer * buffer;
	buffer=g_new(GtkTextBuffer, 1);	
	text_view = (GtkTextView *)gtk_text_view_new ();
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));
	gtk_text_buffer_set_text (buffer, "new page!!!", -1);
	GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)text_view);

	gtk_notebook_append_page((GtkNotebook *) (note_book),  (scrolledwindow), NULL);
	gtk_widget_show_all (window);		
	gtk_notebook_set_current_page(note_book,-1);
}

int main(int argc, char **argv){
	GtkWidget  * text_view, *note_book, *pane, *button, *open_button;
	GtkToolItem * tool_button, *tool_open_button;
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
	
	gtk_widget_show_all (window);

	gtk_main ();

	return 0;
}
