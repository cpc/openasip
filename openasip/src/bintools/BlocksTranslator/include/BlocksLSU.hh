/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file BlocksLSU.hh
 *
 * Declaration of BlocksLSU Class.
 *  This class resembles a single output in the Blocks architecture.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#ifndef BLOCKS_LSU_HH
#define BLOCKS_LSU_HH

#include <memory>

#include "BlocksFU.hh"

class BlocksLSU : public BlocksFU {
public:
    TTAMachine::FunctionUnit* lsu;
    BlocksLSU(
        TTAMachine::Machine& mach, const std::string& name,
        std::list<std::string> sources, TTAMachine::AddressSpace& asData,
        std::shared_ptr<TTAMachine::Socket> in1sock,
        std::shared_ptr<TTAMachine::Socket> in2sock);
    ~BlocksLSU() {
        while (!ops.empty()) {
            delete ops.back();
            ops.pop_back();
        }
    }

private:
    void ConfigurePipeline(
        TTAMachine::ExecutionPipeline* pipeline, bool isLoadOp);
    void BindPorts(TTAMachine::HWOperation* hwOp, bool isLoadOp);
    TTAMachine::HWOperation* CreateHWLoadOp(const std::string& name);
    TTAMachine::HWOperation* CreateHWStoreOp(const std::string& name);
};

/* Declaration of BlocksLSUPair Class. This class is a wrapper for BlocksLSU
 * class to group two units both representing a different output of the same
 * Blocks Unit.
 */
class BlocksLSUPair {
public:
    // Smart pointers for automated memory management
    std::unique_ptr<BlocksLSU> lsu0;
    std::unique_ptr<BlocksLSU> lsu1;
    std::shared_ptr<TTAMachine::Socket> in1sock;
    std::shared_ptr<TTAMachine::Socket> in2sock;
    std::list<std::string> sources;

    BlocksLSUPair(
        TTAMachine::Machine& mach, const std::string& name,
        std::list<std::string> sources, TTAMachine::AddressSpace& asData,
        bool usesOut0, bool usesOut1);

private:
};
#endif
