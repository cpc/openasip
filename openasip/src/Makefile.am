AUTOMAKE_OPTIONS = subdir-objects

SUBDIRS = base tools applibs . bintools procgen codesign 

lib_LTLIBRARIES = libopenasip.la

libopenasip_la_LDFLAGS = ${TCE_LDFLAGS} -version-info ${LIB_VERSION}

# http://www.gnu.org/software/hello/manual/automake/Libtool-Convenience-Libraries.html
# Dummy C++ source to cause C++ linking.
nodist_EXTRA_libopenasip_la_SOURCES = tools/Application.cc

libopenasip_la_SOURCES = 
libopenasip_la_LIBADD = base/libttabase.la tools/libopenasiptools.la \
                   applibs/libapplibs.la 

