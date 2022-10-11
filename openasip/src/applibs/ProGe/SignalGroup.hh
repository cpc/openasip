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
 * @file SignalGroup.hh
 *
 * Implementation/Declaration of SignalGroup class.
 *
 * Created on: 25.5.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef SIGNALGROUP_HH
#define SIGNALGROUP_HH

#include <set>

#include "SignalGroupTypes.hh"
#include "SignalTypes.hh"

namespace ProGe {

/*
 * Represents group of signals presenting usage of a NetlistPortGroup or
 * implementation of interface.
 */
class SignalGroup {
public:
    SignalGroup();

    template <typename... SignalT>
    SignalGroup(SignalGroupType groupType, SignalT... signals)
        : type_(groupType), signals_{signals...} {}

    virtual ~SignalGroup();

    SignalGroupType type() const;
    void addSignalType(SignalType signalType);
    const std::set<SignalType> definedSignalTypes() const;
    bool operator==(SignalGroupType signalGroupType) const {
        return type() == signalGroupType;
    }

private:
    // Allow copy and assignment operators for now.

    /// Signal group type id.
    SignalGroupType type_;
    /// Signals defined by this group
    std::set<SignalType> signals_;
};

} /* namespace ProGe */

#endif /* SIGNALGROUP_HH */
