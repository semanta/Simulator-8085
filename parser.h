#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include "container.h"
#include "helper.h"

void parseFile(FileToLines * progFile, ByteList * byte_list, LabelList * labels, ErrorList * err_list, int start_addr);

#endif
