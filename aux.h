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

	gint server_sock_fd, new_fd, yes,bytesnum; 
	struct sockaddr_in server_addr, client_addr;	
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
	}
	if (listen(server_sock_fd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	g_print("server: waiting for connections...\n");
	socklen_t sin_size;
	sin_size = sizeof(client_addr);
	char s[INET6_ADDRSTRLEN];
	while(1){
		new_fd = accept(server_sock_fd, (struct sockaddr *)&client_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
		}
		inet_ntop(client_addr.sin_family, (void *)&server_addr.sin_addr, s, sizeof s);
		printf("server: got connection from %s\n", s);
		if ((bytesnum = recv(new_fd, buf, MAXDATA-1, 0)) == -1) {
		    perror("recv");
		    exit(1);
		}
		buf[bytesnum] = '\0';
		g_print("server: received '%s'\n",buf);	
		if(not_empty(buf)){
			GtkWidget * label = gtk_label_new (g_strconcat("Friend : ", buf, NULL));
			gtk_label_set_line_wrap ((GtkLabel *)label, TRUE);
			gtk_label_set_selectable ((GtkLabel *)label, TRUE);
			gtk_box_pack_start ((GtkBox *)chat_box, label, FALSE, FALSE, 0);
			gtk_widget_show (label);					
		}
		close(new_fd);
	}

	g_print("thread done\n");	
}
