OSALBUILDER=../../../src/codesign/osal/OSALBuilder/buildopset

#simulator_test_operations.opb: \
#	simulator_test_operations.cc simulator_test_operations.opp
#	${OSALBUILDER} simulator_test_operations

all: $(patsubst %.opp,%.opb,$(wildcard *.opp))

%.opb : %.cc %.opp
	@${OSALBUILDER} $(basename $<)

clean:
	rm -f *.opb