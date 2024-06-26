# Paths
SRCDIR := src
EXDIR := $(SRCDIR)/examples
OBJDIR := build
SRCHTREE_OBJ := $(OBJDIR)/srchtree
EXPRTREE_OBJ := $(OBJDIR)/exprtree
RPNCALC_OBJ := $(OBJDIR)/rpncalc
SPATH_OBJ := $(OBJDIR)/spath
SRCHTREE_SRC := $(EXDIR)/srchtree.c $(SRCDIR)/bitree.c $(SRCDIR)/bistree.c
EXPRTREE_SRC := $(EXDIR)/exprtree.c $(SRCDIR)/traverse.c $(SRCDIR)/list.c $(SRCDIR)/bitree.c
RPNCALC_SRC := $(EXDIR)/rpncalc.c $(SRCDIR)/stack.c $(SRCDIR)/queue.c $(SRCDIR)/list.c
SPATH_SRC := $(EXDIR)/spath.c $(SRCDIR)/bfs.c $(SRCDIR)/graph.c $(SRCDIR)/queue.c $(SRCDIR)/set.c $(SRCDIR)/list.c

# Flags
CFLAGS = -Wall -Wextra -Iinclude -g -lm -o $@

# Commands
CC= gcc
RM := rm -rf
MKDIR := mkdir -p

# Targets
.PHONY: all clean exprtree srchtree rpncalc spath

all: exprtree srchtree rpncalc spath

exprtree: $(EXPRTREE_OBJ)

srchtree: $(SRCHTREE_OBJ)

rpncalc: $(RPNCALC_OBJ)

spath: $(SPATH_OBJ)

$(SRCHTREE_OBJ): $(SRCHTREE_SRC)
	$(CC) $(CFLAGS) $(SRCHTREE_SRC)

$(EXPRTREE_OBJ): $(EXPRTREE_SRC)
	$(CC) $(CFLAGS) $(EXPRTREE_SRC)

$(RPNCALC_OBJ): $(RPNCALC_SRC)
	$(CC) $(CFLAGS) $(RPNCALC_SRC)

$(SPATH_OBJ): $(SPATH_SRC)
	$(CC) $(CFLAGS) $(SPATH_SRC)

$(SRCHTREE_SRC): | $(OBJDIR)
$(EXPRTREE_OBJ): | $(OBJDIR)
$(RPNCALC_OBJ): | $(OBJDIR)
$(SPATH_OBJ): | $(OBJDIR)

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

clean:
	$(RM) $(OBJDIR)
