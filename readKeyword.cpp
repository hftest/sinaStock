#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "libxl.h"
#include <stdlib.h>
using namespace libxl;
using namespace std;
/*
void * Read(void * arg);
bool curlInit(CURL * & conn,char *url,string *buffer); 
static int writer(char * data, size_t size,size_t nmemb,string *writerData);
static char errorBuffer[CURL_ERROR_SIZE];
static int curlWrite(char * url, char * name);
*/
int main()
{
	Book * book	= xlCreateBook();
	if(book)
    {
		if(book->load("table.csv");)
		{
			Sheet * sheet=book->getSheet(1);
		}
		Sheet* sheet = book->addSheet("Sheet1");
        if(sheet)
        {
            sheet->writeStr(2, 1, "Hello, World !");
            sheet->writeNum(3, 1, 1000);
        }
        book->save("example.xls");
        book->release();
    } 
	string search("香港中央");
	fstream input;
	int HK=0;
	int limit=5;
	input.open("g:\\SHlist.txt",ios::in);
	input.seekg(0,ios::end);
	int length=input.tellg();
	length=(length-1)/6;
	input.seekg(0,ios::beg);
	vector<string> stockno;
	vector<int> boundary;
	vector<string> name;
	vector<string> date;
	vector<long> have;
	vector<float> rate;
	char (*p_list)[7]=new char[length][7];
	{
		int index=0;
		int shift=0;
		while(shift<length&&input.good())
		{
			p_list[shift][index]=input.get();
			index++;
			if(index==6)
			{
				p_list[shift][index]='\0';
				shift++;
				index=0;
			}
			
		}
	}
	for(int i=0;i<length;i++)
	{
		int i_bound=0;
		fstream input2;
		char url[20]="g:\\data\\";
		strcat(url,p_list[i]);
		input2.open(url,ios::in);
		string a;
		int mark=0;
		int end=0;
		bool found=false;
		while(input2.good())
		{
			input2>>a;
			if((a.find(search))!=string::npos)
			{
				found=true;
				cout<<p_list[i]<<"  ";
				string tmp(p_list[i]);
				stockno.push_back(tmp);
				break;
			}
			
		}
		if(found)
		{	
			input2.seekg(0,ios::beg);
			while(input2.good())
			{
				input2>>a;
				if((a.find("截止日期"))!=string::npos)
				{
					mark++;
					if(mark>1)
					{
						i_bound++;
						//Found data of single year - BEG	
						string tmp;
						string tname;
						input2>>a;	
						input2>>a;
						tmp.append(a+"-");
						input2>>a;	
						tmp.append(a+"-");
						input2>>a;
						tmp.append(a);
						date.push_back(tmp);
						input2.seekg(84,ios::cur);
						{
							bool foundname=false;
							for(int i=0;i<10;i++)
							{
								if(i==9)
								{
									int a=0;
								}
								if(i==10)
								{
									int a=0;
								}
								if(i==8)
								{
									int a=0;
								}
								bool foundpoint=false;
								string tmp1;
								string tmp2;
								input2>>tmp1;
								tname.clear();
								while(!foundpoint)
								{
									tmp2.assign(tmp1);
									input2>>tmp1;
									if(tmp1.find(".")!=string::npos&&tmp1.find_first_of("0987654321")!=string::npos)
									{
										foundpoint=true;
									}
									else tname.append(tmp2);
								}
								//FOUND the SEARCH
								if(tname.find(search)!=string::npos)
								{	
									name.push_back(tname);
									have.push_back(atol(tmp2.c_str()));
									rate.push_back(atof(tmp1.c_str()));
									foundname=true;
									break;
								}
								else
								{
									while(tmp1.size()>2)
									{
										input2>>tmp1;
									}
								}
							}
							if(!foundname)
							{
								name.push_back(search);
								have.push_back(0);
								rate.push_back(0);
							}
						}
						//Found data of single year - END
					}
				}
			
			}
		boundary.push_back(i_bound);
		}
	}
	input.close();
	//OUTPUT BEG
	fstream output;
	output.open("d:\\stockholder\\"+search+".txt",ios::out);
	{
		vector<string>::iterator i_sno=stockno.begin();
		vector<int>::iterator i_boundary=boundary.begin();
		vector<long>::iterator i_have=have.begin();
		vector<float>::iterator i_rate=rate.begin();
		int lowbound=*i_boundary;
		output<<'\n'<<*i_sno<<"========================================"<<endl;
		for(vector<string>::iterator i=date.begin();i!=date.end();i++)
		{
			output<<*i<<" "<<*i_have<<" "<<*i_rate<<endl;
			i_have++;
			i_rate++;
			lowbound--;
			if(lowbound==0&&i_boundary!=boundary.end()-1)
			{
				i_boundary++;
				i_sno++;
				lowbound=*i_boundary;
				output<<'\n'<<*i_sno<<"========================================"<<endl;
			}
		}
		output.close();
	}
	//OUTPUT END
	//FIND the DEFFERENCE - BEG
	{
		bool foundNZ=false;
		vector<string>::iterator i_sno=stockno.begin();
		vector<int>::iterator i_boundary=boundary.begin();
		vector<long>::iterator i_have=have.begin();
		vector<float>::iterator i_rate=rate.begin();
		vector<string>::iterator i_date=date.begin();
		for(int i=0;i<stockno.size();i++)
		{
			vector<string>::iterator t_date=i_date;
			vector<long>::iterator t_have=i_have;
			cout<<*i_sno<<"==================================="<<endl;
			for(int j=0;j<limit;j++)
			{
				if(*i_have!=0&&!foundNZ)
				{
					foundNZ=true;
				}
				if(foundNZ)
				{
					cout<<*i_date<<" "<<*i_have<<" "<<(*i_have-*(i_have+1))<<endl;
					i_date++;
					i_have++;
				}
			}
			i_date=boundary[i]+t_date;
			i_have=boundary[i]+t_have;
			i_sno++;
			foundNZ=false;
		}
	}
	//FIND the DEFFERENCE - END
	return 0;
}
/*
bool curlInit(CURL * & conn,char *url,string *buffer) 
{
	CURLcode code;
	conn=curl_easy_init();
	if(conn==NULL)
	{
		fprintf(stdout,"connection is NULL after creation\n");
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_ERRORBUFFER,errorBuffer);
	if(code!=CURLE_OK)
	{
		fprintf(stdout,"fail to set error buffer: %d\n",code);
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_URL,url);
	if(code!=CURLE_OK)
	{
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_FOLLOWLOCATION,1);
	if(code!=CURLE_OK)
	{
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_WRITEFUNCTION,writer);
	if(code!=CURLE_OK)
	{
		return false;
	}
	code=curl_easy_setopt(conn,CURLOPT_WRITEDATA,buffer);
	if(code!=CURLE_OK)
	{
		return false;
	}
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
	for(int i=0;i<200;i++)
	{
		char url1[100];
		strcpy(url1,tmp1);
		strcat(url1,ptr[i]);
		strcat(url1,".phtml");	
		if((curlWrite(url1,ptr[i]))==!0)
			printf("ERRO:%s\n",ptr[i]);	
		screen[i+shifter]--;
		char url3[100];
		strcpy(url3,tmp3);
		strcat(url3,ptr[i]);
		strcat(url3,".phtml");	
		if((curlWrite(url3,ptr[i]))==!0)
			printf("ERRO:%s\n",ptr[i]);	
		screen[i+shifter]--;
	}
	screen[threadmap[tid]]=3;
	pthread_exit(NULL);
}
*/