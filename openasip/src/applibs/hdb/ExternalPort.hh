/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file ExternalPort.hh
 *
 * Declaration of ExternalPort class.
 *
 * @author Henry Linjamäki 2014 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXTERNALPORT_HH_
#define TTA_EXTERNALPORT_HH_

#include <string>
#include <vector>

#include "HDBTypes.hh"
#include "Exception.hh"

namespace HDB {

/**
 * Represents base class for a non-architectural port of an implementation
 * in HDB.
 */
class ExternalPort {
public:
    ExternalPort(
        const std::string& name,
        Direction direction,
        const std::string& widthFormula,
        const std::string& description);
    virtual ~ExternalPort();

    void setName(const std::string& name);
    std::string name() const;
    void setDirection(Direction direction);
    Direction direction() const;
    void setWidthFormula(const std::string& widthFormula);
    std::string widthFormula() const;
    void setDescription(const std::string& description);
    std::string description() const;

    bool setParameterDependency(const std::string& parameter);
    bool unsetParameterDependency(const std::string& parameter);
    int parameterDependencyCount() const;
    std::string parameterDependency(int index) const;

private:
    /// Typedef for string vector.
    typedef std::vector<std::string> ParameterTable;

    /// Name of the port.
    std::string name_;
    /// Direction of the port.
    Direction direction_;
    /// The formula for the width of the port.
    std::string widthFormula_;
    /// Description of the port.
    std::string description_;
    ParameterTable parameterDeps_;
};

}

#endif
