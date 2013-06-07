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

#include <wx/clrpicker.h>
#include <wx/fileconf.h>
#include <wx/fontpicker.h>
#include <wx/statbox.h>
#include <wx/tglbtn.h>
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

class StyleClientData : public wxClientData
{
public:
    StyleClientData(unsigned int langID = 0, unsigned int styleID =0):languageID(langID), styleID(styleID){}
	StyleClientData(const StyleClientData& data):languageID(data.languageID), styleID(data.styleID){}

	unsigned int languageID, styleID;
};



wxBEGIN_EVENT_TABLE(ConfigStyle, wxPanel)
	EVT_LISTBOX(XRCID("LanguageList"), ConfigStyle::onSelectLanguage)
	EVT_LISTBOX(XRCID("StyleList"), ConfigStyle::onSelectStyle)
wxEND_EVENT_TABLE()

ConfigStyle::ConfigStyle(wxWindow* parent, wxWindowID id):
wxPanel()
{
	if(wxXmlResource::Get()->LoadPanel(this, parent, "ConfigStyle"))
		Initialize();
}

void ConfigStyle::Initialize()
{
	fontPicker      = XRCCTRL(*this, "FontPicker", wxFontPickerCtrl);
	foreColPicker   = XRCCTRL(*this, "ForegroundColor", wxColourPickerCtrl);
	backColPicker   = XRCCTRL(*this, "BackgroundColor", wxColourPickerCtrl);
	boldButton      = XRCCTRL(*this, "BoldButton", wxToggleButton);
	italicButton    = XRCCTRL(*this, "ItalicButton", wxToggleButton);
	underlineButton = XRCCTRL(*this, "UnderlineButton", wxToggleButton);
	
	fillThemeList();
	fillLanguageList();
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

void ConfigStyle::fillLanguageList()
{
	wxListBox* lbox = XRCCTRL(*this, "LanguageList", wxListBox);
	if(lbox)
	{
		lbox->Clear();
		for(size_t n=0; n<FT_COUNT; ++n)
		{
			lbox->Append(FileTypeManager::get().getFileType(n).getName());
		}
	}
}

void ConfigStyle::fillStyleList()
{
	wxListBox* lbox = XRCCTRL(*this, "StyleList", wxListBox);
	int lang = getLanguageSelection();
	if(lbox)
	{
		lbox->Clear();
		if(lang!=wxNOT_FOUND)
		{
			const FileType& type = FileTypeManager::get().getFileType(lang);
			for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
			{
				const Optional<wxString>& name = type.getEditorStyleName(n);
				if(name.set())
				{
					lbox->Append(*name, new StyleClientData(lang, n));
				}
			}
		}
	}
}

void ConfigStyle::fillStyleGroup()
{
	int lang = getLanguageSelection();
	int style = wxNOT_FOUND;
	if(lang!=wxNOT_FOUND)
	{
		style = getStyleSelection();
	}

	if(style!=wxNOT_FOUND)
	{
std::cout << lang << " - " << style << std::endl;
		enableStylePanel(true);
	}
	else if(lang!=wxNOT_FOUND)
	{
std::cout << lang << std::endl;
		enableStylePanel(false);
	}
	else
	{
std::cout << "none" << std::endl;
		enableStylePanel(false);
	}
}

void ConfigStyle::enableStylePanel(bool enabled)
{
	fontPicker->Enable(enabled);
	foreColPicker->Enable(enabled);
	backColPicker->Enable(enabled);
	boldButton->Enable(enabled);
	italicButton->Enable(enabled);
	underlineButton->Enable(enabled);
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
	wxListBox* lbox = XRCCTRL(*this, "StyleList", wxListBox);
	int sel;
	wxClientData *data;
	if(lbox && (sel=lbox->GetSelection())!=wxNOT_FOUND && (data=lbox->GetClientObject(sel))!=NULL)
	{
		return ((StyleClientData*)data)->styleID;
	}
	else
		return wxNOT_FOUND;
}

void ConfigStyle::onSelectLanguage(wxCommandEvent& event)
{
	fillStyleList();
	fillStyleGroup();
}

void ConfigStyle::onSelectStyle(wxCommandEvent& event)
{
	fillStyleGroup();
}

