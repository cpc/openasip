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
 * @file Parameter.hh
 *
 * Declaration of Parameter class.
 *
 * Created on: 20.4.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef PARAMETER_HH
#define PARAMETER_HH

#include <map>
#include <string>

#include "TCEString.hh"
#include "Exception.hh"

namespace ProGe {

class BaseNetlistBlock;

/**
 * Class that represents a parameter for netlist objects such as netlist port
 * and netlist block and is used to define port widths and enable/disable
 * properties of blocks. Parameters are may have two different values: assigned
 * and default where assigned value is real value assigned for the parameter
 * and default value is fallback value in case the assigned have is not
 * defined..
 *
 * From HDL point of view (VHDL, verilog) the assigned value corresponds to
 * the value set for module/entity instantiation's parameter/generic and
 * the default value corresponds to default value for a module's/entity's
 * parameter/generic.
 *
 * Empty value means that the value is not defined.
 */
class Parameter {
public:

    Parameter();
    Parameter(
        const TCEString& name,
        const TCEString& type,
        const TCEString& assignedAndDefaultValue);
    Parameter(
        const TCEString& name,
        const TCEString& type,
        const TCEString& assignedValue,
        const TCEString& defaultValue);
    Parameter(
        const TCEString& name,
        const TCEString& type,
        int assignedValue,
        const TCEString& defaultValue);
    Parameter(
        const TCEString& name,
        const TCEString& type,
        const TCEString& nameOfConstant,
        const TCEString& defaultValue,
        const TCEString& nameOfPackage);
    virtual ~Parameter();

    void set(
        const TCEString& name,
        const TCEString& type,
        const TCEString& value);
    void setName(const TCEString& name);
    void setType(const TCEString& type);
    void setValue(const TCEString& value);

    const TCEString& name() const;
    const TCEString& type() const;
    const TCEString& value() const;
    const TCEString& defaultValue() const;

    bool valueIsSymbol() const;
    bool valueIsConstant() const;
    const std::string& packageNameOfConstant() const;

private:

    /// Name/identifier of the parameter.
    TCEString name_;
    /// Type of the parameter.
    TCEString type_;
    /// Assigned value to the parameter as actual value or as reference to
    /// another parameter or name of a constant.
    TCEString value_;
    /// Default value of the parameter if value_ is not set (empty).
    TCEString default_;
    /// Package reference by name. If non-empty the value_ is treated as
    /// name of a constant.
    TCEString package_;
};

} /* namespace ProGe */

#endif /* PARAMETER_HH */
