import TCE.tools as tools
import TCE.base.mach as mach
import TCE.base.program as prog
import TCE.base.osal 
import sys

def terminalStr(term):
    tn = type(term).__name__
    if tn == 'TerminalImmediate' or tn == 'TerminalInstructionAddress':
	# Should distinguish between signed and unsigned immediates
        return "%s" % term.value().uIntWordValue()
    elif tn == 'TerminalAddress':
        return "%s" % term.value().uIntWordValue()
    elif tn == 'TerminalFUPort':
        if term.isOpcodeSetting():
            return "%s.%s.%s" % (term.functionUnit().name(), term.port().name(), term.operation().name())
        else:
            return "%s.%s" % (term.functionUnit().name(), term.port().name())
    elif tn == 'TerminalRegister':
	return "%s.%s.%s" % (term.registerFile().name(), term.port().name(), term.index())
    

def moveStr(move):
    return "%s -> %s" % (terminalStr(move.source()), terminalStr(move.destination()))

def instructionStr(instr):
    res = ""
    for i in range(0,instr.moveCount()):
        res = res + moveStr(instr.move(i)) + "; "
    return res

def dump(prog,  start, end):
    for a in range(start, end):
        print "%s: %s" % (a, instructionStr(prog.instructionAt(a)))

if __name__ == "__main__":        
    if len(sys.argv) == 4:
	um = mach.UniversalMachine()
	pr = prog.Program.loadFromTPEF(sys.argv[1], um)
        dump(pr, int(sys.argv[2]), int(sys.argv[3]))
    else:
        print "Usage: python disass.py binary start end"
    
