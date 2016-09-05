#include "common.h"

//StrCpy
//StrCpy(void* target,string s,int size)

unsigned long get_file_size(const char *path)
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if(stat(path, &statbuff) < 0){
        return filesize;
    }else{
        filesize = statbuff.st_size;
    }
    return filesize;
}

int get_file_name(const char *path,string& name)
{
	string file(path);
	int i  = file.find_last_of("//");
	i++;
	string name1(file,i,(file.length()-i));
	name=name1;
	return 0;
}/*
int main()
{
	string name;
	 get_file_name("/home/fansiyuan/workspace/03/source/test.txt",name);
	 printf("name=%s\n",name.c_str());
	 return 0;
}*/
