/*
 * train.c
 */
 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "train.h"
 
/* A global to assign IDs to our trains */ 
int idNumber = 0;

/* If this value is set to 1, trains lengths
 * etc will be generated randomly.
 * 
 * If it is set to 0, the lengths etc will be
 * input from a file.
 */
int doRandom = 0;

/* The file to input train data from */
FILE *inputFile;

/* You can assume that no more than 80 characters
 * will be on any line in the input file
 */
#define MAXLINE		80

/*void copy_file(char *old_filename)
{
	char ch;
	
	fp1 = fopen(old_filename, "r");
	

	while (1) {
	ch = fgetc(fp1);

	if (ch == EOF)
	 break;
	else
	 putc(ch, inputFile);
	printf("%s",ch);
	}

	printf("File copied Successfully!");
	fclose(fp1);
	fclose(inputFile);
	
}*/
void	initTrain ( char *filename )
{
	doRandom = 0;
	
	/* If no filename is specified, generate randomly */
	if ( !filename )
	{
		doRandom = 1;
		srandom(getpid());
	}
	else
	{	
	
		inputFile=fopen(filename, "r");
	}
}
 
/*
 * Allocate a new train structure with a new trainId, trainIds are
 * assigned consecutively, starting at 0
 *
 * Either randomly create the train structures or read them from a file
 *
 * This function malloc's space for the TrainInfo structure.  
 * The caller is responsible for freeing it.
 */
TrainInfo *createTrain ( void )
{
	TrainInfo *info = (TrainInfo *)malloc(sizeof(TrainInfo));

	/* I'm assigning the random values here in case
	 * there is a problem with the input file.  Then
	 * at least we know all the fields are initialized.
	 */	 
	info->trainId = idNumber++;
	info->arrival = 0;
	info->direction = (random() % 2 + 1);
	info->length = (random() % MAX_LENGTH) + MIN_LENGTH;

	if (!doRandom)
	{
		char str[MAXLINE];
		fgets (str, MAXLINE, inputFile);
		/*printf("%s",str);
		printf("%i",str[0]);
		printf("%i",str[1]);*/
		
		if (str[0]==87||str[0]==119)
			info->direction=DIRECTION_WEST;
		if (str[0]==69||str[0]==101)
			info->direction=DIRECTION_EAST;
			
		info->length = atoi(str+1);
		
	}
	return info;
}


