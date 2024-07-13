CC := gcc
CFLAGS := -O2 -Wall -Wextra
LIBS := -lm

compile:
	$(CC) $(CFLAGS) ./src/*.c -o minea $(LIBS)

clean:
	rm -f minea
