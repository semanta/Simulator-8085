#ifndef CONTAINER_H
#define CONTAINER_H

#define MAX_PROGRAM_SIZE 1000 // in bytes
#define MAX_LABELS 50
#define MAX_LINES 400
#define MAX_LINE_LENGTH 255

// structure to hold instruction code
typedef struct {
  // first address to hold opcode in int
  // second address to hold the line number
  int bytes[MAX_PROGRAM_SIZE][2];
  int count;
} ByteList;

// structure to hold the array of error messages
typedef struct {
    char err_list[100][255];
    int count;
} ErrorList;

// structure for each label
typedef struct {
  // name of label
    char name[255];
  // effective address of label
    int address;
} Label;

// structure to hold array of labels 
typedef struct {
    Label label[MAX_LABELS];
    int count;
} LabelList;

// structure to hold lines from file/input for parsing 
typedef struct {
  char line[MAX_LINES][MAX_LINE_LENGTH];
  int lineCount;
} FileToLines;

#endif
