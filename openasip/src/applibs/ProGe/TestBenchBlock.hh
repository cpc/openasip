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
 * @file TestBenchBlock.hh
 *
 * Declaration of TestBenchBlock class.
 *
 * Created on: 7.9.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TESTBENCHBLOCK_HH
#define TESTBENCHBLOCK_HH

#include "BaseNetlistBlock.hh"

#include "Exception.hh"

namespace ProGe {

class ProGeContext;
class ProcessorWrapperBlock;

/*
 * Block that generates testbench template for TTA cores to run
 * programs in RTL simulations.
 */
class TestBenchBlock: public BaseNetlistBlock {
public:
    TestBenchBlock() = delete;
    TestBenchBlock(
        const ProGeContext& context, const BaseNetlistBlock& coreBlock);
    virtual ~TestBenchBlock();

    virtual void write(
        const Path& targetBaseDir, HDL targetLang = VHDL) const override;

private:

    /// The ProGe context for additional information.
    const ProGeContext& context_;
    /// The block that wraps the processor and instantiates memories for the
    /// GCU and LSUs
    ProcessorWrapperBlock* proc_;
};

} /* namespace ProGe */

#endif /* TESTBENCHBLOCK_HH */
