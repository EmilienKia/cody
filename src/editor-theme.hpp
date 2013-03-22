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

	static EditorTheme readFromConfig(wxConfig* config, const wxString& path);
};



class EditorStyle
{
public:
	EditorStyle(){}	
	EditorStyle(const EditorStyle& style);	

	bool     has(size_t idx)const;
	wxString get(size_t idx)const;
	void     set(size_t idx, const wxString& value = "");
	
protected:
	Optional<wxString> _styleDef[wxSTC_STYLE_LASTPREDEFINED];
};



class EditorThemeManager
{
private:
	static EditorThemeManager s_manager;
	
public:
	static EditorThemeManager& get();

	const EditorTheme& getTheme(const wxString& name)const;
	const EditorTheme& getCurrentTheme()const;

	wxString getThemeProperty(const wxString& name)const;
	wxString getThemeExpandedProperty(const wxString& name)const;
	wxString getThemeExpandedValue(const wxString& value)const;


	const EditorStyle& getStyle(const wxString& name)const;

	EditorStyle getExpandedStyle(const wxString& name)const;
	
	void readFromConfig(wxConfig* config);
	
protected:
	wxString _theme;
	std::map<wxString, EditorTheme> _themes;
	std::map<wxString, EditorStyle> _styles;

	void readEditorStyleFromConfig(wxConfig* config, const wxString& name);
};






#endif // _EDITOR_THEME_HPP_

