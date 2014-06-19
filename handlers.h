void destroy_signal(GtkWidget * widget, gpointer data){
	gtk_main_quit();
}
gboolean on_key_press_send(GtkWidget *widget, GdkEventKey  *event, GtkSourceBuffer * buffer){
	if(event->keyval==GDK_KEY_KP_Enter || event->keyval==GDK_KEY_Return){

		GtkTextIter start,end;
		gchar *text;
		GError *err=NULL;
	

		gtk_text_buffer_get_start_iter ((GtkTextBuffer *)buffer, &start);
		gtk_text_buffer_get_end_iter ((GtkTextBuffer *)buffer, &end);
		text = gtk_text_buffer_get_text ((GtkTextBuffer *)buffer, &start, &end, FALSE);    
		gtk_text_buffer_set_text( (GtkTextBuffer *)buffer, "", -1);		   
		
		gchar  server_ip[INET6_ADDRSTRLEN];
		gint client_sock_fd, bytesnum,yes=1;
	
		if((client_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
			perror("server: socket");
		}

		if (setsockopt(client_sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(gint)) == -1){
			perror("setsockopt");
			exit(1);
		}
		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(client_port);

		inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr.s_addr);
	
		if (connect(client_sock_fd, (struct sockaddr *)&server_addr, sizeof server_addr) == -1) {
			close(client_sock_fd);
			perror("client: connect");
		}
		inet_ntop(server_addr.sin_family, (void *)&server_addr.sin_addr,  server_ip, sizeof  server_ip);
		printf("client: connected to %s\n",  server_ip);
		if (send(client_sock_fd, text, strlen(text), 0) == -1)	perror("send");	
		close(client_sock_fd);

		if(not_empty(text)){
			GtkWidget * label = gtk_label_new (g_strconcat("You : ", text, NULL));
			gtk_label_set_line_wrap ((GtkLabel *)label, TRUE);
			gtk_label_set_selectable ((GtkLabel *)label, TRUE);
			gtk_box_pack_start ((GtkBox *)chat_box, label, FALSE, FALSE, 0);
			gtk_widget_show (label);					
		}
		return TRUE;
	}else{
		//g_print("%0x\n",event->keyval);
		return FALSE;		
	}

}
gboolean on_key_press(GtkWidget *widget, GdkEventKey  *event, gpointer   user_data){
	GtkWidget * scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	GList * child = gtk_container_get_children ((GtkContainer *)gstack);	
	GList * first = child;
	if(event->keyval==GDK_KEY_Page_Down){
		if(child){
			while(child){
				if(child->data==scrolledwindow) break;
				child=child->next;
			}
			if(child) child=child->next;
			if(!child) child=first;
			if(child) gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)(child->data));
		}	
		return TRUE;
	}
	if(event->keyval==GDK_KEY_Page_Up){
		if(child && child->data==scrolledwindow){
			while(child->next){
				child=child->next;
			}
			gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)(child->data));			
		}else{
			while(child){
				if(child->data==scrolledwindow) break;
				child=child->next;
			}
			if(child) child=child->prev;
			if(child) gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)(child->data));
		}	
		return TRUE;
	}
	return FALSE;
	
}

void on_comment_button_clicked (GtkToolButton * tool_button, gpointer data){
	GtkWidget * scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	GtkWidget * text_view = gtk_bin_get_child ( (GtkBin *) scrolledwindow);
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

void on_save_button_clicked (GtkToolButton * tool_button, gpointer data){

	GtkWidget * scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	const gchar * curr_name = gtk_stack_get_visible_child_name( (GtkStack *)gstack);
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
	
	if(curr_name[0]=='/'){
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
			GValue a = G_VALUE_INIT;
			GValue b = G_VALUE_INIT;
			g_value_init (&a, G_TYPE_STRING);
			g_value_set_static_string (&a, g_strdup(filename));
			g_value_init (&b, G_TYPE_STRING);
			g_value_set_static_string (&b, g_strdup(path));
			gtk_container_child_set_property((GtkContainer *)gstack,scrolledwindow, "name", &b); 
			gtk_container_child_set_property((GtkContainer *)gstack,scrolledwindow, "title", &a); 			
			
		}
		gtk_widget_destroy (dialog);
	}
	g_free(text);
	gtk_widget_show_all (window);	
}

void on_open_button_clicked (GtkToolButton * tool_button, gpointer data){
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL,"Open", GTK_RESPONSE_ACCEPT, NULL);
	gchar *filepath;
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		
		filepath = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		GError *err=NULL;
        	gchar *buff;
		if(g_file_get_contents(filepath, &buff, NULL, &err)){
			gboolean buffer_modified_flag=TRUE;
			GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
			const gchar * curr_name;
			GtkWidget * curr_text_view;
			GtkTextBuffer * curr_buffer;
			if(curr_scrolledwindow){
				curr_name = gtk_stack_get_visible_child_name( (GtkStack *)gstack);
				curr_text_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
				curr_buffer = gtk_text_view_get_buffer ((GtkTextView *)curr_text_view);	
				buffer_modified_flag = gtk_text_buffer_get_modified (curr_buffer);
			}	

			GtkSourceBuffer * source_buffer = gtk_source_buffer_new (NULL);
			GtkWidget * source_view = gtk_source_view_new_with_buffer (source_buffer);
			gtk_source_view_set_show_line_numbers ((GtkSourceView *)source_view, TRUE);
			gtk_source_view_set_auto_indent ((GtkSourceView *)source_view, TRUE);
			gtk_source_view_set_indent_on_tab((GtkSourceView *)source_view, TRUE);
			gtk_text_buffer_set_text ((GtkTextBuffer *)source_buffer, (const gchar *)(buff), -1);
			GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
			
			gchar * dir_name = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (dialog));
			gchar * name=get_only_name(dir_name, filepath);

			GValue file_name = G_VALUE_INIT;
			GValue path = G_VALUE_INIT;
			g_value_init (&file_name, G_TYPE_STRING);
			g_value_set_static_string (&file_name, g_strdup(name));
			g_value_init (&path, G_TYPE_STRING);
			g_value_set_static_string (&path, g_strdup(filepath));

			if(buffer_modified_flag){
				gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)source_view);			
				gtk_stack_add_titled ((GtkStack *)gstack,(GtkWidget *)scrolledwindow, filepath, name);
				gtk_widget_show_all (window);	
				gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)scrolledwindow);
			}else{
				gtk_container_remove ((GtkContainer *)curr_scrolledwindow, (GtkWidget *)curr_text_view);
				gtk_widget_destroy (curr_text_view);
				gtk_container_add(GTK_CONTAINER(curr_scrolledwindow), (GtkWidget *)source_view);
				gtk_container_child_set_property((GtkContainer *)gstack,curr_scrolledwindow, "name", &path); 
				gtk_container_child_set_property((GtkContainer *)gstack,curr_scrolledwindow, "title", &file_name);	
				gtk_widget_show_all (window);	
				gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)curr_scrolledwindow);
			}
		}		
	}
	gtk_widget_destroy (dialog);
}

void on_button_clicked (GtkToolButton * tool_button, gpointer data){
	GtkSourceBuffer * source_buffer = gtk_source_buffer_new (NULL);
	GtkWidget * source_view = gtk_source_view_new_with_buffer (source_buffer);
	gtk_source_view_set_show_line_numbers ((GtkSourceView *)source_view, TRUE);
	gtk_source_view_set_auto_indent ((GtkSourceView *)source_view, TRUE);
	gtk_source_view_set_indent_on_tab((GtkSourceView *)source_view, TRUE);
	GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)source_view);
	gtk_stack_add_titled ((GtkStack *)gstack,(GtkWidget *)scrolledwindow, g_strdup_printf ("New %d",tab_counter), g_strdup_printf ("New %d",tab_counter));
	tab_counter++;	
	gtk_widget_show_all (window);		
	gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)scrolledwindow);
}


