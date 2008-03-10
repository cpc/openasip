/**
 * @file InterPassDatum.hh
 *
 * Declaration of InterPassDatum.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INTER_PASS_DATUM_HH
#define TTA_INTER_PASS_DATUM_HH

#include <map>
#include <string>

class InterPassDatum;

/** 
 * A base class for classes storing inter-pass data of any type.
 */
class InterPassDatum {
public:
    virtual ~InterPassDatum() {};

protected:
    // no direct instantiation assumed
    InterPassDatum() {}
};

/** 
 * A templated class for classes storing inter-pass data of any type.
 *
 * Inherits the interface of the given class and the InterPassDatum, thus
 * acts like the given class, but can be stored as inter-pass data. Should
 * make creating new InterPassDatum types easier.
 */
template <typename T>
class SimpleInterPassDatum : public InterPassDatum, public T {
public:
    SimpleInterPassDatum() : InterPassDatum(), T() {}
    virtual ~SimpleInterPassDatum() {};
};

#endif
