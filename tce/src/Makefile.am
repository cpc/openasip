SUBDIRS = base tools applibs bintools procgen codesign ext

lib_LTLIBRARIES = libtce.la
libtce_la_LDFLAGS = ${SQLITE_LD_FLAGS} ${BOOST_LDFLAGS}
libtce_la_SOURCES =
libtce_la_LIBADD = base/libttabase.la tools/libtcetools.la \
                   applibs/libapplibs.la

EXTRA_DIST = tools applibs base
