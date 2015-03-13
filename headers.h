#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>// exit()
#include <ctype.h> // tolower(), isalpha()


#include "container.h"
#include "parser.h"
#include "simu.h"

/* int getsource(int v) { */
/*     return v & 0x07; */
/* } */
/* int getdest(int v) { */
/*     return ((v >> 3) & 0x07); */
/* } */
/* int ISMEM(int v) { */
/*     return v == 0x06; */
/* } */

/* void uppercase(char * s) { */
/*     int n = strlen(s); */
/*     int i; */
/*     for (i = 0; i < n; ++i) */
/*         s[i] = toupper(s[i]); */
/* } */

/* int GETREGCHAR(char ch) { */
/*     if (ch >= 'A' && ch <= 'E') return ch - 'A'; */
/*     if (ch == 'H') return 5; */
/*     if (ch == 'L') return 6; */
/*     return -1; */
/* } */
/* int regidx(int idx) { */
/*     if (idx == 0) return GETREGCHAR('B'); */
/*     if (idx == 1) return GETREGCHAR('C'); */
/*     if (idx == 2) return GETREGCHAR('D'); */
/*     if (idx == 3) return GETREGCHAR('E'); */
/*     if (idx == 4) return GETREGCHAR('H'); */
/*     if (idx == 5) return GETREGCHAR('L'); */
/*     if (idx == 7) return GETREGCHAR('A'); */
/*     return -1; ///error */
/* } */

/* int GETADDRESS(int MB, int LB) { */
/*     return ((MB & 0xFF) << 8) + (LB & 0xFF); */
/* } */
/* /// */
/* int GETRP(int v) { */
/*     return (v >> 4) & 0x03; */
/* } */
/* ///8-bit twos complement */
/* int b8twos(int n) { */
/*     return (((~n) & 0xFF) + 1); */
/* } */
/* int b4twos(int n) { */
/*     return (((~n) & 0xF) + 1); */
/* } */
/* int popcount(int n) { */
/*     n &= 0xFF; */
/*     int c = 0; */
/*     while (n) { */
/*         c++; */
/*         n -= n & -n; */
/*     } */
/*     return c; */
/* } */
/* int GETHEXVAL(char ch) { */
/*     assert((ch >= 'A' && ch <= 'F') || (ch >= '0' && ch <= '9')); */
/*     return ch >= 'A' ? 10 + ch - 'A' : ch - '0'; */
/* } */

/* int hextodecimal(const char s[], int len) { */
/*     int v = 0; */
/*     int i = 0; */
/*     while (i < len && s[i] == '0') i++; */
/*     for (; i < len; ++i) { */
/*         v <<= 4; */
/*         v += GETHEXVAL(s[i]); */
/*     } */
/*     return v; */
/* } */

/* void addToList(ByteList * bytelist, int opcode, int line) { */
/*     bytelist->bytes[bytelist->count][0] = opcode; */
/*     bytelist->bytes[bytelist->count][1] = line; */
/*     bytelist->count++; */
/* } */

/* int findInLabelList(LabelList * labellist, const char * s, int n) { */
/*     char tmp[255] = {0}; */
/*     strncat(tmp, s, n); */
/*     int i; */
/*     for ( i = 0; i < labellist->count; ++i) { */
/*         if (strcmp(labellist->label[i].name, tmp) == 0) return i; */
/*     } */
/*     return -1; */
/* } */

/* void addLabelToLabelList(LabelList * labellist, const char * s, int n, int addr) { */
/*     labellist->label[labellist->count].address = addr; */
/*     labellist->label[labellist->count].name[0] = 0; */
/*     strncat(labellist->label[labellist->count].name, s, n); */
/*     labellist->count++; */
/* } */

#endif
