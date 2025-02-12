CC      = gcc
CFLAGS  = -Wall -Wextra -g
TARGET  = minishell
OBJS    = main.o parser.o builtins.o executor.o signals.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c parser.h builtins.h executor.h signals.h
	$(CC) $(CFLAGS) -c main.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

builtins.o: builtins.c builtins.h
	$(CC) $(CFLAGS) -c builtins.c

executor.o: executor.c executor.h
	$(CC) $(CFLAGS) -c executor.c

signals.o: signals.c signals.h
	$(CC) $(CFLAGS) -c signals.c

clean:
	rm -f $(OBJS) $(TARGET)
