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
 * @file SinglePortByteMaskSSRAMBlock.hh
 *
 * Declaration of SinglePortByteMaskSSRAMBlock class.
 *
 * Created on: 8.9.2015
 * @author Henry Linjam�ki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef SinglePortByteMaskSSRAMBlock_HH
#define SinglePortByteMaskSSRAMBlock_HH

#include <string>

#include "BaseNetlistBlock.hh"

namespace ProGe {

/*
 * Netlist block of single port synchronous SRAM with byte mask.
 */
class SinglePortByteMaskSSRAMBlock: public BaseNetlistBlock {
public:
    SinglePortByteMaskSSRAMBlock() = delete;
    SinglePortByteMaskSSRAMBlock(
        const std::string& addressWidth,
        const std::string& dataWidth,
        const std::string& initFile,
        bool isForSimulation = true);
    virtual ~SinglePortByteMaskSSRAMBlock();

    void setAccessTraceFile(const std::string filename);
    const NetlistPortGroup& memoryPort() const;

    virtual void write(
        const Path& targetBaseDir, HDL targetLang = VHDL) const override;

private:

    NetlistPortGroup* memoryPortGroup_ = nullptr;

    bool isForSimulation_ = true;
};

} /* namespace ProGe */

#endif /* SinglePortByteMaskSSRAMBlock_HH */
