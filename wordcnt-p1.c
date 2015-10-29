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

  //allocate space to store the results
  result = (struct output *) malloc(line * sizeof(struct output));

  //create the pipe to do multiprocessing
  int pfd[2]; //pipe descriptor
  pipe(pfd);
  pid_t pids[line]; //child processes with number of line
  for (i=0;i<line;i++){
      pids[i] = fork();
      fscanf(keyFile,"%s",word);
      if(pids[i]){//this is the parent process
        
        continue;

      }else if(pids[i]==0){
          result[0].count = search(word);
          strcpy(result[0].keyword,word);
//        close(pfd[0]);//close the read end
        if(write(pfd[1],&result[0],sizeof(result[0]))==-1){
          printf("Child:Encountered write error\n");
        }
          return 0;
      }else{
        printf("fork: error no = %s\n",strerror(errno));
        exit(-1);
      }
  
  }

  for (i=0;i<line;i++){
    int status;
    pid_t child_pid;
    child_pid = waitpid(pids[i],&status,0);
    read(pfd[0],&result[i],sizeof(result[i]));
  }
  fclose(keyFile);
  clock_gettime(CLOCK_REALTIME,&end);//record ending time

  for(i=0;i<line;++i){
    printf("%s : %d\n",result[i].keyword,result[i].count);
  }
    printf("\nTotal elapsed time: %.2lf ms\n",getns(start,end)/1000000.0);
  free(target);
  free(result);
  return 0;
}
