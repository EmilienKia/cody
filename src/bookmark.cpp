/* -*- Mode: C++; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cody
 * Copyright (C) 2012-2013 Émilien Kia <emilien.kia@gmail.com>
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/listctrl.h>
#include <wx/stdpaths.h>
#include <wx/txtstrm.h>
#include <wx/wfstream.h>

#include "bookmark.hpp"

#include "text-document.hpp"
#include "text-frame.hpp"

//
//
// BookmarkList
//
//

void BookmarkList::insert(const Bookmark& bm)
{
    for(iterator it=begin(); it!=end(); it++)
    {
        if(it->line>bm.line)
        {
            List::insert(it, bm);
            return;
        }
    }
    List::insert(end(), bm);
}


void BookmarkList::insert(int line, const wxString& name)
{
    Bookmark bm = {line, name};
    insert(bm);
}


void BookmarkList::remove(int line)
{
    for(iterator it=begin(); it!=end(); it++)
    {
        if(it->line==line)
        {
            erase(it);
            break;
        }
    }
}

bool BookmarkList::has(int line)const
{
    for(const_iterator it=begin(); it!=end(); it++)
    {
        if(it->line==line)
            return true;
    }
    return false;
}

int BookmarkList::getNext(int line)const
{
    const_iterator iter = end();
    for(const_iterator it=begin(); it!=end(); it++)
    {
        if(it->line>line)
        {
            iter = it;
            break;
        }
    }
    if(iter!=end())
        return iter->line;
    else
        return wxNOT_FOUND;
}

int BookmarkList::getPrev(int line)const
{
    const_reverse_iterator iter = rend();
    for(const_reverse_iterator it=rbegin(); it!=rend(); it++)
    {
        if(it->line<line)
        {
            iter = it;
            break;
        }
    }
    if(iter!=rend())
        return iter->line;
    else
        return wxNOT_FOUND;
}

bool BookmarkList::addLines(int pos, int nb)
{
    bool modified = false;
    for(iterator iter=begin(); iter!=end(); iter++)
    {
        Bookmark& bm = *iter;
        if(bm.line>=pos)
        {
            bm.line += nb;
            modified = true;
        }
    }
    return modified;
}

//
//
// BookmarkProvider
//
//

wxIMPLEMENT_DYNAMIC_CLASS(BookmarkProvider, wxModule);

BookmarkProvider BookmarkProvider::s_bookmarks;


bool BookmarkProvider::OnInit()
{
    return load();
}

void BookmarkProvider::OnExit()
{
    save();
}

BookmarkList& BookmarkProvider::get(const wxString& file)
{
    return s_bookmarks[file];
}

bool BookmarkProvider::load()
{
    BookmarkList* list = NULL;

    wxString path = wxStandardPaths::Get().GetUserLocalDataDir();
    if(path.IsEmpty())
        return false;
    m_bookmarkFilePath = path +  wxFileName::GetPathSeparator() + "bookmarks.txt";

    if(wxFile::Exists(m_bookmarkFilePath) && wxFile::Access(m_bookmarkFilePath, wxFile::read))
    {
        wxFFileInputStream file(m_bookmarkFilePath);
        if(file.IsOk())
        {
            wxTextInputStream txt(file);
            wxString line;

            while(!file.Eof())
            {
                line = txt.ReadLine();
                if(!line.IsEmpty())
                {
                    if(line[0]=='>')
                    {
                        line.Remove(0,1);
                        line.Trim();
                        line.Trim(false);
                        if(!line.IsEmpty())
                        {
                            list = &get(line);
                        }
                    }
                    else if(list!=NULL)
                    {
                        wxString num = line.BeforeFirst(':');
                        Bookmark bm;
                        bm.line = 0;
                        num.ToLong(&bm.line);
                        if(bm.line>0)
                        {
                            bm.name = line.AfterFirst(':');
                            list->insert(bm);
                        }
                    }
                }
            }
        }
    }
    return true;
}

void BookmarkProvider::save()
{
    wxFFileOutputStream file(m_bookmarkFilePath);
    if(!file.IsOk())
        return;

    wxTextOutputStream text(file);

    for(iterator iter=s_bookmarks.begin(); iter!=s_bookmarks.end(); iter++)
    {
        BookmarkList& list = iter->second;
        text.WriteString(">" + iter->first + "\n");

        for(BookmarkList::iterator it=list.begin(); it!=list.end(); it++)
        {
            const Bookmark& bm = *it;
            text.Write32(bm.line);
            text.WriteString(" : " + bm.name + "\n");
        }
    }
    file.Close();
}


//
//
// BookmarkPanel
//
//

BEGIN_EVENT_TABLE(BookmarkPanel, wxPanel)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, BookmarkPanel::onItemActivate)
END_EVENT_TABLE()

BookmarkPanel::BookmarkPanel():
    wxPanel(),
    m_list(NULL)
{
}

BookmarkPanel::BookmarkPanel(wxWindow* parent, wxWindowID id):
    wxPanel(),
    m_list(NULL)
{
    Create(parent, id);
}


bool BookmarkPanel::Create(wxWindow* parent, wxWindowID id)
{
    if(!wxPanel::Create(parent, id))
        return false;

    m_list = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);

    wxSizer *sz = new wxBoxSizer(wxVERTICAL);
    sz->Add(m_list, 1, wxEXPAND);
    SetSizer(sz);

    SetMinSize(wxSize(48, 48));

    m_list->InsertColumn(0, _("Line"), wxLIST_FORMAT_LEFT, 48);
    m_list->InsertColumn(1, _("Name"), wxLIST_FORMAT_LEFT, 256);
    return true;
}

void BookmarkPanel::showItem(int line, const wxString& name)
{
    int i = m_list->InsertItem(m_list->GetItemCount(), wxString::Format(wxT("%d"), line+1));
    m_list->SetItem(i, 1, name);
    m_list->SetItemData(i, line);
}

void BookmarkPanel::setView(TextFrame* text)
{
    m_view = text;
    Update();
}

void BookmarkPanel::update()
{
    m_list->DeleteAllItems();
    if(m_view)
    {
        BookmarkList& list = BookmarkProvider::get(m_view->getDocument()->getFile());
        for(BookmarkList::iterator iter=list.begin(); iter!=list.end(); iter++)
            showItem(iter->line, iter->name);
    }
}

void BookmarkPanel::onItemActivate(wxListEvent& WXUNUSED(event))
{
    if(m_view)
    {
        long item = m_list->GetNextItem(wxID_ANY, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if(item!=wxNOT_FOUND)
        {
            m_view->gotoLine(m_list->GetItemData(item));
            m_view->setFocusToTextCtrl();
        }
    }
}
