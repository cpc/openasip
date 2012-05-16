
noinst_LTLIBRARIES = libtpef.la
libtpef_la_SOURCES = AOutDataSectionReader.cc AOutReader.cc \
	AOutRelocationSectionReader.cc AOutStringSectionReader.cc \
	AOutSymbolSectionReader.cc AOutTextSectionReader.cc \
	ASpaceSection.cc Binary.cc BinaryReader.cc BinaryStream.cc \
	DataSection.cc ReferenceKey.cc RelocSection.cc \
	SafePointable.cc SafePointer.cc Section.cc SectionReader.cc \
	StringSection.cc SymbolSection.cc CodeSection.cc \
	UDataSection.cc RelocElement.cc SymbolElement.cc MoveElement.cc \
	ImmediateElement.cc Chunk.cc ASpaceElement.cc BasicElement.cc \
	SectionElement.cc InstructionElement.cc BinaryWriter.cc \
	SectionWriter.cc ValueReplacer.cc AOutSectionReader.cc \
	TPEFWriter.cc TPEFSectionWriter.cc TPEFDataSectionWriter.cc \
	FileOffsetReplacer.cc SectionOffsetReplacer.cc \
	TPEFCodeSectionWriter.cc SectionSizeReplacer.cc \
	TPEFReader.cc TPEFSectionReader.cc \
	TPEFASpaceSectionReader.cc TPEFStringSectionWriter.cc \
	TPEFUDataSectionWriter.cc TPEFStringSectionReader.cc \
	TPEFASpaceSectionWriter.cc SectionIdReplacer.cc \
	SectionIndexReplacer.cc TPEFCodeSectionReader.cc \
	TPEFRelocSectionWriter.cc TPEFDataSectionReader.cc \
	TPEFUDataSectionReader.cc TPEFRelocSectionReader.cc \
	NullSection.cc TPEFNullSectionWriter.cc \
	TPEFNullSectionReader.cc ResourceSection.cc \
	ResourceElement.cc TPEFResourceSectionReader.cc \
	TPEFResourceSectionWriter.cc LineNumSection.cc \
	LineNumElement.cc LineNumProcedure.cc \
	TPEFLineNumSectionWriter.cc TPEFLineNumSectionReader.cc \
	NoTypeSymElement.cc DataSymElement.cc CodeSymElement.cc \
	SectionSymElement.cc FileSymElement.cc TPEFSymbolSectionReader.cc \
	TPEFSymbolSectionWriter.cc Locator.cc \
	TPEFTools.cc ProcedSymElement.cc DebugSection.cc \
	DebugElement.cc DebugStabElem.cc TPEFDebugSectionWriter.cc \
	TPEFDebugSectionReader.cc

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config
AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: clean doc

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da





## headers start
libtpef_la_SOURCES += \
	SectionOffsetReplacer.hh TPEFSymbolSectionWriter.hh \
	TPEFResourceSectionWriter.hh SectionReader.hh \
	ReferenceKey.hh MoveElement.hh \
	ASpaceSection.hh BasicElement.hh \
	ProcedSymElement.hh NullSection.hh \
	SymbolSection.hh TPEFStringSectionWriter.hh \
	TPEFASpaceSectionReader.hh ResourceElement.hh \
	AOutTextSectionReader.hh TPEFSymbolSectionReader.hh \
	TPEFBaseType.hh SectionSymElement.hh \
	TPEFCodeSectionReader.hh TPEFUDataSectionWriter.hh \
	SafePointer.hh AOutDataSectionReader.hh \
	SymbolElement.hh InstructionElement.hh \
	AOutSymbolSectionReader.hh Binary.hh \
	ImmediateElement.hh TPEFLineNumSectionWriter.hh \
	Locator.hh SectionSizeReplacer.hh \
	LineNumSection.hh TPEFNullSectionWriter.hh \
	RelocElement.hh TPEFSectionWriter.hh \
	FileSymElement.hh BinaryWriter.hh \
	SectionElement.hh SectionIndexReplacer.hh \
	UDataSection.hh CodeSymElement.hh \
	TPEFRelocSectionReader.hh TPEFNullSectionReader.hh \
	ASpaceElement.hh TPEFTools.hh \
	StringSection.hh AOutReader.hh \
	TPEFDataSectionReader.hh ValueReplacer.hh \
	TPEFUDataSectionReader.hh TPEFDebugSectionReader.hh \
	Chunk.hh DataSection.hh \
	TPEFDebugSectionWriter.hh TPEFCodeSectionWriter.hh \
	SectionWriter.hh DebugSection.hh \
	ResourceSection.hh CodeSection.hh \
	BinaryReader.hh AOutRelocationSectionReader.hh \
	FileOffsetReplacer.hh LineNumElement.hh \
	AOutStringSectionReader.hh SectionIdReplacer.hh \
	TPEFRelocSectionWriter.hh DebugElement.hh \
	TPEFSectionReader.hh TPEFHeaders.hh \
	TPEFResourceSectionReader.hh \
	DataSymElement.hh TPEFLineNumSectionReader.hh \
	TPEFWriter.hh Section.hh \
	TPEFReader.hh AOutSectionReader.hh \
	RelocSection.hh Swapper.hh \
	DebugStabElem.hh TPEFStringSectionReader.hh \
	SafePointable.hh TPEFASpaceSectionWriter.hh \
	NoTypeSymElement.hh TPEFDataSectionWriter.hh \
	LineNumProcedure.hh BinaryStream.hh \
	ImmediateElement.icc RelocSection.icc \
	BinaryStream.icc ASpaceElement.icc \
	SymbolElement.icc Section.icc \
	SafePointer.icc MoveElement.icc \
	Binary.icc Chunk.icc \
	LineNumElement.icc RelocElement.icc \
	LineNumProcedure.icc Swapper.icc \
	BasicElement.icc AOutReader.icc \
	AOutRelocationSectionReader.icc ResourceElement.icc \
	InstructionElement.icc ReferenceKey.icc 
## headers end
