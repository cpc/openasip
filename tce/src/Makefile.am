SUBDIRS = base tools applibs . bintools procgen codesign 

lib_LTLIBRARIES = libtce.la

# NOTE: TCE 1.2 should be fully API compatible with TCE 1.1 libs,
# therefore the '1' as the 'age' of the libtool version string. 
# Usually backwards compatibility probably won't be maintained.

libtce_la_LDFLAGS = ${TCE_LDFLAGS} -version-info 2:0:1

# http://www.gnu.org/software/hello/manual/automake/Libtool-Convenience-Libraries.html
# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libtce_la_SOURCES = tools/Application.cc

libtce_la_SOURCES = 
libtce_la_LIBADD = base/libttabase.la tools/libtcetools.la \
                   applibs/libapplibs.la 

