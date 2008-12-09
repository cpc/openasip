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
 * @file BEMSerializerTest.hh
 *
 * A test suite for BEMSerializer.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef BEMSerializerTest_HH
#define BEMSerializerTest_HH

#include <string>

#include <TestSuite.h>
#include "BEMSerializer.hh"
#include "BinaryEncoding.hh"
#include "MoveSlot.hh"
#include "ImmediateControlField.hh"
#include "LImmDstRegisterField.hh"
#include "GuardField.hh"
#include "GPRGuardEncoding.hh"
#include "FUGuardEncoding.hh"
#include "BridgeEncoding.hh"
#include "ImmediateEncoding.hh"
#include "SocketEncoding.hh"
#include "NOPEncoding.hh"
#include "SourceField.hh"
#include "DestinationField.hh"
#include "SocketCodeTable.hh"
#include "FUPortCode.hh"
#include "RFPortCode.hh"
#include "IUPortCode.hh"

using std::string;

const string DST_FILE = "./data/testbem.bem";

/**
 * Test suite for testing BEMSerializer class.
 */
class BEMSerializerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testWriteAndRead();
    
private:
    BinaryEncoding* bem_;
};


/**
 * Called before each test.
 */
void
BEMSerializerTest::setUp() {
    bem_ = new BinaryEncoding();
}


/**
 * Called after each test.
 */
void
BEMSerializerTest::tearDown() {
    delete bem_;
}


/**
 * Tests writing a binary encoding map to a file and reading it.
 */
void
BEMSerializerTest::testWriteAndRead() {

    const string bus1 = "bus1";
    const string bus2 = "bus2";
    const string bus3 = "bus3";

    const string rf1 = "rf1";
    const string fu1 = "fu1";
    const string iu1 = "iu1";
    const string iu2 = "iu2";
    const string port1 = "port1";
    const string port2 = "port2";

    const string bridge1 = "br1";
    const string bridge2 = "br2";

    const string socket1 = "socket1";
    const string socket2 = "socket2";

    const string scTableName = "table1";
    const string add = "add";

    const string iTemp1 = "iTemp1";

    MoveSlot* slot1 = new MoveSlot(bus1, *bem_);
    MoveSlot* slot2 = new MoveSlot(bus2, *bem_);
    MoveSlot* slot3 = new MoveSlot(bus3, *bem_);
    LImmDstRegisterField* dstRegField1 = new LImmDstRegisterField(3, *bem_);
    LImmDstRegisterField* dstRegField2 = new LImmDstRegisterField(2, *bem_);
    ImmediateControlField* icField = new ImmediateControlField(*bem_);

    GuardField* gField1 = new GuardField(*slot1);
    new GPRGuardEncoding(rf1, 0, false, 0, *gField1);
    new FUGuardEncoding(fu1, port1, true, 1, *gField1);
    SourceField* srcField1 = new SourceField(BinaryEncoding::LEFT, *slot1);
    new BridgeEncoding(bridge1, 0, 2, *srcField1);
    new BridgeEncoding(bridge2, 1, 2, *srcField1);
    SocketEncoding* sEnc1 = new SocketEncoding(socket1, 2, 1, *srcField1);
    new SocketEncoding(socket2, 3, 1, *srcField1);
    new ImmediateEncoding(2, 0, 8, *srcField1);
    new NOPEncoding(3, 0, *srcField1);

    SocketCodeTable* table = new SocketCodeTable(scTableName, *bem_);

    new FUPortCode(fu1, port1, 24, 0, *table);
    new FUPortCode(fu1, port2, add, 25, 0, *table);
    new RFPortCode(rf1, 2, 0, 3, *table);
    new IUPortCode(iu1, 0, 0, 4, *table);

    sEnc1->setSocketCodes(*table);

    DestinationField* dstField1 = new DestinationField(
        BinaryEncoding::LEFT, *slot1);
    new NOPEncoding(0, 0, *dstField1);

    SourceField* srcField2 = new SourceField(BinaryEncoding::LEFT, *slot2);
    new SocketEncoding(socket1, 0, 0, *srcField2);
    SourceField* srcField3 = new SourceField(BinaryEncoding::LEFT, *slot3);
    new SocketEncoding(socket2, 1, 0, *srcField3);

    dstRegField1->addDestination(iTemp1, iu1);
    dstRegField2->addDestination(iTemp1, iu2);

    icField->addTemplateEncoding(iTemp1, 1);

    BEMSerializer serializer;
    serializer.setDestinationFile(DST_FILE);
    serializer.writeBinaryEncoding(*bem_);

    serializer.setSourceFile(DST_FILE);
    BinaryEncoding* readBEM = serializer.readBinaryEncoding();

    TS_ASSERT(readBEM->hasMoveSlot(bus1));
    TS_ASSERT(readBEM->hasMoveSlot(bus2));
    TS_ASSERT(readBEM->hasMoveSlot(bus3));
    TS_ASSERT(readBEM->longImmDstRegisterFieldCount() == 2);
    TS_ASSERT(readBEM->hasImmediateControlField());
    
    MoveSlot& readSlot1 = readBEM->moveSlot(bus1);
    MoveSlot& readSlot2 = readBEM->moveSlot(bus2);
    MoveSlot& readSlot3 = readBEM->moveSlot(bus3);
    LImmDstRegisterField& readDstRegField1 = 
        readBEM->longImmDstRegisterField(0);
    ImmediateControlField& readICField = readBEM->immediateControlField();
    
    TS_ASSERT(readSlot1.relativePosition() == 0);
    TS_ASSERT(readSlot2.relativePosition() == 1);
    TS_ASSERT(readSlot3.relativePosition() == 2);
    TS_ASSERT(readDstRegField1.relativePosition() == 3);
    TS_ASSERT(readICField.relativePosition() == 5);

    DestinationField& readDstField1 = readSlot1.destinationField();
    TS_ASSERT(readDstField1.hasNoOperationEncoding());
    NOPEncoding& nopEncoding = readDstField1.noOperationEncoding();
    TS_ASSERT(nopEncoding.encoding() == 0);
    SourceField& readSrcField1 = readSlot1.sourceField();
    TS_ASSERT(readSrcField1.componentIDPosition() == BinaryEncoding::LEFT);
    TS_ASSERT(readSrcField1.bridgeEncodingCount() == 2);
    SocketEncoding& readSocketEnc1 = readSrcField1.socketEncoding(socket1);
    TS_ASSERT(readSocketEnc1.socketCodes().name() == scTableName);
    TS_ASSERT(readSocketEnc1.encoding() == 2);
    TS_ASSERT(readSocketEnc1.extraBits() == 1);

    TS_ASSERT(readDstRegField1.usedByInstructionTemplate(iTemp1));
    TS_ASSERT(
        readDstRegField1.immediateUnit(iTemp1) == iu1 ||
        readDstRegField1.immediateUnit(iTemp1) == iu2);

    delete readBEM;
}
    
#endif
