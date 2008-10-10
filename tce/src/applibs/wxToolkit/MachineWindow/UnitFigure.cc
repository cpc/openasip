/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file UnitFigure.cc
 *
 * Definition of UnitFigure class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#include <vector>
#include <map>

#include "Application.hh"
#include "UnitFigure.hh"
#include "MachineCanvasLayoutConstraints.hh"
#include "InputSocketFigure.hh"
#include "OutputSocketFigure.hh"
#include "TriggeringPortFigure.hh"
#include "MachineCanvasOptions.hh"

using std::vector;
using std::map;

const wxColour UnitFigure::DEFAULT_COLOUR = wxColour(0, 0, 0);
const wxColour UnitFigure::DEFAULT_INFO_TEXT_COLOUR = wxColour(150, 150, 150);
const wxColour UnitFigure::DEFAULT_BG_COLOUR = wxColour(255, 255, 255);

const int UnitFigure::MARGIN = 5;
const int UnitFigure::SPACING = 5;


/**
 * The Constructor.
 */
UnitFigure::UnitFigure(): Figure() {
    minSize_ = wxSize(
	MachineCanvasLayoutConstraints::UNIT_WIDTH,
	MachineCanvasLayoutConstraints::UNIT_HEIGHT);
    size_ = minSize_;
}

/**
 * The Destructor.
 */
UnitFigure::~UnitFigure() {
}


/**
 * Draws the function unit's Figure on the given device context.
 *
 * @param dc The device context.
 */
void
UnitFigure::drawSelf(wxDC* dc) {

    wxPen pen = wxPen(DEFAULT_COLOUR, 1, wxSOLID);
    dc->SetPen(pen);
    wxBrush brush = wxBrush(DEFAULT_BG_COLOUR, wxSOLID);
    if (highlighted_) {
        brush = wxBrush(highlight_, wxSOLID);
    }
    dc->SetBrush(brush);


    dc->DrawRectangle(location_.x, location_.y, size_.GetWidth(),
		      size_.GetHeight());

    dc->SetBackgroundMode(wxTRANSPARENT);
    dc->SetTextForeground(DEFAULT_COLOUR);

    int typeWidth;
    int typeHeight;
    dc->GetTextExtent(type_, &typeWidth, &typeHeight);

    int typeX = location_.x + size_.GetWidth() / 2 - typeWidth / 2;
    int typeY = location_.y + MARGIN;

    int nameWidth;
    int nameHeight;

    dc->GetTextExtent(name_, &nameWidth, &nameHeight);

    int nameX = location_.x + size_.GetWidth() / 2 - nameWidth / 2;
    int nameY = location_.y + MARGIN + typeHeight + SPACING;

    dc->DrawText(type_, typeX, typeY);
    dc->DrawText(name_, nameX, nameY);

    if (options() != NULL && options()->optionValue(
            MachineCanvasOptions::SHOW_UNIT_INFO_STRING).isFlagOn()) {

        int infoWidth;
        int infoHeight;
    
        dc->GetTextExtent(info_, &infoWidth, &infoHeight);

        int infoX = location_.x + size_.GetWidth() / 2 - infoWidth / 2;
        int infoY =
            location_.y + MARGIN + typeHeight + SPACING + nameHeight + SPACING;

        dc->SetTextForeground(DEFAULT_INFO_TEXT_COLOUR);
        dc->DrawText(info_, infoX, infoY);
    }
}

/**
 * Lays out the ports to the bottom of the fu's Figure, spaced evenly.
 *
 * @param dc Device context.
 */
void
UnitFigure::layoutChildren(wxDC* dc) {

    // Sort port child figures to containers based on port directions.
    // Map with portname, figure pairs is used so that the ports
    // are automatically ordered by the port name.
    map<std::string, Figure*> inputPorts;
    map<std::string, Figure*> outputPorts;
    map<std::string, Figure*> bidirPorts;
    map<std::string, Figure*> trigPorts;
    map<std::string, Figure*> nodirPorts;

    vector<Figure*>::iterator i = children_.begin();
    for(; i != children_.end(); i++) {
        UnitPortFigure* fig = dynamic_cast<UnitPortFigure*>(*i);
        assert(fig != NULL);
        bool input = false;
        bool output = false;
        for (int j = 0; j < (fig->childCount()); j++) {
            Figure* socketFig = fig->child(j);
            if (dynamic_cast<InputSocketFigure*>(socketFig) != NULL) {
                input = true;
            } else if (dynamic_cast<OutputSocketFigure*>(socketFig) != NULL) {
                output = true;
            }
        }
        if (input && dynamic_cast<TriggeringPortFigure*>(fig) != NULL) {
            trigPorts[fig->name()] = fig;
        } else if (input && output) {
            bidirPorts[fig->name()] =  fig;
        } else if (input) {
            inputPorts[fig->name()]  = fig;
        } else if (output) {
            outputPorts[fig->name()] = fig;
        } else {
            nodirPorts[fig->name()] = fig;
        }
    }
    
    int portX = location_.x + MachineCanvasLayoutConstraints::PORT_SPACE;
    int portY = location_.y + size_.GetHeight();

    // Layout input ports.
    map<std::string, Figure*>::iterator inIter = inputPorts.begin();
    for (; inIter != inputPorts.end(); inIter++) {
        portY -= (*inIter).second->bounds().GetHeight()/2;
        (*inIter).second->setLocation(wxPoint(portX, portY));
        (*inIter).second->layout(dc);
        portY = location_.y + size_.GetHeight();
        portX += (*inIter).second->bounds().GetWidth() +
            MachineCanvasLayoutConstraints::PORT_SPACE;
    }

    // Layout triggering ports.
    map<std::string, Figure*>::iterator trigIter = trigPorts.begin();
    for (; trigIter != trigPorts.end(); trigIter++) {
        portY -= (*trigIter).second->bounds().GetHeight()/2;
        (*trigIter).second->setLocation(wxPoint(portX, portY));
        (*trigIter).second->layout(dc);
        portY = location_.y + size_.GetHeight();
        portX += (*trigIter).second->bounds().GetWidth() +
            MachineCanvasLayoutConstraints::PORT_SPACE;
    }

    // Layout bidirectional ports.
    map<std::string, Figure*>::iterator bdIter = bidirPorts.begin();
    for (; bdIter != bidirPorts.end(); bdIter++) {
        portY -= (*bdIter).second->bounds().GetHeight()/2;
        (*bdIter).second->setLocation(wxPoint(portX, portY));
        (*bdIter).second->layout(dc);
        portY = location_.y + size_.GetHeight();
        portX += (*bdIter).second->bounds().GetWidth() +
            MachineCanvasLayoutConstraints::PORT_SPACE;
    }

    // Layout output ports.
    map<std::string, Figure*>::iterator outIter = outputPorts.begin();
    for (; outIter != outputPorts.end(); outIter++) {
        portY -= (*outIter).second->bounds().GetHeight()/2;
        (*outIter).second->setLocation(wxPoint(portX, portY));
        (*outIter).second->layout(dc);
        portY = location_.y + size_.GetHeight();
        portX += (*outIter).second->bounds().GetWidth() +
            MachineCanvasLayoutConstraints::PORT_SPACE;
    }

    // Layout ports w/ no direction.
    map<std::string, Figure*>::iterator ndIter = nodirPorts.begin();
    for (; ndIter != nodirPorts.end(); ndIter++) {
        portY -= (*ndIter).second->bounds().GetHeight()/2;
        (*ndIter).second->setLocation(wxPoint(portX, portY));
        (*ndIter).second->layout(dc);
        portY = location_.y + size_.GetHeight();
        portX += (*ndIter).second->bounds().GetWidth() +
            MachineCanvasLayoutConstraints::PORT_SPACE;
    }
}

/**
 * Sets the name of the unit.
 *
 * @param name New name.
 */
void
UnitFigure::setName(const wxString& name) {
    int index = name.Find(':');
    if (index == -1) {
	type_ = _T("Unit:");
	name_ = name;
    } else {
	type_ = name.Mid(0, index + 1);
	type_.Trim();
	name_ = name.Mid(index + 2);
	name_.Trim();
    }
}

/**
 * Sets the information string of the unit.
 *
 * @param info New info string..
 */
void
UnitFigure::setInfo(const wxString& info) {
    info_ = info;
}

/**
 * Lays the figure out.
 *
 * Figure of the unit is laid out before it's children, so that the port
 * Y-coordinate can be set correctly when laying out the ports.
 *
 * @param dc Device cotnext to lay the figure out on.
 */
void
UnitFigure::layout(wxDC* dc) {
    layoutSelf(dc);
    layoutChildren(dc);
    laidOut_ = true;
}

/**
 * Calculates and sets the size of the unit.
 *
 * Depends on its name and the amount of ports.
 *
 * @param dc Device context.
 */
void
UnitFigure::layoutSelf(wxDC* dc) {

    bool showInfo = (options() != NULL) && options()->optionValue(
        MachineCanvasOptions::SHOW_UNIT_INFO_STRING).isFlagOn();

    int portsWidth = 0;

    vector<Figure*>::const_iterator i = children_.begin();
    for (; i != children_.end(); i++) {
	portsWidth += (*i)->bounds().GetWidth();
    }

    portsWidth +=
	(children_.size()+1) * MachineCanvasLayoutConstraints::PORT_SPACE;

    int nameWidth = 0;
    int nameHeight = 0;
    int infoWidth = 0;
    int infoHeight = 0;
    int typeWidth = 0;
    int typeHeight = 0;

    dc->GetTextExtent(name_, &nameWidth, &nameHeight);
    dc->GetTextExtent(type_, &typeWidth, &typeHeight);
    if (showInfo) {
        dc->GetTextExtent(info_, &infoWidth, &infoHeight);
    }
    nameWidth += MARGIN * 4;
    typeWidth += MARGIN * 4;
    infoWidth += MARGIN * 4;

    int maxWidth = MachineCanvasLayoutConstraints::MAX_UNIT_NAME_WIDTH;

    if (portsWidth > maxWidth) {
	maxWidth = portsWidth;
    }

    if ((nameWidth + 2 * MARGIN) > maxWidth) {

	unsigned int chars = 0;
	int charsExtent = 0;

	while ((charsExtent + 2 * MARGIN) < maxWidth) {
	    int charWidth = 0;
	    int charHeight = 0;
	    dc->GetTextExtent(name_.GetChar(chars), &charWidth, &charHeight);
	    charsExtent += charWidth;
	    chars++;
	    if (chars > name_.Len()) {
		break;
	    }
	}

	name_.Truncate(chars - 5);
	name_.Append(_T("..."));
        dc->GetTextExtent(name_, &nameWidth, &nameHeight);
        nameWidth += 4 * MARGIN;
    }

    if (showInfo && (infoWidth + 2 * MARGIN) > maxWidth) {

	unsigned int chars = 0;
	int charsExtent = 0;

	while ((charsExtent + 2 * MARGIN) < maxWidth) {
	    int charWidth = 0;
	    int charHeight = 0;
	    dc->GetTextExtent(info_.GetChar(chars), &charWidth, &charHeight);
	    charsExtent += charWidth;
	    chars++;
	    if (chars > info_.Len()) {
		break;
	    }
	}

	info_.Truncate(chars - 5);
	info_.Append(_T("..."));
        dc->GetTextExtent(info_, &infoWidth, &infoHeight);
        infoWidth += 4 * MARGIN;
    }

    if (infoWidth > size_.GetWidth() &&
        infoWidth >= nameWidth && infoWidth > portsWidth) {
       
        size_.SetWidth(infoWidth);
       
    } else if (nameWidth > size_.GetWidth() &&
               nameWidth > infoWidth && nameWidth > portsWidth) {

        size_.SetWidth(nameWidth);

    } else if (portsWidth > size_.GetWidth()) {
        size_.SetWidth(portsWidth);
    }

    // Set figure height.
    int height = 2 * MARGIN +  SPACING + typeHeight + nameHeight +
        (MachineCanvasLayoutConstraints::PORT_WIDTH / 2);
   
    if (showInfo) {
        height += SPACING + infoHeight;
    }
    if (height > minSize_.GetHeight()) {
        size_.SetHeight(height);
    }
}
