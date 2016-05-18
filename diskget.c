#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>



int blockSize;
int blockCount;
int rootStart;
int rootBlock;
int fatStart;
int fatBlock;
int fileStart;
int fileBlock;
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
	fatStart = ntohl(*((long*)(ptr+14)) );
	fatBlock = ntohl(*((long*)(ptr+18)) );
	rootStart = ntohl(*((long*)(ptr+22)) );
	rootBlock = ntohl(*((long*)(ptr+26)) );
		
}

int main(int argc, char *argv[]){
	if(argc != 3){
		printf("The correct input is ./diskget <image file name> <copy file name>\n");
		exit(0);
	}
	char buffer[512];
	char* fileName;
  	 FILE *fp;
 	fileName=argv[1];
	time this_time;
	
	//to read the file in linux
	FILE *writeData;
	writeData =fopen(argv[2],"w");
 
   	fp = fopen(fileName,"r"); // read mode
 
  	 if( fp == NULL )
  	 {
      		perror("Error while opening the file.\n");
      		exit(EXIT_FAILURE);
   	}
 

 
  	
     	fread(buffer, 512, 1, fp);
   	

 
   	fclose(fp);
	CutBlockInfo(buffer);
	
	char* filename_System=argv[2];
	char* tempFilename;
	//Check Root Directory
	fp = fopen(fileName,"r"); 
 
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
    	    	
              
        
              
    	    	tempFilename = buffers+27+j;
				if(strcmp(tempFilename, filename_System)==0){
					printf("Found the file \nCoping...............");
					fileStart = ntohl(*((long*)(buffers+1+j)) );
					fileBlock = ntohl(*((long*)(buffers+5+j)) );
				}
    	    	j += 64;
    	}
 
   	fclose(fp);
	
	
	//Go check FAT
	fp = fopen(fileName,"r");
 
  	 if( fp == NULL )
  	 {
      		perror("Error while opening the file.\n");
      		exit(EXIT_FAILURE);
   	}
 
  	 int nextBlock=fileStart;
	char bufferss[fatStart * blockSize+blockSize*fatBlock];  
  	
		
		
		fseek(fp,blockSize*fatStart,SEEK_SET);
		fread(bufferss,blockSize*fatBlock,1,fp);
	while(!(nextBlock == -1)){
		int i=nextBlock*4;
		
	
					//store data file
	FILE *data;
	data = fopen(argv[1],"r"); 
		char tempData[blockSize];
		//rewind(data);
		fseek(data,nextBlock*blockSize,SEEK_SET);
		fread(tempData,blockSize,1,data);
		fprintf(writeData,"%s",tempData);
		
		nextBlock = ntohl(*((long*)(bufferss+i)) );
		if(nextBlock == -1){
			printf("\nThe Scanner reach the End of File \nCheck to see if copy successful\n");
			break;
		}
		fclose(data);
	
	}

	
 
   	fclose(fp);
	
		fclose(writeData);
	
	
	return 0;


}