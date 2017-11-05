all: work.c killer.c
	gcc -o work work.c
	gcc -o killer killer.c