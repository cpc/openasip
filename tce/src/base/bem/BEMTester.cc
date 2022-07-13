/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file BEMTester.cc
 *
 * Implementation of BEMTester class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <algorithm>

#include "BEMTester.hh"
#include "BEMTools.hh"
#include "SlotField.hh"
#include "MoveSlot.hh"
#include "SocketEncoding.hh"
#include "SourceField.hh"
#include "GuardField.hh"
#include "GPRGuardEncoding.hh"
#include "FUGuardEncoding.hh"
#include "UnconditionalGuardEncoding.hh"
#include "DestinationField.hh"
#include "BridgeEncoding.hh"
#include "ImmediateEncoding.hh"
#include "NOPEncoding.hh"
#include "SocketCodeTable.hh"
#include "FUPortCode.hh"
#include "RFPortCode.hh"
#include "IUPortCode.hh"
#include "MathTools.hh"

/**
 * Tests whether the given encoding can be added to the given slot
 * field.
 *
 * The encoding can not be added to the slot field if some other
 * socket encoding, bridge encoding, immediate encoding or NOP
 * encoding has exactly the same bits in same positions.  For example,
 * if socket (and bridge) ID's are in the left end of the slot field,
 * the encodings '1001' and '10' would conflict, since in both cases
 * the two leftmost bits in the field are '10'.
 *
 * @param field The slot field.
 * @param encoding The encoding.
 * @param extraBits The number of extra bits in the encoding.
 * @return True if the encoding can be added to the slot field, otherwise
 *         false.
 */
bool
BEMTester::canAddComponentEncoding(
    SlotField& field,
    unsigned int encoding,
    unsigned int extraBits) {

    unsigned int encodingWidth = MathTools::bitLength(encoding) + 
        extraBits;

    int socketEncodings = field.socketEncodingCount();
    for (int i = 0; i < socketEncodings; i++) {
        SocketEncoding& existingEnc = field.socketEncoding(i);
        int alignment = calculateAlignment(
            encodingWidth, existingEnc.socketIDWidth(), field);
        int commonBits = commonBitCount(
            encoding, extraBits, existingEnc.encoding(),
            existingEnc.extraBits(), alignment);
        if (commonBits == static_cast<int>(MathTools::bitLength(encoding))
                + static_cast<int>(extraBits) || 
            commonBits == existingEnc.socketIDWidth()) {
            return false;
        }
    }

    SourceField* sField = dynamic_cast<SourceField*>(&field);
    if (sField != NULL) {
        int bridgeEncodings = sField->bridgeEncodingCount();
        for (int i = 0; i < bridgeEncodings; i++) {
            BridgeEncoding& existingEnc = sField->bridgeEncoding(i);
            int alignment = calculateAlignment(
                encodingWidth, existingEnc.width(), field);
            int commonBits = commonBitCount(
                encoding, extraBits, existingEnc.encoding(),
                existingEnc.extraBits(), alignment);
            if (commonBits == static_cast<int>(
                    MathTools::bitLength(encoding)) + 
                    static_cast<int>(extraBits) || 
                commonBits == existingEnc.width()) {
                return false;
            }
        }
        
        if (sField->hasImmediateEncoding()) {
            ImmediateEncoding& immEnc = sField->immediateEncoding();
            int alignment = calculateAlignment(
                encodingWidth, immEnc.width(), field);
            int commonBits = commonBitCount(
                encoding, extraBits, immEnc.encoding(), immEnc.extraBits(),
                alignment);
            if (commonBits == static_cast<int>(
                MathTools::bitLength(encoding)) +
                static_cast<int>(extraBits) || 
                commonBits == immEnc.width()) {
                return false;
            }
        }

        if (sField->hasNoOperationEncoding()) {
            NOPEncoding& nopEnc = sField->noOperationEncoding();
            int alignment = calculateAlignment(
                encodingWidth, nopEnc.width(), field);
            int commonBits = commonBitCount(
                encoding, extraBits, nopEnc.encoding(), nopEnc.extraBits(),
                alignment);
            if (commonBits == static_cast<int>(
                    MathTools::bitLength(encoding)) +
                    static_cast<int>(extraBits) || 
                commonBits == nopEnc.width()) {
                return false;
            }
        }

    }
    return true;
}


/**
 * Tests whether the given priority encoding can be added to the given move
 * slot field.
 *
 * The encoding can be added to the slot if it distinguishable from the other
 * encodings in the slot. The encoding is distinguishable if there is no
 * guard, source and destination field encoding combination that could be
 * misunderstood as the given encoding.
 *
 * The tested encoding is treated as it has higher priority in decoding. That
 * is, is the given encoding is present in the move slot, then any other
 * encoding bits (non-overlapping with the given encoding) in the slot does
 * not have effect.
 *
 * The tested encoding can cross boundaries of guard, source and destination
 * fields, overlapping multiple fields. A par of the encoding (i.e. overlapping
 * some slot field) may be indistinguishable as long as some other parts are
 * distinguishable.
 *
 * @param slot The slot where the encoding could be added.
 * @param encoding The encoding value. Avoid using zero value since its bit
 *                 width is zero too. Instead, use one extrabit denote zero
 *                 value.
 * @param extraBits The extra bits (zeroes) of the the encoding at the left
 *                  side.
 * @param offset The position of the encoding from LSB position of the slot.
 *               Zero means that the given encoding is right aligned in the
 *               slot.
 * @return True if the encoding is distinguishable from the other
 *         encodings and, therefore, can be added to the slot.
 */
bool
BEMTester::canAddComponentPriorityEncoding(
    MoveSlot& slot,
    unsigned int encoding,
    unsigned int extraBits,
    int offset) {

    bool canAdd = false;
    int encWidth = BEMTools::encodingWidth(encoding, extraBits);
    unsigned truncatedEnc;
    unsigned truncatedWidth;
    int offsetInField;

    if (encWidth + offset > slot.width()) {
        // The move NOP encoding can be always added if (part of the) it is
        // located out side of the move slot fields.
        // note: this assumes that the move slot does not have other kinds of
        // move encodings.
        return true;
    }

    if (fieldsOverlap(slot.guardField(), encWidth, offset)) {
        std::tie(truncatedEnc, truncatedWidth, offsetInField) =
            splitEncoding(encoding, encWidth,
                slot.guardField().width(),
                offset-slot.guardField().bitPosition());

        canAdd = canAdd || !conflictsWithGuardEncoding(slot.guardField(),
            truncatedEnc, truncatedWidth, offsetInField);
    }

    if (fieldsOverlap(slot.sourceField(), encWidth, offset)) {
        const SourceField& field = slot.sourceField();
        std::tie(truncatedEnc, truncatedWidth, offsetInField) =
            splitEncoding(encoding, encWidth, field.width(),
                offset-field.bitPosition());

        canAdd = canAdd || !conflictsWithSourceEncodings(
            field, truncatedEnc, truncatedWidth, offsetInField);
    }

    if (fieldsOverlap(slot.destinationField(), encWidth, offset)) {
        const DestinationField& field = slot.destinationField();
        std::tie(truncatedEnc, truncatedWidth, offsetInField) =
            splitEncoding(encoding, encWidth, field.width(),
                offset-field.bitPosition());

        canAdd = canAdd || !conflictsWithDestinationEncodings(
            field, truncatedEnc, truncatedWidth, offsetInField);
    }

    return canAdd;
}


/**
 * Tells whether the given port encoding can be added to the given socket
 * code table.
 *
 * The given encoding is be ambiguous with some other FU port encoding or RF
 * port encoding in the socket code table if all the bits of the encoding are
 * the same with the bits of another encoding in the same position of the
 * field. For example, encodings 1011 and 11 are ambiguous if the LSBs of 
 * the encodings are in the same position.
 *
 * @param table The socket code table.
 * @param encoding The encoding.
 * @param extraBits The number of extra bits in the encoding.
 * @return True if the encoding can be added to the socket code table,
 *         otherwise false.
 */
bool
BEMTester::canAddPortEncoding(
    SocketCodeTable& table,
    unsigned int encoding,
    unsigned int extraBits) {

    int encodingWidth = MathTools::bitLength(encoding) + extraBits;

    int fuPortEncodings = table.fuPortCodeCount();
    for (int i = 0; i < fuPortEncodings; i++) {
        FUPortCode& code = table.fuPortCode(i);
        int commonBits = commonBitCount(
            encoding, extraBits, code.encoding(), code.extraBits(), 
            code.encodingWidth() - encodingWidth);
        if (commonBits == encodingWidth ||
            commonBits == code.encodingWidth()) {
            return false;
        }
    }

    int rfPortEncodings = table.rfPortCodeCount();
    for (int i = 0; i < rfPortEncodings; i++) {
        RFPortCode& code = table.rfPortCode(i);
        int commonBits = commonBitCount(
            encoding, extraBits, code.encoding(), code.extraBits(),
            code.encodingWidth() - encodingWidth);
        if (commonBits == encodingWidth ||
            commonBits == code.encodingWidth()) {
            return false;
        }
    }

    int iuPortEncodings = table.iuPortCodeCount();
    for (int i = 0; i < iuPortEncodings; i++) {
        IUPortCode& code = table.iuPortCode(i);
        int commonBits = commonBitCount(
            encoding, extraBits, code.encoding(), code.extraBits(),
            code.encodingWidth() - encodingWidth);
        if (commonBits == encodingWidth ||
            commonBits == code.encodingWidth()) {
            return false;
        }
    }
    
    return true;
}


/**
 * Tells the number of common bits in the same positions in the given
 * encodings. The LSB bit of the encodings may not be in the same position.
 * That is defined by the alignment parameter.
 *
 * @param enc1 The first encoding.
 * @param extraBits1 The number of extra zero bits in the first encoding.
 * @param enc2 The second encoding.
 * @param extraBits2 The number of extra zero bits in the second encoding.
 * @param alignment The number of bits the second encoding has in the right
 *                  side of the first encoding (negative if the LSB bit of
 *                  the second encoding is on the left side of the LSB bit of
 *                  the first encoding).
 */
int
BEMTester::commonBitCount(
    unsigned int enc1,
    unsigned int extraBits1,
    unsigned int enc2,
    unsigned int extraBits2,
    int alignment) {

    int commonBits(0);

    int enc1Offset = 0;
    int enc2Offset = 0;

    if (alignment > 0) {
        enc2Offset = alignment;
    } else if (alignment < 0) {
        enc1Offset = alignment * (-1);
    }

    int maxEnc1Offset = MathTools::bitLength(enc1) + extraBits1 - 1;
    int maxEnc2Offset = MathTools::bitLength(enc2) + extraBits2 - 1;

    while (enc1Offset <= maxEnc1Offset && enc2Offset <= maxEnc2Offset) {
        if (MathTools::bit(enc1, enc1Offset) == 
            MathTools::bit(enc2, enc2Offset)) {
            commonBits++;
        }
        enc1Offset++;
        enc2Offset++;
    }

    return commonBits;
}


/**
 * Calculates the alignment of encodings that has the given widths when they
 * are added to the given slot field.
 *
 * @param enc1Width Width of the first encoding.
 * @param enc2Width Width of the second encoding.
 * @param field The slot field.
 * @return The alignment.
 */
int
BEMTester::calculateAlignment(
    unsigned int enc1Width, 
    unsigned int enc2Width, 
    const SlotField& field) {

    if (field.componentIDPosition() == BinaryEncoding::RIGHT) {
        return 0;
    } else {
        return enc2Width - enc1Width;
    }
}


/**
 * Checks if a field relatively at position to an instruction field do overlap.
 *
 * @verbatim
 *       |<- instruction field ->|
 *    |<-- width -->|<---- pos --|
 * ------------------------------0
 * => true
 * @endverbatim
 *
 * @param with
 * @param toFieldWidth
 * @param toFieldPos The relative position of the field to the instruction
 *                   field.
 * @return True if the fields do overlap. Otherwise false.
 */
bool
BEMTester::fieldsOverlap(
    const InstructionField& with,
    unsigned int toFieldWidth,
    int toFieldPos) {

    return fieldsOverlap(with.width(), with.bitPosition(),
        toFieldWidth, toFieldPos);
}


/**
 * Checks if the given fields overlaps
 *
 * @verbatim
 *                              |- pos1 -->|
 *                  |<------ width1 ------>|
 *       |<- width2 ->|         |
 *                    |<- pos2 -|
 *                        <- +  0
 *                ->| |<- overlaps => true
 * @endverbatim
 *
 * @param width1 The width of the 1st field.
 * @param pos1 The position of the 1st field.
 * @param width2 The width of the 2nd field.
 * @param pos2 The position of the 2nd field.
 * @return True if there is overlap. Otherwise return false.
 */
bool
BEMTester::fieldsOverlap(
    unsigned width1, int pos1,
    unsigned width2, int pos2) {

    // positive = overlapping width, negative = separation width
    return (std::min(pos1+static_cast<int>(width1),
                     pos2+static_cast<int>(width2)) -
            std::max(pos1, pos2)) > 0;
}


/**
 * Checks that the given encoding and others in the field can be distinguished
 * from each other.
 *
 * @param field The guard field.
 * @param encoding The encoding.
 * @param width The width of the encoding where the encoding value is right
 *              aligned.
 * @param offset The relative position of the encoding to the field's LSB.
 *               Positive shifts the encoding towards MSB and vice versa.
 * @return True if the encoding can not be distinguished from another encoding.
 */
bool
BEMTester::conflictsWithGuardEncoding(
    const GuardField& field,
    unsigned int encoding, unsigned int /*width*/, int offset) {

    for (int i = 0; i < field.gprGuardEncodingCount(); i++) {
        const GPRGuardEncoding& grdEnc = field.gprGuardEncoding(i);

        if (MathTools::bitFieldsEquals(grdEnc.encoding(), 0,
            encoding, offset, field.width())) {
            return true;
        }
    }

    for (int i = 0; i < field.fuGuardEncodingCount(); i++) {
        const FUGuardEncoding& grdEnc = field.fuGuardEncoding(i);

        if (MathTools::bitFieldsEquals(grdEnc.encoding(), 0,
            encoding, offset, field.width())) {
            return true;
        }
    }

    if (field.hasUnconditionalGuardEncoding(false)) {
        const UnconditionalGuardEncoding& grdEnc =
            field.unconditionalGuardEncoding(false);

        if (MathTools::bitFieldsEquals(grdEnc.encoding(), 0,
            encoding, offset, field.width())) {
            return true;
        }
    }

    if (field.hasUnconditionalGuardEncoding(true)) {
        const UnconditionalGuardEncoding& grdEnc =
            field.unconditionalGuardEncoding(true);

        if (MathTools::bitFieldsEquals(grdEnc.encoding(), 0,
            encoding, offset, field.width())) {
            return true;
        }
    }

    return false;
}


/**
 * Checks that the given encoding and others in the field can be distinguished
 * from each other.
 *
 * @param field The source field.
 * @param encoding The encoding.
 * @param width The width of the encoding where the encoding value is right
 *              aligned.
 * @param offset The relative position of the encoding to the field's LSB.
 *               Positive shifts the encoding towards MSB and vice versa.
 * @return True if the encoding can not be distinguished from another encoding.
 */
bool
BEMTester::conflictsWithSourceEncodings(
    const SourceField& field,
    unsigned int encoding, unsigned int width, int offset) {

    unsigned truncatedEncoding;
    unsigned truncatedWidth;
    unsigned truncatedOffset;

    for (int i = 0; i < field.socketEncodingCount(); i++) {
        const SocketEncoding& socketEnc = field.socketEncoding(i);

        bool socketIDOverlap = fieldsOverlap(
            socketEnc.socketIDWidth(), socketEnc.socketIDPosition(),
            width, offset);
        std::tie(truncatedEncoding, truncatedWidth, truncatedOffset) =
            splitEncodingTo(socketEnc, encoding, width, offset);
        bool socketIDMatch = MathTools::bitFieldsEquals(
            socketEnc.encoding(), truncatedOffset,
            truncatedEncoding, 0, truncatedWidth);

        const SocketCodeTable& scTable = socketEnc.socketCodes();
        // Get the part that overlaps with port codes.
        std::tie(truncatedEncoding, truncatedWidth, truncatedOffset) =
            splitEncoding(encoding, width, scTable.width(),
                offset - socketEnc.socketCodePosition());
        bool socketCodeOverlap = truncatedWidth > 0;
        bool socketCodeMatch = conflictsWithSocketTableEncodings(
            scTable, truncatedEncoding, truncatedWidth, truncatedOffset);

        if (socketIDOverlap) {
            if (socketCodeMatch && socketIDMatch && socketCodeMatch) {
                return true;
            } else if (!socketCodeOverlap && socketIDMatch) {
                return true;
            }
        } else if (socketCodeOverlap && socketCodeMatch) {
            return true;
        }
    }

    // check against simm
    if (field.hasImmediateEncoding()) {
        const ImmediateEncoding& simmEnc = field.immediateEncoding();
        std::tie(truncatedEncoding, truncatedWidth, truncatedOffset) =
            splitEncoding(encoding, width, simmEnc.encodingWidth(),
                offset - simmEnc.encodingPosition());

        if (MathTools::bitFieldsEquals(simmEnc.encoding(), truncatedOffset,
            truncatedEncoding, 0, truncatedWidth)) {

            return true;
        }
    }

    return false;
}


/**
 * Checks that the given encoding and others in the field can be distinguished
 * from each other.
 *
 * @param field The destination field.
 * @param encoding The encoding.
 * @param width The width of the encoding where the encoding value is right
 *              aligned.
 * @param offset The relative position of the encoding to the field's LSB.
 *               Positive shifts the encoding towards MSB and vice versa.
 * @return True if the encoding can not be distinguished from another encoding.
 */
bool
BEMTester::conflictsWithDestinationEncodings(
    const DestinationField& field,
    unsigned int encoding, unsigned int width, int offset) {

    unsigned truncatedEncoding;
    unsigned truncatedWidth;
    unsigned truncatedOffset;

    for (int i = 0; i < field.socketEncodingCount(); i++) {
        const SocketEncoding& socketEnc = field.socketEncoding(i);
        std::tie(truncatedEncoding, truncatedWidth, truncatedOffset) =
            splitEncodingTo(socketEnc, encoding, width, offset);

        bool socketIDOverlap = fieldsOverlap(
            socketEnc.socketIDWidth(), socketEnc.socketIDPosition(),
            width, offset);
        bool socketIDMatch = MathTools::bitFieldsEquals(socketEnc.encoding(),
            truncatedOffset, truncatedEncoding, 0, truncatedWidth);

        const SocketCodeTable& scTable = socketEnc.socketCodes();
        // Get the part that overlaps with port codes.
        std::tie(truncatedEncoding, truncatedWidth, truncatedOffset) =
            splitEncoding(encoding, width, scTable.width(),
                offset - socketEnc.socketCodePosition());

        bool socketCodeOverlap = truncatedWidth > 0;
        bool socketCodeMatch = conflictsWithSocketTableEncodings(
            scTable, truncatedEncoding, truncatedWidth, truncatedOffset);

        if (socketIDOverlap) {
            if (socketCodeOverlap && socketIDMatch && socketCodeMatch) {
                return true;
            } else if (!socketCodeOverlap && socketIDMatch) {
                return true;
            }
        } else if (socketCodeOverlap && socketCodeMatch) {
            return true;
        }
    }

    return false;
}


/**
 * Checks that the given encoding and others in the socket table can be
 * distinguished from each other.
 *
 * @param scTable The socket code table.
 * @param encoding The encoding.
 * @param width The width of the encoding where the encoding value is right
 *              aligned.
 * @param offset The relative position of the encoding to the field's LSB.
 *               Positive shifts the encoding towards MSB and vice versa.
 * @return True if the encoding can not be distinguished from another encoding.
 */
bool
BEMTester::conflictsWithSocketTableEncodings(
    const SocketCodeTable& scTable,
    unsigned int encoding, unsigned int width, int offset) {

    // The conflict checking assumes that port code is encoded as:
    //      |<-   width  ->|
    //      |   encoding   |<- (offset > 0) ------------------------|
    // | extrabits | port code encoding | unused bits | index width |
    // |<-               socket code table width                  ->|
    // msb                                                     lsb: 0
    // note: port code encoding is left aligned and index bits are right
    //       aligned, leaving unused bits between them if table width
    //       is wider than the port code width.

    for (int i = 0; i < scTable.portCodeCount(); i++) {
        const PortCode& portCode = scTable.portCode(i);
        // Check port code encoding
        if (portCode.encodingWidth() > 0) {
            // note: extrabits of SocketCodeTable are not considered.
            if (MathTools::bitFieldsEquals(
                encoding, 0, portCode.encoding(),
                portCode.encodingWidth()-width, width)) {
                return true;
            }
        }

        // Check index part
        if (portCode.indexWidth() > 0) {
            if (fieldsOverlap(portCode.indexWidth(), 0, width, offset)) {
                // todo/note: check if suggested encoding lands on unused index
                // (when isMaxIndexSet() == true. Currently the feature is not
                // used anywhere).
                return true;
            }
        }
    }
    return false;
}


/**
 *
 * Splits encoding to the target field.
 *
 * the splitted encoding is returned as tuple where the 1st item is encoding
 * bits leaving out non-overlapping bits, the 2nd item is resulted width
 * (width of overlap) and the 3rd item is position of the resulted encoding
 * within the target field.
 *
 * @verbatim
 *    msb                  lsb
 *     |<- encoding width ->|
 *     |00001010110100100100|
 *                          |<-- offset (>0) --|
 *                  |<-  target width        ->|
 *----------------------------------------------
 * result encoding: |0100100|
 *    result width: |<----->|<-- offset (>0) --|
 * @endverbatim
 */
std::tuple<unsigned, unsigned, int>
BEMTester::splitEncoding(
    unsigned encoding, unsigned encodingWidth,
    unsigned targetWidth, int offsetToTarget) {

    long int resultWidth =
          std::min(static_cast<int>(targetWidth),
                   offsetToTarget+static_cast<int>(encodingWidth))
          - std::max(0, offsetToTarget);

    if (offsetToTarget > static_cast<int>(targetWidth)
        || resultWidth <= 0) {
        return std::make_tuple(0, 0, 0);
    }

    unsigned resultEncoding = encoding;
    int resultOffset = offsetToTarget;

    if (offsetToTarget < 0) {
        assert(-offsetToTarget < 32 && "Invalid shift amount.");
        resultEncoding = resultEncoding >> -offsetToTarget;
        resultOffset = 0;
    }
    assert(resultWidth < 32 && "Invalid shift amount.");
    unsigned mask = ~(~(0u) << resultWidth);
    resultEncoding &= mask;
    return std::make_tuple(resultEncoding, resultWidth, resultOffset);
}


/**
 * Splits the given encoding that overlaps with the SocketEncoding.
 *
 * Returns tuple where the 1st item is encoding bits leaving out
 * non-overlapping bits, the 2nd item is resulted width (width of the overlap)
 * and the 3rd item is position of the resulted encoding within the
 * SocketEncoding.
 */
std::tuple<unsigned, unsigned, int>
BEMTester::splitEncodingTo(
    const SocketEncoding& socketEncoding,
    unsigned encoding, unsigned encodingWidth, int offsetToTarget) {

    return splitEncoding(encoding, encodingWidth,
        socketEncoding.socketIDWidth(),
        offsetToTarget - socketEncoding.socketIDPosition());
}


