# Base objects that contains safe pointer functionality
# and object representation.
TPEF_BASE_OBJECTS = SafePointable.o \
		SafePointer.o \
		ReferenceKey.o \
		Binary.o \
		Section.o \
		NullSection.o \
		ASpaceSection.o \
		CodeSection.o \
		UDataSection.o \
		DataSection.o \
		StringSection.o \
		SymbolSection.o \
		RelocSection.o \
		ResourceSection.o \
		LineNumSection.o \
		BasicElement.o \
		SectionElement.o \
		ASpaceElement.o \
		InstructionElement.o \
		MoveElement.o \
		ImmediateElement.o \
		Chunk.o \
		SymbolElement.o \
		NoTypeSymElement.o \
		CodeSymElement.o \
		ProcedSymElement.o \
		DataSymElement.o \
		FileSymElement.o \
		SectionSymElement.o \
		RelocElement.o \
		ResourceElement.o \
		LineNumElement.o \
		LineNumProcedure.o \
		DebugSection.o \
		DebugElement.o \
		DebugStabElem.o \

# Binary file raw data reader.
BINARY_STREAM_OBJECTS = BinaryStream.o \

# Base interfaces and functionality for reading binary files.
# depends on 
# TPEF_BASE_OBJECTS 
# BINARY_STREAM_OBJECTS
READER_COMMON_OBJECTS = BinaryReader.o \
			SectionReader.o \

# Base interfaces and functionality for all binary file writers.
# depends on 
# TPEF_BASE_OBJECTS 
# BINARY_STREAM_OBJECTS
WRITER_COMMON_OBJECTS = BinaryWriter.o \
			SectionWriter.o \

# Needed by TPEFWriter
# depends on 
# TPEF_BASE_OBJECTS
# BINARY_STREAM_OBJECTS
VALUE_REPLACER_OBJECTS = ValueReplacer.o \
			SectionIdReplacer.o \
			SectionIndexReplacer.o \
			SectionSizeReplacer.o \
			SectionOffsetReplacer.o \
			FileOffsetReplacer.o \

# Add support for writing TPEF binary files.
# depends on 
# TPEF_BASE_OBJECTS
# VALUE_REPLACER_OBJECTS
# WRITER_COMMON_OBJECTS
TPEF_WRITER_OBJECTS = TPEFWriter.o \
		TPEFSectionWriter.o \
		TPEFNullSectionWriter.o \
		TPEFASpaceSectionWriter.o \
		TPEFCodeSectionWriter.o \
		TPEFUDataSectionWriter.o \
		TPEFDataSectionWriter.o \
		TPEFStringSectionWriter.o \
		TPEFSymbolSectionWriter.o \
		TPEFRelocSectionWriter.o \
		TPEFResourceSectionWriter.o \
		TPEFLineNumSectionWriter.o \
		TPEFDebugSectionWriter.o \

# Adds support for reading TPEF binary files.
# depends on
# TPEF_BASE_OBJECTS
# BINARY_STREAM_OBJECTS
# READER_COMMON_OBJECTS
TPEF_READER_OBJECTS = 	TPEFReader.o \
			TPEFSectionReader.o \
			TPEFNullSectionReader.o \
			TPEFASpaceSectionReader.o \
			TPEFCodeSectionReader.o \
			TPEFUDataSectionReader.o \
			TPEFDataSectionReader.o \
			TPEFStringSectionReader.o \
			TPEFSymbolSectionReader.o \
			TPEFRelocSectionReader.o \
			TPEFResourceSectionReader.o \
			TPEFLineNumSectionReader.o \
			TPEFDebugSectionReader.o \
			Locator.o \

# Adds support for reading TTA a.out binary files.
# depends on
# TPEF_COMMON_OBJECTS
# BINARY_STREAM_OBJECTS
# READER_COMMON_OBJECTS
AOUT_READER_OBJECTS = AOutReader.o \
		AOutSectionReader.o \
		AOutTextSectionReader.o \
		AOutDataSectionReader.o \
		AOutStringSectionReader.o \
		AOutSymbolSectionReader.o \
		AOutRelocationSectionReader.o \

DIST_OBJECTS = 	${TPEF_BASE_OBJECTS} \
		${TPEF_READER_OBJECTS} \
		${TPEF_WRITER_OBJECTS} \
		${AOUT_READER_OBJECTS} \
		${VALUE_REPLACER_OBJECTS} \
		${BINARY_STREAM_OBJECTS} \
		${WRITER_COMMON_OBJECTS} \
		${READER_COMMON_OBJECTS} \

TOP_SRCDIR = ../../../..
TOOL_OBJECTS = Exception.o Conversion.o Application.o

include ${TOP_SRCDIR}/test/Makefile_configure_settings
include ${TOP_SRCDIR}/test/Makefile_test.defs
