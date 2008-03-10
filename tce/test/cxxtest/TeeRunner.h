#ifndef __CXXTEST__TEERUNNER_H
#define __CXXTEST__TEERUNNER_H

//
// A TeeRunner is a TeeListener with a run() function
//

#include <cxxtest/TeeListener.h>

namespace CxxTest
{
    class TeeRunner : public TeeListener
    {
    public:
	virtual int run()
	{
	    TestRunner::runAllTests( this );
	    return TestTracker::tracker().failedTests();
	}
    };
};


#endif // __CXXTEST__TEERUNNER_H
