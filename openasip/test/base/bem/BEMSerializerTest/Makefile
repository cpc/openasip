DIST_OBJECTS = BinaryEncoding.o DestinationField.o \
	FUGuardEncoding.o FUPortCode.o GPRGuardEncoding.o GuardEncoding.o \
	GuardField.o ImmediateControlField.o NullImmediateEncoding.o \
	InstructionField.o MoveSlot.o NullInstructionField.o RFPortCode.o \
	SlotField.o SocketCodeTable.o SocketEncoding.o SourceField.o \
	NullImmediateControlField.o NullGPRGuardEncoding.o \
	NullFUGuardEncoding.o NullGuardField.o NullSourceField.o \
	NullDestinationField.o NullSocketEncoding.o BridgeEncoding.o \
	NullFUPortCode.o NullRFPortCode.o NullSocketCodeTable.o \
	BEMTester.o PortCode.o IUPortCode.o BEMTextGenerator.o \
	NullIUPortCode.o BEMSerializer.o ImmediateEncoding.o \
	UnconditionalGuardEncoding.o NullUnconditionalGuardEncoding.o \
	NOPEncoding.o NullNOPEncoding.o Encoding.o ImmediateSlotField.o \
	LImmDstRegisterField.o NullBridgeEncoding.o

TOOL_OBJECTS = *.o
TOP_SRCDIR = ../../../..
INITIALIZATION = init_data_dir

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = ${BOOST_LDFLAGS} ${DL_FLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs

init_data_dir:
	rm -fr data
	mkdir data
