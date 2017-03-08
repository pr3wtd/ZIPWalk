CC=gcc
CFLAGS= -Wall -Wextra

zip: zip.c
	$(CC) $(CFLAGS) -o zip zip.c view.c parse.c

clean:
	rm -f zip

