SUBDIRS = base tools applibs . bintools procgen codesign 

lib_LTLIBRARIES = libtce.la

# TCE 1.1 = 1:0:0
# TCE 1.2 = 2:0:0 (unsure if 100% API compatible with 1.1)
# TCE 1.3 = 3:0:0 (some API changes at least in the simulator)
# TCE 1.4 = 4:0:0
# TCE 1.5 = 5:0:0
libtce_la_LDFLAGS = ${TCE_LDFLAGS} -version-info 5:0:0

# http://www.gnu.org/software/hello/manual/automake/Libtool-Convenience-Libraries.html
# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libtce_la_SOURCES = tools/Application.cc

libtce_la_SOURCES = 
libtce_la_LIBADD = base/libttabase.la tools/libtcetools.la \
                   applibs/libapplibs.la 

