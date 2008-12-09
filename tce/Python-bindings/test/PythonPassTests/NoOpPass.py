# Copyright (c) 2002-2009 Tampere University of Technology.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
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
