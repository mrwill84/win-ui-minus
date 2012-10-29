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

#include "../SplitLayout.h"
#include "MockView.h"

class SplitLayoutTester : public Gui::SplitLayout
{
public:
	SplitLayoutTester(const Gui::Splitter::Type & type)
		:Gui::SplitLayout(type)
	{
	}

    std::vector<Gui::Splitter> & getSplitters()
    {
        return _splitters;
    }

    std::vector<Gui::IView*> & getViews()
    {
        return _views;
    }
};

class SplitLayoutTests : public YesUnit::Test<SplitLayoutTests>
{
public:
    SplitLayoutTests()
    {
        registerMethod(testsAddViewToHorizontalLayout, L"testsAddViewToHorizontalLayout", this);
        registerMethod(testsAddViewToVerticalLayout  , L"testsAddViewToVerticalLayout"  , this);
        registerMethod(testsSplitterMoveLimitations  , L"testsSplitterMoveLimitations"  , this);
        registerMethod(testsLayoutResizing           , L"testsLayoutResizing"           , this);
        registerMethod(testsRemoveView               , L"testsRemoveView"               , this);
        registerMethod(testsInsertView               , L"testsInsertView"               , this);
    }

    std::wstring getName(void) const
    {
        return L"SplitLayoutTest";
    }

    void testsAddViewToHorizontalLayout(void)
    {
		SplitLayoutTester splitLayout(Gui::Splitter::HORIZONTAL);
		YESUNIT_ASSERT(splitLayout.isEmpty());

        splitLayout.setResize(150, 90);

		MockView mockView1;

        splitLayout.add(&mockView1);

		YESUNIT_ASSERT(splitLayout.isEmpty     ()        == false);
        YESUNIT_ASSERT(splitLayout.getViews    ().size() == 1    );
        YESUNIT_ASSERT(splitLayout.getSplitters().size() == 1    );

        YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == splitLayout.getHeight());
        YESUNIT_ASSERT(splitLayout.getSplitter(0).isVisible  () == false);

		MockView mockView2;
        splitLayout.add(&mockView2);
        YESUNIT_ASSERT(splitLayout.getViews    ().size() == 2);
        YESUNIT_ASSERT(splitLayout.getSplitters().size() == 2);
        YESUNIT_ASSERT(splitLayout.getSplitter(0).isVisible() == true );
        YESUNIT_ASSERT(splitLayout.getSplitter(1).isVisible() == false);

        YESUNIT_ASSERT(splitLayout.getSplitter(1).getPosition() == splitLayout.getHeight());
        YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == splitLayout.getHeight()/2);
        YESUNIT_ASSERT(splitLayout.getViews()[0]->getSize().height  == splitLayout.getHeight()/2 - 2);

		MockView mockView3;
        splitLayout.add(&mockView3);

        YESUNIT_ASSERT(splitLayout.getSplitter(0).isVisible() == true );
        YESUNIT_ASSERT(splitLayout.getSplitter(1).isVisible() == true );
        YESUNIT_ASSERT(splitLayout.getSplitter(2).isVisible() == false);

        YESUNIT_ASSERT(splitLayout.getViews    ().size() == 3);
        YESUNIT_ASSERT(splitLayout.getSplitters().size() == 3);

		YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == splitLayout.getHeight()/3);
		YESUNIT_ASSERT(splitLayout.getSplitter(1).getPosition() == (splitLayout.getHeight()/3)*2);
        YESUNIT_ASSERT(splitLayout.getSplitter(2).getPosition() == splitLayout.getHeight());
    }
    void testsAddViewToVerticalLayout(void)
    {
		SplitLayoutTester splitLayout(Gui::Splitter::VERTICAL);
		YESUNIT_ASSERT(splitLayout.isEmpty());

        splitLayout.setResize(150, 90);

		MockView mockView1;

        splitLayout.add(&mockView1);

		YESUNIT_ASSERT(splitLayout.isEmpty() == false);
        YESUNIT_ASSERT(splitLayout.getViews    ().size() == 1);
        YESUNIT_ASSERT(splitLayout.getSplitters().size() == 1);

        YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == splitLayout.getWidth());
        YESUNIT_ASSERT(splitLayout.getSplitter(0).isVisible() == false);

		MockView mockView2;
        splitLayout.add(&mockView2);
        YESUNIT_ASSERT(splitLayout.getViews    ().size() == 2);
        YESUNIT_ASSERT(splitLayout.getSplitters().size() == 2);
        YESUNIT_ASSERT(splitLayout.getSplitter(0).isVisible() == true );
        YESUNIT_ASSERT(splitLayout.getSplitter(1).isVisible() == false);

        YESUNIT_ASSERT(splitLayout.getSplitter(1).getPosition() == splitLayout.getWidth());
        YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == splitLayout.getWidth()/2);
        YESUNIT_ASSERT(splitLayout.getViews()[0]->getSize().width  == splitLayout.getWidth()/2 - 2);

		MockView mockView3;
        splitLayout.add(&mockView3);

        YESUNIT_ASSERT(splitLayout.getSplitter(0).isVisible() == true);
        YESUNIT_ASSERT(splitLayout.getSplitter(1).isVisible() == true);
        YESUNIT_ASSERT(splitLayout.getSplitter(2).isVisible() == false);

        YESUNIT_ASSERT(splitLayout.getViews    ().size() == 3);
        YESUNIT_ASSERT(splitLayout.getSplitters().size() == 3);

        YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == splitLayout.getWidth()/3);
        YESUNIT_ASSERT(splitLayout.getSplitter(1).getPosition() == (splitLayout.getWidth()/3)*2);
        YESUNIT_ASSERT(splitLayout.getSplitter(2).getPosition() == splitLayout.getWidth());
	}

	void testsSplitterMoveLimitations()
	{
		SplitLayoutTester splitLayout(Gui::Splitter::VERTICAL);

		MockView mockView1;
		MockView mockView2;
		splitLayout.add(&mockView1);
		splitLayout.add(&mockView2);

		splitLayout.setResize(150, 90);
		splitLayout.setSplitterPosition(0, 10);

		YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == 10);

		splitLayout.setSplitterPosition(0, 0);
		YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == Gui::SplitLayout::MIN_VIEW_SIZE);

		splitLayout.setSplitterPosition(0, 200);
		YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == (splitLayout.getWidth() - Gui::SplitLayout::MIN_VIEW_SIZE));

		MockView mockView3;
		splitLayout.add(&mockView3);
		YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() ==  50);
		YESUNIT_ASSERT(splitLayout.getSplitter(1).getPosition() == 100);

		splitLayout.setSplitterPosition(0, 110);
		YESUNIT_ASSERT(splitLayout.getSplitter(0).getPosition() == 
			splitLayout.getSplitter(1).getPosition() - Gui::SplitLayout::MIN_VIEW_SIZE);
	}

	void testsLayoutResizing(void)
	{
		SplitLayoutTester splitLayout(Gui::Splitter::VERTICAL);

		splitLayout.setResize(60, 50);

		YESUNIT_ASSERT(splitLayout.getWidth () == 60);
		YESUNIT_ASSERT(splitLayout.getHeight() == 50);

		MockView mockView1;
		MockView mockView2;
		splitLayout.add(&mockView1);
		splitLayout.add(&mockView2);

		splitLayout.setResize(150, 90);
		YESUNIT_ASSERT(splitLayout.getSplitter(1).getPosition() == 150);

		splitLayout.setResize(160, 90);
		YESUNIT_ASSERT(splitLayout.getSplitter(1).getPosition() == 160);
	}

	void testsRemoveView(void)
	{
		SplitLayoutTester splitLayout(Gui::Splitter::VERTICAL);

		MockView mockView1;
		MockView mockView2;
		splitLayout.add(&mockView1);
		splitLayout.add(&mockView2);

		splitLayout.setResize(150, 90);

		splitLayout.removeView(0);
		YESUNIT_ASSERT(splitLayout.getSplitters().size() == 1);
		YESUNIT_ASSERT(splitLayout.getViews().at(0)->getSize().width == 150);
	}

	void testsInsertView(void)
	{
		SplitLayoutTester splitLayout(Gui::Splitter::VERTICAL);

		MockView mockView1;
		MockView mockView2;
		splitLayout.insert(0, &mockView1);
		YESUNIT_ASSERT(splitLayout.getViewsCount() == 1);

		splitLayout.insert(0, &mockView2);
		YESUNIT_ASSERT(splitLayout.getViewsCount() == 2);
		YESUNIT_ASSERT(splitLayout.getViews()[0] == &mockView2);
	}
};