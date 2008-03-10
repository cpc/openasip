#!/usr/bin/python
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
