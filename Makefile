CC = gcc
EXECUTABLES = scheduler.out
SOURCES = scheduler.c
HEADERS =
TEST_INPUT_FIFO = example_fifo.in
TEST_INPUT_SJF = example_sjf.in

.PHONY: all clean test

all: $(EXECUTABLES)

$(EXECUTABLES): $(SOURCES)
	$(CC) -o $@ $^

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $<

clean:
	rm -f $(EXECUTABLES) *.o

test: $(EXECUTABLES)
	./$(EXECUTABLES) 0 FIFO $(TEST_INPUT_FIFO)
	./$(EXECUTABLES) 0 SJF $(TEST_INPUT_SJF)
