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
 * @file ArbiterImplementarion.hh
 *
 * Declaration of ArbiterImplementation class.
 *
 * Created on: 7.9.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef ARBITERIMPLEMENTARION_HH
#define ARBITERIMPLEMENTARION_HH

#include <string>

#include "Exception.hh"
#include "Serializable.hh"

namespace IDF {

/*
 * todo
 */
class ArbiterImplementation: public Serializable {
public:

    enum ArbitrationScheme { ROUND_ROBIN };

    ArbiterImplementation();
    ArbiterImplementation(const ObjectState* state);
    virtual ~ArbiterImplementation();

    ArbitrationScheme arbitrationScheme() const;
    void setArbitrationScheme(ArbitrationScheme scheme);
    unsigned maximumTimeSlice() const;
    void setMaximumTimeSlice(unsigned value);

    void loadState(const ObjectState* state) override;
    ObjectState* saveState() const override;

    /// ObjectState name for this class.
    static const std::string OSNAME_ARBITER_IMPLEMENTATION;
    /// ObjectState name for arbitration scheme.
    static const std::string OSKEY_ARBITRATION_SCHEME;
    /// ObjectState name for maximum time slice.
    static const std::string OSKEY_MAX_TIME_SLICE;

private:
    static ArbitrationScheme unserializeArbitrationScheme(
        const std::string& str);
    static std::string serialize(ArbitrationScheme scheme);

    /// The arbitration scheme.
    ArbitrationScheme arbitrationScheme_ = ROUND_ROBIN;
    /// The maximum period given for processor in time sliced arbitration.
    unsigned maximumTimeSlice_ = 0;

};

} /* namespace ProGe */

#endif /* ARBITERIMPLEMENTARION_HH_ */
