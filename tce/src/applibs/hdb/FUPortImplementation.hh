/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file FUPortImplementation.hh
 *
 * Declaration of FUPortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_PORT_IMPLEMENTATION_HH
#define TTA_FU_PORT_IMPLEMENTATION_HH

#include "PortImplementation.hh"
#include "Exception.hh"

namespace HDB {

class FUImplementation;

/**
 * Represents an architectural port of an FU implementation in HDB.
 */
class FUPortImplementation : public PortImplementation {
public:
    FUPortImplementation(
        const std::string& name,
        const std::string& architecturePort,
        const std::string& widthFormula,
        const std::string& loadPort,
        const std::string& guardPort,
        FUImplementation& parent);
    virtual ~FUPortImplementation();

    void setArchitecturePort(const std::string& name);
    std::string architecturePort() const;
    void setGuardPort(const std::string& name);
    std::string guardPort() const;
    void setWidthFormula(const std::string& formula);
    std::string widthFormula() const;

private:
    /// Name of the corresponding port in architecture.
    std::string architecturePort_;
    /// The formula for the width of the port.
    std::string widthFormula_;
    /// Name of the guard port.
    std::string guardPort_;
};
}

#endif
