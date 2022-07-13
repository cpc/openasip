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
 * @file GlobalPackage.hh
 *
 * Declaration of GlobalPackage class.
 *
 * Created on: 9.6.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef GLOBALPACKAGE_HH
#define GLOBALPACKAGE_HH

#include <string>

namespace ProGe {

/*
 * Class that represents global package (global_pkg) and holds definitions of
 * core specific constants.
 */
class GlobalPackage {
public:
    GlobalPackage() = delete;
    GlobalPackage(const std::string& entityName);
    virtual ~GlobalPackage();

    const std::string name() const;

    // Predefined constants //
    const std::string fetchBlockAddressWidth() const;
    const std::string fetchBlockDataWidth() const;
    const std::string fetchBlockMAUWidth() const;
    const std::string instructionDataWidth() const;


    // Interface to add constants into this package //

    // Interface to generate package //
    //todo migrate code from ProcessorGenerator
    //todo void write(targetdir, hdl) const;


private:

    /// The entity name
    std::string entityName_;

};

} /* namespace ProGe */

#endif /* GLOBALPACKAGE_HH */
