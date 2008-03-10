#
# A PythonSchedulerPass that does nothing but imports all the bindings.
#

import TCE.base.program
import TCE.base.mach
import TCE.base.tpef
import TCE.applibs.Scheduler.ProgramRepresentations.ProgramDependenceGraph
import TCE.applibs.Scheduler.ResourceManager
import TCE.applibs.Scheduler.ResourceModel


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

    def start(self):
        print "Running NoOpPass.py"
