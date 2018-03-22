CC = gcc
CFLAGS = -c -g

.SUFFIXES: .c .o

SRCS = dir.c hashtable.c help.c history.c memory.c main.c

OBJS = $(SRCS:.c=.o)

TARGET = test

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean :
	rm $(OBJS) $(TARGET)
	
# Made with help from juice500ml :)
