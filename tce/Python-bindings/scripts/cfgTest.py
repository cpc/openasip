#!/usr/bin/python
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
import TCE.base.program as prog
import TCE.base.mach as mach
import TCE.applibs.Scheduler.ProgramRepresentations.ControlFlowGraph as cfg
import TCE.base.osal as osal 
import sys

def printOut(pr):
    moveSum = 0
    immediateSum = 0
    instructionSum = 0
    bypassSum = 0
    blocks = 0
    for i in range(pr.procedureCount()):
        cf = cfg.ControlFlowGraph(pr.procedure(i))
        stats = cf.statistics()
        moveSum = moveSum + stats.moveCount()
        immediateSum = immediateSum + stats.immediateCount()
        instructionSum = instructionSum + stats.instructionCount()
        bypassSum = bypassSum + stats.bypassedCount()
        blocks = blocks + stats.normalBBCount()        
    print "Program:"+sys.argv[1]
    print "\tMoves:",moveSum  
    print "\tImmediates:",immediateSum 
    print "\tInstructions:",instructionSum 
    print "\tBasicBlocks:",blocks
    print "\tProcedures:",pr.procedureCount()
    print "\tMovesPerCycle:",1.0*moveSum/instructionSum 

if __name__ == "__main__":        
    if len(sys.argv) < 2 or len(sys.argv) > 4:
        print "Usage: python cfgTest.py tpef [adf] ['sum']"
    if len(sys.argv) == 4 and sys.argv[3] == "sum":
            um = mach.UniversalMachine()
            machine = mach.Machine.loadFromADF(sys.argv[2])
            pr = prog.Program.loadFromTPEF(sys.argv[1], machine, um)
            printOut(pr)
    else: 
        if len(sys.argv) == 3:
            if (sys.argv[2] != "sum") :
                um = mach.UniversalMachine()
                machine = mach.Machine.loadFromADF(sys.argv[2])
                pr = prog.Program.loadFromTPEF(sys.argv[1], machine, um)
                print "Count of procedures in '", sys.argv[1], "' is " 
                print pr.procedureCount()
                for i in range(pr.procedureCount()):
                    cf = cfg.ControlFlowGraph(pr.procedure(i))
                    print cf.printStatistics()
            else :
                um = mach.UniversalMachine()
                pr = prog.Program.loadFromTPEF(sys.argv[1], um)
                printOut(pr)
        else:
            if len(sys.argv) == 2:
                um = mach.UniversalMachine()
                pr = prog.Program.loadFromTPEF(sys.argv[1], um)
                print "Count of procedures in '", sys.argv[1], "' is " 
                print pr.procedureCount()
                for i in range(pr.procedureCount()):
                    cf = cfg.ControlFlowGraph(pr.procedure(i))
                    print cf.printStatistics()     
            else:
                print "Usage: python cfgTest.py tpef [adf] ['sum']"   
