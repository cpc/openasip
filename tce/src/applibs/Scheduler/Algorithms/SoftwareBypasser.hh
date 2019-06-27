/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file SoftwareBypasser.hh
 *
 * Declaration of SoftwareBypasser interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SOFTWARE_BYPASSER_HH
#define TTA_SOFTWARE_BYPASSER_HH

class MoveNodeGroup;
class DataDependenceGraph;
class ResourceManager;
class MoveNodeSelector;
class MoveNode;

/**
 * Classes that implement this interface are able to software bypass
 * moves.
 */
class SoftwareBypasser {
public:
    SoftwareBypasser();
    virtual ~SoftwareBypasser();

    virtual int bypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm, bool bypassTrigger) = 0;

    virtual int bypassNode(
        MoveNode& moveNode,
        int& lastOperandCycle,
        DataDependenceGraph& ddg,
        ResourceManager& rm) = 0;

    virtual void removeBypass(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm) = 0;

    virtual void removeBypass(
        MoveNode& moveNode,
        DataDependenceGraph& ddg,
        ResourceManager& rm, bool restoreSource=true) = 0;

    virtual int removeDeadResults(
        MoveNodeGroup& candidates,
        DataDependenceGraph& ddg,
        ResourceManager& rm) = 0;

    virtual void setSelector(MoveNodeSelector* selector);

    virtual void clearCaches(DataDependenceGraph& ddg, 
                             bool removeDeadResults) = 0;

    int bypassCount() { return bypassCount_; }
    int deadResultCount() { return deadResultCount_; }
protected:
    int bypassCount_;
    int deadResultCount_;
    
};

#endif


