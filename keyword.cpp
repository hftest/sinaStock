//finance.sina.com.cn/stock/message/gxq/sz000776/ggzd.html
//money.finance.sina.com.cn/corp/go.php/vFD_AchievementNotice/stockid/600847.phtml
//vip.stock.finance.sina.com.cn/corp/go.php/vCI_CirculateStockHolder/stockid/6000837.phtml
#include <stdio.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <curl/curl.h>
#include <pthread.h>
#include <map>
using namespace std;
pthread_mutex_t lock;
const int MAX_LIST=926;
static int * screen;
map<int,int> threadmap;
void *display(void *);
void * Read(void * arg);
bool curlInit(CURL * & conn,char *url,string *buffer); 
static int writer(char * data, size_t size,size_t nmemb,string *writerData);
static char errorBuffer[CURL_ERROR_SIZE];
static int curlWrite(char * url, char * name);
int main(int argc,char ** argv)
{
	//char url[]="hq.sinajs.cn/list=sh701006";
	char (*p_list)[7];
	p_list=new char[1000][7];
	FILE * SHlist=fopen("SHlist.l","r");
	fseek(SHlist,2,SEEK_SET);
	int end;
	int index=0;
	int i_list=0;
	pthread_mutex_init(&lock,NULL);
	while((end=getc(SHlist))!=EOF)
	{
		if(index==6)
		{
			p_list[i_list][index]='\0';
			//printf("%s\n",p_list[i_list]);
			i_list++;
			index=0;
		}
		else if(end<='9'&&end>='0')
		{
			p_list[i_list][index]=end;
			index++;
		}
	}
	screen=new int[803];
	for(int i=0;i<803;i++)
	{
		screen[i]=2;
	}
	printf("[%d]\n",i_list);
	char (*arg1)[7]=p_list;
	char (*arg2)[7]=p_list+200;
	char (*arg3)[7]=p_list+400;
	char (*arg4)[7]=p_list+600;
	//char (*arg5)[7]=p_list+800;
	pthread_t threads[5];
	pthread_t display_t;
	pthread_create(&threads[0],NULL,Read,arg1);
	pthread_create(&threads[1],NULL,Read,arg2);
	pthread_create(&threads[2],NULL,Read,arg3);
	pthread_create(&threads[3],NULL,Read,arg4);
	pthread_create(&display_t,NULL,display,NULL);
	//pthread_create(&threads[4],NULL,Read,arg5);
	pthread_join(threads[0],NULL);
	pthread_join(threads[1],NULL);
	pthread_join(threads[2],NULL);
	pthread_join(threads[3],NULL);
	pthread_join(display_t,NULL);
	//pthread_join(threads[4],NULL);
	pthread_mutex_destroy(&lock);
	return 0;
}
bool curlInit(CURL * & conn,char *url,string *buffer) 
{
	CURLcode code;
	conn=curl_easy_init();
//	printf("1");
	if(conn==NULL)
	{
		fprintf(stdout,"connection is NULL after creation\n");
		return false;
	}
//	printf("2");
	code=curl_easy_setopt(conn,CURLOPT_ERRORBUFFER,errorBuffer);
	if(code!=CURLE_OK)
	{
		fprintf(stdout,"fail to set error buffer: %d\n",code);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_URL,url);
//	printf("3");
	if(code!=CURLE_OK)
	{
//		fprintf(stdout,"Failed to set url|%s|:|%s|\n",url,errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_FOLLOWLOCATION,1);
//	printf("4");
	if(code!=CURLE_OK)
	{
//		fprintf(stdout,"Fail to set redirection option |%s|\n",errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,writer);
//	printf("5");
	if(code!=CURLE_OK)
	{
		//fprintf(stdout,"Failed to set writer |%s|\n",errorBuffer);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_WRITEDATA,buffer);
//	printf("6");
	if(code!=CURLE_OK)
	{
		//fprintf(stdout,"Failed to set buffer |%s|\n",errorBuffer);
		return false;
	}
//	printf("7");
	return true;
}

static int writer(char * data, size_t size,size_t nmemb,string *writerData)
{
	unsigned long sizes=size * nmemb;
	if(writerData==NULL) return 0;
	int index=0;
	char * data2=new char[sizes];
	int spaceF=0;
	for(int i=0;i<sizes;i++)
	{
		if(((data[i]<32||data[i]>127)&&data[i]!='\n'&&data[i]!=13&&data[i]!=9)||(data[i]<='9'&&data[i]>='0')||data[i]=='.')
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
	delete data2;
	return sizes;
}
static int curlWrite(char * url,char * name)
{
	FILE *file=fopen(name,"a+");
	fseek(file,0,SEEK_END);
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
//	printf("length: %d\n",buffer.size());
	fwrite(buffer.c_str(),1,buffer.size(),file);
	curl_easy_cleanup(con);
	fclose(file);
	return 0;
}
void * Read(void * arg)
{
	int shifter=3;
	int tid=(int) pthread_self();
	pthread_mutex_lock(&lock);
	threadmap[tid]=threadmap.size()-1;
	pthread_mutex_unlock(&lock);
	shifter+=threadmap[tid]*200;
	char (*ptr)[7]=(char(*)[7])arg;
	char tmp1[]="http://money.finance.sina.com.cn/corp/go.php/vFD_AchievementNotice/stockid/";
	char tmp2[]="http://finance.sina.com.cn/stock/message/gxq/sh";
	char tmp3[]="http://vip.stock.finance.sina.com.cn/corp/go.php/vCI_CirculateStockHolder/stockid/";
	//TEST
//	printf("id: %d, shift: %d\n",tid,shifter);
	for(int i=0;i<200;i++)
	{
		char url1[100];
		strcpy(url1,tmp1);
		strcat(url1,ptr[i]);
		strcat(url1,".phtml");	
		if((curlWrite(url1,ptr[i]))==!0)
			printf("ERRO:%s\n",ptr[i]);	
		screen[i+shifter]--;
//		printf("id: %d, shift: %d, map: %d\n",tid,shifter,threadmap[tid]);
	/*	char url2[100];
		strcpy(url2,tmp2);
		strcat(url2,ptr[i]);
		strcat(url2,"/ggzd.html");	
		if((curlWrite(url2,ptr[i]))==!0)
			printf("ERRO:%s\n",ptr[i]);	
*/
		char url3[100];
		strcpy(url3,tmp3);
		strcat(url3,ptr[i]);
		strcat(url3,".phtml");	
		if((curlWrite(url3,ptr[i]))==!0)
			printf("ERRO:%s\n",ptr[i]);	
		screen[i+shifter]--;
//		printf("id: %d, shift: %d\n",threadmap[tid],i+shifter);
	}
	screen[threadmap[tid]]=3;
	pthread_exit(NULL);
}
void * display(void * arg)
{
	while(screen[0]!=3||screen[1]!=3||screen[2]!=3)
	{
		sleep(2);
		char str1[201];
		char str2[201];
		char str3[201];
		char str4[201];
		for(int i=0;i<200;i++)
		{
			str1[i]=screen[i]+'0';
			str2[i]=screen[i+200]+'0';
			str3[i]=screen[i+400]+'0';	
			str4[i]=screen[i+600]+'0';	
		}
		str1[200]='\0';
		str2[200]='\0';
		str3[200]='\0';
		str4[200]='\0';
		system("clear");
		fprintf(stdout,"|%s|\n|%s|\n|%s|\n|%s|\n",str1+3,str2+3,str3+3,str4+3);	
		fflush(stdout);
	}
}
