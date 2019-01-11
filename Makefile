EXAMPLE-1 = example-1
EXAMPLE-2 = example-2
EXAMPLE-3 = example-3
EXAMPLE-4 = example-4
EXAMPLE-5 = example-5
EXAMPLE-6 = example-6

EXAMPLES = $(EXAMPLE-1) $(EXAMPLE-2) $(EXAMPLE-3) $(EXAMPLE-4) $(EXAMPLE-5) $(EXAMPLE-6)

all:
	for i in $(EXAMPLES) ; do \
	(cd $$i; make); \
	done

clean:

	for i in $(EXAMPLES) ; do \
	(cd $$i; make clean); \
	done
