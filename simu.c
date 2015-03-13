#include "simu.h"

const char * reg_map = "ABCDEHL", * flags_map = "S Z * AC * P * CY";

void loadprogram(Simu * simu, int start_addr, FileToLines * progFile)
{
  int i;
  simu->PC = simu->START_ADDRESS = start_addr;
  // stack pointer points at FFFF location
  simu->SP =  0XFFFF;
  simu->progFile.lineCount = 0;
  simu->errorList.count = 0;
  simu->labelList.count = 0;
  simu->byteList.count = 0;
  // parse and store in memory
  parseFile(progFile, &simu->byteList, &simu->labelList, &simu->errorList,simu->START_ADDRESS);
  int ADDR = simu->PC;
  if(simu->errorList.count == 0)
    {
      for( i = 0; i < simu->byteList.count; ++i)
	{
	  simu->MEMORY[ADDR++] = simu->byteList.bytes[i][0];
	}
    }


}

void execute(Simu * simu)
{
  printf("here we execute\n");
  int i;
    for(i=simu->START_ADDRESS;i<simu->START_ADDRESS+20;++i)
    {
      int v = simu->MEMORY[i];// & 0xFF;
      // NOP instruction
      //if(v == 0) return ;
      printf("%d::%x\n",i,v);
      
    }
}

void stat(Simu * sim) {
    printf("sim->PC: %d, sim->SP: %d\n", sim->PC, sim->SP);
    int i;
    for (i = 0; i < 7; ++i) {
        printf("%c: %d ", reg_map[i], sim->REGISTERS[i] & 0xFF);
    }
    putchar('\n');
    // printf("S: %d Z: %d AC: %d P: %d C: %d\n", getsign(sim), getzero(sim), getAF(sim), getparity(sim), getcarry(sim));
}
