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

class SplitPaneTests : public YesUnit::Test<SplitPaneTests>
{
public:
    SplitPaneTests()
    {
        registerMethod(testsMoveSplitter             , L"testsMoveSplitter"             , this);
        registerMethod(testsAddViewToHorizontalLayout, L"testsAddViewToHorizontalLayout", this);
        registerMethod(testsAddViewToVerticalLayout  , L"testsAddViewToVerticalLayout"  , this);
        registerMethod(testsSplitterMoveLimitations  , L"testsSplitterMoveLimitations"  , this);
        registerMethod(testsLayoutResizing           , L"testsLayoutResizing"           , this);
        registerMethod(testsRemoveView               , L"testsRemoveView"               , this);
        registerMethod(testsInsertView               , L"testsInsertView"               , this);

    }

    std::wstring getName(void) const
    {
        return L"SplitPaneTests";
    }

	void testsAddPane(void)
	{
		Gui::SplitPane splitPane(Gui::Splitter::VERTICAL);
		Gui::Resizable pane1;
		Gui::Resizable pane2;

		splitPane.addPane(&pane1);
		YESUNIT_ASSERT(splitPane.getSplittersCount() == 0);
		YESUNIT_ASSERT(splitPane.getPanesCount    () == 1);

		splitPane.addPane(&pane2);
		YESUNIT_ASSERT(splitPane.getSplittersCount() == 1);
		YESUNIT_ASSERT(splitPane.getPanesCount    () == 2);
	}

	void testsMoveSplitter(void)
	{
		Gui::SplitPane splitPane(Gui::Splitter::VERTICAL);
		Gui::DockContainer::Dock dock1;
		Gui::DockContainer::Dock dock2;

		splitPane.addPane(&dock1);
		splitPane.addPane(&dock2);
		splitPane.setSize(100, 100);
		splitPane.setSplitterPos(0, 50);

		YESUNIT_ASSERT(dock1.x      ==   0);
		YESUNIT_ASSERT(dock1.y      ==   0);
		YESUNIT_ASSERT(dock1.width  ==  48);
		YESUNIT_ASSERT(dock1.height == 100);

		YESUNIT_ASSERT(dock2.x      ==  52);
		YESUNIT_ASSERT(dock2.y      ==   0);
		YESUNIT_ASSERT(dock2.width  ==  48);
		YESUNIT_ASSERT(dock2.height == 100);
	}

	void testsAddViewToHorizontalLayout(void)
    {
		Gui::SplitPane splitPane(Gui::Splitter::HORIZONTAL);
		YESUNIT_ASSERT(splitPane.getPanesCount    () == 0);
		YESUNIT_ASSERT(splitPane.getSplittersCount() == 0);

        splitPane.setSize(150, 90);

		Gui::Resizable mockPane1;

        splitPane.addPane(&mockPane1);

        YESUNIT_ASSERT(splitPane.getPanesCount    () == 1);
        YESUNIT_ASSERT(splitPane.getSplittersCount() == 0);

        YESUNIT_ASSERT(splitPane.getSplitter(0).pos == (splitPane.height + 2));

		Gui::Resizable mockPane2;
        splitPane.addPane(&mockPane2);
        YESUNIT_ASSERT(splitPane.getPanesCount    () == 2);
        YESUNIT_ASSERT(splitPane.getSplittersCount() == 1);

        YESUNIT_ASSERT(splitPane.getSplitter(0).pos == splitPane.height/2);

		Gui::Resizable mockPane3;
        splitPane.addPane(&mockPane3);

        YESUNIT_ASSERT(splitPane.getPanesCount    () == 3);
        YESUNIT_ASSERT(splitPane.getSplittersCount() == 2);

		YESUNIT_ASSERT(splitPane.getSplitter(0).pos == splitPane.height/3);
		YESUNIT_ASSERT(splitPane.getSplitter(1).pos == (splitPane.height/3)*2);
    }

	void testsAddViewToVerticalLayout(void)
    {
		Gui::SplitPane splitPane(Gui::Splitter::VERTICAL);
		YESUNIT_ASSERT(splitPane.getPanesCount() == 0);

        splitPane.setSize(150, 90);

		Gui::Resizable mockPane1;

        splitPane.addPane(&mockPane1);

        YESUNIT_ASSERT(splitPane.getPanesCount    () == 1);
        YESUNIT_ASSERT(splitPane.getSplittersCount() == 0);

        YESUNIT_ASSERT(splitPane.getSplitter(0).pos == splitPane.width + 2);

		Gui::Resizable mockPane2;
        splitPane.addPane(&mockPane2);

        YESUNIT_ASSERT(splitPane.getPanesCount    () == 2);
        YESUNIT_ASSERT(splitPane.getSplittersCount() == 1);

        YESUNIT_ASSERT(splitPane.getSplitter(1).pos == splitPane.width + 2);
        YESUNIT_ASSERT(splitPane.getSplitter(0).pos == splitPane.width/2);
        YESUNIT_ASSERT(splitPane.getPane(0).width   == splitPane.width/2 - 2);

		Gui::Resizable mockPane3;
        splitPane.addPane(&mockPane3);

        YESUNIT_ASSERT(splitPane.getPanesCount    () == 3);
        YESUNIT_ASSERT(splitPane.getSplittersCount() == 2);

        YESUNIT_ASSERT(splitPane.getSplitter(0).pos == splitPane.width/3);
        YESUNIT_ASSERT(splitPane.getSplitter(1).pos == (splitPane.width/3)*2);
        YESUNIT_ASSERT(splitPane.getSplitter(2).pos == splitPane.width + 2);
	}

	void testsSplitterMoveLimitations()
	{
		Gui::SplitPane splitPane(Gui::Splitter::VERTICAL);

		Gui::Resizable mockPane1;
		Gui::Resizable mockPane2;
		splitPane.addPane(&mockPane1);
		splitPane.addPane(&mockPane2);

		splitPane.setSize(150, 90);
		splitPane.setSplitterPos(0, 10);

		YESUNIT_ASSERT(splitPane.getSplitter(0).pos == 10);

		splitPane.setSplitterPos(0, 0);
		YESUNIT_ASSERT(splitPane.getSplitter(0).pos == Gui::SplitPane::MIN_PANE_SIZE);

		splitPane.setSplitterPos(0, 200);
		YESUNIT_ASSERT(splitPane.getSplitter(0).pos == (splitPane.width - Gui::SplitPane::MIN_PANE_SIZE));

		Gui::Resizable mockPane3;
		splitPane.addPane(&mockPane3);
		YESUNIT_ASSERT(splitPane.getSplitter(0).pos ==  50);
		YESUNIT_ASSERT(splitPane.getSplitter(1).pos == 100);

		splitPane.setSplitterPos(0, 110);
		YESUNIT_ASSERT(splitPane.getSplitter(0).pos == 
			splitPane.getSplitter(1).pos - Gui::SplitPane::MIN_PANE_SIZE - 2);
	}

	void testsLayoutResizing(void)
	{
		Gui::SplitPane splitPane(Gui::Splitter::VERTICAL);

		splitPane.setSize(60, 50);

		YESUNIT_ASSERT(splitPane.width  == 60);
		YESUNIT_ASSERT(splitPane.height == 50);

		Gui::Resizable mockPane1;
		Gui::Resizable mockPane2;
		splitPane.addPane(&mockPane1);
		splitPane.addPane(&mockPane2);

		splitPane.setSize(100, 90);
		//YESUNIT_ASSERT(splitPane.getSplitter(0).pos == 50);

		splitPane.setSize(160, 90);
		//YESUNIT_ASSERT(splitPane.getSplitter(0).pos == 50);
	}

	void testsRemoveView(void)
	{
		Gui::SplitPane splitPane(Gui::Splitter::VERTICAL);

		Gui::Resizable mockPane1;
		Gui::Resizable mockPane2;
		splitPane.addPane(&mockPane1);
		splitPane.addPane(&mockPane2);

		splitPane.setSize(150, 90);

		splitPane.removePane(0);
		//YESUNIT_ASSERT(splitPane.getSplittersCount() == 0);
		//YESUNIT_ASSERT(splitPane.getPane(0).width    == 150);
	}

	void testsInsertView(void)
	{
		//Gui::SplitPane splitPane(Gui::Splitter::VERTICAL);

		//Gui::Resizable mockPane1;
		//Gui::Resizable mockPane2;
		//splitPane.insert(0, &mockPane1);
		//YESUNIT_ASSERT(splitPane.getPanesCount() == 1);

		//splitPane.insert(0, &mockPane2);
		//YESUNIT_ASSERT(splitPane.getPanesCount() == 2);
		//YESUNIT_ASSERT(splitPane.getPane(0) == &mockPane2);
	}
};