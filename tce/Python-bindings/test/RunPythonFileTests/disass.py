# Copyright (c) 2002-2009 Tampere University.
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
    
