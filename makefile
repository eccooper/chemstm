# this is just a junction to build our programs easily

all:
	make -f sparse.mk
	make -f stmsim.mk

sparse:
	make -f sparse.mk

stmsim:
	make -f stmsim.mk

clean:
	rm src/*.o