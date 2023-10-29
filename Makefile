# Paths
SRCDIR := src
EXDIR := $(SRCDIR)/examples
OBJDIR := build
SRCHTREE_OBJ := $(OBJDIR)/srchtree
EXPRTREE_OBJ := $(OBJDIR)/exprtree
EXPRTREE_SRC := $(EXDIR)/exprtree.c $(SRCDIR)/traverse.c $(SRCDIR)/list.c $(SRCDIR)/bitree.c
SRCHTREE_SRC := $(EXDIR)/srchtree.c $(SRCDIR)/bitree.c $(SRCDIR)/bistree.c

# Flags
CFLAGS = -Wall -Iinclude -g -o $@

# Commands
CC= gcc
RM := rm -rf
MKDIR := mkdir -p

# Targets
.PHONY: all clean exprtree srchtree

all: exprtree srchtree

exprtree: $(EXPRTREE_OBJ)

srchtree: $(SRCHTREE_OBJ)

$(SRCHTREE_OBJ): $(SRCHTREE_SRC)
	$(CC) $(CFLAGS) $(SRCHTREE_SRC)

$(EXPRTREE_OBJ): $(EXPRTREE_SRC)
	$(CC) $(CFLAGS) $(EXPRTREE_SRC)

$(SRCHTREE_SRC): | $(OBJDIR)
$(EXPRTREE_OBJ): | $(OBJDIR)

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

clean:
	$(RM) $(OBJDIR)
