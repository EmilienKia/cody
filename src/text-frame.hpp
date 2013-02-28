/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cody
 * Copyright (C) Émilien Kia 2012 <emilien.kia@gmail.com>
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

#ifndef _TEXT_FRAME_HPP_
#define _TEXT_FRAME_HPP_

#include <wx/stc/stc.h>

class TextDocument;
class MainFrame;

class wxAuiNotebook;
class wxSearchCtrl;
class wxSpinCtrl;

class TextFrame: public wxPanel
{
	DECLARE_EVENT_TABLE()
public:
	TextFrame(wxWindow* parent, TextDocument* doc);

	TextDocument* getDocument(){return _document;}
	void setDocument(TextDocument* doc){_document = doc;}
	
	void initAfterLoading();
		
	wxStyledTextCtrl* getMainTextCtrl(){return _mainText;}
	wxStyledTextCtrl* getCurrentTextCtrl(){return _mainText;}

	MainFrame* getMainFrame();

	void showFastFind(bool shown=true);
	bool fastFindShown()const{return _fastFindShown;}

	void showFastFindText();
	void showFastFindGoToLine();

	void findNext(int offset = 1);
	void findPrev(int offset = -1);

	void setTitle(const wxString& title);

	void showLineNumbers(bool show = true);

	void toggleBookmark();
	void addBookmark(int line=wxNOT_FOUND, wxString name=wxT(""));
	void remBookmark(int line=wxNOT_FOUND);
	void clearBookmarks();
	void addBookmarksFromProvider();
	void gotoPrevBookmark();
	void gotoNextBookmark();
	void gotoLine(int line);

	void setFocusToTextCtrl();

	
	
protected:
	void CommonInit();
	void InitTextCtrl(wxStyledTextCtrl* txt);

	wxAuiNotebook* getNotebook();

	enum {
		LineNrID,
		DividerID,
		FoldingID
	};

	void UpdateBookmarkPanel();
	
private:
	TextDocument* _document;
	wxStyledTextCtrl* _mainText;

	bool _fastFindShown;
	wxSizer* _fastFindSizer;
	wxSearchCtrl* _fastFindText;
	wxSpinCtrl*   _fastFindLine;
	wxBitmapButton* _fastFindClose;

	// margin variables
	bool _lineNrMarginShown;
	int _foldingMargin;

	void updateLineNbMargin();
	
	void onCloseFastFind(wxCommandEvent& event);
	void onFastFindText(wxCommandEvent& event);
	void onFastFindNext(wxCommandEvent& event);
	void onFastFindPrev(wxCommandEvent& event);
	void onFastFindLine(wxCommandEvent& event);

	void OnTextModified(wxStyledTextEvent& event);
	void onUpdateUI(wxStyledTextEvent& event);

	void onSelectionChanged();
};

#endif // _TEXT_FRAME_HPP_
