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
    
