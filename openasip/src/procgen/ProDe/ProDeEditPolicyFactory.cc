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
 * @file ProDeEditPolicyFactory.cc
 *
 * Implementation of ProDeEditPolicyFactory class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProDeEditPolicyFactory.hh"

#include "ProDeFUEditPolicy.hh"
#include "ProDeRFEditPolicy.hh"
#include "ProDeIUEditPolicy.hh"
#include "ProDeGCUEditPolicy.hh"
#include "ProDePortEditPolicy.hh"
#include "ProDeBusEditPolicy.hh"
#include "ProDeSocketEditPolicy.hh"
#include "ProDeSegmentEditPolicy.hh"
#include "ProDeBridgeEditPolicy.hh"
#include "ProDeFUPortEditPolicy.hh"
#include "ProDeRFPortEditPolicy.hh"
#include "ProDeIUPortEditPolicy.hh"
#include "ProDeSRPortEditPolicy.hh"

/**
 * The Constructor.
 */
ProDeEditPolicyFactory::ProDeEditPolicyFactory() {
}

/**
 * The Destructor.
 */
ProDeEditPolicyFactory::~ProDeEditPolicyFactory() {
}

/**
 * Returns edit policy for function units.
 *
 * @return Function unit edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createFUEditPolicy() {
    return new ProDeFUEditPolicy();
}

/**
 * Returns edit policy for register files.
 *
 * @return Register file edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createRFEditPolicy() {
    return new ProDeRFEditPolicy();
}

/**
 * Returns edit policy for immediate units.
 *
 * @return Immediate unit edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createIUEditPolicy() {
    return new ProDeIUEditPolicy();
}

/**
 * Returns edit policy for global control unit.
 *
 * @return Global control unit edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createGCUEditPolicy() {
    return new ProDeGCUEditPolicy();
}

/**
 * Returns edit policy for function unit ports.
 *
 * @return Function unit port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createFUPortEditPolicy() {
    return new ProDeFUPortEditPolicy();
}

/**
 * Returns edit policy for register file ports.
 *
 * @return Register file port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createRFPortEditPolicy() {
    return new ProDeRFPortEditPolicy();
}

/**
 * Returns edit policy for immediate unit ports.
 *
 * @return Immediate unit port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createIUPortEditPolicy() {
    return new ProDeIUPortEditPolicy();
}

/**
 * Returns edit policy for special register ports.
 *
 * @return Special register port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createSRPortEditPolicy() {
    return new ProDeSRPortEditPolicy();
}

/**
 * Returns edit policy for ports.
 *
 * @return Port edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createPortEditPolicy() {
    return new ProDePortEditPolicy();
}

/**
 * Returns edit policy for buses.
 *
 * @return Bus edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createBusEditPolicy() {
    return new ProDeBusEditPolicy();
}

/**
 * Returns edit policy for sockets.
 *
 * @return Socket edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createSocketEditPolicy() {
    return new ProDeSocketEditPolicy();
}

/**
 * Returns edit policy for bus segments.
 *
 * @return Segment edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createSegmentEditPolicy() {
    return new ProDeSegmentEditPolicy();
}

/**
 * Returns edit policy for bridgs.
 *
 * @return Bridge edit policy.
 */
EditPolicy*
ProDeEditPolicyFactory::createBridgeEditPolicy() {
    return new ProDeBridgeEditPolicy();
}
