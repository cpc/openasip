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
 * @file SocketBusConnCmd.cc
 *
 * Definition of SocketBusConnCmd class.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2019
 * @note rating: red
 */

#include "Application.hh"
#include "SocketBusConnCmd.hh"
#include "FUPort.hh"
#include "RFPort.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "Operand.hh"

using namespace TTAMachine;

/**
 * The Constructor.
 *
 * @param segment Segment to connect/disconnect.
 * @param socket Socket to connect/disconnect.
 */
SocketBusConnCmd::SocketBusConnCmd(Socket* socket, Segment* segment):
    ComponentCommand(),
    socket_(socket), segment_(segment) {
}


/**
 * The Destructor.
 */
SocketBusConnCmd::~SocketBusConnCmd() {
}


/**
 * Executes the command.
 *
 * @return true, if the command was succesfully executed, false otherwise.
 */
bool
SocketBusConnCmd::Do() {

    if (socket_->isConnectedTo(*segment_)) {
        try {
            socket_->detachBus(*segment_);
        } catch (Exception& e) {
            return false;
        }
    } else {
        try {
            // If there was no direction before for this socket, let's
            // try to figure out a better guess for it after connected
            // to a port.
            bool resetDirection =
              socket_->direction() == Socket::UNKNOWN;
            socket_->attachBus(*segment_);

            if (resetDirection && socket_->portCount() > 0) {
              if (FUPort *port = dynamic_cast<FUPort*>(socket_->port(0))) {
                TTAMachine::FunctionUnit* FUnit =
                  dynamic_cast<FunctionUnit*>(port->parentUnit());
                for (int i = 0; i < FUnit->operationCount(); ++i) {
                  HWOperation* hwOp = FUnit->operation(i);
                  if (!hwOp->isBound(*port)) continue;
                  int operandId = hwOp->io(*port);
                  OperationPool opPool;
                  Operation& osalOp = opPool.operation(hwOp->name().c_str());
                  if (osalOp.isNull()) break;
                  if (osalOp.operand(operandId).isInput())
                    socket_->setDirection(Socket::INPUT);
                  else
                    socket_->setDirection(Socket::OUTPUT);
                  break;
                }
              } else if (RFPort* port =
                         dynamic_cast<RFPort*>(socket_->port(0))) {
                // RF ports have no other direction clue but the naming, which
                // is often 'rd' or 'wr'. rd is for a port that is _used for_
                // reading data (thus OUTPUT). This is probably a better guess
                // than always assuming one or another.
                if (port->name().size() > 0 && port->name()[0] == 'r')
                  socket_->setDirection(Socket::OUTPUT);
                else
                  socket_->setDirection(Socket::INPUT);
              }
            }
            // HERE.
        } catch (Exception& e) {
            return false;
        }
    }

    return true;
}
