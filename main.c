#include "headers.h"

int main()
{
  int i = 0;
  int ch;
  FILE *fp;
  // structure to hold lines
  FileToLines progFile;
  progFile.lineCount = 0;
  // 8085 simulator
  Simu newSimulator;
  
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
	  if(progFile.line[progFile.lineCount][length-1] == '\n')
	    progFile.line[progFile.lineCount][length-1] == '\0';
	}
      progFile.lineCount++;
    }

  if(progFile.lineCount == 0)
    {
      printf("EMPTY PROGRAM!!!\n");
      return 0;
    }
  // load the appropriate instruction code to memory
  // instruction codes are stored in int
  // arguments: reference of simulator, start address, reference of FileToLines structure
  loadprogram(&newSimulator, 5000, &progFile);
  if (newSimulator.errorList.count != 0) {
        printf("Error(s) in program:\n");
	int i;
        for (i = 0; i < newSimulator.errorList.count; ++i) {
            printf("%s\n", newSimulator.errorList.err_list[i]);
        }
        return 0;
    }
  execute(&newSimulator);
  stat(&newSimulator);
  return 0;
}

