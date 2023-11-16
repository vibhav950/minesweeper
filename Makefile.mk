.PHONY: all

CC = gcc

SRCS = rand.c\
	   minesweeper-main.c

OBJS = $(SRCS:.c=.o)

EXE = minesweeper

all: $(EXE) clean

$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $(EXE)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f $(OBJS)
