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
 * @file BEMTester.cc
 *
 * Implementation of BEMTester class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <algorithm>

#include "BEMTester.hh"
#include "SlotField.hh"
#include "SocketEncoding.hh"
#include "SourceField.hh"
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

    unsigned int encodingWidth = MathTools::requiredBits(encoding) + 
        extraBits;

    int socketEncodings = field.socketEncodingCount();
    for (int i = 0; i < socketEncodings; i++) {
	SocketEncoding& existingEnc = field.socketEncoding(i);
        int alignment = calculateAlignment(
            encodingWidth, existingEnc.socketIDWidth(), field);
        int commonBits =
	    commonBitCount(
		encoding, extraBits, existingEnc.encoding(),
		existingEnc.extraBits(), alignment);
	if (commonBits == static_cast<int>(MathTools::requiredBits(encoding))
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
	    int commonBits =
		commonBitCount(
		    encoding, extraBits, existingEnc.encoding(),
		    existingEnc.extraBits(), alignment);
	    if (commonBits == static_cast<int>(
                    MathTools::requiredBits(encoding)) + 
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
                    MathTools::requiredBits(encoding)) + 
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
                    MathTools::requiredBits(encoding)) +
                static_cast<int>(extraBits) || 
                commonBits == nopEnc.width()) {
                return false;
            }
        }

    }

    return true;
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

    int encodingWidth = MathTools::requiredBits(encoding) + extraBits;

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

    int maxEnc1Offset = MathTools::requiredBits(enc1) + extraBits1 - 1;
    int maxEnc2Offset = MathTools::requiredBits(enc2) + extraBits2 - 1;

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
 * Calculates the aligment of encodings that has the given widths when they
 * are added to the given slot field.
 *
 * @param enc1Width Width of the first encoding.
 * @param enc2Width Width of the second encoding.
 * @param field The slot field.
 * @return The aligment.
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
