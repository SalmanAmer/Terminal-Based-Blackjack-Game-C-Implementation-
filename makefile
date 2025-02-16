CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -g
TARGET = blackjack
SRC = blackjack.c

# Default target
all: $(TARGET)

# Compile the target
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS)

# Clean up generated files
clean:
	rm -f $(TARGET)

