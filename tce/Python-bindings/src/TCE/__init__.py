#
# Top level package for the TCE libraries.
# Pertti Kellomaki, 2007 (pertti.kellomaki@tut.fi)
#

# Set the RTLD_GLOBAL flag for dlopen(). Without the flag dlopen() fails
# when invoked from inside a loadable Python module.
import sys
import dl
flags = sys.getdlopenflags()
sys.setdlopenflags(flags | dl.RTLD_GLOBAL)

import base
import tools
import applibs
__all__ = ["base", "tools", "applibs"]
