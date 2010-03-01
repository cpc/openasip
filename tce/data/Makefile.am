SUBDIRS = bitmaps

datadir = ${prefix}/share/tce/data

nobase_data_DATA = $(srcdir)/ProDe/confschema.xsd \
                   $(srcdir)/bem/BEM_Schema.xsd \
                   $(srcdir)/mach/ADF_Schema.xsd mach/minimal.adf mach/minimal_with_stdout.adf \
                   $(srcdir)/osal/Operation_Schema.xsd \
				   $(srcdir)/idf/IDF_Schema.xsd \
                   $(srcdir)/OSEd/behavior_definition_template.cc \
                   $(srcdir)/ProGe/decompressor.vhdl ProGe/ifetch.vhdl \
                   $(srcdir)/ProGe/highest_pkg.vhdl ProGe/opcodes_pkg.vhdl \
                   $(srcdir)/icons/*.png \
                   $(srcdir)/ProGe/tb/*.vhdl \
                   $(srcdir)/Proxim/confschema.xsd \
                   $(srcdir)/hdb/*.vhdl.template

EXTRA_DIST = ${nobase_data_DATA}
