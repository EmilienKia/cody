/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * wxMarkBar
 * Copyright (C) 2013 Émilien KIA <emilien.kia@gmail.com>
 * 
 * wxMarkBar is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * wxMarkBar is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.";
 */

#ifndef _MARKBAR_HPP_
#define _MARKBAR_HPP_

#include <wx/defs.h>
#include <wx/control.h>

#include <list>

#define MB_HORIZONTAL	wxHORIZONTAL
#define MB_VERTICAL		wxVERTICAL

#define MB_DEFAULT_CATEGORY	-1

/**
 * Mark bar control.
 */
class wxMarkBar: public wxControl 
{
	DECLARE_EVENT_TABLE()
	DECLARE_DYNAMIC_CLASS(wxMarkBar)
public:
	wxMarkBar();
	wxMarkBar(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=MB_VERTICAL, const wxString &name=wxT("wxMarkCtrl"));
	wxMarkBar(wxWindow *parent, wxWindowID id, int minValue, int maxValue, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=MB_VERTICAL, const wxString &name=wxT("wxMarkCtrl"));
	virtual bool Create(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=MB_VERTICAL, const wxString &name=wxT("wxMarkCtrl"));
	virtual bool Create(wxWindow *parent, wxWindowID id, int minValue, int maxValue, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=MB_VERTICAL, const wxString &name=wxT("wxMarkCtrl"));
	virtual ~wxMarkBar();

	int GetMin()const{return min;}
	int GetMax()const{return max;}
	void SetMin(int min);
	void SetMax(int max);

	int  GetOrientation()const{return GetWindowStyle() & (MB_VERTICAL|MB_HORIZONTAL);}
	void SetOrientation(int orient);

	void Clear();
	int AddMarker(int pos, const wxString& name=wxT(""), int category=MB_DEFAULT_CATEGORY);
	void RemoveMarker(int pos, int category);
	void RemoveMarker(int index);
	void RemoveCategoryMarker(int category);
	void SetMarkerPosition(int idx, int pos);
	void SetMarkerName(int idx, const wxString& name);
	void SetMarkerCategory(int idx, int category);
	int GetMarkerPosition(int idx)const;
	wxString GetMarkerName(int idx)const;
	int GetMarkerCategory(int idx)const;

	void SetCategory(int category, wxColour fore, wxColour back, bool activable=true);
	void ShowCategory(int category, bool show = true);
	void HideCategory(int category){ShowCategory(category, false);}
	bool IsCategoryShown(int category)const{return category>=0&&category<=(sizeof(int)*8)?InternalIsCategoryShown(category):false;}

	int HitTest(const wxPoint& pt)const;

	void MoveMarkers(int pos, int offset);


	int AddPage(int start, int stop, wxColour color);
	void RemPage(int id);
	void SetPage(int id, int start, int end);
	void SetPage(int id, wxColour color);
	size_t GetPageCount()const;
	
protected:
	struct wxMarkBarMarker{
		int idx, pos, category;
		wxString name;
	};

	int min, max;
	int idxCount;

	typedef std::list<wxMarkBarMarker> markerList;
	markerList markers;
	markerList::const_iterator focused;

	struct wxMarkBarCategory{
		wxColour fore, back;
		bool activable;
	};
	wxMarkBarCategory categories[sizeof(int)*8];
	int shownCategories;

	markerList::const_iterator find(int index)const;
	markerList::iterator find(int index);
	bool InternalIsCategoryShown(int category)const{return shownCategories&(1<<category)!=0;}


	struct wxMarkBarPage{
		int start, end;
		wxColour color;
	};
	typedef std::vector<wxMarkBarPage> pageArray;
	pageArray pages;
	
	void OnSetCursor(wxSetCursorEvent& event);
	void OnMouseEnter(wxMouseEvent& event);
	void OnMouseLeave(wxMouseEvent& event);
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseLeftUp(wxMouseEvent& event);
	void OnPaint(wxPaintEvent& event);
	
	void paintItemVertical(wxDC& dc, wxMarkBarMarker& mark, int realpos, int clientwidth);
	void paintItemHorizontal(wxDC& dc, wxMarkBarMarker& mark, int realpos, int clientheight);
	markerList::const_iterator InternalHitTest(const wxPoint& pt)const;
};

class wxMarkBarEvent : public wxCommandEvent
{
public:
	wxMarkBarEvent();
	wxMarkBarEvent(int id, int markid, int pos, int category);
	wxMarkBarEvent(const wxMarkBarEvent& event);
	virtual wxEvent* Clone ()const{return new wxMarkBarEvent(*this);}

	int getMarkId()const{return markid;}
	int getPosition()const{return pos;}
	int getCategory()const{return category;}

	void setMarkId(int markid){this->markid=markid;}
	void setPosition(int pos){this->pos=pos;}
	void setCategory(int category){this->category=category;}
	
protected:
	int markid;
	int pos;
	int category;
};

wxDECLARE_EVENT(MARKBAR_ACTIVATED, wxMarkBarEvent);

#define wxMarkBarEventHandler(func) (&func)
#define MARKBAR_CLICK(id, func) wx__DECLARE_EVT1(MARKBAR_ACTIVATED, id, wxMarkBarEventHandler(func))


#endif // _MARKBAR_HPP_

