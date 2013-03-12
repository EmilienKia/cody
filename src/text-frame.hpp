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
class wxMarkBar;
class wxMarkBarEvent;
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
		
	wxStyledTextCtrl* getMainTextCtrl()const{return _mainText;}
	wxStyledTextCtrl* getCurrentTextCtrl()const{return _mainText;}

	MainFrame* getMainFrame();

	void showFastFind(bool shown=true);
	bool fastFindShown()const{return _fastFindShown;}

	void showFastFindText();
	void showFastFindGoToLine();

	void findNext(int offset = 1);
	void findPrev(int offset = -1);

	void setTitle(const wxString& title);

	void showLineNumbers(bool show = true);
	bool lineNumbersShown()const;
	void showMarkers(bool show = true);
	bool markersShown()const;
	void showFolders(bool show = true);
	bool foldersShown()const;	

	void showCaretLine(bool show = true);
	bool caretLineShown()const;
	void showWhiteSpaces(bool show = true);
	bool whiteSpacesShown()const;
	void showIndentationGuides(bool show = true);
	bool indentationGuidesShown()const;
	void showEOL(bool show = true);
	bool EOLShown()const;
	void showLongLines(bool show = true);
	bool longLinesShown()const;
	void wrapLongLines(bool wrap = true);
	bool longLinesWrapped()const;

	void setZoom(int scale=0);
	int getScale()const;
	void zoomIn();
	void zoomOut();
	
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

	enum MARGIN_ID{
		MARGIN_LINE_NB,
		MARGIN_MARKER,
		MARGIN_FOLD
	};

	void showMargin(unsigned id, bool shown=true);
	bool marginShown(unsigned id)const;
	
	enum MARKER_ID{
		TEXT_MARKER_BOOKMARK,
		TEXT_MARKER_SEARCH
	};

	void UpdateBookmarkPanel();

	void setMarkerStyle(int id, const wxBitmap &bmp, const wxColour& fore, const wxColour& back);
	void setMarkerStyle(int id, int predefStyle, const wxColour& fore, const wxColour& back);
	void addMarker(int id, const wxString& name, int line=wxNOT_FOUND);
	void remMarker(int id, int line=wxNOT_FOUND);
	void remMarkers(int id);

private:
	TextDocument* _document;
	wxStyledTextCtrl* _mainText;

	bool _fastFindShown;
	wxSizer* _fastFindSizer;
	wxSearchCtrl* _fastFindText;
	wxSpinCtrl*   _fastFindLine;
	wxBitmapButton* _fastFindClose;
	wxMarkBar*  _markbar;

	unsigned int _marginShown;

	void updateLineNbMargin();
	
	void onCloseFastFind(wxCommandEvent& event);
	void onFastFindText(wxCommandEvent& event);
	void onFastFindNext(wxCommandEvent& event);
	void onFastFindPrev(wxCommandEvent& event);
	void onFastFindLine(wxCommandEvent& event);

	void OnTextModified(wxStyledTextEvent& event);
	void onUpdateUI(wxStyledTextEvent& event);

	void onMarkerActivated(wxMarkBarEvent& event);
	
	void onSelectionChanged();
};

#endif // _TEXT_FRAME_HPP_
