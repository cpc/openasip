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
#ifndef __cxxtest__TestTracker_h__
#define __cxxtest__TestTracker_h__

//
// The TestTracker tracks running tests
// The actual work is done in CountingListenerProxy,
// but this way avoids cyclic references TestListener<->CountingListenerProxy
//

#include <cxxtest/TestListener.h>
#include <cxxtest/DummyDescriptions.h>

namespace CxxTest
{
    class TestListener;
    
    class TestTracker : public TestListener
    {
    public:
        virtual ~TestTracker();
        
        static TestTracker &tracker();

        const TestDescription *fixTest( const TestDescription *d ) const;
        const SuiteDescription *fixSuite( const SuiteDescription *d ) const;
        const WorldDescription *fixWorld( const WorldDescription *d ) const;

        const TestDescription &test() const { return *_test; }
        const SuiteDescription &suite() const { return *_suite; }
        const WorldDescription &world() const { return *_world; }
        
        bool testFailed() const { return (testFailedAsserts() > 0); }
        bool suiteFailed() const { return (suiteFailedTests() > 0); }
        bool worldFailed() const { return (failedSuites() > 0); }
        
        unsigned warnings() const { return _warnings; }
        unsigned failedTests() const { return _failedTests; }
        unsigned testFailedAsserts() const { return _testFailedAsserts; }
        unsigned suiteFailedTests() const { return _suiteFailedTests; }
        unsigned failedSuites() const { return _failedSuites; }

        void enterWorld( const WorldDescription &wd );
        void enterSuite( const SuiteDescription &sd );
        void enterTest( const TestDescription &td );
        void leaveTest( const TestDescription &td );
        void leaveSuite( const SuiteDescription &sd );
        void leaveWorld( const WorldDescription &wd );
        void trace( const char *file, unsigned line, const char *expression );
        void warning( const char *file, unsigned line, const char *expression );
        void failedTest( const char *file, unsigned line, const char *expression );
        void failedAssert( const char *file, unsigned line, const char *expression );
        void failedAssertEquals( const char *file, unsigned line,
                                 const char *xStr, const char *yStr,
                                 const char *x, const char *y );
        void failedAssertSameData( const char *file, unsigned line,
                                   const char *xStr, const char *yStr,
                                   const char *sizeStr, const void *x,
                                   const void *y, unsigned size );
        void failedAssertDelta( const char *file, unsigned line,
                                const char *xStr, const char *yStr, const char *dStr,
                                const char *x, const char *y, const char *d );
        void failedAssertDiffers( const char *file, unsigned line,
                                  const char *xStr, const char *yStr,
                                  const char *value );
        void failedAssertLessThan( const char *file, unsigned line,
                                   const char *xStr, const char *yStr,
                                   const char *x, const char *y );
        void failedAssertLessThanEquals( const char *file, unsigned line,
                                         const char *xStr, const char *yStr,
                                         const char *x, const char *y );
        void failedAssertPredicate( const char *file, unsigned line,
                                    const char *predicate, const char *xStr, const char *x );
        void failedAssertRelation( const char *file, unsigned line,
                                   const char *relation, const char *xStr, const char *yStr,
                                   const char *x, const char *y );
        void failedAssertThrows( const char *file, unsigned line,
                                 const char *expression, const char *type,
                                 bool otherThrown );
        void failedAssertThrowsNot( const char *file, unsigned line, const char *expression );
        
    private:
        TestTracker( const TestTracker & );
        TestTracker &operator=( const TestTracker & );

        static bool _created;
        TestListener _dummyListener;
        DummyWorldDescription _dummyWorld;
        unsigned _warnings, _failedTests, _testFailedAsserts, _suiteFailedTests, _failedSuites;
        TestListener *_l;
        const WorldDescription *_world;
        const SuiteDescription *_suite;
        const TestDescription *_test;

        const TestDescription &dummyTest() const;
        const SuiteDescription &dummySuite() const;
        const WorldDescription &dummyWorld() const;
        
        void setWorld( const WorldDescription *w );
        void setSuite( const SuiteDescription *s );
        void setTest( const TestDescription *t );
        void countWarning();
        void countFailure();

        friend class TestRunner;
        
        TestTracker();
        void initialize();
        void setListener( TestListener *l );
    };

    inline TestTracker &tracker() { return TestTracker::tracker(); }
}

#endif // __cxxtest__TestTracker_h__
