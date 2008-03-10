/** 
 * @file SegmentTest.hh 
 *
 * A test suite for Segment.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEGMENT_TEST_HH
#define TTA_SEGMENT_TEST_HH

#include <string>

#include <TestSuite.h>

#include "Segment.hh"
#include "Bus.hh"
#include "Exception.hh"
#include "Machine.hh"
#include "Socket.hh"
#include "ObjectState.hh"

using std::string;
using namespace TTAMachine;

// constants used in tests
const string SEGMENT_NAME = "segmentName";
const string BUS_NAME = "busName";
const string BUS_2_NAME = "bus2Name";
const string SOCKET_NAME = "socketName";


/**
 * Test suite for testing functionality of Segment class.
 */
class SegmentTest : public CxxTest::TestSuite {

public:
    void setUp();
    void tearDown();

    void testConstructorAndDestructor();
    void testSetName();
    void testAttachAndDetachSocket();
    void testSegmentOrderModification();
    void testLoadState();

private:
    /// A segment used in tests.
    Segment* segment_;
    /// A bus used in tests.
    Bus* bus_;
    /// A socket used in tests.
    Socket* socket_;
    /// The machine used in tests.
    Machine* machine_;
};


/**
 * Called before each test.
 *
 * Initializes the components used in tests. The segment is added to the bus
 * and the bus is added to the machine. The socket is added to the machine.
 */
void
SegmentTest::setUp() {
    bus_ = new Bus(BUS_NAME, 16, 16, Machine::SIGN);
    segment_ = new Segment(SEGMENT_NAME, *bus_);
    machine_ = new Machine();
    machine_->addBus(*bus_);
    socket_ = new Socket(SOCKET_NAME);
    machine_->addSocket(*socket_);
}


/**
 * Called after each test.
 *
 * If needed, deletes the components used in tests.
 */
void
SegmentTest::tearDown() {
    if (bus_ != NULL) {
        delete bus_;
        bus_ = NULL;
    }
    if (socket_ != NULL) {
        delete socket_;
        socket_ = NULL;
    }
    if (machine_ != NULL) {
        delete machine_;
        machine_ = NULL;
    }
}


/**
 * Tests the constructor of the Segment class.
 */
void
SegmentTest::testConstructorAndDestructor() {

    TS_ASSERT(bus_->segment(SEGMENT_NAME) == segment_);

    Segment* segment2 = NULL;
    TS_ASSERT_THROWS(
        segment2 = new Segment(SEGMENT_NAME, *bus_), ComponentAlreadyExists);

    delete segment_;
    segment_ = NULL;
    TS_ASSERT(bus_->hasSegment(SEGMENT_NAME) == false);

    delete segment2;
    segment2 = NULL;
}


/**
 * Tests setting the name of the segment.
 */
void
SegmentTest::testSetName() {

    TS_ASSERT_EQUALS(segment_->name(), SEGMENT_NAME);

    segment_->setName(SEGMENT_NAME);
    TS_ASSERT_EQUALS(segment_->name(), SEGMENT_NAME);

    const string SEGMENT_NAME_2 = "someName";

    Segment* segment2 = new Segment(SEGMENT_NAME_2, *bus_);
    TS_ASSERT_THROWS(segment2->setName(SEGMENT_NAME), ComponentAlreadyExists);

    delete segment2;
    segment2 = NULL;

    TS_ASSERT_THROWS_NOTHING(segment_->setName(SEGMENT_NAME_2));
    TS_ASSERT_EQUALS(segment_->name(), SEGMENT_NAME_2);
}


/**
 * Tests attaching and detaching a socket.
 */
void
SegmentTest::testAttachAndDetachSocket() {

    segment_->attachSocket(*socket_);
    TS_ASSERT(segment_->isConnectedTo(*socket_) == true);
    TS_ASSERT_THROWS(segment_->attachSocket(*socket_), IllegalConnectivity);

    segment_->detachSocket(*socket_);
    TS_ASSERT(segment_->isConnectedTo(*socket_) == false);
    TS_ASSERT_THROWS(segment_->detachSocket(*socket_), InstanceNotFound);

    segment_->attachSocket(*socket_);
    delete socket_;
    socket_ = NULL;
    TS_ASSERT(segment_->isConnectedTo(*socket_) == false);
}


/**
 * Tests the functionality when a segment is deleted or the order is changed.
 */
void
SegmentTest::testSegmentOrderModification() {

    const string SEGMENT_NAME_2 = "segment2";
    const string SEGMENT_NAME_3 = "segment3";

    Segment* segment2 = new Segment(SEGMENT_NAME_2, *bus_);
    Segment* segment3 = new Segment(SEGMENT_NAME_3, *bus_);

    TS_ASSERT(segment_->destinationSegment() == segment2);
    TS_ASSERT(segment_->sourceSegment() == NULL);
    TS_ASSERT(segment2->destinationSegment() == segment3);
    TS_ASSERT(segment2->sourceSegment() == segment_);
    TS_ASSERT(segment3->destinationSegment() == NULL);
    TS_ASSERT(segment3->sourceSegment() == segment2);

    segment_->moveAfter(*segment2);
    TS_ASSERT(segment_->destinationSegment() == segment3);
    TS_ASSERT(segment_->sourceSegment() == segment2);
    TS_ASSERT(segment2->sourceSegment() == NULL);
    TS_ASSERT(segment2->destinationSegment() == segment_);
    TS_ASSERT(segment3->sourceSegment() == segment_);

    segment3->moveBefore(*segment2);
    TS_ASSERT(segment_->sourceSegment() == segment2);
    TS_ASSERT(segment_->destinationSegment() == NULL);
    TS_ASSERT(segment2->sourceSegment() == segment3);
    TS_ASSERT(segment2->destinationSegment() == segment_);
    TS_ASSERT(segment3->sourceSegment() == NULL);
    TS_ASSERT(segment3->destinationSegment() == segment2);

    const string SEGMENT_NAME_4 = "segment4";
    Bus* bus2 = new Bus("bus2", 1, 1, Machine::SIGN);
    Segment* segment4 = new Segment(SEGMENT_NAME_4, *bus2);

    TS_ASSERT_THROWS(segment_->moveBefore(*segment4), IllegalRegistration);
    TS_ASSERT_THROWS(segment_->moveAfter(*segment4), IllegalRegistration);

    delete segment4;
    segment4 = NULL;
    delete bus2;
    bus2= NULL;

    TS_ASSERT(bus_->hasSegment(SEGMENT_NAME));
    TS_ASSERT(bus_->hasSegment(SEGMENT_NAME_2));
    TS_ASSERT(bus_->hasSegment(SEGMENT_NAME_3));

    delete segment2;
    segment2 = NULL;

    TS_ASSERT(segment_->sourceSegment() == segment3);
    TS_ASSERT(segment_->destinationSegment() == NULL);
    TS_ASSERT(segment3->sourceSegment() == NULL);
    TS_ASSERT(segment3->destinationSegment() == segment_);

    delete segment3;
    segment3 = NULL;

    TS_ASSERT(segment_->hasSourceSegment() == false);
    TS_ASSERT(segment_->hasDestinationSegment() == false);
}


/**
 * Tests constructing the segment from an ObjectState object.
 */
void
SegmentTest::testLoadState() {

    ObjectState* state = bus_->saveState();
    TS_ASSERT_THROWS(
        segment_->loadState(state), ObjectStateLoadingException);

    ObjectState* state2 = segment_->saveState();

    Segment* segment2 = new Segment("someName", *bus_);
    TS_ASSERT_THROWS(
        segment2->loadState(state2), ObjectStateLoadingException);
    
    ObjectState* state3 = segment2->saveState();
    string newName = "newName";
    state3->setAttribute(Segment::OSKEY_NAME, newName);
    TS_ASSERT_THROWS_NOTHING(segment2->loadState(state3));
    TS_ASSERT(segment2->name() == newName);

    delete state;
    delete state2;
    delete state3;
}


#endif
