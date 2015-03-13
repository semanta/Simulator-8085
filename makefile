OBJS  = main.o parser.o simu.o opcodecheck.o helper.o
CC  = gcc
main: $(OBJS)
	$(CC) -o main $(OBJS) -g

main.o: main.c headers.h
	$(CC) main.c -c -g

parser.o: parser.c parser.h
	$(CC) parser.c -c -g

opcodecheck.o: opcodecheck.c opcodecheck.h
	$(CC) opcodecheck.c -c -g

helper.o: helper.c helper.h
	$(CC) helper.c -c -g

simu.o: simu.c simu.h 
	$(CC) simu.c -c -g
