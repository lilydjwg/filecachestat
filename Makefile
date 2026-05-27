CC=gcc

.PHONY: all clean

filecachestat: filecachestat.c
	$(CC) -O2 -o filecachestat filecachestat.c -lm

clean:
	-rm filecachestat
