/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file AddressSpaceMapperPass.cc
 *
 * Implementation of AddressSpaceMapperPass class
 *
 * scheduler pass for address space mapping.
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "AddressSpaceMapper.hh"

#include "AddressSpaceMapperPass.hh"

using namespace TTAProgram;
using namespace TTAMachine;

SCHEDULER_PASS(AddressSpaceMapperPass)

AddressSpaceMapperPass::AddressSpaceMapperPass() {}

AddressSpaceMapperPass::~AddressSpaceMapperPass() {}

void AddressSpaceMapperPass::start() 
    throw (Exception) {
    AddressSpaceMapper::mapAddressSpace(*program_,*target_);
}

/**
 * A short description of the module, usually the module name,
 * in this case "AddressSpaceMapperPass".
 *
 * @return The description as a string.
 */   
std::string
AddressSpaceMapperPass::shortDescription() const {
    return "Startable: AddressSpaceMapperPass";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
AddressSpaceMapperPass::longDescription() const {
    std::string answer = "Startable: AddressSpaceMapperPass";
    return answer;
}
