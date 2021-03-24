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
 * @file BlocksTranslator.hh
 *
 * Declaration of the Blocks translator (Blocks to TTA) functions.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#ifndef BLOCKS_TRANSLATOR_HH
#define BLOCKS_TRANSLATOR_HH

#include "BlocksALU.hh"
#include "BlocksGCU.hh"
#include "BlocksIMM.hh"
#include "BlocksLSU.hh"
#include "BlocksMUL.hh"
#include "BlocksModel.hh"
#include "BlocksRF.hh"
#include "ControlUnit.hh"
#include "FUPort.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ImmediateUnit.hh"
#include "Socket.hh"
#include "SpecialRegisterPort.hh"

namespace BlocksTranslator {
void BuildTTAModel(BlocksModel& blocksModel, const std::string& outputName);
TTAMachine::Bus* CreateConnection(
    TTAMachine::Machine& mach, TTAMachine::Socket* inputSocket,
    TTAMachine::Socket* outputSocket, BlocksGCU& gcu);

void Deinitialize(
    TTAMachine::Machine& mach, std::list<BlocksALUPair*> aluList,
    std::list<BlocksLSUPair*> lsuList, std::list<BlocksRF*> rfList,
    std::list<BlocksIMM*> iuList, std::list<BlocksMULPair*> mulList);

void ConnectInputs(
    TTAMachine::Machine& mach, BlocksGCU& gcu,
    std::list<BlocksALUPair*> aluList, std::list<BlocksLSUPair*> lsuList,
    std::list<BlocksRF*> rfList, std::list<BlocksMULPair*> mulList);
TTAMachine::Socket* FindOutputSocket(
    TTAMachine::Machine::SocketNavigator nav, std::string source);
}  // namespace BlocksTranslator
#endif
