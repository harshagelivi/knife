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

void on_save_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book){
	gint page_no = gtk_notebook_get_current_page(note_book);
	GtkWidget * scrolledwindow = gtk_notebook_get_nth_page (note_book, page_no);
	const gchar * curr_name = gtk_notebook_get_tab_label_text (note_book, scrolledwindow);
	//g_print("%s\n",curr_name);
	GtkWidget * text_view = gtk_bin_get_child ( (GtkBin *) scrolledwindow);
	GtkTextBuffer * buffer = gtk_text_view_get_buffer ((GtkTextView *)text_view);	

	GtkTextIter start,end;
	gchar *text;
	GError *err=NULL;
	
	gtk_widget_set_sensitive (text_view, FALSE);
	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_end_iter (buffer, &end);
	text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);       
	gtk_text_buffer_set_modified (buffer, FALSE);
	gtk_widget_set_sensitive (text_view, TRUE);
	
	
	
	
	if(curr_name!=NULL){
		g_file_set_contents (curr_name, text, -1, &err);
	}else{
		GtkWidget *dialog;
		dialog = gtk_file_chooser_dialog_new ("Save File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL,"Open", GTK_RESPONSE_ACCEPT, NULL);
	
		if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
			gchar *filename, *dir_name;
			filename = gtk_file_chooser_get_current_name (GTK_FILE_CHOOSER (dialog));
			dir_name = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
			gchar* path = g_strconcat(dir_name,"/",filename,NULL);
			g_file_set_contents (path, text, -1, &err);
			gtk_notebook_set_tab_label_text ((GtkNotebook *) (note_book), (GtkWidget *)scrolledwindow, path);					
		}
		gtk_widget_destroy (dialog);
	}
	g_free(text);
	gtk_widget_show_all (window);	
}

void on_open_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book){
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL,"Open", GTK_RESPONSE_ACCEPT, NULL);
	gchar *filename;
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gint page_no=gtk_notebook_get_current_page(note_book);
		GError *err=NULL;
        	gchar *buff;
		if(g_file_get_contents(filename, &buff, NULL, &err)){
			GtkSourceBuffer * source_buffer = gtk_source_buffer_new (NULL);
			GtkWidget * source_view = gtk_source_view_new_with_buffer (source_buffer);
			gtk_source_view_set_show_line_numbers ((GtkSourceView *)source_view, TRUE);
			gtk_source_view_set_auto_indent ((GtkSourceView *)source_view, TRUE);
			gtk_source_view_set_indent_on_tab((GtkSourceView *)source_view, TRUE);
			gtk_text_buffer_set_text ((GtkTextBuffer *)source_buffer, (const gchar *)(buff), -1);
			GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
			gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)source_view);

			GtkWidget * curr_scrolledwindow = gtk_notebook_get_nth_page (note_book, page_no);
			const gchar * curr_name = gtk_notebook_get_tab_label_text (note_book, curr_scrolledwindow);
			gint set_page;
			if(!curr_name){
				gtk_notebook_remove_page(note_book,page_no);	
				set_page = page_no;
				gtk_notebook_insert_page((GtkNotebook *) (note_book), (scrolledwindow), NULL, page_no);
			}else{
				set_page=-1;
				gtk_notebook_append_page((GtkNotebook *) (note_book), (scrolledwindow), NULL);
			}

			gchar * dir_name = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
			gchar * name=get_only_name(dir_name, filename);
			gtk_notebook_set_tab_label_text ((GtkNotebook *) (note_book), (GtkWidget *)scrolledwindow, filename);
			
			gtk_widget_show_all (window);	
			gtk_notebook_set_current_page(note_book, set_page);
		}		
	}
	gtk_widget_destroy (dialog);
}

void on_button_clicked (GtkToolButton * tool_button, GtkNotebook * note_book){
	GtkSourceBuffer * source_buffer = gtk_source_buffer_new (NULL);
	GtkWidget * source_view = gtk_source_view_new_with_buffer (source_buffer);
	gtk_source_view_set_show_line_numbers ((GtkSourceView *)source_view, TRUE);
	gtk_source_view_set_auto_indent ((GtkSourceView *)source_view, TRUE);
	gtk_source_view_set_indent_on_tab((GtkSourceView *)source_view, TRUE);
	GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)source_view);
	gtk_notebook_append_page((GtkNotebook *) (note_book),  (scrolledwindow), NULL);
	gtk_widget_show_all (window);		
	gtk_notebook_set_current_page(note_book,-1);
}


