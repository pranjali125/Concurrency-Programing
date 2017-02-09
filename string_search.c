
//This is a string search program with POSIX threads.
// This is a working code which satisfies all the requirements of the concurrency question.
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include <sys/stat.h>
#include <stdlib.h>
#include<time.h>
char * buff;

int fileSize=0;
char fileName[20]="file.txt";

int NUM_THREADS=0;
#define NUM_THREADS_MAX 200


int findsPerThread[NUM_THREADS_MAX]={0};

void *SearchOperation(void * my_argv);

struct srchPara
{
	
	char * fname;
	char * str;
	int threadNum; //to keep thread number
	int offset; // to store the offset to read file.
	int blockSize;
	int num;
}srchPara;

void readStdInput(FILE *input)
{
 char c;
 FILE *fpWrite ;
 int retvalue;
 
 while (EOF != (c = fgetc(input))) {
		         retvalue++;
		}


		
    	fpWrite = fopen(fileName, "w+");

   	//printf("Size of file: %d\n", retvalue);
   	fseek(input,0,SEEK_SET);
  	fread(buff,retvalue+1,1,input);
  	fseek(fpWrite,0,SEEK_SET);
  	fwrite(buff, strlen(buff) + 1, 1, fpWrite);
	 
	fclose(fpWrite);
	fileSize=strlen(buff);
 	//printf("Done writing file\n");  
}


void *SearchOperation(void * my_argv)
{	//--Declarations--//	
	FILE *fp;
	long pos;
	struct srchPara *my_data;
	char *temp;
	char *tempRem;
	char *p1, *p2, *p3;
	int i,j;
	
	//--End of Declarations--//
	
	//--Initialisation----
	my_data = (struct srchPara *) my_argv;
	temp=malloc((my_data->blockSize)+1);
	//--End of Initialiasation--//
	
	if((fp=fopen(my_data->fname,"r"))==NULL)
	{
		perror("Error opening file");
		
		
	}
	else
	{
		fseek(fp,my_data->offset,SEEK_SET);
		pos = ftell(fp);
		//printf("t--%d--pos--%ld--bs--%d--of--%d\n",my_data->threadNum,pos,my_data->blockSize,my_data->offset);
		if(fread(temp,1,(my_data->blockSize),fp)!=0)
		{	
			temp[my_data->blockSize]='\0';
			//printf("The offset %d and blocksize %d temp length %d for thread number %d. \n\n",my_data->offset,my_data->blockSize,strlen(temp),my_data->threadNum);
					
	  			p1=temp;
				p2=my_data->str;
				for(i = 0; i<my_data->blockSize; i++)
	  			{	//printf("%di=%d\n",my_data->threadNum,i);
				   	if(*p1 == *p2) //checks if matches with the first character of the search string.
	      				{
			  			p3 = p1;
			  			for(j = 0;j<strlen(my_data->str);j++)
			  			{
			    				if(*p3 == *p2)
			    				{
			      					p3++;p2++; // something matched
			    				} 
			    				else
			      					break;
			  			}
			  			p2 = my_data->str;
			  			if(j == strlen(my_data->str)) //checking if entire searc string was matched.
			  			{
			     				
			      			           if(i !=0 ) // to consider the overlap case. In order to avoid double counting.
			      			            {	
								    	my_data->num=my_data->num+1;
								    	
							    }
							    else
							    {
							    	if(my_data->threadNum==1)
							    	{
							    			
								    	my_data->num=my_data->num+1;
								    		
							    	}
							    }
							    
							   // my_data->num=my_data->num+1;
			 			 }	
	      				}
	    				p1++; 
	  			}
				//pthread_mutex_lock (&mutexArray);
				findsPerThread[((my_data->threadNum)-1)]=my_data->num;
				//pthread_mutex_unlock (&mutexArray);
		
		
		
			
		}
		fclose(fp);
	}	
	free(temp);
	pthread_exit((void*) 0);

}

	
int main(int argc, char *argv[])
{
	//----Declarations----//
	buff=(char *) malloc(16000000);
	FILE *input;
	input = stdin;
	int blockSize,i,rc,rcj,iter;

 	void *status;
 	clock_t start,end;
 	double cpu_time_used;
 	int totalFindArray=0; // to add the values per thread.
	int searchStrlen=strlen(argv[1]); // length of the string to be searched for.
	char *searchString;
	//----End of Declarations----
	
	//--initialisations----//
	
	searchString=argv[1];
	readStdInput(input);
	
	//creation of threads.
	printf("Strn length:%d\n",searchStrlen);
	for (iter=1;iter<=NUM_THREADS_MAX;iter++)
	{
		NUM_THREADS=iter;
		pthread_t threads[NUM_THREADS];
		struct srchPara s[NUM_THREADS];
		totalFindArray=0; 
		for (i = 0; i < NUM_THREADS_MAX; i++)
		{
		  findsPerThread[i] = 0;
		}
		
		blockSize=(fileSize/NUM_THREADS);
	
	
		if(blockSize<searchStrlen)
		{
			printf("Blocksize smaller than search string. Exiting the Program.\n");
			return(0);
		}
	
		start=clock(); 
		for (i=0;i<NUM_THREADS;i++)
		{
			//initialise structures
			s[i].fname=fileName;
			s[i].str=searchString;
			s[i].threadNum=i+1;
			s[i].num=0;

			if(i==0)
			{
				s[i].offset=0;
			}
			else
			{
				s[i].offset=i*(blockSize);
			}
			 s[i].blockSize=blockSize+searchStrlen;//searchStrlen added to make the blocks overlap so that if the string being searched for if is in two different blocks then also should be found.
			if(i==(NUM_THREADS-1))//last thread
			{
				s[i].blockSize=fileSize-s[i].offset;
			}
			//printf("thread in main %d blockSize %d.\n",i+1,s[i].blockSize);
			//create threads
			rc=pthread_create(&threads[i],NULL,SearchOperation, (void *) &s[i]);

			if(rc!=0)
			{
				printf("Error occured in creation of thread number : %d-rc-%d\n",i+1,rc);
			}
		}
		//printf("\nIn main--waiting for threads to join.\n");
		for (i=0;i<NUM_THREADS;i++)
		{
			//create threads
			rcj=pthread_join(threads[i],&status);
		}
		//printf("\nThreads joined.\n");
		end=clock();

		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		for (i=0;i<NUM_THREADS;i++)
		{
			//printf("%d--%d\n",i,findsPerThread[i]);
			totalFindArray=totalFindArray+findsPerThread[i];
		}
	
		printf("blockSize:\t%d\tnumOfThread:\t%d\tmatchCount:\t%d\trunningTime:\t%f\n",blockSize,NUM_THREADS,totalFindArray,cpu_time_used);
	}	
	//printf("\n--Done--\n");
	free(buff);
	pthread_exit(NULL);
	return (0);
}


