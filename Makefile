# Paths
SRCDIR := src
OBJDIR := build
SRC := $(wildcard $(SRCDIR)/*.c )
OBJ := $(OBJDIR)/exptree

# Flags
CFLAGS = -Wall -Iinclude -o $@

# Commands
CC= gcc
RM := rm -rf
MKDIR := mkdir -p

# Targets
.PHONY: all clean

all: $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) $(SRC)

$(SRC): | $(OBJDIR)

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

clean:
	$(RM) $(OBJDIR)
