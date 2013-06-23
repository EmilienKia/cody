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

#ifndef _EDITOR_THEME_HPP_
#define _EDITOR_THEME_HPP_

#include <wx/stc/stc.h>
#include <wx/fileconf.h>

#include <map>

#include "template-tools.hpp"


class EditorTheme : public std::map<wxString, wxString>
{
	typedef std::map<wxString, wxString> parent;
public:
	EditorTheme(){}	
	EditorTheme(const EditorTheme& theme);

	bool has(const wxString& name)const;
	wxString get(const wxString& name)const;
	void set(const wxString& name, const wxString& value);

	wxString getID()const{return get("id");}
	wxString getName()const{return get("name");}

	static EditorTheme readFromConfig(wxFileConfig* config, const wxString& path);
};




class StyleDef
{
public:
	StyleDef();
	StyleDef(const wxString& str);
	StyleDef(const StyleDef& style);
	
	Optional<wxString> font; // Font name
	Optional<int>	   size; // Font size (in point ?)
	// Optional<int>	   weight; // Font weight (1..999: 100=light, 400=normal, 700=bold) // Not used, only bold or not
	Optional<wxColour> fore; // Foreground color
	Optional<wxColour> back; // Background color
	Optional<bool>	   italic; // Is font in italic
	Optional<bool>	   bold; // Is font in bold
	Optional<bool>	   underline; // Is font in underline
	Optional<bool>	   eolfilled; // Are EOL filled
	Optional<char>	   charcase; // Case modification ('m', 'u', or 'l' for mixed, upper or lower case)

	wxString toString()const;
	
	static StyleDef fromString(const wxString& str);
};



class EditorStyle
{
	friend class EditorThemeManager;
public:
	EditorStyle(){}	
	EditorStyle(const EditorStyle& style);	

	bool hasStyle(unsigned short idx)const{return _styleDef[idx].set();}
	const Optional<wxString>& getStyle(unsigned short idx)const{return _styleDef[idx];}
	void setStyle(unsigned short idx, const wxString& name){_styleDef[idx] = name;}
	
	bool hasStyleName(unsigned short idx)const{return _styleName[idx].set();}
	const Optional<wxString>& getStyleName(unsigned short idx)const{return _styleName[idx];}
	void setStyleName(unsigned short idx, const wxString& name){_styleName[idx] = name;}

	static EditorStyle readFromConfig(wxFileConfig* config, const wxString& path);
	
protected:
	Optional<wxString> _styleDef[wxSTC_STYLE_LASTPREDEFINED];
	Optional<wxString> _styleName[wxSTC_STYLE_LASTPREDEFINED];
};



class EditorThemeManager
{
private:
	static EditorThemeManager s_manager;
	
public:
	static EditorThemeManager& get();

	const EditorTheme& getTheme(const wxString& name)const;
	const EditorTheme& getCurrentTheme()const;
	wxString getCurrentThemeName()const{return _theme;}

	wxString getCurrentThemeProperty(const wxString& name)const;
	wxString expandCurrentThemeProperty(const wxString& name)const;
	wxString getThemeExpandedValue(const wxString& value)const;

	const EditorStyle& getStyle(const wxString& name)const;

	void expandStyle(EditorStyle& style)const;
	EditorStyle getExpandedStyle(const wxString& name)const;
	
	
	void readFromConfig(wxFileConfig* config);

	
	std::map<wxString, EditorTheme>& getThemes(){return _themes;}
	std::map<wxString, EditorStyle>& getStyles(){return _styles;}
	
protected:
	wxString _theme;
	std::map<wxString, EditorTheme> _themes;
	std::map<wxString, EditorStyle> _styles;
};






#endif // _EDITOR_THEME_HPP_

