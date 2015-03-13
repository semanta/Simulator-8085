#ifndef SIMU_H
#define SIMU_H
#include "parser.h"
#include "opcodecheck.h"
#include "helper.h"
// structure for 8085 simulator
typedef struct {
  // (sizeof(int) machine dependent 16 or 32 bits )
  // total of 10^16 (int) addresses memory
  int MEMORY[1<<16];
  // registers 8
  int REGISTERS[8];
  int SP,PC,START_ADDRESS;
  int IOPORTS[1<<8];
  // structur to hold the lines from file while parsing
  FileToLines progFile;
  // structure to hold the error messages
  ErrorList errorList;
  // structure to hold labels for branching instructions
  LabelList labelList;
  // structure to hold instruction code
  ByteList byteList;
  // to check if the program is halted
  int HALT;
} Simu;

// load the appropriate instruction code to memory
// instruction codes are stored in int
// arguments: reference of simulator, start address, reference of FileToLines structure
void loadprogram(Simu *, int start_addr, FileToLines * progFile);
void execute(Simu *);
#endif
