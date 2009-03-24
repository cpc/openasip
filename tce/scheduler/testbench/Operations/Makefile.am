OSALBUILDER=../../../src/codesign/osal/OSALBuilder/buildopset

#simulator_test_operations.opb: \
#	simulator_test_operations.cc simulator_test_operations.opp
#	${OSALBUILDER} simulator_test_operations

all-local: $(patsubst %.opp,%.opb,$(notdir $(wildcard $(srcdir)/*.opp)))

%.opb : %.cc %.opp
	@${OSALBUILDER} $(basename $<) && if test ! -e $@; then mv -f $(basename $<).opb $@; fi

clean:
	rm -f *.opb