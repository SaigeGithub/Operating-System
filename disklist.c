#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>



int blockSize;
int blockCount;
int rootStart;
int rootBlock;
int fileSize;
char* fileName;

typedef struct time{
	unsigned int seconds;
	unsigned int minutes;
	unsigned int hour;
	unsigned int day;
	unsigned int month;
	int year;
	
}time;

void CutBlockInfo(char* ptr){
	blockSize = ntohs( *((short*)(ptr+8)) );
	blockCount = ntohl(*((long*)(ptr+10)) );
	rootStart = ntohl(*((long*)(ptr+22)) );
	rootBlock = ntohl(*((long*)(ptr+26)) );
		
}

int main(int argc, char *argv[]){
	char buffer[512];
	char* fileName;
  	 FILE *fp;
 	fileName=argv[1];
	time this_time;
 
   	fp = fopen(fileName,"r"); // read mode
 
  	 if( fp == NULL )
  	 {
      		perror("Error while opening the file.\n");
      		exit(EXIT_FAILURE);
   	}
 
  	 printf("The contents of %s file are :\n", fileName);
 
  	
     	fread(buffer, 512, 1, fp);
   	

 
   	fclose(fp);
	CutBlockInfo(buffer);

		fp = fopen(fileName,"r"); // read mode
 
  	 if( fp == NULL )
  	 {
      		perror("Error while opening the file.\n");
      		exit(EXIT_FAILURE);
   	}
 
  	 
	char buffers[blockSize*rootBlock];  
  	
		
		
		fseek(fp,blockSize*rootStart,SEEK_SET);
		fread(buffers,blockSize*rootBlock,1,fp);
	int j=0;
	while (j<blockSize*rootBlock){
    	    	while(buffers[j] == 0 && j<blockSize*rootBlock) {
    	    	    j += 64;
    	    	}
    	    	if(buffers[j] == 3 || buffers[j] == 2) {
    	    	    	printf("%c ", 'F');
    	    	} else if (buffers[j] == 5 || buffers[j] == 4) {
    	    	    	printf("%c ", 'D');
    	    	} else {
    	    	    break;
    	    	}
              
                fileSize = ntohl(*((long*)(buffers+9+j)) );
              
    	    	fileName = buffers+27+j;
    	    
    	    	this_time.year = (buffers[20]<<8)+(buffers[21])+256;
    	    	
    	    	this_time.month = buffers[j+22];
    	    	this_time.day = buffers[j+23];
    	    	this_time.hour = buffers[j+24];
    	    	this_time.minutes = buffers[j+25];
    	    	this_time.seconds = buffers[j+26];
    	    	printf("%10u %30s %u/%02u/%02u %02u:%02u:%02u\n", fileSize, fileName, this_time.year, this_time.month, this_time.day, this_time.hour, this_time.minutes, this_time.seconds);
    	    	j += 64;
    	}
 
   	fclose(fp);

	
	
	return 0;


}