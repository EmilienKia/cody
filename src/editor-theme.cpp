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

#include <wx/config.h>

#include "editor-theme.hpp"

#include "decls.hpp"

//
// EditorTheme
//

EditorTheme::EditorTheme(const EditorTheme& theme):
parent(theme)
{
}

bool EditorTheme::has(const wxString& name)const
{
	return find(name)!=end();
}

wxString EditorTheme::get(const wxString& name)const
{
	const_iterator it = find(name);
	if(it!=end())
	   return it->second;
	else
		return "";
}

void EditorTheme::set(const wxString& name, const wxString& value)
{
	(*this)[name] = value;
}

EditorTheme EditorTheme::readFromConfig(wxConfig* config, const wxString& path)
{
	EditorTheme theme;
	wxString oldPath = config->GetPath();
	config->SetPath(wxString(CONFPATH_EDITOR_THEME_ROOT) + "/" + path);

	// Iterate to store all properties
	wxString name;
	long index;
	if(config->GetFirstEntry(name, index))
	{
		theme.set(name, config->Read(name, ""));
		while(config->GetNextEntry(name, index))
		{
			theme.set(name, config->Read(name, ""));
		}
	}
	
	config->SetPath(oldPath);
	return theme;
}


//
// EditorStyle
//

EditorStyle::EditorStyle(const EditorStyle& style)
{
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
		_styleDef[n] = style._styleDef[n];
}

bool EditorStyle::has(size_t idx)const
{
	if(idx >= wxSTC_STYLE_LASTPREDEFINED)
		return false;
	return _styleDef[idx]; 
}

Optional<wxString> EditorStyle::get(size_t idx)const
{
	if(idx >= wxSTC_STYLE_LASTPREDEFINED)
		return Optional<wxString>();
	return _styleDef[idx];
}

void EditorStyle::set(size_t idx, const wxString& value)
{
	if(idx < wxSTC_STYLE_LASTPREDEFINED)
		_styleDef[idx] = value;
}

Optional<wxString>& EditorStyle::operator[](size_t idx)
{
	return _styleDef[idx];
}

const Optional<wxString>& EditorStyle::operator[](size_t idx)const
{
	return _styleDef[idx];
}

//
// EditorThemeManager
//

static EditorTheme nullEditorTheme;
static EditorStyle nullEditorStyle;

EditorThemeManager EditorThemeManager::s_manager;

EditorThemeManager& EditorThemeManager::get()
{
	return s_manager;
}

const EditorTheme& EditorThemeManager::getTheme(const wxString& name)const
{
	std::map<wxString, EditorTheme>::const_iterator it = _themes.find(name);
	if(it!=_themes.end())
		return it->second;
	else
		return nullEditorTheme;
}

const EditorTheme& EditorThemeManager::getCurrentTheme()const
{
	return getTheme(_theme);	
}

wxString EditorThemeManager::getCurrentThemeProperty(const wxString& name)const
{
	const EditorTheme& cur = getCurrentTheme();
	EditorTheme::const_iterator itcur = cur.find(name);
	if(itcur!=cur.end())
		return itcur->second;

	const EditorTheme& def = getTheme("default");
	EditorTheme::const_iterator itdef = def.find(name);
	if(itdef!=def.end())
		return itdef->second;	
	
	return "";
}

wxString EditorThemeManager::expandCurrentThemeProperty(const wxString& name)const
{
	return getThemeExpandedValue(getCurrentThemeProperty(name));
}

wxString EditorThemeManager::getThemeExpandedValue(const wxString& value)const
{
	// TODO do cache and optimization !!!
	
	wxString res = value;

	size_t idx;	
	while(idx = res.find("$("), idx != wxString::npos)
	{
		wxString before = res.Mid(0, idx),
			     middle = res.Mid(idx+2);
		
		size_t idx2 = middle.find(")");

		wxString subvar, after;

		if(idx2 == 0)
		{
			subvar = "";
			after  = middle;
		}
		else if(idx2 == wxString::npos)
		{
			subvar = middle;
			after  = "";
		}
		else
		{
			subvar = middle.Mid(0, idx2);
			after  = middle.Mid(idx2+1);
		}

		res = before + expandCurrentThemeProperty(subvar) + after;
	}
	
	return res;
}

const EditorStyle& EditorThemeManager::getStyle(const wxString& name)const
{
	std::map<wxString, EditorStyle>::const_iterator it = _styles.find(name);
	if(it!=_styles.end())
		return it->second;
	else
		return nullEditorStyle;
}

void EditorThemeManager::expandStyle(EditorStyle& style)const
{
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		if(style.has(n))
		   style.set(n, getThemeExpandedValue(style.get(n)));
	}
}

EditorStyle EditorThemeManager::getExpandedStyle(const wxString& name)const
{
	EditorStyle res;

	EditorStyle style = getStyle(name);
	EditorStyle def   = getStyle("default");

	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		if(style.has(n))
		   res.set(n, getThemeExpandedValue(style.get(n)));
		else if(def.has(n))
		   res.set(n, getThemeExpandedValue(def.get(n)));
	}
	
	return res;
}

void EditorThemeManager::readFromConfig(wxConfig* config)
{
	wxString oldPath = config->GetPath();
	bool expand = config->IsExpandingEnvVars();
	config->SetExpandEnvVars(false);

	wxString name;
	long index;
	
	//
	// Load themes
	//
	config->SetPath(CONFPATH_EDITOR_THEME_ROOT);
	_themes.clear();
	
	// Read current (default) theme name
	_theme = config->Read("theme", ""); 

	// Iterate to read all theme definitions
	EditorTheme theme;
	if(config->GetFirstGroup(name, index))
	{
		theme = EditorTheme::readFromConfig(config, name);
		if(!theme.getID().IsEmpty())
			_themes[theme.getID()] = theme;
		
		while(config->GetNextGroup(name, index))
		{
			theme = EditorTheme::readFromConfig(config, name);
			if(!theme.getID().IsEmpty())
				_themes[theme.getID()] = theme;
		}
	}

	// Add a "default" if not found
	_themes["default"];


	//
	// Load styles
	//
	config->SetPath(CONFPATH_EDITOR_STYLE_ROOT);
	_styles.clear();

	// Iterate to read all style definitions
	EditorStyle style;
	if(config->GetFirstGroup(name, index))
	{
		readEditorStyleFromConfig(config, name);
		while(config->GetNextGroup(name, index))
		{
			readEditorStyleFromConfig(config, name);
		}
	}
	
	// Add a "default" if not found
	_styles["default"];	

	config->SetExpandEnvVars(expand);
	config->SetPath(oldPath);
}

void EditorThemeManager::readEditorStyleFromConfig(wxConfig* config, const wxString& name)
{
	wxString oldPath = config->GetPath();
	config->SetPath(wxString(CONFPATH_EDITOR_STYLE_ROOT) + "/" + name);

	EditorStyle style;

	// Style definitions (0 - 39)
	wxString str;
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		if(config->Read(wxString::Format("style.%lu", n), &str))
		{
			style.set(n, str);
		}
	}
	
	_styles[name] = style;
	
	config->SetPath(oldPath);
}



