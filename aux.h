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
