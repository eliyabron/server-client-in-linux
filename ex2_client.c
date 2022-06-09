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
#define SIZE 1

/* the client code send request to the server using SIGUSR2
   the server send a signal using SIGUSR1 so the client looks for
   file to_client_xxxxxx and print the result.
*/


//print the result when signal from server is sent

void printResults(int signal1)
{
 //variables
 int resultFd;
 char result_file[50];
 result_file[0]='\0';
 strcat(result_file,"to_client_");
 char myPid[40];
 myPid[0]='\0';
 sprintf(myPid,"%d",getpid());
 strcat(result_file,myPid);
 //try to open file if there was some problem un server 
 if((resultFd=open(result_file,O_RDONLY))<0)
     {
       perror("Error:client says could'nt get file");
       exit(-1);
     }
 int counter2=0;
 int charsrFile=1;
 char result[40];
 result[0]='\0';
 char buf[2]; /*input buffer for reading file*/
 //reading file
   charsrFile=read(resultFd,buf,SIZE);
   while(charsrFile!=0)
    {
       
       
       if(charsrFile<0)
       {
         perror("Error:could'nt read file");
        /*free allocated memory */
         close(resultFd);
         remove(result_file);
         exit(-1);
       }
       
      result[counter2]=buf[0];
      counter2++;
      charsrFile=read(resultFd,buf,SIZE);
     

     } 
   //if the file was empty
 if(charsrFile==0&&counter2==0)
       {
         perror("Error:file empty");
         close(resultFd);
         remove(result_file);
         exit(-1);
       }
    result[counter2]='\n';
    result[counter2+1]='\0';
  write(1,result,strlen(result));
  remove(result_file);

}




int main(int argc,char*argv[])
  {
   //anouncing the signal
   signal(SIGUSR1,printResults);
   pid_t serverPid=atoi(argv[1]);
   int toServerfd,counter=0,flag=0;
   
  
   //checking for enough arguments
   if(argc!=5)
     {
       perror("Error:not the right amount of arguments");
       exit(-1);
     }

   //trying to open file
   while(flag==0&&counter<10)

   {//if client can not open file try again in a few second until 10 tries
     if((toServerfd=open("to_srv",O_CREAT|O_EXCL|O_WRONLY,0666))<0)
     {
        counter++;
        sleep((rand()%5)+1);

     }
     else
       flag=1;
   
    }

  if (flag==0)
  {
       perror("Error:could'nt open file");
       exit(-1);
  }
  char textToServer[200];//cant be over 200 chars for sure
  textToServer[0]='\0';
 
  char myPid[40];
  myPid[0]='\0';

  sprintf(myPid,"%d",getpid());
  //writing text to server
  strcat(textToServer,myPid);
  strcat(textToServer,"\n");
  strcat(textToServer,argv[2]);
  strcat(textToServer,"\n");
  strcat(textToServer,argv[3]);
  strcat(textToServer,"\n");
  strcat(textToServer,argv[4]);
  strcat(textToServer,"\n");
  if(write(toServerfd,textToServer,strlen(textToServer))<0)
  {
       perror("Error:could'nt write to file");
       close(toServerfd);
        remove("to_srv");  /* delete the existing file */
       exit(-1);
  }

  close(toServerfd);
  //send signal
  kill(serverPid,SIGUSR2);
  pause();


   exit(0);
 }
