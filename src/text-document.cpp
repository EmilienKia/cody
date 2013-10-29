/* -*- Mode: C++; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cody
 * Copyright (C) 2012-2013 Émilien KIA <emilien.kia@gmail.com>
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

#include "cody-app.hpp"
#include "decls.hpp"
#include "text-frame.hpp"
#include "bookmark.hpp"

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
	setEOLMode((TextDocument::EOLMode)wxGetApp().getConfig()->ReadLong(CONFPATH_EDITOR_DEFAULTENDOFLINES, CONFDEFAULT_EDITOR_DEFAULTENDOFLINES));
    return _frame;
}

wxStyledTextCtrl* TextDocument::getMainCtrl()const
{
    if(!_frame)
        return NULL;
    return _frame->getMainTextCtrl();
}

void TextDocument::setModified(bool modified)
{
    if(_modified != modified)
    {
        _modified = modified;
        updateTitle();
    }
}

void TextDocument::setTitle(const wxString& title)
{
    _title = title;
    updateTitle();
}

void TextDocument::setTitleFromFile(const wxString& file)
{
    wxFileName name(file);
    setTitle(name.GetFullName());
}

void TextDocument::updateTitle()
{
    if(getFrame())
    {
        if(isModified())
            getFrame()->setTitle("* " + getTitle());
        else
            getFrame()->setTitle(getTitle());
    }
}

bool TextDocument::loadFile(const wxString& file)
{
    wxStyledTextCtrl* txt = getMainCtrl();
    if(txt)
    {
        txt->ClearAll();
        txt->LoadFile(file);
        txt->ClearSelections();
        txt->SetSavePoint();
        _file = file;
        setModified(false);
        setTitleFromFile(_file);
        getFrame()->initAfterLoading();
		setEOLMode((TextDocument::EOLMode)wxGetApp().getConfig()->ReadLong(CONFPATH_EDITOR_DEFAULTENDOFLINES, CONFDEFAULT_EDITOR_DEFAULTENDOFLINES));
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
        txt->SetSavePoint();
        setModified(false);
        getFrame()->initAfterLoading();
		setEOLMode((TextDocument::EOLMode)wxGetApp().getConfig()->ReadLong(CONFPATH_EDITOR_DEFAULTENDOFLINES, CONFDEFAULT_EDITOR_DEFAULTENDOFLINES));
        return true;
    }
    return false;
}

bool TextDocument::saveFile()
{
    wxStyledTextCtrl* txt = getMainCtrl();
    if(!txt)
        return false;

    if(_file.IsEmpty())
    {
        wxString file  = wxSaveFileSelector("", "*", getFile());
        if(file.IsEmpty())
            return false;

        _file = file;
    }
    if(txt->SaveFile(_file))
    {
        txt->SetSavePoint();
        setModified(false);
        return true;
    }
    else
    {
        return false;
    }
}

bool TextDocument::saveFileAs(const wxString& file)
{
    _file = file;
    return saveFile();
}

BookmarkList& TextDocument::getBookmarks()
{
    return BookmarkProvider::get(getFile());
}

void TextDocument::setDocumentType(int doctype)
{
    _docType = doctype;

    // Update frame styles
    TextFrame* frame = getFrame();
    if(frame)
    {
        frame->applyFileType();
    }
}

const FileType& TextDocument::getDocFileType()const
{
    return FileTypeManager::get().getFileType(getDocumentType());
}

TextDocument::EOLMode TextDocument::getEOLMode() const
{
  wxStyledTextCtrl* ctrl = getMainCtrl();
  if(ctrl)
    return (EOLMode) ctrl->GetEOLMode();
  else
    return EOL_DEFAULT;
}

void TextDocument::setEOLMode(TextDocument::EOLMode mode)
{
  wxStyledTextCtrl* ctrl = getMainCtrl();
  if(ctrl)
    ctrl->SetEOLMode((int)mode);
}

void TextDocument::convertEOL(EOLMode mode)
{
  wxStyledTextCtrl* ctrl = getMainCtrl();
  if(ctrl)
    ctrl->ConvertEOLs(mode);
}

bool TextDocument::useTabs() const
{
  wxStyledTextCtrl* ctrl = getMainCtrl();
  if(ctrl)
    return ctrl->GetUseTabs();
  else
    return false;
}

void TextDocument::useTabs(bool use)
{
  wxStyledTextCtrl* ctrl = getMainCtrl();
  if(ctrl)
    ctrl->SetUseTabs(use);
}

int TextDocument::getIndent() const
{
  wxStyledTextCtrl* ctrl = getMainCtrl();
  if(ctrl)
    return ctrl->GetIndent();
  else
    return 2;
}

void TextDocument::setIndent(int indent)
{
  wxStyledTextCtrl* ctrl = getMainCtrl();
  if(ctrl)
  {
    ctrl->SetIndent(indent);
    ctrl->SetTabWidth(indent);
  }
}

wxString TextDocument::createIndentation(int indent, int tabSize, bool insertSpaces)
{
  wxString indentation;
  if(!insertSpaces)
  {
    while(indent >= tabSize)
    {
      indentation.Append("\t");
      indent -= tabSize;
    }
  }
  while(indent-- > 0)
  {
    indentation.Append(" ");
  }
  return indentation;
}

void TextDocument::convertIndent()
{
  wxStyledTextCtrl* ctrl = getMainCtrl();
  if(ctrl)
  {
    // Note: this code is largely inspirated from SciTE code by Neil Hodgson <neilh@scintilla.org>
    bool usetabs = useTabs();
    int tabsize  = getIndent();

    ctrl->BeginUndoAction();

    int maxLine = ctrl->GetLineCount();
    for(int line = 0; line< maxLine; ++line)
    {
      int lineStart = ctrl->PositionFromLine(line);
      int indent    = ctrl->GetLineIndentation(line);
      int indentPos = ctrl->GetLineIndentPosition(line);
      static const int maxIndentation = 1000;
      if(indent < maxIndentation)
      {
        wxString indentNow    = ctrl->GetRange(lineStart, indentPos);
        wxString indentWanted = createIndentation(indent, tabsize, !usetabs);
        if(indentNow != indentWanted)
        {
          ctrl->SetTargetStart(lineStart);
          ctrl->SetTargetEnd(indentPos);
          ctrl->ReplaceTarget(indentWanted);
        }
      }
    }
    ctrl->EndUndoAction();
  }
}
