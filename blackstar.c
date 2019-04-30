/*  This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
    I hereby claims all copyright interes in the program “blackstar.c”
    Written by Jon Cox at cloneozone@gmail.com.
 
    You may compile this code with the following:
    
    gcc -O0 blackstar.c -o blackstar -lpthread -s */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <pthread.h>

/* counter for threads */
int count = 0;

/* perform_work function args struct */
typedef struct {
    char *arg_1;
} arg_struct;

union options {
  char *user;
  char *host;
  char *pass;  
  char *sshpass;
  char *port; 
  char *dcrypt;
  char *ccrypt; 
  char *input;
  char *exec;
};

int usage(){
	printf("\nUsage: ./blackstar [flags] [arguments]\n"
            "-f  Path to file of hosts.\n"
            "-l  Host: ip or domain.\n"
            "-u  Username: should have sudo, but not enforced.\n"
            "-p  Port: if different that 22 for SSH.\n"
	    "-w  Password for user.\n"
	    "-d  Decrypt password file made with ccrypt.\n"
            "-e  Commands to be executed. Use quotes for multiple.\n"
	    "-o  Skip host checking\n"
	    "-h  Usage text.\n\n");
	return 1;
}

/* function to execute in every thread */
void *perform_work(void *argument){
   arg_struct *actual_args = argument;
   system(actual_args->arg_1);
   ++count;

   return NULL;
}

int file_open(char *name, char *comm){
    int lines_allocated = 100;
    int	max_line_len = 100;

    /* allocate lines of text */
    char **words = (char **)malloc(sizeof(char*)*lines_allocated);
    if (words==NULL) {
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }

    FILE *fp = fopen(name, "r");
    int i;
    for (i=0;1;i++) {
        int j;

        /* have we gone over our line allocation? */
        if (i >= lines_allocated) {
            int new_size;

            /* Double our allocation and re-allocate */
            new_size = lines_allocated*2;
            words = (char **)realloc(words,sizeof(char*)*new_size);
            if (words==NULL){
                fprintf(stderr,"Out of memory.\n");
                exit(3);
            }
            lines_allocated = new_size;
        }

        /* allocate space for the next line */
        words[i] = malloc(max_line_len);
        if (words[i]==NULL){
            fprintf(stderr,"Out of memory (3).\n");
            exit(4);
        }
        if (fgets(words[i],max_line_len-1,fp)==NULL)
            break;

        /* get rid of CR or LF at end of line */
        for (j=strlen(words[i])-1;j>=0 && (words[i][j]=='\n' || words[i][j]=='\r');j--)
            ;
        words[i][j+1]='\0';
    }
    fclose(fp);

    pthread_t threads[i];
    int result_code, index;
    int *ptr[i];

    /* create all threads one by one */
    arg_struct *args = malloc(sizeof(arg_struct)*i);
    for (index = 0; index < i; ++index) { 
        args[index].arg_1 = malloc((sizeof(words) + sizeof(comm))*i);    
        strcpy(args[index].arg_1, words[index]);
	strcat(args[index].arg_1, comm);
        result_code = pthread_create(&threads[index], NULL, perform_work, &args[index]);
        assert(0 == result_code);
    }

    /* wait for each thread to complete */
    for (index = 0; index < i; ++index) {
      // block until thread 'index' completes
      result_code = pthread_join(threads[index], (void**)&(ptr[index]));
      assert(0 == result_code);
    }

    /* free memory */
    for (;i>=0;i--)
        free(words[i]);
    free(words);

    return 0;
}

int main (int argc, char **argv) {
  unsigned short int fflag, lflag, uflag, wflag, pflag, eflag, oflag, dflag = 0;
  short int opt = 0;

  union options opt1, *ptr1 = &opt1;
  union options opt2, *ptr2 = &opt2;
  union options opt3, *ptr3 = &opt3;
  union options opt4, *ptr4 = &opt4;
  union options opt5, *ptr5 = &opt5;
  union options opt6, *ptr6 = &opt6;
  union options opt7, *ptr7 = &opt7;
  union options opt8, *ptr8 = &opt8;
  union options opt9, *ptr9 = &opt9;

  opt1.input = NULL;
  opt2.host = NULL;
  opt3.user = NULL;
  opt4.port = NULL;
  opt5.pass = NULL;
  opt6.dcrypt = NULL;
  opt7.exec = NULL;
  opt8.sshpass = NULL;
  opt9.ccrypt = NULL;

while(optind < argc) {
  if(( opt = getopt(argc, argv, "f:l:u:p:w:e:d:ho")) != -1){
   switch(opt){
     case 'f':
       fflag = 1;
       ptr1->input=(char *)malloc(strlen(optarg) + 1);
       strcpy(ptr1->input, optarg);
       break;
     case 'l':
       lflag = 1;
       ptr2->host=(char *)malloc(strlen(optarg) + 1);
       strcpy(ptr2->host, optarg);
       break;
     case 'u':
       uflag = 1;
       ptr3->user=(char *)malloc(strlen(optarg) + 1);
       strcpy(ptr3->user, optarg);
       break;
     case 'p':
       pflag = 1;
       ptr4->port=(char *)malloc(strlen(optarg) + 1);
       strcpy(ptr4->port, optarg);
       break;
     case 'w':
       wflag = 1;
       ptr5->pass=(char *)malloc(strlen(optarg) + 1);
       strcpy(ptr5->pass, optarg);
       break;
     case 'd':
       dflag = 1;
       ptr6->dcrypt=(char *)malloc(strlen(optarg) + 1);
       strcpy(ptr6->dcrypt, optarg);
       break;
     case 'e':
       eflag = 1;
       ptr7->exec=(char *)malloc(strlen(optarg) + 1);
       strcpy(ptr7->exec, " ");
       strcat(ptr7->exec, optarg);
       break;
     case 'o':
       oflag = 1;
       break;
     case 'h':
       usage();
       break;
     default:
      usage();
      break;
     }
   }
   else {
      optind++;
   }
}

   if(dflag == 1){
	ptr9->ccrypt=(char *)malloc(strlen(ptr6->dcrypt) + 1);   
	strcpy(ptr9->ccrypt, "ccrypt -d ");
	strcat(ptr9->ccrypt, ptr6->dcrypt);
	system(ptr9->ccrypt);
   }

   if (fflag == 1 && eflag == 1){ 
	file_open(ptr1->input, ptr7->exec);
   }
   else  if(fflag == 0 && lflag == 1 && uflag == 1 && eflag == 1){
	ptr8->sshpass=(char *)malloc((sizeof(ptr2->host) + sizeof(ptr3->user) + sizeof(ptr7->exec) + sizeof(ptr4->port) + sizeof(ptr5->pass)) + 1);
	if(wflag == 1){
		strcpy(ptr8->sshpass, "sshpass -p \"");
		strcat(ptr8->sshpass, ptr5->pass);
		strcat(ptr8->sshpass, "\" ");
	}
	strcat(ptr8->sshpass, "ssh ");
	if(oflag == 1){
		strcat(ptr8->sshpass, "-o StrictHostKeyChecking=no ");
	}
	strcat(ptr8->sshpass, "-T ");
	strcat(ptr8->sshpass, ptr3->user);
	strcat(ptr8->sshpass, "@");
	strcat(ptr8->sshpass, ptr2->host);
	strcat(ptr8->sshpass, " '");
	strcat(ptr8->sshpass, ptr7->exec);
	strcat(ptr8->sshpass, "'");
	if(pflag == 1){
		strcat(ptr8->sshpass, " -p ");
		strcat(ptr8->sshpass, ptr4->port);
	}
	//system(ptr8->sshpass);
	printf("%s\n", ptr8->sshpass);
   }
   else{
	usage();
   }	   

   if(dflag == 1){ 
	printf("*File was decrypted for usage. Now re-encrypt it.*\n");
	ptr6->dcrypt[strlen(ptr6->dcrypt)-4] = 0;
	strcpy(ptr9->ccrypt, "ccrypt ");
        strcat(ptr9->ccrypt, ptr6->dcrypt);	
	system(ptr8->sshpass);	
   }

   if(!ptr1->input){free(ptr1->input);}
   if(!ptr8->sshpass){free(ptr8->sshpass);} 
   if(!ptr2->host){free(ptr2->host);} 
   if(!ptr3->user){free(ptr3->user);} 
   if(!ptr7->exec){free(ptr7->exec);} 
   if(!ptr5->pass){free(ptr5->pass);} 
   if(!ptr4->port){free(ptr4->port);} 
   if(!ptr9->ccrypt){free(ptr9->ccrypt);} 
   if(!ptr6->dcrypt){free(ptr6->dcrypt);}  

  return 0;
}
