/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file MoveNodeGroupBuilder.hh
 * 
 * Declaration of MoveNodeGroupBuilder class 
 * 
 * @author Pekka Jääskeläinen 2010
 * @note rating: red
 */

#ifndef TTA_MOVE_NODE_GROUP_BUILDER_HH
#define TTA_MOVE_NODE_GROUP_BUILDER_HH

#include <list>

class ProgramOperation;
class MoveNodeGroup;
class BasicBlock;

/**
 * Builds a list of MoveNodeGroups (MNG) from the input basic block.
 *
 * Also builds MoveNodes and ProgramOperations and ensures the produced
 * MNG list is in the input sequential order and that each MNG contains
 * either one MoveNode or a set of MoveNodes consisting a ProgramOperation.
 */
class MoveNodeGroupBuilder {
public:
    typedef std::list<MoveNodeGroup*> MoveNodeGroupList;

    MoveNodeGroupBuilder() {}
    virtual ~MoveNodeGroupBuilder() {}    

    MoveNodeGroupList* build(BasicBlock& bb);    
    
private:

    typedef std::list<ProgramOperation*> ProgramOperationList;
    ProgramOperationList programOperations_;
};

#endif
