##### http://autoconf-archive.cryp.to/ax_tce.html
#
# SYNOPSIS
#
#   AX_TCE 
#
# DESCRIPTION
#
#   Test for TCE library.
#
#   This macro calls:
#     
#     AC_DEFINE(HAVE_TCE)
#
#     AC_SUBST(HAVE_TCE_LIBRARY,[yes])
#     AC_SUBST(TCE_PATH)
#     AC_SUBST(TCE_INCLUDES)
#     AC_SUBST(TCE_LDFLAGS)
#     AC_SUBST(TCE_SOLFAGS)
#
#   And sets:
#
#     HAVE_TCE
#
# LAST MODIFICATION
#
#   2007-07-18
#
# COPYLEFT
#
#   Copyright (c) 2007 Mikael Lepistö <lepisto.mikael@gmail.com>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_TCE],
[
    # The location of TCE can be defined with --with-tce switch
    AC_ARG_WITH([tce],
    	[AS_HELP_STRING(
            [--with-tce@<:@=tce-install-path@:>@],
            [use the TCE library. It is possible to specify a certain location where your tce library is installed: e.g. --with-tce=/opt])
        ],
        [ax_tce_path="${withval}"], 
        [ax_tce_path=""]
    )

    ########### FIND TCE INSTALL PATH
    AC_MSG_CHECKING([Looking for installed TCE])
	if test -z "$ax_tce_path"; then
        # from path
        if test -r `which tce-config`; then
            ax_tce_path=`tce-config --prefix`
        fi

        # form /usr /usr/local/ or /opt directories       
        for ac_tce_path_tmp in /usr /usr/local /opt ; do
            if test -r "$ac_tce_path_tmp/bin/tce-config"; then
                ax_tce_path="$ac_tce_path_tmp"
                break;
            fi
	    done
    fi

    # set variables about tce
    if ! test -r $ax_tce_path/bin/tce-config; then
        AC_MSG_ERROR([tce-config not found.])
    fi
    
    ax_tce_includes=`$ax_tce_path/bin/tce-config --includes`
    ax_tce_ldflags=`$ax_tce_path/bin/tce-config --libs`
    ax_tce_soflags=`$ax_tce_path/bin/tce-config --so-flags`
    
    AC_DEFINE([HAVE_TCE],[1],[TCE library is installed to system.])
    AC_SUBST([HAVE_TCE_LIBRARY],     [yes])
    AC_SUBST([TCE_PATH],     [$ax_tce_path])
    AC_SUBST([TCE_INCLUDES], [$ax_tce_includes])
    AC_SUBST([TCE_LDFLAGS],  [$ax_tce_ldflags])
    AC_SUBST([TCE_SOFLAGS],  [$ax_tce_soflags])

    AC_MSG_RESULT([$TCE_PATH])

    ########### CHECK INSTALLED HEADERS
    CPPFLAGS_SAVED="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $TCE_INCLUDES"
	export CPPFLAGS

   	LDFLAGS_SAVED="$LDFLAGS"
	LDFLAGS="$LDFLAGS $TCE_LDFLAGS"
	export LDFLAGS

    AC_LANG_PUSH([C++])

#     # Check if there is plugin headers
#     AC_MSG_CHECKING([Checking TCE plugin headers])
#     AC_PREPROC_IFELSE(
#           [AC_LANG_PROGRAM([[
#               #include <tce/Application.hh>
#               #include <tce/BaseType.hh>
#               #include <tce/config.h>
#               #include <tce/Conversion.hh>
#               #include <tce/Exception.hh>
#               #include <tce/MathTools.hh>
#               #include <tce/Memory.hh>
#               #include <tce/ObjectState.hh>
#               #include <tce/Operand.hh>
#               #include <tce/OperationBehavior.hh>
#               #include <tce/OperationContext.hh>
#               #include <tce/Operation.hh>
#               #include <tce/OperationState.hh>
#               #include <tce/OSAL.hh>
#               #include <tce/Serializable.hh>
#               #include <tce/SimValue.hh>
#               #include <tce/TargetMemory.hh>
#           ]], [[ ]] )],
#           [AC_MSG_RESULT([yes])],
#           [
#              AC_MSG_FAILURE([TCE plugin headers not found.])
#              # TODO: define HAVE_TCE_PLUGIN            
#           ])

#     # Check if there is development headers
#     AC_MSG_CHECKING([Checking TCE developer headers])
#     AC_PREPROC_IFELSE(
#         [AC_LANG_PROGRAM([[
#              #include <tce/Program.hh>
#              #include <tce/TPEFWriter.hh>
#              #include <tce/Binary.hh>
#              #include <tce/TPEFProgramFactory.hh>
#              #include <tce/Exception.hh>            
#         ]], [[ ]] )],
#         [AC_MSG_RESULT([yes])],
#         [
#             AC_MSG_FAILURE([TCE development headers not found.])
#             # TODO: define HAVE_TCE_DEVEL            
#         ])
   

    ########### TRY TO COMPILE TEST PROGRAM
    AC_MSG_CHECKING([Checking TCE library linkage])
    AC_LINK_IFELSE(
        [AC_LANG_PROGRAM([[
            #include <tce/Binary.hh>
        ]], [[ ; ]] )],
        [AC_MSG_RESULT([yes])],
        [AC_MSG_ERROR([Problems with using TCE library. Check config.log for more information.])]
    )
    
    ########### TRY TO COMPILE PLUGIN
    # TODO: Check plugin linking functionality

    AC_LANG_POP([C++])
	CPPFLAGS="$CPPFLAGS_SAVED"
    LDFLAGS="$LDFLAGS_SAVED"
])
