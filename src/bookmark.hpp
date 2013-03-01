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

#ifndef _BOOKMARK_PANEL_HPP_
#define _BOOKMARK_PANEL_HPP_

#include <list>
#include <set>
#include <map>

class wxListEvent;
class wxListCtrl;

class TextFrame;

/**
 * A bookmark, a line number and a label.
 */
struct Bookmark
{
	long line;
	wxString name;
	
	bool operator<(const Bookmark& bm)const{return line<bm.line;}
};

/**
 * A list of bookmark
 */
class BookmarkList : public std::list<Bookmark>
{
	typedef std::list<Bookmark> List;
public:	
	void insert(const Bookmark& bm);
	void insert(int line, const wxString& name);
	
	void remove(int line);
	bool has(int line)const;
	
	int getNext(int line)const;
	int getPrev(int line)const;
	
	bool addLines(int pos, int nb);
	
};

/**
 * Bookmark provider.
 * Singleton allowing to load and save bookmark to conf.
 */
class BookmarkProvider : public wxModule, public std::map<wxString, BookmarkList>
{
	wxDECLARE_DYNAMIC_CLASS(BookmarkProvider);	
protected:
	static BookmarkProvider s_bookmarks;
	wxString m_bookmarkFilePath;

	bool load();
	void save();
	
public:
	BookmarkProvider(){}
	virtual bool OnInit();
	virtual void OnExit();
	
	static BookmarkList& get(const wxString& file);
};




class BookmarkPanel: public wxPanel 
{
	DECLARE_EVENT_TABLE()
public:
	BookmarkPanel();
	BookmarkPanel(wxWindow* parent, wxWindowID id = wxID_ANY);
	bool Create(wxWindow* parent, wxWindowID id);

	void setView(TextFrame* text);
	
	void update();
	
protected:
	void showItem(int line, const wxString& name);
	
private:
	void onItemActivate(wxListEvent& event);
	wxListCtrl* m_list;
	TextFrame*  m_view;
};

#endif // _BOOKMARK_PANEL_HPP_
