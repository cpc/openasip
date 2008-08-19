# Copyright 2002-2008 Tampere University of Technology.  All Rights Reserved.
#
# This file is part of TTA-Based Codesign Environment (TCE).
#
# TCE is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License version 2 as published by the Free
# Software Foundation.
#
# TCE is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin St,
# Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this file
# does not by itself cause the resulting executable to be covered by the GNU
# General Public License.  This exception does not however invalidate any
# other reasons why the executable file might be covered by the GNU General
# Public License.
#
# A PythonSchedulerPass that does nothing but imports all the bindings.
#

import TCE.base.program
import TCE.base.mach
import TCE.applibs.Scheduler.ProgramRepresentations.ProgramDependenceGraph
import TCE.applibs.Scheduler.ResourceManager
import TCE.applibs.Scheduler.ResourceModel
import TCE.applibs.Scheduler.SchedulerModules


#
# This class will be instantiated by the scheduler frontend.
#

class NoOpPass:
    def __init__(self):
        print "NoOpPass instantiated"

    def needsTarget(self):
        print "needsTarget() called"
        return True

    def needsProgram(self):
        print "needsProgram() called"
        return True

    def needsProgramRepresentation(self):
        print "needsProgramRepresentation() called"
        return False

    def setTarget(self, target):
        print "setTarget() called with", target
        self.target = target

    def setProgram(self, program):
        print "setProgram() called with", program
        self.program = program

    def registerHelperModule(self, module):
        print "registerHelperModule() called"

    def start(self):
        print "Running NoOpPass.py"
