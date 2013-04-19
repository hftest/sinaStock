#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
using namespace std;
bool curlInit(CURL * & conn,char *url,string *buffer); 
static int writer(char * data, size_t size,size_t nmemb,string *writerData);
static char errorBuffer[CURL_ERROR_SIZE];
static int curlRead(char * url, char * result);
int reply(int fd)
{
	char line[100];
	pid_t child;
	memset(line,'0',100*sizeof(char));
	int readSize=0;
		if((readSize=read(fd,line,99))==0)
			return 0;
		printf("##received a connection##: %s\n",line);
		char year[5];
		char month[3];
		char day[3];
		char stockno[7];
		strncpy(year,line,4);
		strncpy(month,line+5,2);
		strncpy(day,line+8,2);
		strncpy(stockno,line+11,6);
		year[4]='\0';
		month[2]='\0';
		day[2]='\0';
		stockno[6]='\0';
		if(month[1]=='0')
		{	
			month[0]='0';
			month[1]='9';
		}
		else
		{
			month[1]--;
		}
		printf("##REQUEST##: %s-%s-%s for %s\n Processing...\n",year,month,day,stockno);
		char url[200]="http://ichart.yahoo.com/table.csv?s=";
		strcat(url,stockno);
		strcat(url,".SS&a=");
		strcat(url,month);
		strcat(url,"&b=");
		strcat(url,day);
		strcat(url,"&c=");
		strcat(url,year);
		strcat(url,"&d=");
		strcat(url,month);
		strcat(url,"&e=");
		strcat(url,day);
		strcat(url,"&f=");
		strcat(url,year);
		printf("#: %s\n",url);
	//	if((child=fork())==0)
	//	{
			char urlt[]="hq.sinajs.cn/list=sh000001";
			curlRead(urlt,line);
			printf("curl finished\n");
			printf("contain1: %s",line);
			write(fd,line,strlen(line));	
			printf("contain2: %s",line);
	//		exit(0);
		//}
//		waitpid(child,NULL,0);
	
}
int main(int argc,char ** argv)
{
	int listenfd,connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(9877);
	bind(listenfd,(sockaddr *)&servaddr,sizeof(servaddr));
	listen(listenfd,1024);
	printf("Data server started\n");
	while(true)
	{

		clilen=sizeof(cliaddr);
		connfd=accept(listenfd,(sockaddr *)&cliaddr,&clilen);
	//	if((childpid=fork())==0)
	//	{
	//		close(listenfd);
			reply(connfd);
			printf("Returned\n");
	//		exit(0);
	//	}
	//	waitpid(childpid,NULL,0);
		close(connfd);
	}
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
		fprintf(stdout,"Failed to set url|%s|:|%s|\n",url,errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_FOLLOWLOCATION,1);
	printf("4");
	if(code!=CURLE_OK)
	{
		fprintf(stdout,"Fail to set redirection option |%s|\n",errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,writer);
	printf("5");
	if(code!=CURLE_OK)
	{
		fprintf(stdout,"Failed to set writer |%s|\n",errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_WRITEDATA,buffer);
	printf("6");
	if(code!=CURLE_OK)
	{
		fprintf(stdout,"Failed to set buffer |%s|\n",errorBuffer);
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
static int curlRead(char * url,char * result)
{
	CURL *con=NULL;
	CURLcode code;
	string buffer;
	printf("\n**1");
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
	printf("9");
	strcpy(result,buffer.c_str());
	curl_easy_cleanup(con);
	printf("\ncleaned up\n");
	printf("result: %s\n",result);
	return 0;
}
