CC := gcc
CFLAGS := -O2 -Wall -Wextra
LIBS := $(shell pkg-config --cflags --libs ncurses) -lm -lmenu

compile:
	$(CC) $(CFLAGS) ./src/*.c -o minea $(LIBS)

clean:
	rm -f minea
