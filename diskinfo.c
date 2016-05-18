
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>



int blockSize;
int blockCount;
int fatStart;
int fatBlock;
int rootStart;
int rootBlock;

void CutBlockInfo(char* ptr){
	blockSize = ntohs( *((short*)(ptr+8)) );
	blockCount = ntohl(*((long*)(ptr+10)) );
	fatStart = ntohl(*((long*)(ptr+14)) );
	fatBlock = ntohl(*((long*)(ptr+18)) );
	rootStart = ntohl(*((long*)(ptr+22)) );
	rootBlock = ntohl(*((long*)(ptr+26)) );
		
}

int main(int argc, char *argv[]){
	char buffer[512];
	char* file_name;
  	 FILE *fp;
 	file_name=argv[1];
 
   	fp = fopen(file_name,"r"); // read mode
 
  	 if( fp == NULL )
  	 {
      		perror("Error while opening the file.\n");
      		exit(EXIT_FAILURE);
   	}
 
  	 printf("The contents of %s file are :\n", file_name);
 
  	
     	fread(buffer, 512, 1, fp);
   	

 
   	fclose(fp);
	CutBlockInfo(buffer);
	printf("Super block information:\n");
	printf("Block size: %d\n", blockSize);
	printf("Block count: %d\n", blockCount);
	printf("FAT starts: %d\n", fatStart);
	printf("FAT blocks: %d\n", fatBlock);
	printf("Root directory start: %d\n", rootStart);
	printf("Root directory blocks: %d\n", rootBlock);
	
	
	
		fp = fopen(file_name,"r"); // read mode
 
  	 if( fp == NULL )
  	 {
      		perror("Error while opening the file.\n");
      		exit(EXIT_FAILURE);
   	}
 
  	 
	char buffers[fatStart * blockSize+blockSize*fatBlock];  
  	
		
		
		fseek(fp,blockSize*fatStart,SEEK_SET);
		fread(buffers,blockSize*fatBlock,1,fp);
	int free = 0, reserved = 0, allocated = 0;
	int i;
	for(i=0; i<=blockSize*fatBlock-3; i+=4){
		int entry = (buffers[i]<<24) + (buffers[i+1]<<16) + (buffers[i+2]<<8) + (buffers[i+3]);
		if(entry == 0){
			free++;
		}else if(entry == 1){
			reserved++;
		}else{
			allocated++;
		}
		
	}
	printf("\nFAT information:\n");

	printf("Free Blocks: %d\n", free);
	printf("Reserved Blocks: %d\n", reserved);
	printf("Allocated Blocks: %d\n", allocated);
 
   	fclose(fp);

	
	
	return 0;


}














