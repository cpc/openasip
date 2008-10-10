BUILD_OPSET = ../../src/codesign/osal/OSALBuilder/buildopset
basedir = ${prefix}/share/tce/opset/base

nobase_base_DATA = base.cc base.opp base.opb

all-local: base.opb

base.opb: base.cc base.opp
	${BUILD_OPSET} $(srcdir)/base

clean:
	rm -f base.opb
