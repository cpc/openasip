
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

libbem_la_LDFLAGS = -lpthread

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
	BEMSerializer.hh BEMTester.hh \
	BEMTextGenerator.hh BinaryEncoding.hh \
	BridgeEncoding.hh DestinationField.hh \
	Encoding.hh FUGuardEncoding.hh \
	FUPortCode.hh GPRGuardEncoding.hh \
	GuardEncoding.hh GuardField.hh \
	IUPortCode.hh ImmediateControlField.hh \
	ImmediateEncoding.hh ImmediateSlotField.hh \
	InstructionField.hh LImmDstRegisterField.hh \
	MoveSlot.hh NOPEncoding.hh \
	NullBridgeEncoding.hh NullDestinationField.hh \
	NullFUGuardEncoding.hh NullFUPortCode.hh \
	NullGPRGuardEncoding.hh NullGuardField.hh \
	NullIUPortCode.hh NullImmediateControlField.hh \
	NullImmediateEncoding.hh NullInstructionField.hh \
	NullMoveSlot.hh NullNOPEncoding.hh \
	NullRFPortCode.hh NullSocketCodeTable.hh \
	NullSocketEncoding.hh NullSourceField.hh \
	NullUnconditionalGuardEncoding.hh PortCode.hh \
	RFPortCode.hh SlotField.hh \
	SocketCodeTable.hh SocketEncoding.hh \
	SourceField.hh UnconditionalGuardEncoding.hh 
## headers end
