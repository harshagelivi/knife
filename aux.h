gchar * get_only_name(gchar * dir_name, gchar * file_name){
	gint i=0,j=0;
	gint file_len = strlen(file_name);
	gchar * name = new char[file_len-strlen(dir_name)];
	while(dir_name[i]==file_name[i]) i++;
	i++;
	while(i<file_len){
		name[j++]=file_name[i++];
	}
	name[j]=0;
	return name;
}
gboolean not_empty(gchar * buff){
	gint i=0,len=strlen(buff);
	while(i<len){
		if(buff[i]!=' ') return TRUE;
		i++;
	}
	return FALSE;
}
void * server_init(void * ptr){
	gint server_sock_fd, new_fd, yes=1,bytesnum,backlog=10; 
	struct sockaddr_in server_addr, friend_addr;	
	gchar buf[MAXDATA];
	if((server_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("server: socket");
	}

	if (setsockopt(server_sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(gint)) == -1){
		perror("setsockopt");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);	
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	
	if (bind(server_sock_fd, (struct sockaddr *)&server_addr, sizeof server_addr) == -1){
		close(server_sock_fd);
		perror("server: bind");
	}else{
		if (listen(server_sock_fd, backlog) == -1) {
			perror("listen");
		}else{
			socklen_t sin_size;
			sin_size = sizeof(friend_addr);
			char s[INET6_ADDRSTRLEN];
			while(1){
				g_print("server: waiting for connections...\n");
				new_fd = accept(server_sock_fd, (struct sockaddr *)&friend_addr, &sin_size);
				if (new_fd == -1) {
					perror("accept");
				}else{
					inet_ntop(friend_addr.sin_family, (void *)&server_addr.sin_addr, s, sizeof s);
					system(g_strdup_printf ("notify-send '%s would like to connect to you'", s));
					GtkWidget *dialog;
					dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,	GTK_MESSAGE_QUESTION, 
					GTK_BUTTONS_YES_NO, "%s would like to connect to you. Do you accept?", s);
					gtk_window_set_title(GTK_WINDOW(dialog), "New Conection request");
					if(gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES){
						gtk_widget_destroy(dialog);					
						g_print("server: got connection from %s\n", s);
						while(1){
							if ((bytesnum = recv(new_fd, buf, MAXDATA-1, 0)) == -1) {
								perror("recv");
								break;
							}else if((!bytesnum)){
								break;
							}else{
								buf[bytesnum] = '\0';
								g_print("server: received '%s'\n",buf);	
								if(not_empty(buf)){
									GtkWidget * label = gtk_label_new (buf);
									gtk_misc_set_alignment ((GtkMisc *)label, 0, 0);
									gtk_label_set_line_wrap ((GtkLabel *)label, TRUE);
									gtk_label_set_selectable ((GtkLabel *)label, TRUE);
									GdkRGBA label_color;
									gdk_rgba_parse (&label_color, "blue");
									gtk_widget_override_color((GtkWidget *)label, GTK_STATE_FLAG_DIR_LTR, &label_color);
									gtk_widget_show (label);
									pthread_mutex_lock( &mutex_chat_box );
									gtk_box_pack_start ((GtkBox *)chat_box, label, FALSE, FALSE, 0);
									pthread_mutex_unlock( &mutex_chat_box );
													
								}
							}
						}
					}else{
						g_print("server: rejected connection with %s\n", s);
						close(new_fd);
						//end_client_connection();  //this is needed when connected to a loopback interface
						gtk_widget_destroy(dialog);					
						continue;
					}	
					
				}
				g_print("server: connection ended with %s\n", s);
				end_client_connection();
				close(new_fd);
			}
		}
	}
	g_print("thread done\n");	
}
void end_client_connection(){
	if(client_flag){
		client_flag=0;
		close(client_sock_fd);
		g_print("client: ended connection with %s\n", friend_ip);
		gtk_switch_set_active ((GtkSwitch *)connect_switch, FALSE);	
		gtk_label_set_text ((GtkLabel *)sep_label, "Tell me where can i find your friend");
		gtk_widget_set_sensitive ((GtkWidget *) connect_ip_entry, TRUE);	
		gtk_widget_set_sensitive ((GtkWidget *) connect_port_entry, TRUE);
		gtk_text_view_set_editable ((GtkTextView *)chat_source_view, FALSE);					
	}	
}	

int client_init(){
		if((client_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
			perror("client: socket");
			return 0;
		}

		if (setsockopt(client_sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(gint)) == -1){
			perror("setsockopt");
			return 0;
		}
		friend_addr.sin_family = AF_INET;
		friend_addr.sin_port = htons(friend_port);

		inet_pton(AF_INET, friend_ip, &friend_addr.sin_addr.s_addr);
	
		if (connect(client_sock_fd, (struct sockaddr *)&friend_addr, sizeof friend_addr) == -1) {
			close(client_sock_fd);
			perror("client: connect");
			return 0;
		}
		inet_ntop(friend_addr.sin_family, (void *)&friend_addr.sin_addr,  friend_ip, sizeof  friend_ip);		
		g_print("client: connected to %s\n",  friend_ip);
		gtk_label_set_text ((GtkLabel *)sep_label, g_strdup_printf("You are connected to %s at %d",friend_ip, friend_port));
		gtk_widget_set_sensitive ((GtkWidget *) connect_ip_entry, FALSE);	
		gtk_widget_set_sensitive ((GtkWidget *) connect_port_entry, FALSE);
		gtk_text_view_set_editable ((GtkTextView *)chat_source_view, TRUE);
		return 1;
}
