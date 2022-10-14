CC=gcc
EXE=shell loop

all: $(EXE)

shell: shell.c
	$(CC) shell.c -o $@

loop: loop.c
	$(CC) loop.c -o $@

clean:
	rm $(EXE)