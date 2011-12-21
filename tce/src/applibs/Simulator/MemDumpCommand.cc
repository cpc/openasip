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
 * @file MemDumpCommand.cc
 *
 * Implementation of MemDumpCommand class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MemDumpCommand.hh"
#include "SimulatorFrontend.hh"
#include "MemorySystem.hh"
#include "SimulatorToolbox.hh"
#include "Memory.hh"
#include "StringTools.hh"
#include "Address.hh"
#include "NullAddressSpace.hh"
#include "SimValue.hh"
#include "Conversion.hh"
#include <iostream>
#include <fstream>

/**
 * Constructor.
 *
 * Sets the name of the command to the base class.
 */
MemDumpCommand::MemDumpCommand() : 
    SimControlLanguageCommand("x") {
}

/**
 * Destructor.
 *
 * Does nothing.
 */
MemDumpCommand::~MemDumpCommand() {
}

/**
 * Executes the "x" command.
 *
 * This low-level command prints the data in memory starting at specified 
 * addresses addr.
 *
 * @param arguments The address to print.
 * @return True in case simulation is initialized and arguments are ok.
 * @exception NumberFormatException Is never thrown by this command.
 */
bool 
MemDumpCommand::execute(const std::vector<DataObject>& arguments)
    throw (NumberFormatException) {

    static size_t displayedCount = 1;
    static size_t lastDisplayedAddress = 0;
    static size_t MAUsToDisplay = 1;

    const int argumentCount = arguments.size() - 1;
    if (!checkArgumentCount(argumentCount, 0, 7)) {
        return false;
    } 

    if (!checkProgramLoaded()) {
        return false;
    }

    bool illegalArguments = false;
    size_t newDisplayedCount = displayedCount;
    size_t newMAUCount = MAUsToDisplay;
    size_t newDisplayedAddress = lastDisplayedAddress + MAUsToDisplay;
    std::string addressSpaceName = "";
    std::string fileName = "";
    for (size_t i = 1; i < arguments.size(); ++i) {
        if (StringTools::ciEqual(arguments.at(i).stringValue(), "/n")) {
            if (i == arguments.size() - 1 || 
                !checkUnsignedIntegerArgument(arguments.at(i + 1))) {
                illegalArguments = true;
                break;
            }
            newDisplayedCount = 
                static_cast<size_t>(arguments.at(i + 1).integerValue());
            ++i;
        } else if (StringTools::ciEqual(arguments.at(i).stringValue(), "/u")) {
            if (i == arguments.size() - 1) {
                illegalArguments = true;
                break;
            }
            const std::string size = arguments.at(i + 1).stringValue();
            if (StringTools::ciEqual(size, "b")) {
                newMAUCount = 1;
            } else if (StringTools::ciEqual(size, "h")) {
                newMAUCount = 2;
            } else if (StringTools::ciEqual(size, "w")) {
                newMAUCount = 4;
            } else {
                illegalArguments = true;
                break;
            }
            ++i;
        } else if (StringTools::ciEqual(arguments.at(i).stringValue(), "/a")) {
            if (i == arguments.size() - 1) {
                illegalArguments = true;
                break;
            }
            addressSpaceName = arguments.at(i + 1).stringValue();
            ++i;
        } else if (StringTools::ciEqual(arguments.at(i).stringValue(), "/f")) {
            if (i == arguments.size() - 1) {
                illegalArguments = true;
                break;
            }
            fileName = arguments.at(i + 1).stringValue();
            ++i;
        } else if (i == arguments.size() - 1) {
            const std::string addressString = arguments.at(i).stringValue();
            try {
                const TTAProgram::Address& parsedAddress =
                    parseDataAddressExpression(addressString);
                if (&parsedAddress.space() != 
                    &TTAMachine::NullAddressSpace::instance()) {
                    addressSpaceName = parsedAddress.space().name();
                }
                newDisplayedAddress = parsedAddress.location();
            } catch(const IllegalParameters& n) {
                return false;
            }
        } else {
            illegalArguments = true;
            break;
        }
    }

    if (illegalArguments) {
        interpreter()->setResult(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_ILLEGAL_ARGUMENTS).str());
        interpreter()->setError(true);
        return false;
    }

    displayedCount = newDisplayedCount;
    lastDisplayedAddress = newDisplayedAddress;

    Memory* memory = NULL;

    size_t MAUSize = 8;

    if (simulatorFrontend().memorySystem().memoryCount() < 1) {
        interpreter()->setError(
            SimulatorToolbox::textGenerator().text(
                Texts::TXT_ADDRESS_SPACE_NOT_FOUND).str());
        return false;
    } else if (simulatorFrontend().memorySystem().memoryCount() == 1) {
        MAUSize = 
            simulatorFrontend().memorySystem().addressSpace(0).width();
        memory = &simulatorFrontend().memorySystem().memory(0);
    } else {
        /// must have the address space defined
        if (addressSpaceName == "") {
            interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_NO_ADDRESS_SPACE_GIVEN).str());
            return false;
        }
        try {
            MAUSize = 
                simulatorFrontend().memorySystem().
                addressSpace(addressSpaceName).width();

            memory = 
                &simulatorFrontend().memorySystem().memory(addressSpaceName);
        } catch (const InstanceNotFound&) {
            interpreter()->setError(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_ADDRESS_SPACE_NOT_FOUND).str());
            return false;
        }
    }

    if (MAUSize*newMAUCount > SIMULATOR_MAX_INTWORD_BITWIDTH) {
        interpreter()->setResult(
            (boost::format("Maximum printable integer size %d.") %
             SIMULATOR_MAX_INTWORD_BITWIDTH).str());
        interpreter()->setError(true);
        return false;        
    }

    std::ofstream* out = NULL;
    const bool dumpToFile = fileName != "";
    if (dumpToFile) {
        if (MAUSize != sizeof(char)*8) {
            interpreter()->setResult(
                (boost::format(
                    "Can only dump 8 bit memories to files. The given "
                    "address space is %d.") % MAUSize).str());
            interpreter()->setError(true);
            return false;
        }
        newMAUCount = 1;
        out = new std::ofstream(fileName.c_str(), std::ios::binary);
    }

    MAUsToDisplay = newMAUCount;
    DataObject* result = new DataObject("");
    // read the wanted number (given with /n) of chunks of data to the result 
    while (newDisplayedCount > 0) {

        UIntWord data = 0;
        try {
            memory->read(newDisplayedAddress, MAUsToDisplay, data);
        } catch (const OutOfRange&) {
            interpreter()->setResult(
                SimulatorToolbox::textGenerator().text(
                    Texts::TXT_ADDRESS_OUT_OF_RANGE).str());
            interpreter()->setError(true);
            return false;        
        }

        
        newDisplayedCount--;
        newDisplayedAddress += MAUsToDisplay;

        if (!dumpToFile) {
            const int HEX_DIGITS = MAUSize*newMAUCount/4;
            result->setString(
                result->stringValue() + 
                Conversion::toHexString(data, HEX_DIGITS));

            if (newDisplayedCount > 0) {
                result->setString(result->stringValue() + " ");
            }
        } else {
            *out << (char)data;
        }
    }

    if (dumpToFile) {
        out->close();
        delete out;
        out = NULL;
    }
    interpreter()->setResult(result);
    return true;
}

/**
 * Returns the help text for this command.
 * 
 * Help text is searched from SimulatorTextGenerator.
 *
 * @return The help text.
 */
std::string 
MemDumpCommand::helpText() const {
    return SimulatorToolbox::textGenerator().text(
        Texts::TXT_INTERP_HELP_X).str();
}
