
noinst_LTLIBRARIES = libbem.la
libbem_la_SOURCES = BinaryEncoding.cc ImmediateControlField.cc \
	MoveSlot.cc InstructionField.cc NullInstructionField.cc \
        SlotField.cc GuardField.cc GuardEncoding.cc GPRGuardEncoding.cc \
        FUGuardEncoding.cc NullImmediateEncoding.cc NOPEncoding.cc \
	SourceField.cc DestinationField.cc UnconditionalGuardEncoding.cc \
	SocketEncoding.cc SocketCodeTable.cc FUPortCode.cc RFPortCode.cc \
	NullImmediateControlField.cc NullGPRGuardEncoding.cc \
	NullFUGuardEncoding.cc NullGuardField.cc NullSourceField.cc \
	NullDestinationField.cc BridgeEncoding.cc NullSocketEncoding.cc \
	NullFUPortCode.cc NullRFPortCode.cc NullSocketCodeTable.cc \
	BEMTester.cc BEMSerializer.cc ImmediateEncoding.cc \
	IUPortCode.cc NullIUPortCode.cc LImmDstRegisterField.cc \
	NullUnconditionalGuardEncoding.cc Encoding.cc NullNOPEncoding.cc \
	ImmediateSlotField.cc BEMTextGenerator.cc PortCode.cc \
	NullBridgeEncoding.cc

libbem_la_LDFLAGS = $PTHREAD_LIBS

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: doc

MAINTAINERCLEAN = *~ *.gcov *.bbg *.bb *.da




## headers start
libbem_la_SOURCES += \
	NullDestinationField.hh SourceField.hh \
	NullGuardField.hh SlotField.hh \
	NullImmediateControlField.hh GPRGuardEncoding.hh \
	RFPortCode.hh NullGPRGuardEncoding.hh \
	ImmediateEncoding.hh LImmDstRegisterField.hh \
	SocketCodeTable.hh InstructionField.hh \
	NullFUPortCode.hh MoveSlot.hh \
	NOPEncoding.hh BinaryEncoding.hh \
	FUGuardEncoding.hh BEMTester.hh \
	BEMTextGenerator.hh Encoding.hh \
	UnconditionalGuardEncoding.hh NullSocketCodeTable.hh \
	NullFUGuardEncoding.hh BridgeEncoding.hh \
	GuardEncoding.hh NullNOPEncoding.hh \
	NullSocketEncoding.hh NullSourceField.hh \
	BEMSerializer.hh GuardField.hh \
	NullInstructionField.hh FUPortCode.hh \
	NullBridgeEncoding.hh PortCode.hh \
	SocketEncoding.hh NullIUPortCode.hh \
	NullUnconditionalGuardEncoding.hh NullRFPortCode.hh \
	NullImmediateEncoding.hh ImmediateControlField.hh \
	IUPortCode.hh ImmediateSlotField.hh \
	DestinationField.hh 
## headers end
