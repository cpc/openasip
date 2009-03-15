
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
	TPEFDebugSectionReader.cc NullBinary.cc

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
	AOutDataSectionReader.hh AOutReader.hh \
	AOutRelocationSectionReader.hh AOutSectionReader.hh \
	AOutStringSectionReader.hh AOutSymbolSectionReader.hh \
	AOutTextSectionReader.hh ASpaceElement.hh \
	ASpaceSection.hh BasicElement.hh \
	Binary.hh BinaryReader.hh \
	BinaryStream.hh BinaryWriter.hh \
	Chunk.hh CodeSection.hh \
	CodeSymElement.hh DataSection.hh \
	DataSymElement.hh DebugElement.hh \
	DebugSection.hh DebugStabElem.hh \
	FileOffsetReplacer.hh FileSymElement.hh \
	ImmediateElement.hh InstructionElement.hh \
	LineNumElement.hh LineNumProcedure.hh \
	LineNumSection.hh Locator.hh \
	MoveElement.hh NoTypeSymElement.hh \
	NullBinary.hh NullSection.hh \
	ProcedSymElement.hh ReferenceKey.hh \
	RelocElement.hh RelocSection.hh \
	ResourceElement.hh ResourceSection.hh \
	SafePointable.hh SafePointer.hh \
	Section.hh SectionElement.hh \
	SectionIdReplacer.hh SectionIndexReplacer.hh \
	SectionOffsetReplacer.hh SectionReader.hh \
	SectionSizeReplacer.hh SectionSymElement.hh \
	SectionWriter.hh StringSection.hh \
	Swapper.hh SymbolElement.hh \
	SymbolSection.hh TPEFASpaceSectionReader.hh \
	TPEFASpaceSectionWriter.hh TPEFBaseType.hh \
	TPEFCodeSectionReader.hh TPEFCodeSectionWriter.hh \
	TPEFDataSectionReader.hh TPEFDataSectionWriter.hh \
	TPEFDebugSectionReader.hh TPEFDebugSectionWriter.hh \
	TPEFHeaders.hh TPEFLineNumSectionReader.hh \
	TPEFLineNumSectionWriter.hh TPEFNullSectionReader.hh \
	TPEFNullSectionWriter.hh TPEFReader.hh \
	TPEFRelocSectionReader.hh TPEFRelocSectionWriter.hh \
	TPEFResourceSectionReader.hh TPEFResourceSectionWriter.hh \
	TPEFSectionReader.hh TPEFSectionWriter.hh \
	TPEFStringSectionReader.hh TPEFStringSectionWriter.hh \
	TPEFSymbolSectionReader.hh TPEFSymbolSectionWriter.hh \
	TPEFTools.hh TPEFUDataSectionReader.hh \
	TPEFUDataSectionWriter.hh TPEFWriter.hh \
	UDataSection.hh ValueReplacer.hh \
	AOutReader.icc AOutRelocationSectionReader.icc \
	ASpaceElement.icc BasicElement.icc \
	Binary.icc BinaryStream.icc \
	Chunk.icc ImmediateElement.icc \
	InstructionElement.icc LineNumElement.icc \
	LineNumProcedure.icc MoveElement.icc \
	ReferenceKey.icc RelocElement.icc \
	RelocSection.icc ResourceElement.icc \
	SafePointer.icc Section.icc \
	Swapper.icc SymbolElement.icc 
## headers end
