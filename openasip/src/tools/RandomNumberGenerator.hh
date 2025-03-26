/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file RandomNumberGenerator.hh
 *
 * Declaration of RandomNumberGenerator class.
 *
 * Created on: 25.2.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef RANDOMNUMBERGENERATOR_HH
#define RANDOMNUMBERGENERATOR_HH

#include <string>

#include "BaseType.hh"
#include "HalfFloatWord.hh"

/*
 * Pseudo random number generator that aims to be consistent in generating
 * same number sequence across platforms.
 */
class RandomNumberGenerator {
public:

    typedef SIntWord SeedType;
    typedef SeedType RNGValueType;

    static const SeedType DEFAULTSEED;

    RandomNumberGenerator();
    RandomNumberGenerator(SeedType seed);
    virtual ~RandomNumberGenerator();

    RNGValueType operator()();
    void seed(SeedType seed);
    static RNGValueType generateNewSeed();
    static FloatWord asFloat(RNGValueType seed);
    static HalfFloatWord asHalfFloat(RNGValueType seed);
    static SeedType convertToSeed(const std::string& src);

private:

    RNGValueType value_;
};


class RandomDoubleGenerator : public ::RandomNumberGenerator {

public:

    typedef RandomNumberGenerator::SeedType SeedType;
    typedef Double RNGValueType;

    RandomDoubleGenerator();
    RandomDoubleGenerator(SeedType seed);
    RNGValueType operator()();
};


#endif /* RANDOMNUMBERGENERATOR_HH */
