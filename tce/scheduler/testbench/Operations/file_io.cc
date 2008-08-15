/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * Operations to emulate system calls used for file I/O, for easy compilation
 * of benchmarks, etc.
 */

#include "OSAL.hh"
#include "OperationGlobals.hh"

#include <fcntl.h>
#include <unistd.h>
#include "Application.hh"


//#define DEBUG_FILE_IO_EMU

OPERATION(OPEN_EMU)
    
TRIGGER 

// read the path from memory
std::string fileName = "";
char c = 'x';
for (int addr = INT(1); c != 0; ++addr) {
    UIntWord temp = 0;
    MEMORY.read(addr, 1, temp);
    c = (char)temp;
    fileName += c;
}

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << "open(" << fileName << "," << INT(2) 
    << "," << INT(3) << ") = ";
#endif

// need to convert the frontend mode constants to host's constants
int mod = INT(2) & 0x0F;
if (INT(2) & 0x0200)
    mod |= O_CREAT;

if (INT(2) & 0x0400)
    mod |= O_TRUNC;

if (INT(2) & 0x0008)
    mod |= O_APPEND;

IO(4) = open(fileName.c_str(), mod, 0777);

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << INT(4) << std::endl;
#endif

RETURN_READY;
END_TRIGGER

END_OPERATION(OPEN_EMU)

OPERATION(CLOSE_EMU)
    
TRIGGER 
#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << "close(" << INT(1) << ") = ";
#endif

IO(2) = close(INT(1));

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << INT(2) << std::endl;
#endif
RETURN_READY;
END_TRIGGER
END_OPERATION(CLOSE_EMU)

OPERATION(WRITE_EMU)
TRIGGER

// read the data from memory
std::string data = "";

for (unsigned addr = UINT(2); addr < UINT(2) + UINT(3); ++addr) {
    UIntWord temp = 0;
    MEMORY.read(addr, 1, temp);
    data += (char)temp;
}

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << "write(" << INT(1) << "," << INT(2) 
    << "," << INT(3) << ") = ";
#endif

IO(4) = static_cast<SimValue>(write(INT(1), data.c_str(), INT(3)));

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << INT(4) << std::endl;
#endif

    RETURN_READY;
END_TRIGGER
END_OPERATION(WRITE_EMU)


OPERATION(READ_EMU)
TRIGGER

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << "read(" << INT(1) << "," << INT(2) 
    << "," << INT(3) << ") = ";
#endif


char* buf = new char[INT(3)];

IO(4) = static_cast<SimValue>(read(INT(1), buf, INT(3)));

// write the data to memory
for (unsigned i = 0; i < UINT(4); ++i) {
    MEMORY.write(INT(2) + i, 1, (UIntWord)buf[i]);
}

delete[] buf;
buf = NULL;

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << INT(4) << std::endl;
#endif

    RETURN_READY;
END_TRIGGER
END_OPERATION(READ_EMU)

OPERATION(LSEEK_EMU)
TRIGGER

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << "lseek(" << INT(1) << "," << INT(2) 
    << "," << INT(3) << ") = ";
#endif

    IO(4) = (unsigned)lseek(INT(1), INT(2), INT(3));

#ifdef DEBUG_FILE_IO_EMU
Application::logStream() << INT(4) << std::endl;
#endif

RETURN_READY;
END_TRIGGER
END_OPERATION(LSEEK_EMU)
