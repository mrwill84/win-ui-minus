/*
 * Copyright (c) 2006-2007, Andry Korolyuk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   * Neither the name of the Andry Korolyuk nor the names of its contributors 
 *     may be used to endorse or promote products derived from this software 
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <list>
#include <map>
#include <stdio.h>

namespace YesUnit
{
#define YESUNIT_ASSERT(condition)				\
	{											\
		if(!(condition))						\
		{										\
			std::wcout << L"\n\tassertion failed" << std::endl;\
			std::wcout << L"\t";\
            std::wcout << #condition << std::endl;\
			std::wcout << L"\t" << __FILE__ << L":" << __LINE__ << std::endl;\
		}										\
	}

    class ITest
    {
    public:
        virtual void         run    ()       = 0;
		virtual std::wstring getName() const = 0;
    };

    template<typename T>
    class Test : public ITest
    {
        typedef void (T::*Method)();
    public:
        Test()
        {
        }
        
        virtual void run()
        {
            _errorCount = 0;
            std::list<std::pair<Method, std::wstring> >::iterator it = _methods.begin();
            while(it != _methods.end())
            {
                Method method = it->first;
                std::wstring name = it->second;
                std::wcout << name.c_str();
                try
                {
                    (_test->*method)();
                    if(_errorCount == 0)
                    {
                        std::wcout << L".....................[OK]" << std::endl;
                    }
                    else
                    {
                        std::wcout << L".....................[ERROR]" << std::endl;
                    }
                }
                catch(...)
                {
                    std::wcout << L"[EXCEPTION]" << std::endl;
                }
                it++;
            }
        }

        void ensure(bool condition, const std::wstring & text = L"")
        {
            if(condition == false)
            {
                _errorCount++;
                std::wcout << L"-------------- Assertion failed  ";
                std::wcout << text.c_str() << std::endl;
            }
        }

        virtual void registerMethod(Method method, const std::wstring & methodName, T * test)
        {
            std::pair<Method, std::wstring> pair(method, methodName);
            _methods.push_back(pair);
            _test = test;
        }

        ~Test() {};
    private:
        std::list<std::pair<Method, std::wstring> > _methods;
        int _errorCount;
        T * _test;
    };

    /**
     *  This one actually runs tests and reports on execution
     */
    class Tester
    {
    private:
		std::list<std::pair<std::wstring, ITest*> > _tests;

    public:
		void addTest(ITest * test)
        {
			_tests.push_back(std::pair<std::wstring, ITest*>(test->getName(), test));
        }

        void run()
        {
			std::list<std::pair<std::wstring, ITest*> >::iterator it = _tests.begin();
            while(it != _tests.end())
            {
                std::wcout << L"Testing: ";
                std::wcout << it->first.c_str() << std::endl;
                (it->second)->run();
                ++it;
            }

            std::wcout << L"<Press any key>";
            getchar();
        }
    };

};