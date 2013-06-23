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

#ifndef _CONFIG_STYLE_HPP_
#define _CONFIG_STYLE_HPP_

#include "editor-theme.hpp"

class wxChoice;
class wxFontPickerCtrl;
class wxFontPickerEvent;
class wxColourPickerCtrl;
class wxColourPickerEvent;
class wxToggleButton;


class ConfigStyle: public wxPanel 
{
	wxDECLARE_EVENT_TABLE();
public:
	ConfigStyle(wxWindow* parent, wxWindowID id = wxID_ANY);
protected:

private:
	void Initialize();

	void fillThemeList();
	void fillLanguageList();
	void fillStyleList();
	void fillStyleGroup();

	int getLanguageSelection();
	int getStyleSelection();

	void enableStylePanel(bool enabled=true);

	void saveCurrentStyleDef();

	
	void onSelectLanguage(wxCommandEvent& event);
	void onSelectStyle(wxCommandEvent& event);

	void onSelectFont(wxFontPickerEvent& event);
	void onSelectBackground(wxColourPickerEvent& event);
	void onSelectForeground(wxColourPickerEvent& event);
	void onToggleBold(wxCommandEvent& event);
	void onToggleItalic(wxCommandEvent& event);
	void onToggleUnderline(wxCommandEvent& event);
	
	StyleDef currentStyleDef;
	
	wxChoice* themeChoice;
	wxListBox* languageList;
	wxListBox* styleList;
	
	wxFontPickerCtrl *fontPicker;
	wxColourPickerCtrl *foreColPicker;
	wxColourPickerCtrl *backColPicker;
	wxToggleButton *boldButton;
	wxToggleButton *italicButton;
	wxToggleButton *underlineButton;
};

#endif // _CONFIG_STYLE_HPP_

