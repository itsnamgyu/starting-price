CC = gcc
CFLAGS = -g -Wall

.SUFFIXES: .c .o

ifndef MAINC
MAINC = main.c
endif

SRCS = file.c hashtable.c help.c history.c memory.c $(MAINC)

OBJS = $(SRCS:.c=.o)

ifndef TARGET
TARGET = a.out
endif

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean :
	rm $(OBJS) $(TARGET)
