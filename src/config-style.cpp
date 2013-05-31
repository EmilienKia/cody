/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * cody
 * Copyright (C) 2013 Émilien KIA <emilien.kia@gmail.com>
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

#include <wx/fileconf.h>
#include <wx/statbox.h>
#include <wx/xrc/xmlres.h>

#include "config-style.hpp"

#include "cody-app.hpp"
#include "decls.hpp"
#include "editor-theme.hpp"
#include "file-type.hpp"
#include "text-document.hpp"
#include "text-frame.hpp"

enum ConfigStyleID
{
	ConfigStyle_ColorTheme = 100,
	ConfigStyle_LanguageList

	
};


wxBEGIN_EVENT_TABLE(ConfigStyle, wxPanel)
	EVT_LISTBOX(XRCID("LanguageList"), ConfigStyle::onSelectLanguage)

wxEND_EVENT_TABLE()

ConfigStyle::ConfigStyle(wxWindow* parent, wxWindowID id):
wxPanel()
{
	if(wxXmlResource::Get()->LoadPanel(this, parent, "ConfigStyle"))
		Initialize();
}

void ConfigStyle::Initialize()
{
	fillThemeList();
	fillStyleList();
}

void ConfigStyle::fillThemeList()
{
	wxChoice* choice = XRCCTRL(*this, "ThemeChoice", wxChoice);
	if(choice)
	{
		for(std::map<wxString, EditorTheme>::iterator it=EditorThemeManager::get().getThemes().begin();
														it!=EditorThemeManager::get().getThemes().end(); ++it)
			choice->Append(it->second.getName());

		if(!choice->IsEmpty())
			choice->SetSelection(0);
	}
}

void ConfigStyle::fillStyleList()
{
	wxListBox* lbox = XRCCTRL(*this, "LanguageList", wxListBox);
	if(lbox)
	{
		for(size_t n=0; n<FT_COUNT; ++n)
		{
			lbox->Append(FileTypeManager::get().getFileType(n).getName());
		}
	}
}

int ConfigStyle::getLanguageSelection()
{
	wxListBox* lbox = XRCCTRL(*this, "LanguageList", wxListBox);
	if(lbox)
		return lbox->GetSelection();
	else
		return wxNOT_FOUND;
}

int ConfigStyle::getStyleSelection()
{
/*	wxListBox* lbox = XRCCTRL(*this, "LanguageList", wxListBox);
	if(lbox)
		return lbox->GetSelection();
	else*/
		return wxNOT_FOUND;
}

void ConfigStyle::onSelectLanguage(wxCommandEvent& event)
{
	int sel = getLanguageSelection();
	if(sel!=wxNOT_FOUND)
	{
		wxListBox* lbox = XRCCTRL(*this, "StyleList", wxListBox);
		if(lbox)
		{
			lbox->Clear();

			wxString stylename = FileTypeManager::get().getFileType(sel).getDefaultStyle();
			const EditorStyle& style = EditorThemeManager::get().getStyle(stylename);

			for(unsigned short n = 0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
			{
				wxString val = style.getStyleName(n);
				if(!val.IsEmpty())
					lbox->Append(val);
			}
		}
	}
}

