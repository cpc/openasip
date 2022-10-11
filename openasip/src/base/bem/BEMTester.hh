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
 * @file BEMTester.hh
 *
 * Declaration of BEMTester class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_TESTER_HH
#define TTA_BEM_TESTER_HH

#include <tuple>

#include "BinaryEncoding.hh"

class SlotField;
class MoveSlot;
class SocketEncoding;
class GuardField;
class SourceField;
class DestinationField;
class SocketCodeTable;
class PortCode;

/**
 * BEMTester is a helper class which provides useful validity checking methods
 * for the BEM module itself and for other clients.
 */
class BEMTester {
public:
    static bool canAddComponentEncoding(
        SlotField& field,
        unsigned int encoding,
        unsigned int extraBits);
    static bool canAddComponentPriorityEncoding(
        MoveSlot& slot,
        unsigned int encoding,
        unsigned int extraBits,
        int offset = 0);
    static bool canAddPortEncoding(
        SocketCodeTable& table,
        unsigned int encoding,
        unsigned int extraBits);

private:
    static int commonBitCount(
        unsigned int enc1,
        unsigned int extraBits1,
        unsigned int enc2,
        unsigned int extraBits2,
        int alignment);
    static int calculateAlignment(
        unsigned int enc1,
        unsigned int enc2,
        const SlotField& field);
    static bool fieldsOverlap(
        const InstructionField& with,
        unsigned int toFieldWidth,
        int toFieldPos);
    static bool fieldsOverlap(
        unsigned width1, int pos1,
        unsigned width2, int pos2);
    static bool conflictsWithGuardEncoding(
        const GuardField& field,
        unsigned int encoding, unsigned int width, int offset);
    static bool conflictsWithSourceEncodings(
        const SourceField& field,
        unsigned int encoding, unsigned int width, int offset);
    static bool conflictsWithDestinationEncodings(
        const DestinationField& field,
        unsigned int encoding, unsigned int width, int offset);
    static bool conflictsWithSocketTableEncodings(
        const SocketCodeTable& scTable,
        unsigned int encoding, unsigned int width, int offset);
    static std::tuple<unsigned, unsigned, int> splitEncoding(
        unsigned encoding, unsigned encodingWidth,
        unsigned targetWidth, int offsetToTarget);
    static std::tuple<unsigned, unsigned, int> splitEncodingTo(
        const SocketEncoding& socketEncoding,
        unsigned encoding, unsigned encodingWidth, int offsetToTarget);
};

#endif
