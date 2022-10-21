CC=gcc
EXE=shell loop loopever

BINDIR := ./bin
OBJDIR := ./obj
SRCDIR := ./src
MYSHELL_SRC := $(wildcard $(SRCDIR)/*.c)
MYSHELL_OBJ := $(MYSHELL_SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

MYSHELL := $(BINDIR)/myshell

all: $(EXE)

shell: $(MYSHELL)

$(MYSHELL) : $(MYSHELL_OBJ) | $(BINDIR)
	$(CC) -o $@ $^ $(CFLAGS)
	chmod +x $@

$(BINDIR) :
	mkdir $@

$(OBJDIR) :
	mkdir $@

$(MYSHELL_OBJ) : $(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) -c -o $@ $^ $(CFLAGS)

loop: loop.c
	$(CC) loop.c -o $@

loopever: loopever.c
	$(CC) loopever.c -o $@

clean:
	rm $(EXE)