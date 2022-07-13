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
 * @file LoopBufferBlock.hh
 *
 * Declaration of LoopBufferBlock class.
 *
 * Created on: 3.12.2015
 * @author Henry Linjam�ki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef LOOPBUFFERBLOCK_HH_
#define LOOPBUFFERBLOCK_HH_

#include <string>

#include "BaseNetlistBlock.hh"

#include "NetlistPort.hh"
#include "ProGeContext.hh"

#include "Exception.hh"

namespace ProGe {

/*
 * Block that represents loop buffer unit.
 */
class LoopBufferBlock: public BaseNetlistBlock {
public:
    LoopBufferBlock() = delete;
    LoopBufferBlock(
        const ProGeContext& context, BaseNetlistBlock* parent = nullptr);
    virtual ~LoopBufferBlock();

    void setBlockWidthParameter(const std::string value);
    void setBufferSizeParameter(const std::string value);
    void setIterationPortWidthParameter(const std::string value);
    void setCoreIdParameter(const std::string value);
    void setUsageTracingParameter(bool setting);

    const NetlistPort& lockReqPortIn() const;
    const NetlistPort& lockReqPortOut() const;
    const NetlistPort& lockPortIn() const;
    const NetlistPort& lockPortOut() const;
    const NetlistPort& instructionPortIn() const;
    const NetlistPort& instructionPortOut() const;
    const NetlistPort& startPortIn() const;
    const NetlistPort* stopPortIn() const;
    const NetlistPort& loopBodySizePortIn() const;
    const NetlistPort* loopIterationPortIn() const;
    const NetlistPort& lenCntrPortOut() const;
    const NetlistPort& loopFromImemPortOut() const;

    virtual void write(
        const Path& targetBaseDir, HDL targetLang = VHDL) const override;

private:

    NetlistPort* lockReqPortIn_ = nullptr;
    NetlistPort* lockReqPortOut_ = nullptr;
    NetlistPort* lockPortIn_ = nullptr;
    NetlistPort* lockPortOut_ = nullptr;
    NetlistPort* instructionPortIn_ = nullptr;
    NetlistPort* instructionPortOut_ = nullptr;
    NetlistPort* startPortIn_ = nullptr;
    NetlistPort* stopPortIn_ = nullptr;
    NetlistPort* loopBodySizePortIn_ = nullptr;
    NetlistPort* loopIterationPortIn_ = nullptr;
    NetlistPort* lenCntrPortOut_ = nullptr;
    NetlistPort* loopFromImemPortOut_ = nullptr;

    std::string implmenetationFile_ = "";
    std::string entityStr_ = "tta0";
};

} /* namespace ProGe */

#endif /* LOOPBUFFERBLOCK_HH_ */
