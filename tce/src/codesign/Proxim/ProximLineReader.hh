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
 * @file ProximLineReader.hh
 *
 * Declaration of ProximLineReader class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROXIM_LINE_READER_HH
#define TTA_PROXIM_LINE_READER_HH

#include <iostream>
#include <wx/wx.h>
#include <string>
#include <queue>
#include "LineReader.hh"
#include "Application.hh"
#include "Exception.hh"

/**
 * Handles user input passing between the simulator and GUI threads in Proxim.
 *
 * This class is designed to act as an interface between two threads:
 * A GUI thread handling the user interface, and a worker thread running the
 * simulator engine. The worker thread waits for the user input
 * by listening to the 'input_' condition. When the line reader receives
 * input from the GUI thread, the worker thread is signaled about the input
 * by setting the input_ condition. If the worker thread is busy and not
 * listening to the input_ condition signal when the GUI thread signals
 * about user input, the signal is lost and the user input is ignored.
 * Therefore the user input should be disabled in the GUI when the worker
 * thread is busy. ProximLineReader requests user input and sends simulator
 * engine output to the GUI thread using SimulatorEvents and the standard
 * wxWidgets event handling system.
 */
class ProximLineReader : public LineReader {
public:
    ProximLineReader();
    virtual ~ProximLineReader();

    virtual void initialize(
	std::string defPrompt = "",
	FILE* in = stdin,
	FILE* out = stdout,
	FILE* err = stderr);

    virtual std::string readLine(std::string prompt = "")
	throw (ObjectNotInitialized, EndOfFile);

    virtual char charQuestion(
	std::string question,
	std::string allowedChars,
	bool caseSensitive = false,
	char defaultAnswer = '\0') throw (ObjectNotInitialized);

    void input(std::string command);

    virtual std::ostream& outputStream();
    void output(std::string text);

    virtual void setInputHistoryLog(const std::string& historyFilename)
        throw (IOException);

    std::string inputHistoryFilename() const;

private:
    /// Copying not allowed.
    ProximLineReader(const ProximLineReader&);
    /// Assignment not allowed.
    ProximLineReader& operator=(const ProximLineReader&);
    wxMutex* mutex_;
    /// Condition, which is signaled when user input is received from the GUI.
    wxCondition* input_;
    /// Input prompt.
    std::string prompt_;
    /// Input queue.
    std::queue<std::string> inputQueue_;
    /// GUI to send the events to.
    wxEvtHandler* gui_;
    /// Output stream which converts the output to SimulatorEvents.
    std::ostream* outputStream_;

    /// Name of the input history file.
    std::string historyFile_;

    static const std::string DEFAULT_LOG_FILE_NAME;
};


/**
 * Stream buffer for the ProximLROutputStream.
 *
 * This streambuffer converts the stream output to SimulatorEvents when
 * the buffer cotnetnts is flushed.
 */
class ProximLROutputBuffer : public std::streambuf {
public:
    ProximLROutputBuffer(ProximLineReader* lineReader);
    virtual ~ProximLROutputBuffer();
protected:
    int overflow(int);
    int sync();
private:
    void flushBuffer();
    ProximLineReader* lineReader_;
    const unsigned int BUFFER_SIZE;

};


/**
 * An output stream which converts stream output to SimulatorEvents.
 */
class ProximLROutputStream : public std::ostream {
public:
    ProximLROutputStream(ProximLineReader* lineReader);
    virtual ~ProximLROutputStream();
};

#endif
