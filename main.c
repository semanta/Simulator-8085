// main.c
#include "headers.h"

// main function
int main()
{

  int ch;
  FILE *fp;
  // structure to hold lines
  FileToLines progFile;
  progFile.lineCount = 0;
  // a 8085 simulator
  Simu newSimulator;
  // open file in read mode and display error message on failure
  fp = fopen("program","r");
  if(fp == NULL)
    {
      fprintf(stderr,"Error:: unable to open file\n");
      exit(-1);
    }
  // read each line from file to the FileToLines container
  while(!feof(fp))
    {
      fgets(progFile.line[progFile.lineCount], MAX_LINE_LENGTH,fp);
      int length = strlen(progFile.line[progFile.lineCount]);
      if(length)
	{
	  int j;
	  for(j = 0; j  < length ; ++j)
	    {
	      progFile.line[progFile.lineCount][j] = toupper(progFile.line[progFile.lineCount][j]);

	    }
	  
	  if(progFile.line[progFile.lineCount][length-1] == '\n')
	    progFile.line[progFile.lineCount][length-1] == '\0';
	}
      
      progFile.lineCount++;
    }
  // check if the program file is empty
  if(progFile.lineCount == 0)
    {
      printf("EMPTY PROGRAM!!!\n");
      return 0;
    }
  // load the appropriate instruction code to memory
  // instruction codes are stored in int
  // arguments: reference of simulator, start address, reference of FileToLines structure
  loadprogram(&newSimulator, 5000, &progFile);
  
  // display the error messages for debugging
  if (newSimulator.errorList.count != 0) {
        printf("Error(s) in program:\n");
	int i;
        for (i = 0; i < newSimulator.errorList.count; ++i) {
            printf("%s\n", newSimulator.errorList.err_list[i]);
        }
        return 0;
    }
  
  // execute the program 
  execute(&newSimulator);
  // show the statistics 
  stat(&newSimulator);
  return 0;
}

