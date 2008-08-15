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
 * @file DumpTPEF.cc
 *
 * Program for outputting TPEF hierarchy in textual format.
 *
 * @author Mikael Lepistö 2005 (tmlepist@tut.fi)
 * @note rating: red
 */

#include <ostream>

#include "Exception.hh"
#include "CmdLineOptions.hh"
#include "BinaryReader.hh"
#include "BinaryStream.hh"
#include "Binary.hh"
#include "TPEFDumper.hh"
#include "config.h"

using TPEF::BinaryReader;
using TPEF::Binary;
using TPEF::BinaryStream;

#if 0
   Those flags that dumptpef support are marked with asterisk.

   objdump
   [-a|---archive-headers]
   [-b bfdname| ---target=bfdname]
   [-C|---demangle[= style] ]
   [-d|---disassemble]
   [-D|---disassemble-all]
   [-z|---disassemble-zeroes]
   [-EB|-EL| ---endian={big | little }]
(*)[-f|---file-headers]
   [---file-start-context]
   [-g|---debugging]
(*)[-h|---section-headers| ---headers] NOTE: -h is replaced with -s
   [-i|---info]
(*)[-j section| ---section=section]
   [-l|---line-numbers]
   [-S|---source]
   [-m machine| ---architecture=machine]
   [-M options| ---disassembler-options=options]
   [-p|---private-headers]
(*)[-r|---reloc]
   [-R|---dynamic-reloc]
   [-s|---full-contents]
   [-G|---stabs]
(*)[-t|---syms]
   [-T|---dynamic-syms]
   [-x|---all-headers]
   [-w|---wide]
   [---start-address= address]
   [---stop-address= address]
   [---prefix-addresses]
   [--[no-]show-raw-insn]
   [---adjust-vma= offset]
(*)[-V|---version]
(*)[-H|---help]
   objfile...
#endif

/**
 * Commandline options.
 */
class DumperCmdLineOptions : public CmdLineOptions {
public:
    DumperCmdLineOptions() :
        CmdLineOptions("Usage: dumptpef [options] tpeffile") {

        BoolCmdLineOptionParser* fileHeadersFlag =
            new BoolCmdLineOptionParser(
                "file-headers", "Print file headers.", "f");
        addOption(fileHeadersFlag);

        // NOTE: -h was reserved for help
        BoolCmdLineOptionParser* sectionHeadersFlag =
            new BoolCmdLineOptionParser(
                "section-headers", "Print section headers.", "s");
        addOption(sectionHeadersFlag);

        IntegerListCmdLineOptionParser* sectionId =
            new IntegerListCmdLineOptionParser(
                "section",
                "Print elements of section by section index.", "j");
        addOption(sectionId);

        BoolCmdLineOptionParser* relocTablesFlag =
            new BoolCmdLineOptionParser(
                "reloc", "Print relocation tables.", "r");
        addOption(relocTablesFlag);

        BoolCmdLineOptionParser* symbolTablesFlag =
            new BoolCmdLineOptionParser("syms", "Print symbol tables.", "t");
        addOption(symbolTablesFlag);

        BoolCmdLineOptionParser* memoryInfoFlag =
            new BoolCmdLineOptionParser(
                "mem", "Print information about memory usage of reserved sections.", "m");
        addOption(memoryInfoFlag);

        BoolCmdLineOptionParser* logicalInfoFlag =
            new BoolCmdLineOptionParser(
                "logical", "Print only logical information. "
                "Can be used for checking if two files contains same program "
                "and data and connections even if it's in different order in "
                "tpef file.",
                "l");
        addOption(logicalInfoFlag);
    }

    bool printFileHeaders() {
        return findOption("file-headers")->isDefined();
    }

    bool printSectionHeaders() {
        return findOption("section-headers")->isDefined();
    }

    bool printSymbols() {
        return findOption("syms")->isDefined();
    }

    bool printRelocations() {
        return findOption("reloc")->isDefined();
    }

    bool printMemoryInfo() {
        return findOption("mem")->isDefined();
    }

    bool onlyLogicalInfo() {
        return findOption("logical")->isDefined();
    }

    int sectionIdCount() {
        return findOption("section")->listSize();
    }

    int sectionId(int index) {
        return findOption("section")->integer(index);
    }

    void printVersion() const {
        std::cout << "dumptpef - TPEF Dumper " << VERSION << std::endl;
    }
};


/**
 * Calls TPEFDumper with parameters given in commandline.
 */
int main(int argc, char* argv[]) {

    DumperCmdLineOptions options;

    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
	    return 0;
    } catch (IllegalCommandLine& e) {
        std::cerr << "Illegal command line parameters: "
                  << e.errorMessage() << std::endl;

        options.printHelp();
        return 0;
    }

    if (options.numberOfArguments() != 1) {
        std::cerr << "Error: Must give input binary file.\n\n";
        options.printHelp();
        return 0;
    }

    BinaryStream stream(options.argument(1));

    Binary* tpef = NULL;
    try {
        tpef =  BinaryReader::readBinary(stream);
    } catch (UnresolvedReference& e) {
        std::cerr
            << "Misread the input file: " << options.argument(1) << std::endl
            << "error: " << e.errorMessage() << std::endl;
        return 0;
    } catch (const Exception& e) {
        std::cerr
            << "Can't read input file: " << options.argument(1) << std::endl
            << "error: " << e.errorMessage() << std::endl;
        return 0;
    }

    TPEFDumper dumper(*tpef, std::cout);

    dumper.setOnlyLogical(options.onlyLogicalInfo());

    if (options.printFileHeaders()) {
        dumper.fileHeaders();
    }

    if (options.printSectionHeaders()) {
        dumper.sectionHeaders();
    }

    if (options.printMemoryInfo()) {
        dumper.memoryInfo();
    }

    if (options.printSymbols()) {
        dumper.symbolTables();
    }

    if (options.printRelocations()) {
        dumper.relocationTables();
    }

    for (int i = 1; i < options.sectionIdCount() + 1; i++) {
        dumper.section(options.sectionId(i));
    }

    delete tpef;

    return 0;
}
