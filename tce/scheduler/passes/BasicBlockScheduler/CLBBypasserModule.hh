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
#ifndef TCE_CLB_BYPASSER_MODULE_HH
#define TCE_CLB_BYPASSER_MODULE_HH

//#include "HelperSchedulerModule.hh"

#include "BypasserModule.hh"

#include <map>
#include <string>

class CycleLookBackSoftwareBypasser;
class SoftwareBypasser;

class CLBBypasserModule : public BypasserModule {
    
public:
    CLBBypasserModule();
    virtual ~CLBBypasserModule();

    SoftwareBypasser& bypasser();
    void setOptions(const std::vector<ObjectState*>& options);
    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;

private:
    static const std::string LOOKBACK_DISTANCE_OPTION;
    static const std::string KILL_DEAD_RESULTS_OPTION;

    int bypassDistance_;
    bool killDeadResults_;
    CycleLookBackSoftwareBypasser* clbs_;
};

#endif
