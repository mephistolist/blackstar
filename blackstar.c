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
    
    gcc -Ofast blackstar.c -o blackstar -lpthread -s --std=c18 
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>

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
  unsigned short int fflag; 
  unsigned short int lflag; 
  unsigned short int uflag; 
  unsigned short int wflag; 
  unsigned short int pflag; 
  unsigned short int eflag; 
  unsigned short int oflag; 
  unsigned short int dflag;
  int lines_allocated;
  int max_line_len;
  int i;
  int j;
  int new_size;
  int result_code;
  char **words;
  int index;
  int count;
  int opt;
  unsigned short int x;
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
	    "-h  Usage text.\n\n");
	return 0;
}

void single(char* howmany, ...){ 
        int argc=0;
        char *argv[250];
	union options opt8, *ptr8 = &opt8;
	opt8.sshpass = NULL;
        va_list ap; 
        va_start(ap, howmany);

        while((argc<(16-2))&&howmany[argc++]) argv[argc]=va_arg(ap, char *);

        argv[argc]=NULL;         
	va_end(ap);

	if(argv[5] == NULL){
		ptr8->sshpass = (char *)malloc(strlen(argv[3]) + strlen(argv[1]) + strlen(argv[2]) + strlen(argv[4]) + 58);
	}
	else{
		ptr8->sshpass = (char *)malloc(strlen(argv[3]) + strlen(argv[1]) + strlen(argv[2]) + strlen(argv[4]) + strlen(argv[5]) + 58);
	}

	if(argv[3] != NULL){	
		strcpy(ptr8->sshpass, "sshpass -p \"");
		strcat(ptr8->sshpass, argv[3]);
		strcat(ptr8->sshpass, "\" ");
	}
	strcat(ptr8->sshpass, "ssh -o StrictHostKeyChecking=no -T ");
	if(argv[5] != NULL){
		strcat(ptr8->sshpass, " -p ");
		strcat(ptr8->sshpass, argv[5]);
		strcat(ptr8->sshpass, " ");
	}
	strcat(ptr8->sshpass, argv[1]);
	strcat(ptr8->sshpass, "@");
	strcat(ptr8->sshpass, argv[2]);
	strcat(ptr8->sshpass, " \"");
	strcat(ptr8->sshpass, argv[4]);
	strcat(ptr8->sshpass, "\"");

	system(ptr8->sshpass);
	free(ptr8->sshpass);
}

/* function to execute in every thread */
void *perform_work(void *argument){
   union options opt25, *ptr25 = &opt25;
   opt25.count = 0;

   arg_struct *actual_args = argument;
   system(actual_args->arg_1);
   ++ptr25->count;
   return NULL;
}

int file_open(char *name, char *comm){

    union options opt17, *ptr17 = &opt17;
    union options opt18, *ptr18 = &opt18;
    union options opt19, *ptr19 = &opt19;
    union options opt20, *ptr20 = &opt20;
    union options opt21, *ptr21 = &opt21;
    union options opt22, *ptr22 = &opt22;
    union options opt23, *ptr23 = &opt23;
    union options opt24, *ptr24 = &opt24;

    opt17.lines_allocated = 100;
    opt18.max_line_len = 100;
    opt19.i = 0;
    opt20.j = 0;
    opt21.new_size = 0;
    opt22.result_code = 0;
    opt23.words = (char **)malloc(sizeof(char*)*ptr17->lines_allocated);
    opt24.index = 0;

    if( access(name, F_OK) == -1 ){
	fprintf(stderr, "File not found.\n");
	usage();
	exit(2);
    }
	
    if (ptr23->words==NULL) {
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }

    FILE *fp = fopen(name, "r");
    for(;1;ptr19->i++) {

        /* have we gone over our line allocation? */
        if (ptr19->i >= ptr17->lines_allocated) {

            /* Double our allocation and re-allocate */
            ptr21->new_size = ptr17->lines_allocated*2;
            ptr23->words = (char **)realloc(ptr23->words,sizeof(char*)*ptr21->new_size);
            if (ptr23->words==NULL){
                fprintf(stderr,"Out of memory.\n");
                exit(3);
            }
            ptr17->lines_allocated = ptr21->new_size;
        }

        /* allocate space for the next line */
        ptr23->words[ptr19->i] = malloc(ptr18->max_line_len);
        if (ptr23->words[ptr19->i]==NULL){
            fprintf(stderr,"Out of memory (3).\n");
            exit(4);
        }
        if (fgets(ptr23->words[ptr19->i],ptr18->max_line_len-1,fp)==NULL)
            break;

        /* get rid of CR or LF at end of line */
        for (ptr20->j=strlen(ptr23->words[ptr19->i])-1;ptr20->j>=0 && (ptr23->words[ptr19->i][ptr20->j]=='\n' || ptr23->words[ptr19->i][ptr20->j]=='\r');ptr20->j--)
            ;
        ptr23->words[ptr19->i][ptr20->j+1]='\0';
    }
    fclose(fp);

    pthread_t threads[ptr19->i];
    int *ptr[ptr19->i];

    /* create all threads one by one */
    arg_struct *args = malloc(sizeof(arg_struct)*ptr19->i);
    for (ptr24->index = 0; ptr24->index < ptr19->i; ++ptr24->index) { 
        args[ptr24->index].arg_1 = malloc((sizeof(ptr23->words) + sizeof(comm))*ptr19->i);    
        strcpy(args[ptr24->index].arg_1, ptr23->words[ptr24->index]);
	strcat(args[ptr24->index].arg_1, comm);
        ptr22->result_code = pthread_create(&threads[ptr24->index], NULL, perform_work, &args[ptr24->index]);
        assert(0 == ptr22->result_code);
    }

    /* wait for each thread to complete */
    for (ptr24->index = 0; ptr24->index < ptr19->i; ++ptr24->index) {
      // block until thread 'index' completes
      ptr22->result_code = pthread_join(threads[ptr24->index], (void**)&(ptr[ptr24->index]));
      assert(0 == ptr22->result_code);
    }

    /* free memory */
    for (;ptr19->i>=0;ptr19->i--){
        free(ptr23->words[ptr19->i]);
    }
    free(ptr23->words);

    return 0;
}

int main (int argc, char **argv) {

  union options opt1, *ptr1 = &opt1;
  union options opt2, *ptr2 = &opt2;
  union options opt3, *ptr3 = &opt3;
  union options opt4, *ptr4 = &opt4;
  union options opt5, *ptr5 = &opt5;
  union options opt6, *ptr6 = &opt6;
  union options opt7, *ptr7 = &opt7;
  union options opt9, *ptr9 = &opt9;
  union options opt10, *ptr10 = &opt10;
  union options opt11, *ptr11 = &opt11;
  union options opt12, *ptr12 = &opt12;
  union options opt13, *ptr13 = &opt13;
  union options opt14, *ptr14 = &opt14;
  union options opt15, *ptr15 = &opt15;
  union options opt16, *ptr16 = &opt16;
  union options opt26, *ptr26 = &opt26;
  union options opt27, *ptr27 = &opt27;

  opt1.input = NULL;
  opt2.host = NULL;
  opt3.user = NULL;
  opt4.port = NULL;
  opt5.pass = NULL;
  opt6.dcrypt = NULL;
  opt7.exec = NULL;
  opt9.ccrypt = NULL;
  opt10.fflag = 0;
  opt11.lflag = 0;
  opt12.uflag = 0;
  opt13.wflag = 0;
  opt14.pflag = 0;
  opt15.eflag = 0;
  opt16.dflag = 0;
  opt26.opt = 0;
  opt27.x = 0;

while(optind < argc) {
  if((ptr26->opt = getopt(argc, argv, "f:l:u:p:w:e:d:h")) != -1){
   switch(ptr26->opt){
     case 'f':
       ptr10->fflag = 1;
       ptr1->input=(char *)malloc(strlen(optarg)+1);
       strcpy(ptr1->input, optarg);
       break;
     case 'l':
       ptr11->lflag = 1;
       ptr2->host=(char *)malloc(strlen(optarg)+1);
       strcpy(ptr2->host, optarg);
       break;
     case 'u':
       ptr12->uflag = 1;
       ptr3->user=(char *)malloc(strlen(optarg)+1);
       strcpy(ptr3->user, optarg);
       break;
     case 'p':
       ptr14->pflag = 1;
       ptr4->port=(char *)malloc(strlen(optarg)+1);
       strcpy(ptr4->port, optarg);
       break;
     case 'w':
       ptr13->wflag = 1;
       ptr5->pass=(char *)malloc(strlen(optarg)+1);
       strcpy(ptr5->pass, optarg);
       break;
     case 'd':
       ptr16->dflag = 1;
       ptr6->dcrypt=(char *)malloc(strlen(optarg)+1);
       strcpy(ptr6->dcrypt, optarg);
       break;
     case 'e':
       ptr15->eflag = 1;
       ptr7->exec=(char *)malloc(strlen(optarg)+1);
       strcpy(ptr7->exec, " ");
       strcat(ptr7->exec, optarg);
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

   if(ptr16->dflag == 1){
	ptr9->ccrypt=(char *)malloc(strlen(ptr6->dcrypt) + 1);   
	strcpy(ptr9->ccrypt, "ccrypt -d ");
	strcat(ptr9->ccrypt, ptr6->dcrypt);
	system(ptr9->ccrypt);
   }

   if(ptr10->fflag == 1 && ptr15->eflag == 1){
	file_open(ptr1->input, ptr7->exec);
   }
   else if(ptr10->fflag == 0 && ptr11->lflag == 1 && ptr12->uflag == 1 && ptr13->wflag == 1 && ptr15->eflag == 1){
	single("abcd", ptr3->user, ptr2->host, ptr5->pass, ptr7->exec);
   }
   else if(ptr10->fflag == 0 && ptr11->lflag == 1 && ptr12->uflag == 1 && ptr13->wflag == 1 && ptr15->eflag == 1 && ptr14->pflag == 1){
	single("abcde", ptr3->user, ptr2->host, ptr5->pass, ptr7->exec, ptr4->port);
   }
   else{
	usage();
   }   

   if(ptr16->dflag == 1){ 
	printf("*File was decrypted for usage. Now re-encrypt it.*\n");
	ptr6->dcrypt[strlen(ptr6->dcrypt)-4] = 0;
	strcpy(ptr9->ccrypt, "ccrypt ");
        strcat(ptr9->ccrypt, ptr6->dcrypt);	
	system(ptr9->ccrypt);	
   }

   char *freeit[7] = {ptr1->input, ptr2->host, ptr3->user, ptr4->port, ptr5->pass, ptr6->dcrypt, ptr9->ccrypt};

   for(; ptr27->x < 7; ptr27->x++){
	   if(freeit[ptr27->x] != NULL){free(freeit[ptr27->x]);}
   }
  
  return 0;
}
