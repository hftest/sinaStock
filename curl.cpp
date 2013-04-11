#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <fcntl.h>
#include "/root/source/curl-7.29.0/include/curl/curl.h"
using namespace std;
bool curlInit(CURL * & conn,char *url,string *buffer); 
static int writer(char * data, size_t size,size_t nmemb,string *writerData);
static char errorBuffer[CURL_ERROR_SIZE];
int main()
{
	//char url[]="hq.sinajs.cn/list=sh701006";
	char *list=new char[50000];
	FILE * SHlist=fopen("/root/Programs/Data/SHlist.l","r");
	fseek(SHlist,0,SEEK_SET);
	int end;
	int index=0;
	while((end=getc(SHlist))!='\n')
	{
		list[index]=end;
		index++;
		//printf("%d:",index);
	}
	list[index]='\0';	
	char * url=new char[strlen(list)+19];
	strcpy(url,"hq.sinajs.cn/list=");
//	char url[]="hq.sinajs.cn/list=";
	strcat(url,list);
	CURL *con=NULL;
	CURLcode code;
	string buffer;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	if(!curlInit(con,url,&buffer))
	{
		printf("Initialization Failed\n");		
		return -1;
	}
	code=curl_easy_perform(con);
	if(code!=CURLE_OK)
	{
		printf("Failed perform\n");
		return -2;
	}
//	printf("#%s#\n",buffer.c_str());
	printf("length: %d\n",buffer.size());
//	printf("ERROR: %s\n",errorBuffer);
	FILE *file=fopen("a.thm","w+");
	fseek(file,0,SEEK_SET);
	fwrite(buffer.c_str(),1,buffer.size(),file);
	fclose(file);
	curl_easy_cleanup(con);
	return 0;
}
bool curlInit(CURL * & conn,char *url,string *buffer) 
{
	CURLcode code;
	conn=curl_easy_init();
	printf("1");
	if(conn==NULL)
	{
		fprintf(stdout,"connection is NULL after creation\n");
		return false;
	}
	printf("2");
	code=curl_easy_setopt(conn,CURLOPT_ERRORBUFFER,errorBuffer);
	if(code!=CURLE_OK)
	{
		fprintf(stdout,"fail to set error buffer: %d\n",code);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_URL,url);
	printf("3");
	if(code!=CURLE_OK)
	{
//		fprintf(stdout,"Failed to set url|%s|:|%s|\n",url,errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_FOLLOWLOCATION,1);
	printf("4");
	if(code!=CURLE_OK)
	{
//		fprintf(stdout,"Fail to set redirection option |%s|\n",errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,writer);
	printf("5");
	if(code!=CURLE_OK)
	{
		//fprintf(stdout,"Failed to set writer |%s|\n",errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_WRITEDATA,buffer);
	printf("6");
	if(code!=CURLE_OK)
	{
		//fprintf(stdout,"Failed to set buffer |%s|\n",errorBuffer);
		return false;
	}
	printf("7");
	return true;
}

static int writer(char * data, size_t size,size_t nmemb,string *writerData)
{
	unsigned long sizes=size * nmemb;
	if(writerData==NULL) return 0;
	writerData->append(data,sizes);
	return sizes;
}
