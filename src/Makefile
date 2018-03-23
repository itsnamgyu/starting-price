CC = gcc
CFLAGS = -c -g -std=c99 -Wall

.SUFFIXES: .c .o

SRCS = dir.c hashtable.c help.c history.c memory.c 20161662.c

OBJS = $(SRCS:.c=.o)

TARGET = a.out

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean :
	rm $(OBJS) $(TARGET)
	
# Made with help from juice500ml :)
