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

	wxStyledTextCtrl* getMainTextCtrl(){return _mainText;}
	wxStyledTextCtrl* getCurrentTextCtrl(){return _mainText;}

	void showFastFind(bool shown=true);
	bool fastFindShown()const{return _fastFindShown;}

	void showFastFindText();
	void showFastFindGoToLine();

	void findNext(int offset = 1);
	void findPrev(int offset = -1);

	void setTitle(const wxString& title);
	
protected:
	void CommonInit();

	wxAuiNotebook* getNotebook();
	
private:
	TextDocument* _document;
	wxStyledTextCtrl* _mainText;

	bool _fastFindShown;
	wxSizer* _fastFindSizer;
	wxSearchCtrl* _fastFindText;
	wxSpinCtrl*   _fastFindLine;
	wxBitmapButton* _fastFindClose;

	void onCloseFastFind(wxCommandEvent& event);
	void onFastFindText(wxCommandEvent& event);
	void onFastFindNext(wxCommandEvent& event);
	void onFastFindPrev(wxCommandEvent& event);
	void onFastFindLine(wxCommandEvent& event);
};

#endif // _TEXT_FRAME_HPP_
