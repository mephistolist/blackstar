/* gcc -O0 -std=c18 blackstar.c -o blackstar -lpthread -s // Find why other optimization doesn't work. */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <pthread.h>

/* perform_work function args struct */
typedef struct {
    char arg_1[200];
} arg_struct;

/* counter for threads */
int count = 0;

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
   printf("%s\n", actual_args->arg_1);
   //system(actual_args->arg_1);
   ++count;

   return NULL;
}

int file_open(char *name, char *comm){
    int lines_allocated = 128;
    int max_line_len = 200;

    /* allocate lines of text */
    char **words = (char **)malloc(sizeof(char*)*lines_allocated);
    if (words==NULL) {
        fprintf(stderr,"Out of memory (1).\n");
        exit(1);
    }

    FILE *fp = fopen(name, "r");
    if (fp == NULL) {
        fprintf(stderr,"Error opening default file.\n");
	usage();
        exit(2);
    }

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
    //int thread_args[i];
    int result_code, index;
    int *ptr[i];

    /* create all threads one by one */
    arg_struct *args = malloc(sizeof(arg_struct)*i);
    for (index = 0; index < i; ++index) {
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
  int opt, fflag, lflag, uflag, wflag, pflag, eflag, oflag, dflag, sp = 0;
  char *fname  = "./test";
  char *passst = "sshpass -p '";
  char *passcl = "' ";
  char *empty;
  char *skip = "-o StrictHostKeyChecking=no ";
  char *exec, *user, *host, *pass, *sshpass, *port, *input, *dcrypt, *ccrypt;
  
  int base = 180;

while(optind < argc) {
  if(( opt = getopt(argc, argv, "f:l:u:p:w:e:d:ho")) != -1){
   switch(opt){
     case 'f':
       fflag = 1;
       input=(char *)malloc(base);
       strcpy(input, optarg);
       break;
     case 'l':
       lflag = 1;
       host=(char *)malloc(base);
       strcpy(host, optarg);
       break;
     case 'u':
       uflag = 1;
       user=(char *)malloc(base);
       strcpy(user, optarg);
       break;
     case 'p':
       pflag = 1;
       port=(char *)malloc(base);
       strcpy(port, optarg);
       break;
     case 'w':
       wflag = 1;
       pass=(char *)malloc(base);
       strcpy(pass, optarg);
       break;
     case 'd':
       dflag = 1;
       dcrypt=(char *)malloc(base);
       strcpy(dcrypt, optarg);
       break;
     case 'e':
       eflag = 1;
       exec=(char *)malloc(base);
       strcpy(exec, " ");
       strcat(exec, optarg);
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
      file_open(fname, exec);
      optind++;
   }
}

   if(dflag == 1){
	ccrypt=(char *)malloc(base);   
	strcpy(ccrypt, "ccrypt -d ");
	strcat(ccrypt, dcrypt);
	system(ccrypt);
   }

   if(fflag == 1 && eflag == 1){
	file_open(input, exec);
   }
   else if(fflag == 0 && lflag == 1 && uflag == 1 && eflag == 1){
	sp = 1;
	sshpass=(char *)malloc(base);
	if(wflag == 1){
		strcpy(sshpass, passst);
		strcat(sshpass, pass);
		strcat(sshpass, passcl);
	}
	strcat(sshpass, "ssh ");
	if(oflag == 1){
		strcat(sshpass, skip);
	}
	strcat(sshpass, "-T ");
	strcat(sshpass, user);
	strcat(sshpass, "@");
	strcat(sshpass, host);
	strcat(sshpass, "'");
	strcat(sshpass, exec);
	strcat(sshpass, "'");
	if(pflag == 1){
		strcat(sshpass, " -p ");
		strcat(sshpass, port);
	}
	system(sshpass);
   }
   else{
	usage();
   }	   

   if(dflag == 1){
	printf("*File was decrypted for usage. Now re-encrypt it.*\n");
	dcrypt[strlen(dcrypt)-4] = 0;
	strcpy(ccrypt, "ccrypt ");
        strcat(ccrypt, dcrypt);	
	system(ccrypt);	
   }

   if(sp == 1){free(sshpass);}
   if(lflag == 1){free(host);}
   if(uflag == 1){free(user);}
   if(eflag == 1){free(exec);}
   if(wflag == 1){free(pass);}
   if(pflag == 1){free(port);}
   if(dflag == 1){free(ccrypt); free(dcrypt);}

  return 0;
}
