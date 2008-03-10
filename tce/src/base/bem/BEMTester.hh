/**
 * @file BEMTester.hh
 *
 * Declaration of BEMTester class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
