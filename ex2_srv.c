#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>

/*the server get a request with signal calculate and send the result in file
to_client_xxxxxx
*/


void recivedClient(int signal1)
{ 
    signal(SIGUSR2,recivedClient);//reannouncing the signal
    //variables
    char pidArray[32];
    char firstNum[50];
    char arithmetic[2];
    char secondNum[50];
    char resultArray[50];
    char buf[2]={0,0};
    int i=0;
    int firstNum1,arithmetic1,secondNum1,result;
    int fd=open("to_srv",O_RDONLY),fdResult;
    //opening to_srv
     if (fd<0)
    {
     write(2,"Erro:can not open file\n",25);
     exit(-1);
     }
    
   //trying to opn son process

    pid_t child_pid;
    if((child_pid=fork())<0)
    {//if fork doesnt work
    write(2,"Erro:can not fork\n",20);
    remove("to_srv");
    exit(-1);
    }
 /*if we are at the child process we do the calculation and send the result in file 
  to_client_xxxxxx” */
  if(child_pid==0) 
  {
    int charsread;
    //reading information from file to_srv
 
        //client pid//

	if((charsread=read(fd,buf,1))<0)
        {
           perror("cant not read file");
           remove("to_srv");
           exit(-1);
        }
	while(buf[0]!='\n'&&buf[0]!=EOF)
	{
	 pidArray[i]=buf[0];
	 i++;
	 if((charsread=read(fd,buf,1))<0)
        {
           perror("cant not read file");
           remove("to_srv");
           exit(-1);
        }

	}
	pidArray[i]='\0';
	i=0;
        int pidClient=atoi(pidArray);
	//reading first number//
	if((charsread=read(fd,buf,1))<0)
        {
           perror("cant not read file");
           kill(pidClient,SIGUSR1);
           exit(-1);
        }
	while(buf[0]!='\n'&&buf[0]!=EOF)
	{
	 firstNum[i]=buf[0];
	 i++;
	 if((charsread=read(fd,buf,1))<0)
        {
           perror("cant not read file");
           remove("to_srv");
           kill(pidClient,SIGUSR1);
           exit(-1);
        }

	}

	
	firstNum[i]='\0';
	i=0;
       //reading if add/sub...//
	if((charsread=read(fd,buf,1))<0)
        {
           perror("cant not read file");
           remove("to_srv");
           kill(pidClient,SIGUSR1);
           exit(-1);
        }
	while(buf[0]!='\n'&&buf[0]!=EOF)
	{
	arithmetic[i]=buf[0];
	 i++;
         
	 if((charsread=read(fd,buf,1))<0)
        {
           perror("cant not read file");
           remove("to_srv");
           kill(pidClient,SIGUSR1);
           exit(-1);
        }

	}
        
	arithmetic[i]='\0';
	
	i=0;
        //reading second number//
	if((charsread=read(fd,buf,1))<0)
        {
           perror("cant not read file");
           remove("to_srv");
           kill(pidClient,SIGUSR1);
           exit(-1);
        }
	while(buf[0]!='\n'&&buf[0]!=EOF)
	{
	 secondNum[i]=buf[0];
	 i++;
	if((charsread=read(fd,buf,1))<0)
        {
           perror("cant not read file");
           remove("to_srv");
           kill(pidClient,SIGUSR1);
           exit(-1);
        }

	}
	secondNum[i]='\0';
	i=0;
        //removing the file for next client
        remove("to_srv");
        //checking what the client asked and calculate
       
       firstNum1 = atoi(firstNum);
       arithmetic1 = atoi(arithmetic);
       secondNum1 = atoi(secondNum);
       
         //opening result file
         char result_file[60];
	 result_file[0]='\0';
	 strcat(result_file,"to_client_");
	 strcat(result_file,pidArray);
         //opening the file for the client
	 if((fdResult=open(result_file,O_CREAT|O_WRONLY,0666))<0)
         {
        
           perror("cant open file");
           kill(pidClient,SIGUSR1);
           exit(-1);
           
         }
       //switch case to know what result to send to client
        switch (arithmetic1) 
	   { 
	       case 1:  result=firstNum1+secondNum1; //add
		       break; 
	       case 2:  result=firstNum1-secondNum1 ;//sub
		        break; 
	       case 3: result=firstNum1*secondNum1 ; //multiply
		       break; 
	       case 4: //divide
               //if asking to divide by zero
	       if(secondNum1==0)
                {
                 resultArray[0]='\0';
                 strcat(resultArray,"can not divide by 0");
		 write(fdResult,resultArray,strlen(resultArray));
		 kill(pidClient,SIGUSR1);
                 exit(-1);

                }
	       else
		result=firstNum1/secondNum1 ; 
		       break; 
	       default:
                {
                  resultArray[0]='\0';
                  strcat(resultArray,"you can choose only 1/2/3/4");
		  write(fdResult,resultArray,strlen(resultArray));
                  kill(pidClient,SIGUSR1);
                  exit(-1);
                }
	   
	   } 
         
        
        
         
        //writing  the result client back using the file to_client_xxxxxx
        
       sprintf(resultArray, "%d", result); 
       resultArray[strlen(resultArray)]='\0';
       if(write(fdResult,resultArray,strlen(resultArray))<0)
       {
        perror("Error:could'nt write to file");
        close(fdResult);
        remove(result_file); 
        kill(pidClient,SIGUSR1);
        exit(-1);
       }
    
    //send the signal and exit child code
    kill(pidClient,SIGUSR1);            
    exit(0);
  }

}



int main(int argc,char*argv[])
  {
   
   signal(SIGUSR2,recivedClient);
   //to avoid zombies
   signal(SIGCHLD,SIG_IGN);
   if (access("to_srv", F_OK) != -1)
   {
     
     remove("to_srv");  /* delete the existing file */
   }
   //doesnt waist cpu most of the time he spend in pause
   while(1)
   {
    pause();
    signal(SIGCHLD,SIG_IGN);
   } 
//since it is not specified i decided to shut the server using the terminal
   exit(0);
  }