CC ?= gcc
CFLAGS ?= -O0 -Wall -std=gnu99
SRCS = main.c
EXEC = natural-mergesort natural-mergesort_change gen_testcase

all: $(EXEC)

natural-mergesort: $(SRCS) natural-mergesort.h natural-mergesort.c
	$(CC) $(CFLAGS) -DIMPL="\"$@.h\"" -o $@ $(SRCS) $@.c

natural-mergesort_change: $(SRCS) natural-mergesort_change.h natural-mergesort_change.c
        $(CC) $(CFLAGS) -DIMPL="\"$@.h\"" -o $@ $(SRCS) $@.c

gen_testcase: gen_testcase.c
	$(CC) $(CFLAGS) -o $@ $@.c

test: $(EXEC)
	perf stat --repeat 100 \
		-e cache-miss,cache-references,instructions,cycles \
		./natural-mergesort \
        perf stat --repeat 100 \
                -e cache-miss,cache-references,instructions,cycles \
                ./natural-mergesort_change

.PHONY: clean

clean:
	rm -f \
	$(EXEC)
