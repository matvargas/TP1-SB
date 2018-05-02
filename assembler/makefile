CFLAGS += -Wall -Wextra -std=c++11 -O0
CC      = g++

# source files:
C_SRC = $(wildcard *.cc)

# object files:
OBJ = $(C_SRC:.cc=.o)

# Executable file:
EXEC = mont

# Build:
all: release

release:	$(C_SRC)
	@$(CC) $(CFLAGS) *.cc -o $(EXEC)

# Clean:
clean:
	@rm -f $(EXEC)
