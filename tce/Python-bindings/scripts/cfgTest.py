#!/usr/bin/python
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
