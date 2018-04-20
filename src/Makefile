CC = gcc
CFLAGS = -g -Wall

.SUFFIXES: .c .o

ifndef MAINC
MAINC = main.c
endif

OBJDIR = obj

SRC = register.c generic_list.c generic_dict.c file.c hashtable.c help.c history.c memory.c parser.c interpreter.c symbol.c reserved.c assemble.c asm_helper.c $(MAINC)
#OBJ = $(SRC:.c=.o)
OBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(SRC))

ifndef TARGET
TARGET = a.out
endif

$(TARGET) : $(OBJ)
	$(CC) -o $@ $(OBJ)

$(OBJDIR)/%.o : %.c
	@mkdir -p $@D
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	@rm -rf $(OBJDIR)
	@rm a.out
