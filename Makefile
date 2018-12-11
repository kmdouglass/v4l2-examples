EXAMPLE-1 = example-1
EXAMPLE-2 = example-2

EXAMPLES = $(EXAMPLE-1) $(EXAMPLE-2)

all:
	for i in $(EXAMPLES) ; do \
	(cd $$i; make); \
	done

clean:

	for i in $(EXAMPLES) ; do \
	(cd $$i; make clean); \
	done
