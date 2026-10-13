CC = gcc
CFLAGS = -Wall -std=c11

charcount: main.c charcount.c printcounts.c huff_tree.c
	$(CC) $(CFLAGS) -o charcount main.c charcount.c printcounts.c huff_tree.c

test: test_charcount.c charcount.c printcounts.c
	$(CC) $(CFLAGS) -o test_charcount test_charcount.c charcount.c printcounts.c
	./test_charcount

clean:
	rm -f charcount test_charcount

run: charcount
	./charcount easy.txt