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
 * @file ProximEditPolicyFactory.cc
 *
 * Implementation of ProximEditPolicyFactory class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProximEditPolicyFactory.hh"
#include "ProximRFEditPolicy.hh"
#include "ProximFUEditPolicy.hh"
#include "ProximIUEditPolicy.hh"
#include "ProximSocketEditPolicy.hh"
#include "ProximPortEditPolicy.hh"
#include "ProximBusEditPolicy.hh"

/**
 * The Constructor
 */
ProximEditPolicyFactory::ProximEditPolicyFactory() :
    EditPolicyFactory() {
}

/**
 * The Destructor.
 */
ProximEditPolicyFactory::~ProximEditPolicyFactory() {
}

/**
 * Creates edit policy for register files.
 */
EditPolicy*
ProximEditPolicyFactory::createRFEditPolicy() {
    return new ProximRFEditPolicy();
}

/**
 * Creates edit policy for function units.
 */
EditPolicy*
ProximEditPolicyFactory::createFUEditPolicy() {
    return new ProximFUEditPolicy();
}

/**
 * Creates edit policy for immediate units.
 */
EditPolicy*
ProximEditPolicyFactory::createIUEditPolicy() {
    return new ProximIUEditPolicy();
}

/**
 * Creates edit policy for control units.
 */
EditPolicy*
ProximEditPolicyFactory::createGCUEditPolicy() {
    return new ProximFUEditPolicy();
}


/**
 * Creates edit policy for sockets.
 */
EditPolicy*
ProximEditPolicyFactory::createSocketEditPolicy() {
    return new ProximSocketEditPolicy();
}

/**
 * Creates edit policy for ports.
 */
EditPolicy*
ProximEditPolicyFactory::createPortEditPolicy() {
    return new ProximPortEditPolicy();
}


/**
 * Creates edit policy for buses.
 */
EditPolicy*
ProximEditPolicyFactory::createBusEditPolicy() {
    return new ProximBusEditPolicy();
}



/**
 * Creates edit policy for bus segments.
 */
EditPolicy*
ProximEditPolicyFactory::createSegmentEditPolicy() {
    return new ProximBusEditPolicy();
}

