.PHONY: check clean

test_suite: test.c ini_parser.c
	gcc -W -Wall -std=gnu11 -o $@ $^

check: test_suite
	./test_suite

clean:
	$(RM) test_suite
