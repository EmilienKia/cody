/* -*- Mode: C++; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*-  */
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>

#include "markbar.hpp"

wxDEFINE_EVENT(MARKBAR_ACTIVATED, wxMarkBarEvent);

wxMarkBarEvent::wxMarkBarEvent():
    wxCommandEvent(MARKBAR_ACTIVATED, wxNOT_FOUND)
{
}

wxMarkBarEvent::wxMarkBarEvent(int id, int markid, int pos, int category):
    wxCommandEvent(MARKBAR_ACTIVATED, id),
    markid(markid),
    pos(pos),
    category(category)
{
}


wxMarkBarEvent::wxMarkBarEvent(const wxMarkBarEvent& event):
    wxCommandEvent(MARKBAR_ACTIVATED, event.GetId()),
    markid(event.markid),
    pos(event.pos),
    category(event.category)
{
}




IMPLEMENT_DYNAMIC_CLASS(wxMarkBar, wxControl)

BEGIN_EVENT_TABLE(wxMarkBar, wxControl)
    EVT_PAINT(wxMarkBar::OnPaint)
    EVT_SET_CURSOR(wxMarkBar::OnSetCursor)
    EVT_MOTION(wxMarkBar::OnMouseMove)
    EVT_ENTER_WINDOW(wxMarkBar::OnMouseEnter)
    EVT_LEAVE_WINDOW(wxMarkBar::OnMouseLeave)
    EVT_LEFT_UP(wxMarkBar::OnMouseLeftUp)
END_EVENT_TABLE()


wxMarkBar::wxMarkBar():
    wxControl(),
    min(0),
    max(100),
    idxCount(0),
    shownCategories(0),
    focused(markers.end())
{
}

wxMarkBar::wxMarkBar(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name):
    wxControl(),
    min(0),
    max(100),
    idxCount(0),
    shownCategories(0),
    focused(markers.end())
{
    Create(parent, id, pos, size, style, name);
}

wxMarkBar::wxMarkBar(wxWindow *parent, wxWindowID id, int minValue, int maxValue, const wxPoint &pos, const wxSize &size, long style, const wxString &name):
    wxControl(),
    min(minValue),
    max(maxValue),
    idxCount(0),
    shownCategories(0),
    focused(markers.end())
{
    Create(parent, id, pos, size, style, name);
}

bool wxMarkBar::Create(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    return Create(parent, id, 0, 1, pos, size, style, name);
}

bool wxMarkBar::Create(wxWindow *parent, wxWindowID id, int minValue, int maxValue, const wxPoint &pos, const wxSize &size, long style, const wxString &name)
{
    // Set vertical style by default
    if((style&(MB_VERTICAL|MB_HORIZONTAL))==(MB_VERTICAL|MB_HORIZONTAL))
        style &= ~(MB_VERTICAL|MB_HORIZONTAL);
    if((style&(MB_VERTICAL|MB_HORIZONTAL))==0)
        style |= MB_VERTICAL;

    this->min = minValue;
    this->max = maxValue;

    if(!wxWindow::Create(parent, id, pos, size, style, name))
        return false;

    if(style&MB_VERTICAL)
    {
        if(size.x>0)
            SetMinSize(wxSize(size.x, 0));
    }
    else
    {
        if(size.y>0)
            SetMinSize(wxSize(0, size.y));
    }

    SetCategory(0, wxColour(192, 192, 192), wxColour(192, 192, 192, 128));

    return true;
}

wxMarkBar::~wxMarkBar()
{
}

void wxMarkBar::SetOrientation(int orient)
{
    if((orient&(MB_VERTICAL|MB_HORIZONTAL))==(MB_VERTICAL|MB_HORIZONTAL))
        orient &= ~(MB_VERTICAL|MB_HORIZONTAL);
    if((orient&(MB_VERTICAL|MB_HORIZONTAL))==0)
        orient |= MB_VERTICAL;

    SetWindowStyle( (GetWindowStyle() & ~(MB_VERTICAL|MB_HORIZONTAL)) | orient);
    Refresh();
}

void wxMarkBar::SetMin(int min)
{
    this->min = min;
    Refresh();
}

void wxMarkBar::SetMax(int max)
{
    this->max = max;
    Refresh();
}

void wxMarkBar::Clear()
{
    markers.clear();
    idxCount = 0;
    Refresh();
}

int wxMarkBar::AddMarker(int pos, const wxString& name, int category)
{
    wxMarkBarMarker mark = {idxCount++, pos, category>=0&&category<=(sizeof(int)*8)?category:0, name};

    markerList::iterator it = markers.begin();
    for(; it!=markers.end(); it++)
        if(it->pos>=pos)
            break;
    markers.insert(it, mark);
    Refresh();
    return mark.idx;
}

void wxMarkBar::RemoveMarker(int index)
{
    markerList::iterator it = find(index);
    if(it!=markers.end())
        markers.erase(it);
    Refresh();
}

void wxMarkBar::RemoveMarker(int pos, int category)
{
    markerList::iterator it = markers.begin();
    for(; it!=markers.end(); it++)
    {
        if(it->category==category && it->pos==pos)
        {
            markers.erase(it);
            Refresh();
            return;
        }
    }
}

void wxMarkBar::RemoveCategoryMarker(int category)
{
    markerList::iterator it1, it2;
    for(it1=it2=markers.begin(); it1!=markers.end(); it1=it2)
    {
        it2++;
        if(it1->category==category)
            markers.erase(it1);
    }
    Refresh();
}

void wxMarkBar::SetMarkerPosition(int idx, int pos)
{
    markerList::iterator it = find(idx);
    if(it!=markers.end())
        it->pos = pos;
    Refresh();
}

void wxMarkBar::SetMarkerName(int idx, const wxString& name)
{
    markerList::iterator it = find(idx);
    if(it!=markers.end())
        it->name = name;
}

void wxMarkBar::SetMarkerCategory(int idx, int category)
{
    markerList::iterator it = find(idx);
    if(it!=markers.end())
        it->category = category;
    Refresh();
}

int wxMarkBar::GetMarkerPosition(int idx)const
{
    markerList::const_iterator it = find(idx);
    if(it!=markers.end())
        return it->pos;
    return -1;
}

wxString wxMarkBar::GetMarkerName(int idx)const
{
    markerList::const_iterator it = find(idx);
    if(it!=markers.end())
        return it->name;
    return wxT("");
}

int wxMarkBar::GetMarkerCategory(int idx)const
{
    markerList::const_iterator it = find(idx);
    if(it!=markers.end())
        return it->category;
    return -1;
}

wxMarkBar::markerList::const_iterator wxMarkBar::find(int index)const
{
    markerList::const_iterator it = markers.begin();
    for(; it!=markers.end(); it++)
        if(it->idx==index)
            break;
    return it;
}

wxMarkBar::markerList::iterator wxMarkBar::find(int index)
{
    markerList::iterator it = markers.begin();
    for(; it!=markers.end(); it++)
        if(it->idx==index)
            break;
    return it;
}

// sz.y    | yreal
// --------+--------
// max-min | pos-min

#define WXMARKBAR_CALCPOSITION(clientsize, pos, min, max) (clientsize*(pos-min)/(max-min))
#define WXMARKBAR_MARK_WIDTH	2

void wxMarkBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxSize sz = GetClientSize();
    wxPaintDC dc(this);

    if((GetWindowStyle()&MB_VERTICAL)!=0)
    {
        size_t count = 0;
        for(pageArray::reverse_iterator it=pages.rbegin(); it!=pages.rend(); it++)
        {
            wxMarkBarPage& page = *it;
            int start = WXMARKBAR_CALCPOSITION(sz.y, page.start, min, max);
            int end   = WXMARKBAR_CALCPOSITION(sz.y, page.end, min, max);
            wxPen pen(page.color);
            wxDCPenChanger pc(dc, pen);
            int x = sz.x - (2*count++) -1;
            dc.DrawLine(x, start, x, end);
            dc.DrawLine(x-1, start, x-1, end);
        }
        for(markerList::iterator it=markers.begin(); it!=markers.end(); it++)
        {
            if(InternalIsCategoryShown(it->category))
            {
                int y = WXMARKBAR_CALCPOSITION(sz.y, it->pos, min, max);
                paintItemVertical(dc, *it, y, sz.x);
            }
        }
    }
    else
    {
        size_t count = 0;
        for(pageArray::reverse_iterator it=pages.rbegin(); it!=pages.rend(); it++)
        {
            wxMarkBarPage& page = *it;
            int start = WXMARKBAR_CALCPOSITION(sz.x, page.start, min, max);
            int end   = WXMARKBAR_CALCPOSITION(sz.x, page.end, min, max);
            wxPen pen(page.color);
            wxDCPenChanger pc(dc, pen);
            int y = sz.y - (2*count++) -1;
            dc.DrawLine(start, y, end, y);
            dc.DrawLine(start, y-1, end, y-1);
        }
        for(markerList::iterator it=markers.begin(); it!=markers.end(); it++)
        {
            if(InternalIsCategoryShown(it->category))
            {
                int x = WXMARKBAR_CALCPOSITION(sz.x, it->pos, min, max);
                paintItemHorizontal(dc, *it, x, sz.y);
            }
        }
    }
}

void wxMarkBar::paintItemVertical(wxDC& dc, wxMarkBarMarker& mark, int realpos, int clientwidth)
{
    wxPen   pen(categories[mark.category].fore);
    wxBrush brush(categories[mark.category].back);
    wxDCPenChanger   pc(dc, pen);
    wxDCBrushChanger bc(dc, brush);
    dc.DrawRectangle(1, realpos-WXMARKBAR_MARK_WIDTH, clientwidth-2, WXMARKBAR_MARK_WIDTH*2+1);
}

void wxMarkBar::paintItemHorizontal(wxDC& dc, wxMarkBarMarker& mark, int realpos, int clientheight)
{
    wxPen   pen(categories[mark.category].fore);
    wxBrush brush(categories[mark.category].back);
    wxDCPenChanger   pc(dc, pen);
    wxDCBrushChanger bc(dc, brush);
    dc.DrawRectangle(realpos-WXMARKBAR_MARK_WIDTH, 1, WXMARKBAR_MARK_WIDTH*2+1, clientheight-2);
}

void wxMarkBar::SetCategory(int category, wxColour fore, wxColour back, bool activable)
{
    if(category<0 ||category>(sizeof(int)*8))
        return;

    categories[category].fore = fore;
    categories[category].back = back;
    categories[category].activable = activable;

    shownCategories |= 1<<category;
}

void wxMarkBar::ShowCategory(int category, bool show)
{
    if(category<0 ||category>(sizeof(int)*8))
        return;

    if(show)
        shownCategories |= 1<<category;
    else
        shownCategories &= ~(1<<category);
}

void wxMarkBar::OnMouseEnter(wxMouseEvent& event)
{
    focused = InternalHitTest(event.GetPosition());
    if(focused!=markers.end())
        SetToolTip(focused->name);
}

void wxMarkBar::OnMouseLeave(wxMouseEvent& WXUNUSED(event))
{
    focused = markers.end();
    UnsetToolTip();
}

void wxMarkBar::OnMouseMove(wxMouseEvent& event)
{
    focused = InternalHitTest(event.GetPosition());
    if(focused!=markers.end())
        SetToolTip(focused->name);
    else
        UnsetToolTip();
}

void wxMarkBar::OnMouseLeftUp(wxMouseEvent& WXUNUSED(event))
{
    if(focused!=markers.end())
    {
        if(categories[focused->category].activable)
        {
            wxMarkBarEvent evt(GetId(), focused->idx, focused->pos, focused->category);
            ProcessEvent(evt);
        }
    }
}

wxMarkBar::markerList::const_iterator wxMarkBar::InternalHitTest(const wxPoint& pt)const
{
    wxSize csz = GetClientSize();
    if(GetOrientation()==wxVERTICAL)
    {
        for(markerList::const_iterator it=markers.begin(); it!=markers.end(); it++)
        {
            if(InternalIsCategoryShown(it->category))
            {
                int pos = WXMARKBAR_CALCPOSITION(csz.y, it->pos, min, max);
                if(pt.y>= pos-WXMARKBAR_MARK_WIDTH && pt.y<= pos+WXMARKBAR_MARK_WIDTH)
                    return it;
            }
        }
    }
    else
    {
        for(markerList::const_iterator it=markers.begin(); it!=markers.end(); it++)
        {
            if(InternalIsCategoryShown(it->category))
            {
                int pos = WXMARKBAR_CALCPOSITION(csz.x, it->pos, min, max);
                if(pt.x>= pos-WXMARKBAR_MARK_WIDTH && pt.x<= pos+WXMARKBAR_MARK_WIDTH)
                    return it;
            }
        }
    }
    return markers.end();
}

int wxMarkBar::HitTest(const wxPoint& pt)const
{
    markerList::const_iterator it = InternalHitTest(pt);
    if(it!=markers.end())
        return it->idx;
    else
        return wxNOT_FOUND;
}

void wxMarkBar::OnSetCursor(wxSetCursorEvent& event)
{
    if(focused==markers.end())
        event.SetCursor(wxCursor(wxCURSOR_ARROW));
    else if(categories[focused->category].activable)
        event.SetCursor(wxCursor(wxCURSOR_HAND));
    else
        event.SetCursor(wxCursor(wxCURSOR_ARROW));
}

void wxMarkBar::MoveMarkers(int pos, int offset)
{
    for(markerList::iterator it=markers.begin(); it!=markers.end(); it++)
    {
        if(it->pos>=pos)
            it->pos+=offset;
    }
}

int wxMarkBar::AddPage(int start, int stop, wxColour color)
{
    wxMarkBarPage page = {start, stop, color};
    pages.push_back(page);
    return pages.size()-1;
}

void wxMarkBar::RemPage(int id)
{
    pages.erase(pages.begin()+id);
}

void wxMarkBar::SetPage(int id, int start, int end)
{
    wxMarkBarPage &page = pages[id];
    page.start = start;
    page.end  = end;
}

void wxMarkBar::SetPage(int id, wxColour color)
{
    pages[id].color = color;
}

size_t wxMarkBar::GetPageCount()const
{
    return pages.size();
}
