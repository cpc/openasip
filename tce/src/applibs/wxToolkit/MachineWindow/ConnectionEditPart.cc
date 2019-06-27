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
 * @file ConnectionEditPart.cc
 *
 * Definition of ConnectionEditPart class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 13 2004 by vpj, ll, jn, am
 */

#include "Application.hh"
#include "ConnectionEditPart.hh"
#include "ConnectionFigure.hh"

/**
 * The Constructor.
 */
ConnectionEditPart::ConnectionEditPart(): source_(NULL), target_(NULL) {
}

/**
 * The Destructor.
 */
ConnectionEditPart::~ConnectionEditPart() {
}

/**
 * Sets the source of this connection.
 *
 * @param source New source.
 */
void
ConnectionEditPart::setSource(EditPart* source) {
    assert(figure_ != NULL);
    ConnectionFigure* fig = dynamic_cast<ConnectionFigure*>(figure_);
    assert(fig != NULL);
    fig->setSource(source->figure());
    figure_ = fig;
    source_ = source;
}

/**
 * Sets the target of this connection.
 *
 * @param target New target.
 */
void
ConnectionEditPart::setTarget(EditPart* target) {
    assert(figure_ != NULL);
    ConnectionFigure* fig = dynamic_cast<ConnectionFigure*>(figure_);
    assert(fig != NULL);
    fig->setTarget(target->figure());
    figure_ = fig;
    target_ = target;
}
