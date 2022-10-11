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
 * @file Signal.hh
 *
 * Declaration of Signal class.
 *
 * Created on: 25.5.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef SIGNAL_HH
#define SIGNAL_HH

#include <cstddef>

#include "SignalTypes.hh"

namespace ProGe {

/*
 * Class that holds useful information about NetlistPort's purpose.
 */
class Signal {
public:
    Signal();
    Signal(
        SignalType type,
        ActiveState activeState = ActiveState::HIGH);
    virtual ~Signal();

    SignalType type() const;
    ActiveState activeState() const;
    bool operator==(SignalType type) const {
        return (this->type_ == type);
    }
    bool operator==(const Signal& other) const {
        return (this->type_ == other.type_
            && this->activeState_ == other.activeState_);
    }

private:

    // Allow copy and assignment operators for now.

    /// Signal purpose type id.
    SignalType type_ = SignalType::UNDEFINED;
    /// The active state of the signal
    ActiveState activeState_ = ActiveState::HIGH;
};

} /* namespace ProGe */

#endif /* SIGNAL_HH */
