
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
	BasicElement.hh TPEFNullSectionReader.hh \
	ASpaceSection.hh AOutDataSectionReader.hh \
	AOutRelocationSectionReader.hh TPEFDebugSectionWriter.hh \
	TPEFASpaceSectionWriter.hh LineNumSection.hh \
	TPEFRelocSectionWriter.hh AOutStringSectionReader.hh \
	InstructionElement.hh LineNumElement.hh \
	BinaryWriter.hh ImmediateElement.hh \
	FileSymElement.hh SafePointable.hh \
	TPEFReader.hh TPEFStringSectionWriter.hh \
	TPEFResourceSectionWriter.hh SectionIdReplacer.hh \
	SectionOffsetReplacer.hh AOutSymbolSectionReader.hh \
	TPEFWriter.hh Binary.hh \
	SafePointer.hh TPEFBaseType.hh \
	AOutReader.hh TPEFDebugSectionReader.hh \
	SectionElement.hh TPEFCodeSectionReader.hh \
	TPEFUDataSectionReader.hh ResourceElement.hh \
	AOutTextSectionReader.hh Chunk.hh \
	TPEFResourceSectionReader.hh TPEFSectionReader.hh \
	TPEFTools.hh BinaryStream.hh \
	TPEFStringSectionReader.hh TPEFSymbolSectionReader.hh \
	RelocElement.hh TPEFRelocSectionReader.hh \
	SectionIndexReplacer.hh TPEFDataSectionWriter.hh \
	ProcedSymElement.hh TPEFSectionWriter.hh \
	ASpaceElement.hh DebugStabElem.hh \
	SymbolSection.hh UDataSection.hh \
	BinaryReader.hh TPEFLineNumSectionReader.hh \
	TPEFSymbolSectionWriter.hh MoveElement.hh \
	AOutSectionReader.hh Swapper.hh \
	CodeSection.hh SectionWriter.hh \
	ResourceSection.hh StringSection.hh \
	SectionReader.hh SectionSizeReplacer.hh \
	DebugSection.hh CodeSymElement.hh \
	Section.hh NullSection.hh \
	TPEFCodeSectionWriter.hh TPEFASpaceSectionReader.hh \
	TPEFNullSectionWriter.hh DataSymElement.hh \
	NoTypeSymElement.hh TPEFHeaders.hh \
	FileOffsetReplacer.hh SectionSymElement.hh \
	DataSection.hh Locator.hh \
	LineNumProcedure.hh TPEFUDataSectionWriter.hh \
	SymbolElement.hh DebugElement.hh \
	ValueReplacer.hh RelocSection.hh \
	ReferenceKey.hh TPEFDataSectionReader.hh \
	TPEFLineNumSectionWriter.hh SafePointer.icc \
	ASpaceElement.icc Section.icc \
	MoveElement.icc ReferenceKey.icc \
	ResourceElement.icc LineNumElement.icc \
	Swapper.icc Binary.icc \
	AOutRelocationSectionReader.icc AOutReader.icc \
	SymbolElement.icc RelocSection.icc \
	LineNumProcedure.icc ImmediateElement.icc \
	Chunk.icc InstructionElement.icc \
	BinaryStream.icc RelocElement.icc \
	BasicElement.icc 
## headers end
