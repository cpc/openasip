/*
    Copyright (c) 2002-2016 Tampere University.

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
 * @file MemWriteCommand.cc
 *
 * Implementation of MemWriteCommand class
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @author Alex Hirvonen 2016 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#include "MemWriteCommand.hh"
#include "SimulatorFrontend.hh"
#include "MemorySystem.hh"
#include "SimulatorToolbox.hh"
#include "Memory.hh"
#include "StringTools.hh"
#include "Address.hh"
#include "NullAddressSpace.hh"
#include "SimValue.hh"
#include "Conversion.hh"
#include "FileSystem.hh"
#include <iostream>
#include <fstream>


MemWriteCommand::MemWriteCommand() : 
    SimControlLanguageCommand("load_data") {
}


MemWriteCommand::~MemWriteCommand() {
}

/**
 * Executes the "load_data" command.
 *
 * This low-level command reads binary data from file and loads it to memory
 * starting at specified address.
 *
 * @param arguments Memory address to be written, file to read from, read size.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool
MemWriteCommand::execute(const std::vector<DataObject>& arguments) {
    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 2, 5)) {
        return false;
    } 

    if (!checkProgramLoaded()) {
        return false;
    }

    size_t nextArg = 1;
    size_t writeAddress;
    std::string addressSpaceName = "";

    if (StringTools::ciEqual(arguments.at(nextArg).stringValue(), "/a")) {
        addressSpaceName = arguments.at(2).stringValue();
        nextArg = 3;
    }

    const std::string addressString = arguments.at(nextArg).stringValue();
    if (!setMemoryAddress(addressString, addressSpaceName, writeAddress)) {
        return false;
    }

    std::string fileName = arguments.at(nextArg + 1).stringValue();
    if (!FileSystem::fileExists(fileName) ||
        !FileSystem::fileIsReadable(fileName)) {
        interpreter()->setResult("Error reading input file.");
        interpreter()->setError(true);
        return false;
    }

    size_t readSize = FileSystem::sizeInBytes(fileName);
    if (argumentCount == 3 || argumentCount == 5) {
        readSize = arguments.at(nextArg + 2).integerValue();
    }

    MemorySystem::MemoryPtr memory;
    if (!setMemoryPointer(memory, addressSpaceName)) {
        return false;
    }

    std::ifstream inputFile(fileName.c_str(), std::ios::binary);
    char dataByte;

    while (inputFile.get(dataByte) && readSize > 0) {
        try {
            memory->write(writeAddress, 1, (UIntWord)dataByte);
        } catch (const OutOfRange&) {
            interpreter()->setResult(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_ADDRESS_OUT_OF_RANGE).str());
            interpreter()->setError(true);
            return false;        
        }
        writeAddress++;
        readSize--;
    }
    memory->advanceClock();
    inputFile.close();

    return true;
}

std::string 
MemWriteCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_LOADDATA).str();
}
