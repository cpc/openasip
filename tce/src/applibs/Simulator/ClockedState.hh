/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file ClockedState.hh
 *
 * Declaration of ClockedState class.
 *
 * @author Jussi Nykänen 2004,2010
 * @note rating: red
 */

#ifndef TTA_CLOCKED_STATE_HH
#define TTA_CLOCKED_STATE_HH

/**
 * Interface implemented by state classes that need to update their internal
 * state whenever elapsing of a processor clock cycle is simulated.
 */
class ClockedState {
public:
    ClockedState();
    virtual ~ClockedState();

    virtual void endClock() = 0;
    virtual void advanceClock() = 0;
    /// this is called at (re)initialization of the simulation
    virtual void reset() {}

private:
    /// Copying not allowed.
    ClockedState(const ClockedState&);
    /// Assignment not allowed.
    ClockedState& operator=(const ClockedState&);
};

#endif
