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
 * @file BEMTester.hh
 *
 * Declaration of BEMTester class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_TESTER_HH
#define TTA_BEM_TESTER_HH

#include "BinaryEncoding.hh"

class SlotField;
class SocketEncoding;

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
};

#endif
