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
 * @file CostEstimationPlugin.cc
 *
 * Implementation of CostEstimationPlugin class
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: red
 */

#include "CostEstimationPlugin.hh"

namespace CostEstimator {

/**
 * Constructor.
 *
 * @param name The name of the cost estimation plugin in the HDB.
 * @param dataSource The HDB that stores the cost estimation data of
 *                   the plugin. This reference can also be used to add new 
 *                   data, in case required by the algorithm.
 */
CostEstimationPlugin::CostEstimationPlugin(
    const std::string& name) :    
    name_(name) {
}

/**
 * Destructor.
 */
CostEstimationPlugin::~CostEstimationPlugin() {
}

/**
 * Returns the name of this cost estimation plugin.
 *
 * @return The name of the plugin.
 */
std::string
CostEstimationPlugin::name() const {
    return name_;
}

/**
 * Returns the description of this cost estimation plugin.
 *
 * @return The default description is empty.
 */
std::string
CostEstimationPlugin::description() const {
    return "";
}

}
