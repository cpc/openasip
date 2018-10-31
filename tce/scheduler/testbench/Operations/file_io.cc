/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
    ULongWord temp = 0;
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
    ULongWord temp = 0;
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
