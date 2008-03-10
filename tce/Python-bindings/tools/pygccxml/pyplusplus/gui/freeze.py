#! /usr/bin/python
# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)


"""this module contains "freeze" functionality.

It allows to create and distribute Py++ GUI as executable, that could be 
run without installing Python, pygccxml and Py++.
"""

import os
import sys
import shutil

def freeze_gui(source_dir, packages_dir, freeze_executable, target_dir):      
    target_name = 'demo'
    target_dir = os.path.join( target_dir, target_name, sys.platform )
    if not os.path.exists( target_dir ):
        os.makedirs( target_dir )
    if 'win32' == sys.platform:
        target_name = target_name + '.exe'

    cmd = [ freeze_executable ]
    cmd.append( '--install-dir=%s' % target_dir )
    cmd.append( '--target-name=%s' % target_name )
    cmd.append( '--include-path=%s' % packages_dir )
    cmd.append( os.path.join( source_dir, 'ui.py' ) )
    cmd = ' '.join( cmd )

    input, output = os.popen4( cmd )
    input.close()
    reports = []
    while True:
        data = output.readline()
        reports.append( data )
        if not data:
            break
    exit_status = output.close()
    msg = ''.join(reports)
    if exit_status:
        raise RuntimeError('unable to create executable. error: %s' % msg )

    if sys.platform == 'win32':
        dlls = os.path.join( os.path.split( sys.executable )[0], 'dlls' )        
        files_to_copy = [ 'tk84.dll', 'tcl84.dll' ]
        for f in files_to_copy:
            shutil.copyfile( os.path.join( dlls, f )
                             , os.path.join( target_dir, f ) )