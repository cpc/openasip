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
 * @file ViewBEM.cc
 *
 * Implements the viewbem application which prints out human readable 
 * information of given binary encoding map.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>

#include "BEMViewerCmdLineOptions.hh"

#include "BEMSerializer.hh"
#include "BinaryEncoding.hh"
#include "ImmediateControlField.hh"
#include "MoveSlot.hh"
#include "ImmediateSlotField.hh"
#include "LImmDstRegisterField.hh"
#include "GuardField.hh"
#include "DestinationField.hh"
#include "SourceField.hh"
#include "GPRGuardEncoding.hh"
#include "FUGuardEncoding.hh"
#include "UnconditionalGuardEncoding.hh"
#include "ImmediateEncoding.hh"
#include "BridgeEncoding.hh"
#include "NOPEncoding.hh"
#include "SocketEncoding.hh"
#include "SocketCodeTable.hh"
#include "FUPortCode.hh"
#include "RFPortCode.hh"
#include "IUPortCode.hh"

#include "FileSystem.hh"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

static void
printBEMInfo(const BinaryEncoding& bem, const std::string& bemFile);
static void
printInstructionLayout(const BinaryEncoding& bem);
static void
printImmediateControlField(const BinaryEncoding& bem);
static void
printLImmDstRegisterField(const LImmDstRegisterField& field);
static void
printMoveSlot(const MoveSlot& moveSlot);
static void
printMoveSlotLayout(const MoveSlot& moveSlot);
static void
printGuardFieldEncodings(const MoveSlot& moveSlot);
static void
printSlotFieldEncodings(const SlotField& slotField);
static string
portCodeBits(const PortCode& code);
static string
portCodeDescription(const RFPortCode& code);
static string
portCodeDescription(const IUPortCode& code);
static string
portCodeDescription(const FUPortCode& code);
static void
printSourceFieldEncodings(const MoveSlot& moveSlot);
static void
printDestinationFieldEncodings(const MoveSlot& moveSlot);
static void
printSeparator();
static void
printPattern(const std::string& pattern, int times);


/**
 * The main function.
 */
int main(int argc, char* argv[]) {

    BEMViewerCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& e) {
        cerr << e.errorMessage() << endl;
        return EXIT_FAILURE;
    }

    string bemFile = options.bemFile();
    if (bemFile == "") {
        options.printHelp();
        return EXIT_FAILURE;
    }

    BEMSerializer serializer;
    serializer.setSourceFile(bemFile);
    BinaryEncoding* bem;
    try {
        bem = serializer.readBinaryEncoding();
    } catch (const Exception& e) {
        cerr << e.errorMessage() << endl;
        return EXIT_FAILURE;
    }
    
    printBEMInfo(*bem, bemFile);
    delete bem;
    return EXIT_SUCCESS;
}


/**
 * Prints information of BEM to standard output.
 *
 * @param bem The BEM.
 * @param bemFile Name of the BEM file.
 */
static void
printBEMInfo(const BinaryEncoding& bem, const std::string& bemFile) {

    cout << "Binary Encoding Map: " << FileSystem::fileOfPath(bemFile)
         << endl << endl;
    cout << "X = unused bit" << endl;
    cout << "I = immediate bit" << endl;
    cout << "S = socket code bit" << endl;
    cout << "R = register index bit" << endl << endl;

    cout << "Total instruction width: " << bem.width() << endl << endl;

    printInstructionLayout(bem);
    cout << endl;
    printImmediateControlField(bem);
    cout << endl;
    for (int i = 0; i < bem.longImmDstRegisterFieldCount(); i++) {
        LImmDstRegisterField& field = bem.longImmDstRegisterField(i);
        printLImmDstRegisterField(field);
        cout << endl;
    }
    for (int i = 0; i < bem.moveSlotCount(); i++) {
        MoveSlot& slot = bem.moveSlot(i);
        printMoveSlot(slot);
        cout << endl;
    }
}


/**
 * Prints the instruction layout of the given BEM.
 *
 * @param bem The BEM.
 */
static void
printInstructionLayout(const BinaryEncoding& bem) {

    if (bem.childFieldCount() == 0) {
        return;
    }

    cout << "|";
    for (int i = bem.childFieldCount() - 1; i >= 0; i--) {
        InstructionField& field = bem.childField(i);
        ImmediateControlField* icField = 
            dynamic_cast<ImmediateControlField*>(&field);
        MoveSlot* moveSlot = dynamic_cast<MoveSlot*>(&field);
        ImmediateSlotField* immSlot = 
            dynamic_cast<ImmediateSlotField*>(&field);
        LImmDstRegisterField* dstRegField = 
            dynamic_cast<LImmDstRegisterField*>(&field);

        if (icField != NULL) {
            cout << " limm cntrl: " << field.width();
        } else if (moveSlot != NULL) {
            cout << " move slot " << moveSlot->name() << ": "
                 << moveSlot->width();
        } else if (immSlot != NULL) {
            cout << " limm slot " << immSlot->name() << ": "
                 << immSlot->width();
        } else {
            assert(dstRegField != NULL);
            cout << " dst reg field: " << dstRegField->width();
        }
        cout << " |";
    }
    cout << endl;
}


/**
 * Prints information of immediate control field of the given BEM.
 *
 * @param bem The BEM.
 */
static void
printImmediateControlField(const BinaryEncoding& bem) {

    if (!bem.hasImmediateControlField()) {
        return;
    }

    ImmediateControlField& icField = bem.immediateControlField();
    printSeparator();
    cout << "Immediate Control Field" << endl << endl;
    cout << "Position: " << icField.bitPosition() << endl;
    cout << "Width: " << icField.width() << endl;
    cout << "Encodings:" << endl;

    int encodingWidth = icField.width() - icField.extraBits();
    string extraBits;
    for (int i = 0; i < icField.extraBits(); i++) {
        extraBits.append("X");
    }

    for (int i = 0; i < icField.templateCount(); i++) {
        string iTemp = icField.instructionTemplate(i);
        unsigned int encoding = icField.templateEncoding(iTemp);
        cout << extraBits << Conversion::toBinary(encoding, encodingWidth)
             << " : " << iTemp << endl;
    }
}


/**
 * Prints information of the given long immediate destination register field.
 *
 * @param field The field.
 */
static void
printLImmDstRegisterField(const LImmDstRegisterField& field) {

    printSeparator();
    cout << "Long Immediate Destination Register Field" << endl << endl;
    cout << "Position: " << field.bitPosition() << endl;
    cout << "Width: " << field.width() << endl << endl;
    
    cout << "Usage" << endl;
    for (int i = 0; i < field.instructionTemplateCount(); i++) {
        string iTemp = field.instructionTemplate(i);
        string iu = field.immediateUnit(iTemp);
        cout << iTemp << ": " << iu << endl;
    }
}   


/**
 * Prints information of the given move slot.
 *
 * @param moveSlot The move slot.
 */
static void
printMoveSlot(const MoveSlot& moveSlot) {

    printSeparator();
    cout << "Move Slot: " << moveSlot.name() << endl << endl;
    cout << "Position: " << moveSlot.bitPosition() << endl;
    cout << "Width: " << moveSlot.width() << endl << endl;

    printMoveSlotLayout(moveSlot);
    cout << endl;
    printGuardFieldEncodings(moveSlot);
    cout << endl;
    printSourceFieldEncodings(moveSlot);
    cout << endl;
    printDestinationFieldEncodings(moveSlot);
}


/**
 * Prints the layout of the given move slot.
 *
 * @param moveSlot The move slot.
 */
static void
printMoveSlotLayout(const MoveSlot& moveSlot) {
    cout << "|";
    for (int i = moveSlot.childFieldCount() - 1; i >= 0; i--) {
        InstructionField& childField = moveSlot.childField(i);
        GuardField* grdField = dynamic_cast<GuardField*>(&childField);
        SourceField* srcField = dynamic_cast<SourceField*>(&childField);
        DestinationField* dstField = 
            dynamic_cast<DestinationField*>(&childField);
        if (grdField != NULL) {
            cout << " grd field: " << grdField->width();
        } else if (srcField != NULL) {
            cout << " src field: " << srcField->width();
        } else {
            assert(dstField != NULL);
            cout << " dst field: " << dstField->width();
        }
        cout << " |";
    }
    cout << endl;
}


/**
 * Prints the encodings of the guard field of the given move slot.
 *
 * @param moveSlot The move slot.
 */
static void
printGuardFieldEncodings(const MoveSlot& moveSlot) {

    if (!moveSlot.hasGuardField()) {
        return;
    }

    GuardField& grdField = moveSlot.guardField();
    int encodingWidth = grdField.width() - grdField.extraBits();
    string extraBits;
    for (int i = 0; i < grdField.extraBits(); i++) {
        extraBits.append("X");
    }

    cout << "Guard field encodings:" << endl;
    if (grdField.hasUnconditionalGuardEncoding(false)) {
        UnconditionalGuardEncoding& enc = 
            grdField.unconditionalGuardEncoding(false);
        cout << extraBits 
             << Conversion::toBinary(enc.encoding(), encodingWidth)
             << " : always-true" << endl;
    }
    if (grdField.hasUnconditionalGuardEncoding(true)) {
        UnconditionalGuardEncoding& enc = 
            grdField.unconditionalGuardEncoding(true);
        cout << extraBits
             << Conversion::toBinary(enc.encoding(), encodingWidth) 
             << " : always-false" << endl;
    }
    for (int i = 0; i < grdField.gprGuardEncodingCount(); i++) {
        GPRGuardEncoding& enc = grdField.gprGuardEncoding(i);
        cout << extraBits
             << Conversion::toBinary(enc.encoding(), encodingWidth) 
             << " : ";
        if (enc.isGuardInverted()) {
            cout << "inverted ";
        } else {
            cout << "non-inverted ";
        }
        cout << "GPR " << enc.registerIndex() << " of RF "
             << enc.registerFile() << endl;
    }
    for (int i = 0; i < grdField.fuGuardEncodingCount(); i++) {
        FUGuardEncoding& enc = grdField.fuGuardEncoding(i);
        cout << extraBits
             << Conversion::toBinary(enc.encoding(), encodingWidth) << " : ";
        if (enc.isGuardInverted()) {
            cout << "inverted ";
        } else {
            cout << "non-inverted ";
        }
        cout << "port " << enc.port() << " of FU " << enc.functionUnit()
             << endl;
    }
}


/**
 * Prints the socket and NOP encodings of the given slot field.
 *
 * @param slotField The source or destination field.
 */
static void
printSlotFieldEncodings(const SlotField& slotField) {

    if (slotField.hasNoOperationEncoding()) {
        NOPEncoding& enc = slotField.noOperationEncoding();
        if (slotField.componentIDPosition() == BinaryEncoding::RIGHT) {
            printPattern("X", slotField.width() - enc.width());
            cout << Conversion::toBinary(enc.encoding(), enc.width());
        } else {
            printPattern("X", slotField.extraBits());
            cout << Conversion::toBinary(enc.encoding(), enc.width());
            printPattern(
                "X",
                slotField.width() - slotField.extraBits() - enc.width());
        }
        cout << " : NOP" << endl;
    }

    for (int i = 0; i < slotField.socketEncodingCount(); i++) {
        SocketEncoding& enc = slotField.socketEncoding(i);
        printPattern("X", slotField.extraBits());
        if (slotField.componentIDPosition() == BinaryEncoding::RIGHT) {
            if (enc.hasSocketCodes()) {
                SocketCodeTable& scTable = enc.socketCodes();
                printPattern("S", scTable.width());
                printPattern(
                    "X",
                    slotField.width() - slotField.extraBits() - 
                    scTable.width() - enc.socketIDWidth());
            } else {
                printPattern(
                    "X",
                    slotField.width() - slotField.extraBits() -
                    enc.socketIDWidth());
            }
            cout << 
                Conversion::toBinary(enc.encoding(), enc.socketIDWidth());
        } else {
            cout << 
                Conversion::toBinary(enc.encoding(), enc.socketIDWidth());
            if (enc.hasSocketCodes()) {
                SocketCodeTable& scTable = enc.socketCodes();
                printPattern(
                    "X",
                    slotField.width() - slotField.extraBits() -
                    scTable.width() - enc.socketIDWidth());
                printPattern("S", scTable.width());
            } else {
                printPattern(
                    "X",
                    slotField.width() - slotField.extraBits() - enc.width());
            }
        }
        cout << " : socket " << enc.socketName() << endl;

        if (enc.hasSocketCodes()) {
            SocketCodeTable& scTable = enc.socketCodes();
            if (slotField.componentIDPosition() == BinaryEncoding::RIGHT) {
                for (int i = 0; i < scTable.rfPortCodeCount(); i++) {
                    printPattern(" ", slotField.extraBits());
                    RFPortCode& code = scTable.rfPortCode(i);
                    cout << portCodeBits(code);
                    printPattern(" ", enc.width() - scTable.width());
                    cout << " : " << portCodeDescription(code) << endl;
                }
                for (int i = 0; i < scTable.iuPortCodeCount(); i++) {
                    printPattern(" ", slotField.extraBits());
                    IUPortCode& code = scTable.iuPortCode(i);
                    cout << portCodeBits(code);
                    printPattern(" ", enc.width() - scTable.width());
                    cout << " : " << portCodeDescription(code) << endl;
                }
                for (int i = 0; i < scTable.fuPortCodeCount(); i++) {
                    printPattern(" ", slotField.extraBits());
                    FUPortCode& code = scTable.fuPortCode(i);
                    cout << portCodeBits(code);
                    printPattern(" ", enc.width() - scTable.width());
                    cout << " : " << portCodeDescription(code) << endl;
                }
            } else {
                for (int i = 0; i < scTable.rfPortCodeCount(); i++) {
                    printPattern(" ", slotField.width() - scTable.width());
                    RFPortCode& code = scTable.rfPortCode(i);
                    cout << portCodeBits(code);
                    cout << " : " << portCodeDescription(code) << endl;
                }
                for (int i = 0; i < scTable.iuPortCodeCount(); i++) {
                    printPattern(" ", slotField.width() - scTable.width());
                    IUPortCode& code = scTable.iuPortCode(i);
                    cout << portCodeBits(code);
                    cout << " : " << portCodeDescription(code) << endl;
                }
                for (int i = 0; i < scTable.fuPortCodeCount(); i++) {
                    printPattern(" ", slotField.width() - scTable.width());
                    FUPortCode& code = scTable.fuPortCode(i);
                    cout << portCodeBits(code);
                    cout << " : " << portCodeDescription(code) << endl;
                }
            }
        }
    }
}


/**
 * Returns the bit string of the given port code.
 *
 * @param code The port code.
 */
static string
portCodeBits(const PortCode& code) {
    string bits;
    if (code.hasEncoding()) {
        bits += Conversion::toBinary(
            code.encoding(), code.encodingWidth());
    }
    int unusedBits = code.parent()->width() - code.width();
    for (int i = 0; i < unusedBits; i++) {
        bits.append("X");
    }
    for (int i = 0; i < code.indexWidth(); i++) {
        bits.append("R");
    }
    return bits;
}


/**
 * Returns description of the given port code.
 *
 * @param code The port code.
 */
static string
portCodeDescription(const RFPortCode& code) {
    return "RF: " + code.unitName();
}


/**
 * Returns description of the given port code.
 *
 * @param code The port code.
 */
static string
portCodeDescription(const IUPortCode& code) {
    return "IU: " + code.unitName();
}


/**
 * Returns description of the given port code.
 *
 * @param code The port code.
 */
static string
portCodeDescription(const FUPortCode& code) {
    string desc = "FU port: " + code.unitName() + ", " + code.portName();
    if (code.hasOperation()) {
        desc.append(", " + code.operationName());
    }
    return desc;
}


/**
 * Prints the encodings of the source field of the given move slot.
 *
 * @param moveSlot The move slot.
 */
static void
printSourceFieldEncodings(const MoveSlot& moveSlot) {

    if (!moveSlot.hasSourceField()) {
        return;
    }

    SourceField& srcField = moveSlot.sourceField();
    cout << "Source field encodings:" << endl;
    printSlotFieldEncodings(srcField);

    if (srcField.hasImmediateEncoding()) {
        ImmediateEncoding& enc = srcField.immediateEncoding();
        printPattern("X", srcField.extraBits());
        if (srcField.componentIDPosition() == BinaryEncoding::LEFT) {
            cout << Conversion::toBinary(
                enc.encoding(), enc.encodingWidth());
            printPattern(
                "X", srcField.width() - srcField.extraBits() - enc.width());
            printPattern("I", enc.immediateWidth());
        } else {
            printPattern("I", enc.immediateWidth());
            printPattern(
                "X", srcField.width() - srcField.extraBits() - enc.width());
            cout << Conversion::toBinary(
                enc.encoding(), enc.encodingWidth());
        }
        cout << " : short immediate" << endl;
    }

    for (int i = 0; i < srcField.bridgeEncodingCount(); i++) {
        BridgeEncoding& enc = srcField.bridgeEncoding(i);
        printPattern("X", srcField.extraBits());
        if (srcField.componentIDPosition() == BinaryEncoding::RIGHT) {
            printPattern(
                "X", srcField.width() - srcField.extraBits() - enc.width());
            cout << Conversion::toBinary(enc.encoding(), enc.width());
        } else {
            cout << Conversion::toBinary(enc.encoding(), enc.width());
            printPattern(
                "X", srcField.width() - srcField.extraBits() - enc.width());
        }
        cout << " : bridge " << enc.bridgeName() << endl;
    }
}   


/**
 * Prints the encodings of the destination field of the given move slot.
 *
 * @param moveSlot The move slot.
 */
static void
printDestinationFieldEncodings(const MoveSlot& moveSlot) {

    if (!moveSlot.hasDestinationField()) {
        return;
    }

    DestinationField& dstField = moveSlot.destinationField();
    cout << "Destination field encodings:" << endl;
    printSlotFieldEncodings(dstField);
}


/**
 * Prints the separator.
 */
static void
printSeparator() {
    cout << "------------------------------------------------------------" 
         << endl;
}


/**
 * Prints the given pattern the given times.
 *
 * @param pattern The pattern.
 * @param times Times the pattern is written.
 */
static void
printPattern(const std::string& pattern, int times) {
    for (int i = 0; i < times; i++) {
        cout << pattern;
    }
}
