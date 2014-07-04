void destroy_signal(GtkWidget * widget, gpointer data){
	gtk_main_quit();
}
gboolean on_key_press_send(GtkWidget *widget, GdkEventKey  *event, GtkSourceBuffer * buffer){
	if(event->keyval==GDK_KEY_KP_Enter || event->keyval==GDK_KEY_Return){
		if(client_flag){
			GtkTextIter start,end;
			gchar *text;
	
			gtk_text_buffer_get_start_iter ((GtkTextBuffer *)buffer, &start);
			gtk_text_buffer_get_end_iter ((GtkTextBuffer *)buffer, &end);
			text = gtk_text_buffer_get_text ((GtkTextBuffer *)buffer, &start, &end, FALSE);    
			gtk_text_buffer_set_text( (GtkTextBuffer *)buffer, "", -1);		   

			if (send(client_sock_fd, text, strlen(text), 0) == -1){
				perror("send");	
				end_client_connection();		
			}else{
				if(not_empty(text)){
					g_print("client: sent '%s'\n", text);

					GtkWidget * label = gtk_label_new (text);
					gtk_misc_set_alignment ((GtkMisc *)label, 0.9, 0);
					gtk_label_set_line_wrap ((GtkLabel *)label, TRUE);
					gtk_label_set_selectable ((GtkLabel *)label, TRUE);
					GdkRGBA label_color;
					gdk_rgba_parse (&label_color, "green");
					gtk_widget_override_color((GtkWidget *)label, GTK_STATE_FLAG_DIR_LTR, &label_color);
					pthread_mutex_lock( &mutex_chat_box );
					gtk_box_pack_start ((GtkBox *)chat_box, label, FALSE, FALSE, 0);
					gtk_widget_show_all (window);					
					pthread_mutex_unlock( &mutex_chat_box );
				}			
			}
		}
		return TRUE;
	}else{
		//g_print("%0x\n",event->keyval);
		return FALSE;		
	}

}
gboolean on_key_press(GtkWidget *widget, GdkEventKey  *event, GtkStack * gstack){
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

void on_comment_button_clicked (GtkToolButton * tool_button, GtkStack * gstack){
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

void on_save_button_clicked (GtkToolButton * tool_button, GtkStack * gstack){

	GtkWidget * scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	if(scrolledwindow){
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
			dialog = gtk_file_chooser_dialog_new ("Save File", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL,"Save", GTK_RESPONSE_ACCEPT, NULL);
	
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
}

void on_open_button_clicked (GtkToolButton * tool_button, GtkStack * gstack){
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
			gtk_source_view_set_highlight_current_line((GtkSourceView *)source_view, TRUE);
			gtk_text_view_set_wrap_mode ((GtkTextView *)source_view, GTK_WRAP_WORD);
//By Madhavi:start
			gtk_widget_override_font ((GtkWidget *) source_view, font_desc);
			gtk_widget_override_color((GtkWidget *) source_view, GTK_STATE_FLAG_DIR_LTR, &color);
			gtk_widget_override_background_color((GtkWidget *) source_view, GTK_STATE_FLAG_DIR_LTR, &bgcolor);
			gtk_widget_set_sensitive ((GtkWidget *)source_view,TRUE);
//By Madhavi:end
			
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
void on_connect_switch_activate (GtkSwitch * connect_switch, gpointer data){
//	GtkWidget * ip_dialog = gtk_dialog_new ();
//	gtk_dialog_run( (GtkDialog *)ip_dialog);
	if(gtk_switch_get_active ((GtkSwitch *)connect_switch)){
		if(client_flag) return;
		const gchar * ip=gtk_entry_get_text ((GtkEntry *)connect_ip_entry);
		const gchar * port =gtk_entry_get_text((GtkEntry *) connect_port_entry);
		friend_port = atoi(port);
		strcpy(friend_ip,ip);
		client_flag=client_init() ? 1 : 0;
	}else{
		end_client_connection();
	}	
}
void on_button_clicked (GtkToolButton * tool_button, GtkStack * gstack){
	GtkSourceBuffer * source_buffer = gtk_source_buffer_new (NULL);
	GtkWidget * source_view = gtk_source_view_new_with_buffer (source_buffer);
	gtk_source_view_set_show_line_numbers ((GtkSourceView *)source_view, TRUE);
	gtk_source_view_set_auto_indent ((GtkSourceView *)source_view, TRUE);
	gtk_source_view_set_indent_on_tab((GtkSourceView *)source_view, TRUE);
	gtk_source_view_set_highlight_current_line((GtkSourceView *)source_view, TRUE);
	gtk_text_view_set_wrap_mode ((GtkTextView *)source_view, GTK_WRAP_WORD);
	GtkWidget* scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scrolledwindow), (GtkWidget *)source_view);
	gtk_stack_add_titled ((GtkStack *)gstack,(GtkWidget *)scrolledwindow, g_strdup_printf ("New %d",tab_counter), g_strdup_printf ("New %d",tab_counter));

//by Madhavi: start
	gtk_widget_override_font ((GtkWidget *) source_view, (PangoFontDescription*) font_desc);
	gtk_widget_override_color((GtkWidget *) source_view, GTK_STATE_FLAG_DIR_LTR, &color);
	gtk_widget_override_background_color((GtkWidget *) source_view, GTK_STATE_FLAG_DIR_LTR, &bgcolor);
//by Madhavi: end

	tab_counter++;	
	gtk_widget_show_all (window);		
	gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)scrolledwindow);

}


// By Madhavi:	start

void initialise_font_and_color(){
	color.red=0.0;
	color.green=0.0;
	color.blue=0.0;
	color.alpha=1.0;
	bgcolor.red=1.0;
	bgcolor.green=1.0;
	bgcolor.blue=1.0;
	bgcolor.alpha=1.0;
}

void on_font_menu_selected(GtkMenuItem * menu_i,GtkStack *gstack){
	GtkWidget * dialog, *font_chooser;
	GtkWidget * scrolledwindow, * textview;
	gint pageno;
	dialog = 	gtk_font_chooser_dialog_new ("Choose Font",  NULL);
	if(gtk_dialog_run (GTK_DIALOG (dialog))!=GTK_RESPONSE_CANCEL){
		font_desc = gtk_font_chooser_get_font_desc ((GtkFontChooser *) dialog);
		GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
		if(curr_scrolledwindow){
			GtkWidget * curr_text_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
			gtk_widget_override_font ((GtkWidget *) curr_text_view, (PangoFontDescription*) font_desc);
		}
	}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}


void on_color_menu_selected(GtkMenuItem * menu_i, GtkStack *gstack){
	GtkWidget * dialog;
	GtkWidget * scrolledwindow, * textview;
	gint pageno;
	dialog = gtk_color_chooser_dialog_new ("Choose your color", NULL);
	if(gtk_dialog_run (GTK_DIALOG (dialog))!=GTK_RESPONSE_CANCEL){
		gtk_color_chooser_get_rgba ((GtkColorChooser *)dialog, &color);	
		GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
		if(curr_scrolledwindow){
			GtkWidget * curr_text_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
			gtk_widget_override_color((GtkWidget *) curr_text_view, GTK_STATE_FLAG_DIR_LTR, &color);
		}
	}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}


void on_background_color_menu_selected(GtkMenuItem * menu_i, GtkStack *gstack){
	GtkWidget * dialog;
	GtkWidget * scrolledwindow, * textview;
	gint pageno;
	dialog = gtk_color_chooser_dialog_new ("Choose your color", NULL);
	if(gtk_dialog_run (GTK_DIALOG (dialog))!=GTK_RESPONSE_CANCEL){
		gtk_color_chooser_get_rgba ((GtkColorChooser *)dialog, &bgcolor);
		GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
		if(curr_scrolledwindow){	
			GtkWidget * curr_text_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
			gtk_widget_override_background_color((GtkWidget *) curr_text_view, GTK_STATE_FLAG_DIR_LTR, &bgcolor);
		}
	}
	gtk_widget_destroy (dialog);
	gtk_widget_show_all(window);
}


void on_remove_page_button_clicked (GtkToolButton * tool_button, GtkStack * gstack){
	GtkWidget * scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	if(scrolledwindow)
		gtk_widget_destroy ((GtkWidget *) scrolledwindow);
}

void on_search_insert(GtkWidget *search_buffer, guint position, gchar  *chars, guint n_chars, GtkStack * gstack){
	on_search_delete(search_buffer, position, n_chars, gstack);
}

void on_search_delete(GtkWidget *search_buffer, guint position, guint n_chars, GtkStack * gstack){
	GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	if(curr_scrolledwindow){
		const gchar * text = gtk_entry_buffer_get_text ((GtkEntryBuffer *)search_buffer);
		if(strlen(text)){
			GtkWidget * curr_source_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
			GtkSourceBuffer * buffer = (GtkSourceBuffer * )gtk_text_view_get_buffer ((GtkTextView *)curr_source_view);	
			GtkSourceSearchSettings * source_search_settings =  gtk_source_search_settings_new ();
			gtk_source_search_settings_set_case_sensitive((GtkSourceSearchSettings *) source_search_settings,FALSE);
			gtk_source_search_settings_set_search_text((GtkSourceSearchSettings *) source_search_settings,text);
			gtk_source_search_settings_set_wrap_around((GtkSourceSearchSettings *) source_search_settings,TRUE);
			GtkSourceSearchContext * search_context = gtk_source_search_context_new ((GtkSourceBuffer *) buffer, (GtkSourceSearchSettings *) source_search_settings);
			gtk_source_search_context_set_highlight((GtkSourceSearchContext *)search_context, TRUE);
		}
	}
}

void on_search_activate(GtkWidget *search_entry, GtkStack *gstack){
	GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	if(curr_scrolledwindow){
		const gchar * text = gtk_entry_get_text ((GtkEntry *)search_entry);
		GtkWidget * curr_source_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
		GtkSourceBuffer * curr_source_buffer = (GtkSourceBuffer * )gtk_text_view_get_buffer ((GtkTextView *)curr_source_view);	

		GtkEntryBuffer * search_buffer = gtk_entry_get_buffer ((GtkEntry *)search_entry);

		gchar * buffer_text;
		GtkTextIter start,end;
		GError *err=NULL;
		gtk_text_buffer_get_start_iter ((GtkTextBuffer * )curr_source_buffer, &start);
		gtk_text_buffer_get_end_iter ((GtkTextBuffer * )curr_source_buffer, &end);
		buffer_text = gtk_text_buffer_get_text ((GtkTextBuffer * )curr_source_buffer, &start, &end, FALSE);  
		     
		gtk_widget_destroy ((GtkWidget *)curr_source_view);
		GtkSourceBuffer * source_buffer = gtk_source_buffer_new (NULL);
		gtk_text_buffer_set_text( (GtkTextBuffer *)source_buffer, buffer_text, -1);		   
		GtkWidget * source_view = gtk_source_view_new_with_buffer (source_buffer);
		gtk_source_view_set_show_line_numbers ((GtkSourceView *)source_view, TRUE);
		gtk_source_view_set_auto_indent ((GtkSourceView *)source_view, TRUE);
		gtk_source_view_set_indent_on_tab((GtkSourceView *)source_view, TRUE);
		gtk_source_view_set_highlight_current_line((GtkSourceView *)source_view, TRUE);
		gtk_text_view_set_wrap_mode ((GtkTextView *)source_view, GTK_WRAP_WORD);
		gtk_container_add(GTK_CONTAINER(curr_scrolledwindow), (GtkWidget *)source_view);

		gtk_widget_override_font ((GtkWidget *) source_view, (PangoFontDescription*) font_desc);
		gtk_widget_override_color((GtkWidget *) source_view, GTK_STATE_FLAG_DIR_LTR, &color);
		gtk_widget_override_background_color((GtkWidget *) source_view, GTK_STATE_FLAG_DIR_LTR, &bgcolor);

		if(strlen(text)){
			GtkSourceSearchSettings * source_search_settings =  gtk_source_search_settings_new ();
			gtk_source_search_settings_set_case_sensitive((GtkSourceSearchSettings *) source_search_settings,FALSE);
			gtk_source_search_settings_set_search_text((GtkSourceSearchSettings *) source_search_settings,text);
			gtk_source_search_settings_set_wrap_around((GtkSourceSearchSettings *) source_search_settings,TRUE);
			GtkSourceSearchContext * search_context = gtk_source_search_context_new ((GtkSourceBuffer *) source_buffer, (GtkSourceSearchSettings *) source_search_settings);
			gtk_source_search_context_set_highlight((GtkSourceSearchContext *)search_context, TRUE);
		}

		gtk_widget_show_all (window);		
		gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)curr_scrolledwindow);
		
	}
}


void on_search_replace_button_clicked(GtkButton * search_replace_button, GtkStack * gstack){
	if(!search_replace_flag){
		search_replace_flag=1;
		GtkWidget * snrwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (snrwindow), "Search and Replace!");
		gtk_window_set_resizable ((GtkWindow *) snrwindow, FALSE);
		g_signal_connect ((GtkWidget *) (snrwindow), "destroy", G_CALLBACK(destroy_snr),NULL);
		snr_list_box = gtk_list_box_new ();

		GtkWidget * snr_search_label = gtk_label_new("\n	Enter the string you are searching for:	\n");
		GtkWidget * snr_replace_label = gtk_label_new("\n	Enter the string you want to replace with:	\n");
		GtkWidget * snr_search_entry =  gtk_search_entry_new ();
		GtkWidget * snr_replace_entry =  gtk_entry_new ();
		GtkWidget * snr_match_case_check_button = gtk_check_button_new_with_label ("Match Case");
		GtkWidget * snr_find_button = gtk_button_new_with_label (" Find All ");
		GtkWidget * snr_find_next_button = gtk_button_new_with_label (" Find next ");
		GtkWidget * snr_replace_one_button = gtk_button_new_with_label ("Replace One Word");
		GtkWidget * snr_replace_all_button = gtk_button_new_with_label ("Replace All Matches");

		gtk_list_box_insert ((GtkListBox *)snr_list_box, (GtkWidget *)snr_search_label,-1);				//index 0
		gtk_list_box_insert ((GtkListBox *)snr_list_box, (GtkWidget *)snr_search_entry,-1);				//index 1
		gtk_list_box_insert ((GtkListBox *)snr_list_box, (GtkWidget *)snr_replace_label,-1);				//index 2
		gtk_list_box_insert ((GtkListBox *)snr_list_box, (GtkWidget *)snr_replace_entry,-1);				//index 3
		gtk_list_box_insert ((GtkListBox *)snr_list_box, (GtkWidget *)snr_match_case_check_button,-1);		//index 4
		gtk_list_box_insert ((GtkListBox *)snr_list_box, (GtkWidget *)snr_find_button,-1);					//index 5
		gtk_list_box_insert ((GtkListBox *)snr_list_box, (GtkWidget *)snr_replace_one_button,-1);			//index 6
		gtk_list_box_insert ((GtkListBox *)snr_list_box, (GtkWidget *)snr_replace_all_button,-1);			//index 7
		struct snr * argument;
		argument=new snr;
		argument->gstack=(GtkWidget * ) gstack;
		argument->listbox=(GtkWidget * ) snr_list_box;
		
		g_signal_connect ((GtkButton *)(snr_find_button), "clicked", G_CALLBACK (on_snr_find_button_clicked), argument);
		g_signal_connect ((GtkButton *)(snr_replace_one_button), "clicked", G_CALLBACK (on_snr_replace_one_button_clicked), argument);
		g_signal_connect ((GtkButton *)(snr_replace_all_button), "clicked", G_CALLBACK (on_snr_replace_all_button_clicked), argument);

		gtk_container_add (GTK_CONTAINER (snrwindow), snr_list_box);
		gtk_widget_show_all (snrwindow);
		gtk_main ();
	}
}
void destroy_snr(){
	gtk_main_quit();
	search_replace_flag=1-search_replace_flag;
}

void on_snr_find_button_clicked(GtkButton *snr_find_button, struct snr * argument){
	GtkStack * gstack = (GtkStack * ) argument->gstack;
	GtkListBox * list_box = (GtkListBox *) argument->listbox;
	GtkListBoxRow * lbr = gtk_list_box_get_row_at_index ((GtkListBox *)list_box, 1);
	GtkSearchEntry * search_entry = (GtkSearchEntry *)gtk_bin_get_child ( (GtkBin *)  lbr);
	GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	if(curr_scrolledwindow){
		const gchar * text = gtk_entry_get_text ((GtkEntry *)search_entry);
		GtkWidget * curr_source_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
		GtkSourceBuffer * curr_source_buffer = (GtkSourceBuffer * )gtk_text_view_get_buffer ((GtkTextView *)curr_source_view);	

		GtkEntryBuffer * search_buffer = gtk_entry_get_buffer ((GtkEntry *)search_entry);

		gchar * buffer_text;
		GtkTextIter start,end;
		GError *err=NULL;
		gtk_text_buffer_get_start_iter ((GtkTextBuffer * )curr_source_buffer, &start);
		gtk_text_buffer_get_end_iter ((GtkTextBuffer * )curr_source_buffer, &end);
		buffer_text = gtk_text_buffer_get_text ((GtkTextBuffer * )curr_source_buffer, &start, &end, FALSE);  
		     
		gtk_widget_destroy ((GtkWidget *)curr_source_view);
		GtkSourceBuffer * source_buffer = gtk_source_buffer_new (NULL);
		gtk_text_buffer_set_text( (GtkTextBuffer *)source_buffer, buffer_text, -1);		   
		GtkWidget * source_view = gtk_source_view_new_with_buffer (source_buffer);
		gtk_source_view_set_show_line_numbers ((GtkSourceView *)source_view, TRUE);
		gtk_source_view_set_auto_indent ((GtkSourceView *)source_view, TRUE);
		gtk_source_view_set_indent_on_tab((GtkSourceView *)source_view, TRUE);
		gtk_source_view_set_highlight_current_line((GtkSourceView *)source_view, TRUE);
		gtk_text_view_set_wrap_mode ((GtkTextView *)source_view, GTK_WRAP_WORD);
		gtk_container_add(GTK_CONTAINER(curr_scrolledwindow), (GtkWidget *)source_view);

		gtk_widget_override_font ((GtkWidget *) source_view, (PangoFontDescription*) font_desc);
		gtk_widget_override_color((GtkWidget *) source_view, GTK_STATE_FLAG_DIR_LTR, &color);
		gtk_widget_override_background_color((GtkWidget *) source_view, GTK_STATE_FLAG_DIR_LTR, &bgcolor);

		if(strlen(text)){
			GtkListBoxRow * match_case_lbr = gtk_list_box_get_row_at_index ((GtkListBox *)list_box, 4);
			GtkCheckButton * match_case_check_button = (GtkCheckButton *)gtk_bin_get_child ( (GtkBin *)  match_case_lbr);

			GtkSourceSearchSettings * source_search_settings =  gtk_source_search_settings_new ();
			if(gtk_toggle_button_get_active((GtkToggleButton *) match_case_check_button))
				gtk_source_search_settings_set_case_sensitive((GtkSourceSearchSettings *) source_search_settings,TRUE);
			else
				gtk_source_search_settings_set_case_sensitive((GtkSourceSearchSettings *) source_search_settings,FALSE);
			gtk_source_search_settings_set_search_text((GtkSourceSearchSettings *) source_search_settings,text);
			gtk_source_search_settings_set_wrap_around((GtkSourceSearchSettings *) source_search_settings,TRUE);
			GtkSourceSearchContext * search_context = gtk_source_search_context_new ((GtkSourceBuffer *) source_buffer, (GtkSourceSearchSettings *) source_search_settings);
			gtk_source_search_context_set_highlight((GtkSourceSearchContext *)search_context, TRUE);
		}

		gtk_widget_show_all (window);		
		gtk_stack_set_visible_child ((GtkStack *)gstack, (GtkWidget *)curr_scrolledwindow);
	}
}


void on_snr_replace_one_button_clicked(GtkButton *snr_find_button, struct snr * argument){
	GtkStack * gstack = (GtkStack * ) argument->gstack;
	GtkListBox * list_box = (GtkListBox *) argument->listbox;
	GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	gint flag=1;
	GtkTextSearchFlags searchflag;
	if(curr_scrolledwindow){
		GtkListBoxRow * search_entry_lbr = gtk_list_box_get_row_at_index ((GtkListBox *)list_box, 1);
		GtkListBoxRow * replace_entry_lbr = gtk_list_box_get_row_at_index ((GtkListBox *)list_box, 3);
		GtkListBoxRow * match_case_lbr = gtk_list_box_get_row_at_index ((GtkListBox *)list_box, 4);

		GtkEntry * search_entry = (GtkEntry *)gtk_bin_get_child ( (GtkBin *)  search_entry_lbr);
		GtkEntry * replace_entry = (GtkEntry *)gtk_bin_get_child ( (GtkBin *)  replace_entry_lbr);
		GtkCheckButton * match_case_check_button = (GtkCheckButton *)gtk_bin_get_child ( (GtkBin *)  match_case_lbr);

		GtkEntryBuffer * search_entry_buffer = (GtkEntryBuffer *) gtk_entry_get_buffer( (GtkEntry *) search_entry);
		GtkEntryBuffer * replace_entry_buffer = (GtkEntryBuffer *) gtk_entry_get_buffer((GtkEntry *) replace_entry);

		const gchar * search_text = gtk_entry_buffer_get_text ((GtkEntryBuffer *)search_entry_buffer);
		const gchar * replace_text = gtk_entry_buffer_get_text ((GtkEntryBuffer *)replace_entry_buffer);

		if(strlen(search_text)){
			GtkWidget * curr_source_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
			GtkSourceBuffer * curr_source_buffer = (GtkSourceBuffer * )gtk_text_view_get_buffer ((GtkTextView *)curr_source_view);	

			GtkSourceSearchSettings * source_search_settings =  gtk_source_search_settings_new ();
			if(gtk_toggle_button_get_active((GtkToggleButton *) match_case_check_button))
				gtk_source_search_settings_set_case_sensitive((GtkSourceSearchSettings *) source_search_settings,TRUE);
			else{
				gtk_source_search_settings_set_case_sensitive((GtkSourceSearchSettings *) source_search_settings,FALSE);
				flag=0;
			}
			gtk_source_search_settings_set_search_text((GtkSourceSearchSettings *) source_search_settings,search_text);
			gtk_source_search_settings_set_wrap_around((GtkSourceSearchSettings *) source_search_settings,TRUE);
			GtkSourceSearchContext * search_context = gtk_source_search_context_new ((GtkSourceBuffer *) curr_source_buffer, (GtkSourceSearchSettings *) source_search_settings);
			if(flag==0)
				searchflag = GTK_TEXT_SEARCH_CASE_INSENSITIVE;
			
			GtkTextIter match_start;
			GtkTextIter match_end;
			GtkTextIter iter;
			gtk_text_buffer_get_start_iter((GtkTextBuffer *) curr_source_buffer,(GtkTextIter *) &iter);
			if(&iter){
				gtk_text_iter_forward_search (&iter, search_text, searchflag,&match_start,&match_end,NULL);
				gtk_source_search_context_replace ((GtkSourceSearchContext *)search_context, &match_start,&match_end,replace_text,strlen(replace_text),NULL);
			}
		}
	}
}

void on_snr_replace_all_button_clicked(GtkButton *snr_find_button, struct snr * argument){
	GtkStack * gstack = (GtkStack * ) argument->gstack;
	GtkListBox * list_box = (GtkListBox *) argument->listbox;
	GtkWidget * curr_scrolledwindow = gtk_stack_get_visible_child( (GtkStack *)gstack);
	if(curr_scrolledwindow){
		GtkListBoxRow * search_entry_lbr = gtk_list_box_get_row_at_index ((GtkListBox *)list_box, 1);
		GtkListBoxRow * replace_entry_lbr = gtk_list_box_get_row_at_index ((GtkListBox *)list_box, 3);
		GtkListBoxRow * match_case_lbr = gtk_list_box_get_row_at_index ((GtkListBox *)list_box, 4);

		GtkEntry * search_entry = (GtkEntry *)gtk_bin_get_child ( (GtkBin *)  search_entry_lbr);
		GtkEntry * replace_entry = (GtkEntry *)gtk_bin_get_child ( (GtkBin *)  replace_entry_lbr);
		GtkCheckButton * match_case_check_button = (GtkCheckButton *)gtk_bin_get_child ( (GtkBin *)  match_case_lbr);

		GtkEntryBuffer * search_entry_buffer = (GtkEntryBuffer *) gtk_entry_get_buffer( (GtkEntry *) search_entry);
		GtkEntryBuffer * replace_entry_buffer = (GtkEntryBuffer *) gtk_entry_get_buffer((GtkEntry *) replace_entry);

		const gchar * search_text = gtk_entry_buffer_get_text ((GtkEntryBuffer *)search_entry_buffer);
		const gchar * replace_text = gtk_entry_buffer_get_text ((GtkEntryBuffer *)replace_entry_buffer);

		if(strlen(search_text)){
			GtkWidget * curr_source_view = gtk_bin_get_child ( (GtkBin *) curr_scrolledwindow);
			GtkSourceBuffer * curr_source_buffer = (GtkSourceBuffer * )gtk_text_view_get_buffer ((GtkTextView *)curr_source_view);	

			GtkSourceSearchSettings * source_search_settings =  gtk_source_search_settings_new ();
			if(gtk_toggle_button_get_active((GtkToggleButton *) match_case_check_button))
				gtk_source_search_settings_set_case_sensitive((GtkSourceSearchSettings *) source_search_settings,TRUE);
			else
				gtk_source_search_settings_set_case_sensitive((GtkSourceSearchSettings *) source_search_settings,FALSE);
			gtk_source_search_settings_set_search_text((GtkSourceSearchSettings *) source_search_settings,search_text);
			gtk_source_search_settings_set_wrap_around((GtkSourceSearchSettings *) source_search_settings,TRUE);
			GtkSourceSearchContext * search_context = gtk_source_search_context_new ((GtkSourceBuffer *) curr_source_buffer, (GtkSourceSearchSettings *) source_search_settings);
			gtk_source_search_context_replace_all ((GtkSourceSearchContext *)search_context, replace_text,strlen(replace_text),NULL);
		}
	}
}
//By Madhavi: end
