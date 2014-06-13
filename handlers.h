void destroy_signal(GtkWidget * widget, gpointer data){
	gtk_main_quit();
}

void on_comment_button_clicked (GtkToolButton * tool_button, GtkNotebook  * notebook){
	gint page_no = gtk_notebook_get_current_page(notebook);
	GtkWidget * text_view = gtk_bin_get_child ( (GtkBin *)gtk_notebook_get_nth_page (notebook, page_no) );
	GtkTextBuffer * buffer = gtk_text_view_get_buffer ((GtkTextView *)text_view);
	GtkTextIter iter,end;
	GtkTextMark *cursor;
	gchar *text;

	cursor = gtk_text_buffer_get_mark (buffer, "insert");

	gtk_text_buffer_get_iter_at_mark (buffer, &iter, cursor);

	gtk_text_iter_set_line_offset (&iter, 0);
	gtk_text_buffer_insert (buffer, &iter, "/* ", -1);
	gtk_text_iter_forward_to_line_end (&iter);
	gtk_text_buffer_insert (buffer, &iter, " */", -1);
	gtk_widget_show_all (window);		
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


