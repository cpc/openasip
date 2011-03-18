
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
	SourceField.hh NullImmediateEncoding.hh \
	FUPortCode.hh NullFUGuardEncoding.hh \
	DestinationField.hh PortCode.hh \
	NullGuardField.hh NOPEncoding.hh \
	NullRFPortCode.hh RFPortCode.hh \
	BridgeEncoding.hh BEMTester.hh \
	NullSourceField.hh NullUnconditionalGuardEncoding.hh \
	NullFUPortCode.hh NullSocketEncoding.hh \
	NullImmediateControlField.hh NullSocketCodeTable.hh \
	LImmDstRegisterField.hh IUPortCode.hh \
	FUGuardEncoding.hh NullDestinationField.hh \
	GuardField.hh SlotField.hh \
	ImmediateEncoding.hh NullInstructionField.hh \
	NullMoveSlot.hh NullBridgeEncoding.hh \
	GPRGuardEncoding.hh InstructionField.hh \
	GuardEncoding.hh BEMSerializer.hh \
	SocketEncoding.hh ImmediateSlotField.hh \
	SocketCodeTable.hh UnconditionalGuardEncoding.hh \
	ImmediateControlField.hh NullGPRGuardEncoding.hh \
	BEMTextGenerator.hh NullNOPEncoding.hh \
	Encoding.hh BinaryEncoding.hh \
	MoveSlot.hh NullIUPortCode.hh 
## headers end
