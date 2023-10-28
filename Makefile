# Paths
SRCDIR := src
EX_SRCDIR := $(SRCDIR)/examples
OBJDIR := build
EX_SRC := $(wildcard $(EX_SRCDIR)/*.c)
SRC := $(wildcard $(SRCDIR)/*.c) $(EX_SRC)
OBJ := $(patsubst $(EX_SRCDIR)/%.c, $(OBJDIR)/%, $(EX_SRC))

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
