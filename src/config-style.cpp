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
#include <wx/tokenzr.h>
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
	EVT_FONTPICKER_CHANGED(XRCID("FontPicker"), ConfigStyle::onSelectFont)
	EVT_COLOURPICKER_CHANGED(XRCID("BackgroundColor"), ConfigStyle::onSelectBackground)
	EVT_COLOURPICKER_CHANGED(XRCID("ForegroundColor"), ConfigStyle::onSelectForeground)
	EVT_TOGGLEBUTTON(XRCID("BoldButton"), ConfigStyle::onToggleBold)
	EVT_TOGGLEBUTTON(XRCID("ItalicButton"), ConfigStyle::onToggleItalic)
	EVT_TOGGLEBUTTON(XRCID("UnderlineButton"), ConfigStyle::onToggleUnderline)

	EVT_LISTBOX(XRCID("KeywordList"), ConfigStyle::onSelectKeyword)
	EVT_TEXT(XRCID("KeywordContent"), ConfigStyle::onUpdateKeyword)

wxEND_EVENT_TABLE()

ConfigStyle::ConfigStyle(wxWindow* parent, wxWindowID id):
wxPanel()
{
	if(wxXmlResource::Get()->LoadPanel(this, parent, "ConfigStyle"))
		Initialize();
}

void ConfigStyle::Initialize()
{
	themeChoice     = XRCCTRL(*this, "ThemeChoice", wxChoice);
	languageList    = XRCCTRL(*this, "LanguageList", wxListBox);
	styleList       = XRCCTRL(*this, "StyleList", wxListBox);
	
	fontPicker      = XRCCTRL(*this, "FontPicker", wxFontPickerCtrl);
	foreColPicker   = XRCCTRL(*this, "ForegroundColor", wxColourPickerCtrl);
	backColPicker   = XRCCTRL(*this, "BackgroundColor", wxColourPickerCtrl);
	boldButton      = XRCCTRL(*this, "BoldButton", wxToggleButton);
	italicButton    = XRCCTRL(*this, "ItalicButton", wxToggleButton);
	underlineButton = XRCCTRL(*this, "UnderlineButton", wxToggleButton);

	keywordList     = XRCCTRL(*this, "KeywordList", wxListBox);
	keywordContent  = XRCCTRL(*this, "KeywordContent", wxTextCtrl);

	// Add an hidden styled text ctrl
	styleSample = new wxStyledTextCtrl(this, wxID_ANY);
	styleSample->Hide();
	
	fillThemeList();
	fillLanguageList();
}

void ConfigStyle::fillThemeList()
{
	for(std::map<wxString, EditorTheme>::iterator it=EditorThemeManager::get().getThemes().begin();
													it!=EditorThemeManager::get().getThemes().end(); ++it)
		themeChoice->Append(it->second.getName());

	if(!themeChoice->IsEmpty())
		themeChoice->SetSelection(0);
}

void ConfigStyle::fillLanguageList()
{
	languageList->Clear();
	for(size_t n=0; n<FT_COUNT; ++n)
	{
		languageList->Append(FileTypeManager::get().getFileType(n).getName());
	}
}

void ConfigStyle::fillStyleList()
{
	int lang = getLanguageSelection();
	styleList->Clear();
	if(lang!=wxNOT_FOUND)
	{
		const FileType& type = FileTypeManager::get().getFileType(lang);
		for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
		{
			const Optional<wxString>& name = type.getEditorStyleName(n);
			if(name.set())
			{
				styleList->Append(*name, new StyleClientData(lang, n));
				
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
		const FileType& type = FileTypeManager::get().getFileType(lang);
			
		currentStyleDef = StyleDef(type.getStyleDef(style));
		StyleDef applied = type.getAppliedStyle(style);
		
		fontPicker->SetSelectedFont(applied.getFont());
		italicButton->SetValue(applied.italic);
		boldButton->SetValue(applied.bold);
		underlineButton->SetValue(applied.underline);
		foreColPicker->SetColour(applied.fore);
		backColPicker->SetColour(applied.back);
		
		enableStylePanel(true);
	}
	else if(lang!=wxNOT_FOUND)
	{
		enableStylePanel(false);
	}
	else
	{
		enableStylePanel(false);
	}
}


void ConfigStyle::fillKeywordList()
{
	keywordList->SetSelection(wxNOT_FOUND);
	keywordList->Clear();

	wxStringTokenizer tokenizer(styleSample->DescribeKeyWordSets(), "\n");
	while(tokenizer.HasMoreTokens())
	{
		keywordList->Append(tokenizer.GetNextToken());
	}
	
	fillKeywordContent();
}

void ConfigStyle::fillKeywordContent()
{
	int lang = getLanguageSelection();
	int kw   = getKeywordSelection();

	keywordContent->ChangeValue("");
	keywordContent->Disable();
	
	if(lang!=wxNOT_FOUND && kw!=wxNOT_FOUND && kw<wxSTC_KEYWORDSET_MAX)
	{
		const FileType& type = FileTypeManager::get().getFileType(lang);
		if(type.getKeywords(kw).set())
		{
			keywordContent->ChangeValue(*type.getKeywords(kw));
		}
		keywordContent->Enable();
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

int ConfigStyle::getLanguageSelection()const
{
	return languageList->GetSelection();
}

int ConfigStyle::getStyleSelection()const
{
	int sel;
	wxClientData *data;
	if((sel=styleList->GetSelection())!=wxNOT_FOUND && (data=styleList->GetClientObject(sel))!=NULL)
	{
		return ((StyleClientData*)data)->styleID;
	}
	else
		return wxNOT_FOUND;
}

int ConfigStyle::getKeywordSelection()const
{
	return keywordList->GetSelection();
}

void ConfigStyle::onSelectFont(wxFontPickerEvent& event)
{
	wxFont font = event.GetFont();

	// Look for bold, italic and underline changes
	italicButton->SetValue(font.GetStyle()==wxFONTSTYLE_ITALIC || font.GetStyle()==wxFONTSTYLE_SLANT);
	boldButton->SetValue(font.GetWeight()==wxFONTWEIGHT_BOLD);
	underlineButton->SetValue(font.GetUnderlined());

	// Apply font changes to style def
	currentStyleDef.italic.set(font.GetStyle()==wxFONTSTYLE_ITALIC || font.GetStyle()==wxFONTSTYLE_SLANT);
	currentStyleDef.bold.set(font.GetWeight()==wxFONTWEIGHT_BOLD);
	currentStyleDef.underline.set(font.GetUnderlined());
	currentStyleDef.font.set(font.GetFaceName());
	currentStyleDef.size.set(font.GetPointSize());

	// Apply style def to global style defs
	saveCurrentStyleDef();
}

void ConfigStyle::onSelectBackground(wxColourPickerEvent& event)
{
	currentStyleDef.back.set(event.GetColour());
	saveCurrentStyleDef();
}

void ConfigStyle::onSelectForeground(wxColourPickerEvent& event)
{
	currentStyleDef.fore.set(event.GetColour());
	saveCurrentStyleDef();
}

void ConfigStyle::onToggleBold(wxCommandEvent& event)
{
	currentStyleDef.bold.set(event.IsChecked());
	saveCurrentStyleDef();
}

void ConfigStyle::onToggleItalic(wxCommandEvent& event)
{
	currentStyleDef.italic.set(event.IsChecked());
	saveCurrentStyleDef();
}

void ConfigStyle::onToggleUnderline(wxCommandEvent& event)
{
	currentStyleDef.underline.set(event.IsChecked());
	saveCurrentStyleDef();
}

void ConfigStyle::saveCurrentStyleDef()
{
	int lang = getLanguageSelection();
	int style = wxNOT_FOUND;
	if(lang!=wxNOT_FOUND)
	{
		style = getStyleSelection();
	}
	if(style!=wxNOT_FOUND)
	{
		FileTypeManager::get().setFileTypeStyle(lang, style, currentStyleDef.toString());
	}	
}

void ConfigStyle::saveCurrentKeywords()
{
	int lang = getLanguageSelection();
	int kw   = getKeywordSelection();

	if(lang!=wxNOT_FOUND && kw!=wxNOT_FOUND && kw<wxSTC_KEYWORDSET_MAX)
	{
		wxString str = keywordContent->GetValue();
		str.Replace("\n", " ");
		FileTypeManager::get().setFileTypeKeywords(lang, kw, str);
	}
}

void ConfigStyle::onSelectLanguage(wxCommandEvent& event)
{
	int lang = getLanguageSelection();
	if(lang!=wxNOT_FOUND)
	{
		styleSample->SetLexer(FileTypeManager::get().getFileType(lang).getLexer());
	}
	else
	{
		styleSample->SetLexer(wxSTC_LEX_NULL);
	}
	
	fillStyleList();
	fillStyleGroup();
	
	fillKeywordList();
}

void ConfigStyle::onSelectStyle(wxCommandEvent& event)
{
	fillStyleGroup();
}

void ConfigStyle::onSelectKeyword(wxCommandEvent& event)
{
	fillKeywordContent();
}

void ConfigStyle::onUpdateKeyword(wxCommandEvent& event)
{
	saveCurrentKeywords();
}

