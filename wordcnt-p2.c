/*
   *Project part1
   *Bing Xu
   *UID: 3035027710
 */
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<ctype.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<errno.h>
#include<algorithm>
#include<stdbool.h>
#include<signal.h>
#define MAXLEN 116
//this is to adapt to OS X
#ifdef __MACH__
#include <mach/mach_time.h>
#define CLOCK_MONOTONIC 0
#define CLOCK_REALTIME 0
int clock_gettime(int clk_id,struct timespec *t){
  mach_timebase_info_data_t timebase;
  mach_timebase_info(&timebase);
  uint64_t time;
  time = mach_absolute_time();
  double nseconds = ((double)time *(double)timebase.numer)/(double)timebase.denom;
  double seconds = ((double)time *(double)timebase.numer)/((double)timebase.denom *1e9);
  t->tv_sec = seconds;
  t->tv_nsec = nseconds;
  return 0;
}
#endif


//align to 8-byte boundary
struct output{
  char keyword[MAXLEN];
  unsigned int count;
  pid_t cid;
};

char * target;//the filename of the keyword file

char * lower(char * input){
  unsigned int i;
  for(i=0;i<strlen(input);++i)
    input[i] = tolower(input[i]);
  return input;
  
}


unsigned int search(char * keyword){
  int count = 0;
  FILE * f;
  char * pos;
  char input[MAXLEN];
  char * word = strdup(keyword); //clone the keyword

  lower(word);
  f = fopen(target,"r");

  while(fscanf(f,"%s",input)!=EOF){
    lower(input);
    if(strcmp(input,word)==0){
    
      count++;
    }else{
      pos = strstr(input,word);//search for substring
      if (pos!=NULL){
      //keyword is a substring of this string
      //if the character before keyword in this input string is 
      //an alphabet,skip the checking 
        if((pos-input>0)&&isalpha(*(pos-1))) continue;
        //if the character after the keyword in this input string 
        //is an alphabet,skip the checking
        if((pos-input+strlen(word)<strlen(input))&& (isalpha(*(pos+strlen(word))))) continue;
        //well, we count this keyword as the characters before 
        //and after the keyword in this input string are non-alphabets
        count++;
      }
    
     }
     }
  fclose(f);
  free(word);
  return count;
}

//pre: Given the start and end time
//post: return the elapsed time in nanoseconds (stored in 64 bits)
inline unsigned long long getns(struct timespec start, struct timespec end) {
	return (unsigned long long)(end.tv_sec-start.tv_sec)*1000000000ULL + (end.tv_nsec-start.tv_nsec);
}

bool finished = false;
void sigint_handler(int signum){
  if(signum==SIGINT){
  finished  = true;
//  printf("A SIGINT is received\n");
  }else{
  printf("SIGINT something wrong here!!");
  }
}

bool has_arrived = false;
void sigusr1_handler(int signum){
  if(signum==SIGUSR1){
    has_arrived = true;
//  printf("A SIGUSR1 is received\n");
  }else{
    printf("SIGUSR1 something worng here!!");
  }
}


int main(int argc,char * argv[]){
  if(argc<3){
    printf("Usage: ./wordcnt0 [target plaintext file] [keyword file]\n");
    return 0;
  }

  int line, i;
  char word[MAXLEN];
  struct output * result;
  FILE * keyFile;
  struct timespec start,end;

  //set the target file name
  target = strdup(argv[1]);
  
  clock_gettime(CLOCK_REALTIME,&start);//record the starting time
  keyFile = fopen(argv[2],"r");
  fscanf(keyFile,"%d",&line); // read in the number of keywords

  result = (struct output *) malloc(line * sizeof(struct output));

  //create the pipe to do multiprocessing
  int task_pfd[2]; //pipe descriptor
  int data_pfd[2]; //pipe descriptor
  pipe(task_pfd);
  pipe(data_pfd);
  int word_count = 0;
  int numOfChild = std::min(line,10);

  pid_t pids[numOfChild]; //child processes with number of line
  signal(SIGINT,sigint_handler);
  signal(SIGUSR1,sigusr1_handler);

  for (i=0;i<numOfChild;i++){
      pids[i] = fork();
      
      if(pids[i]>0){//this is the parent process
 //      printf("Here is the parent process\n"); 
        fscanf(keyFile,"%s",word);
        write(task_pfd[1],word,sizeof(word));
        kill(pids[i],SIGUSR1);

        continue;

      }else if(pids[i]==0){
       printf("Child %d will start to work\n",getpid());

        while(true){
          while(!has_arrived && !finished){
           sleep(1);
          }
          if(finished){
//            printf("Child (%d) is going to exit\n",(int)getpid());
            exit(word_count);
          }

          has_arrived = false;
          read(task_pfd[0],word,sizeof(word));
          word_count++;
          result[0].cid = (int)getpid();
          result[0].count = search(word);
          strcpy(result[0].keyword,word);
          write(data_pfd[1],&result[0],sizeof(struct output));
        
        }
      }else{
        printf("fork: error no = %s\n",strerror(errno));
        exit(-1);
      }
  }  
  

  int remaining = line-numOfChild;
  for(i=0;i<line;i++){
    read(data_pfd[0],&result[i],sizeof(result[i]));
    if(remaining>0){
      fscanf(keyFile,"%s",word);
      write(task_pfd[1],word,sizeof(word));
      kill(result[i].cid,SIGUSR1);
      remaining--;
    }
  
  }


  fclose(keyFile);

  int status;
  for (i=0;i<numOfChild;i++){
    kill(pids[i],SIGINT);
    waitpid(pids[i],&status,0);
    printf("Child process (%d) exited and it searched %d words \n",pids[i],status);
  }
  clock_gettime(CLOCK_REALTIME,&end);//record ending time

  for(i=0;i<line;++i){
    printf("%s : %d\n",result[i].keyword,result[i].count);
  }
    printf("\nTotal elapsed time: %.2lf ms\n",getns(start,end)/1000000.0);
  free(target);
  free(result);
  return 0;
}
