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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>

#include <wx/aui/auibook.h>
#include <wx/filename.h>

#include "text-document.hpp"

#include "text-frame.hpp"

TextDocument::TextDocument(const wxString& title):
_title(title),
_modified(false),
_frame(NULL)
{
}

TextFrame* TextDocument::createFrame(wxAuiNotebook* parent)
{
	if(_frame!=NULL)
		return _frame;
	if(parent==NULL)
		return NULL;
	
	_frame = new TextFrame(parent, this);
	_parent = parent;
	parent->AddPage(_frame, getTitle(), true);
}

wxStyledTextCtrl* TextDocument::getMainCtrl()
{
	if(!_frame)
		return NULL;
	return _frame->getMainTextCtrl();
}

void TextDocument::setTitle(const wxString& title)
{
	_title = title;
	
	if(getFrame())
	{
		getFrame()->setTitle(getTitle());
	}
}

void TextDocument::setTitleFromFile(const wxString& file)
{
	wxFileName name(file);
	setTitle(name.GetFullName());
}

bool TextDocument::loadFile(const wxString& file)
{
	wxStyledTextCtrl* txt = getMainCtrl();
	if(txt)
	{
		txt->ClearAll();
		txt->LoadFile(file);
		txt->ClearSelections();
		_file = file;
		setTitleFromFile(_file);
		return true;
	}
	return false;
}

bool TextDocument::reloadFile()
{
	wxStyledTextCtrl* txt = getMainCtrl();
	if(txt && !_file.IsEmpty())
	{
		txt->ClearAll();
		txt->LoadFile(_file);
		txt->ClearSelections();
		return true;
	}
	return false;	
}

bool TextDocument::saveFile()
{
	wxStyledTextCtrl* txt = getMainCtrl();
	if(txt && !_file.IsEmpty())
	{
		return txt->SaveFile(_file);
	}
	return false;
}

bool TextDocument::saveFileAs(const wxString& file)
{
	_file = file;
	return saveFile();
}
