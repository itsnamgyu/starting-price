CC = gcc
CFLAGS = -g -Wall

.SUFFIXES: .c .o

ifndef MAINC
MAINC = main.c
endif

SRCS = generic_list.c generic_dict.c file.c hashtable.c help.c history.c memory.c parser.c interpreter.c symbol.c reserved.c assemble.c asm_helper.c $(MAINC)

OBJS = $(SRCS:.c=.o)

ifndef TARGET
TARGET = a.out
endif

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean :
	rm $(OBJS) $(TARGET)
