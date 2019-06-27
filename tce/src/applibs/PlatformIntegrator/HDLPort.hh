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
 * @file HDLPort.hh
 *
 * Declaration of HDLPort class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_HDL_PORT_HH
#define TTA_HDL_PORT_HH

#include "TCEString.hh"
#include "ProGeTypes.hh"
#include "HDBTypes.hh"
#include "NetlistPort.hh"

namespace ProGe {
    class NetlistBlock;
}


/**
 * Represents a HDL port which is on higher abstraction level than NetlistPort
 */
class HDLPort {
public:

    HDLPort(
        const TCEString name,
        const TCEString& widthFormula,
        ProGe::DataType type,
        HDB::Direction direction,
        bool needsInversion);

    HDLPort(
        const TCEString name,
        const TCEString& widthFormula,
        ProGe::DataType type,
        HDB::Direction direction,
        bool needsInversion,
        int width);

    HDLPort(const HDLPort& old);

    HDLPort(const ProGe::NetlistPort& port);

    TCEString name() const;

    TCEString widthFormula() const;

    void updateWidthFormula(const TCEString& widthFormula);

    bool hasRealWidth() const;

    int realWidth() const;

    HDB::Direction direction() const;

    ProGe::DataType type() const;

    bool needsInversion() const;

    ProGe::NetlistPort* convertToNetlistPort(ProGe::NetlistBlock& block) const;

    void setToStatic(ProGe::StaticSignal value);

    bool hasStaticValue() const;

    ProGe::StaticSignal staticValue() const;

private:

    TCEString name_;
    TCEString widthFormula_;
    ProGe::DataType type_;
    HDB::Direction direction_;
    bool needsInversion_;
    bool hasWidth_;
    int width_;
    bool hasStaticValue_;
    ProGe::StaticSignal staticValue_;

};

#endif
