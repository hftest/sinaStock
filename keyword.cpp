#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <curl/curl.h>
using namespace std;
bool curlInit(CURL * & conn,char *url,string *buffer); 
static int writer(char * data, size_t size,size_t nmemb,string *writerData);
static char errorBuffer[CURL_ERROR_SIZE];
int main(int argc,char ** argv)
{
	//char url[]="hq.sinajs.cn/list=sh701006";
	//char *list=new char[3000];
	char (*p_list)[3000];
	p_list=new char[3][3000];
	/*FILE * SHlist=fopen("/root/Programs/Data/SHlist.l","r");
	fseek(SHlist,0,SEEK_SET);
	int end;
	int index=0;
	int i_list=0;
	while((end=getc(SHlist))!=EOF)
	{
		//list[index]=end;
		//index++;
		//printf("%d:",index);
//		p_list[i_list][index]=end;
//		index++;
		if(end=='\n')
		{
			p_list[i_list][index]='\0';
			i_list++;
			index=0;
		}
		else
		{
			p_list[i_list][index]=end;
			index++;
		}
	}
	*/
	//list[index]='\0';	
	FILE *file=fopen("b.thm","w+");
	fseek(file,0,SEEK_SET);
//	for(int i=0;i<3;i++){
//	char * url=new char[strlen(p_list[i])+19];
	char * url=new char[500];
	strcpy(url,"http://finance.sina.com.cn/realstock/company/");
//	char url[]="hq.sinajs.cn/list=";
//	strcat(url,p_list[i]);
	strcat(url,"sh600837/nc.shtml");
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
	fwrite(buffer.c_str(),1,buffer.size(),file);
	printf("【%d】\n",buffer.find(argv[1]));
	delete url;
	curl_easy_cleanup(con);
	//}
	fclose(file);
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
	int index=0;
	char * data2=(char *)malloc(sizes*sizeof(char));
	for(int i=0;i<sizes;i++)
	{
		int spaceF=0;
		if((data[i]<32||data[i]>127)&&data[i]!='\n')
		{
			data2[index]=data[i];
			index++;
			spaceF=1;
		}	
		else {
			if(spaceF==1)
			{
				data2[index]=' ';
				index++;
				spaceF=0;
			}
		}
	}
	writerData->append(data2,index);
	free(data2);
	return sizes;
}
