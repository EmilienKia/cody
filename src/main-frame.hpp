/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cody
 * Copyright (C) Émilien Kia 2012 <emilien@emilien-vm-lm-maya>
 * 
cody is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cody is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _MAIN_FRAME_HPP_
#define _MAIN_FRAME_HPP_

#include <wx/aui/auibook.h>
#include <wx/ribbon/bar.h>

class CodyApp;
class TextFrame;
class TextDocument;
class wxRibbonButtonBarEvent;
class wxStyledTextCtrl;
class BookmarkPanel;


class MainFrame: public wxFrame 
{
	friend class CodyApp;
	DECLARE_EVENT_TABLE();
	
public:
	MainFrame();

	wxStyledTextCtrl* getCurrentTextCtrl();
	TextFrame* getCurrentTextFrame();
	TextDocument* getCurrentDocument();
	BookmarkPanel* getBookmarkPanel(){return _bookmark;}
	
	virtual bool Destroy();

	void toggleBookmarkPanel();
	
protected:
	void CommonInit();
	void InitRibbon();
	void InitAcceleratorTable();

	wxAuiNotebook* getNotebook(){return _notebook;}

private:
	wxPanel*	   _panel;
	wxAuiManager   _manager;
	wxRibbonBar*   _ribbon;
	wxAuiNotebook* _notebook;
	BookmarkPanel* _bookmark;

	wxMenu*        _recentFileMenu;

	void onRibbonButtonClicked(wxEvent& event);
	
	void onPageClosing(wxAuiNotebookEvent& event);
	void onPageChanged(wxAuiNotebookEvent& event);

	void onNewDocument(wxCommandEvent& event);
	void onOpenDocument(wxCommandEvent& event);
	void onRecentDocumentMenu(wxRibbonButtonBarEvent& event);
	void onRecentDocument(wxCommandEvent& event);	
	void onRevertDocument(wxCommandEvent& event);
	void onSaveDocument(wxCommandEvent& event);
	void onSaveDocumentAs(wxCommandEvent& event);
	void onCloseDocument(wxCommandEvent& event);
	void onCloseAllDocuments(wxCommandEvent& event);

	void onUndo(wxCommandEvent& event);
	void onRedo(wxCommandEvent& event);
	void onDelete(wxCommandEvent& event);
	void onCut(wxCommandEvent& event);
	void onCopy(wxCommandEvent& event);
	void onPaste(wxCommandEvent& event);

	void onFindRibbonBarExtActivated(wxRibbonPanelEvent& event);	
	void onFind(wxCommandEvent& event);
	void onFindNext(wxCommandEvent& event);
	void onFindPrev(wxCommandEvent& event);
	void onGoToLine(wxCommandEvent& event);

	void onUpdateHasOpenDocument(wxUpdateUIEvent& event);
	void onUpdateCanUndo(wxUpdateUIEvent& event);
	void onUpdateCanRedo(wxUpdateUIEvent& event);
	void onUpdateCanPaste(wxUpdateUIEvent& event);
	void onUpdateHasSelection(wxUpdateUIEvent& event);

	void onBookmarkRibbonBarExtActivated(wxRibbonPanelEvent& event);
	void onToggleBookmark(wxCommandEvent& event);
	void onPreviousBookmark(wxCommandEvent& event);
	void onNextBookmark(wxCommandEvent& event);

	void onDisplayLineNumber(wxCommandEvent& event);
	void onUpdateDisplayLineNumber(wxUpdateUIEvent& event);
	void onDisplayMarkers(wxCommandEvent& event);
	void onUpdateDisplayMarkers(wxUpdateUIEvent& event);
	void onDisplayFolders(wxCommandEvent& event);
	void onUpdateDisplayFolders(wxUpdateUIEvent& event);
	
	void onDisplayCaretLine(wxCommandEvent& event);
	void onUpdateDisplayCaretLine(wxUpdateUIEvent& event);
	void onDisplayWhiteSpaces(wxCommandEvent& event);
	void onUpdateDisplayWhiteSpaces(wxUpdateUIEvent& event);
	void onDisplayIndentationGuides(wxCommandEvent& event);
	void onUpdateDisplayIndentationGuides(wxUpdateUIEvent& event);
	void onDisplayEOL(wxCommandEvent& event);
	void onUpdateDisplayEOL(wxUpdateUIEvent& event);
	void onDisplayLongLines(wxCommandEvent& event);
	void onUpdateDisplayLongLines(wxUpdateUIEvent& event);
	void onDisplayWrapLongLines(wxCommandEvent& event);
	void onUpdateDisplayWrapLongLines(wxUpdateUIEvent& event);

	void onSplitView(wxCommandEvent& event);
	void onUpdateSplitView(wxUpdateUIEvent& event);
	void onSwapView(wxCommandEvent& event);
	void onUpdateSwapView(wxUpdateUIEvent& event);
	
	void onZoomIn(wxCommandEvent& event);
	void onZoomOut(wxCommandEvent& event);
	void onZoomReset(wxCommandEvent& event);
	
};

#endif // _MAIN_FRAME_HPP_
