SUBDIRS = base tools applibs . bintools procgen codesign 

lib_LTLIBRARIES = libtce.la
libtce_la_LDFLAGS = ${TCE_LDFLAGS}

# http://www.gnu.org/software/hello/manual/automake/Libtool-Convenience-Libraries.html
# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libtce_la_SOURCES = tools/Application.cc

libtce_la_SOURCES = 
libtce_la_LIBADD = base/libttabase.la tools/libtcetools.la \
                   applibs/libapplibs.la 

