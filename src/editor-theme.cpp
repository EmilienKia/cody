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
#include <wx/tokenzr.h>

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

EditorTheme EditorTheme::readFromConfig(wxFileConfig* config, const wxString& path)
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
// StyleDef
//

StyleDef::StyleDef()
{
}

StyleDef::StyleDef(const wxString& str)
{
	*this = StyleDef::fromString(str);
}

StyleDef::StyleDef(const StyleDef& style):
font(style.font),
size(style.size),
//weight(style.weight),
fore(style.fore),
back(style.back),
italic(style.italic),
bold(style.bold),
underline(style.underline),
eolfilled(style.eolfilled),
charcase(style.charcase)
{
}

void StyleDef::override(const StyleDef& style)
{
	if(style.font.set())
		font = style.font;
	if(style.size.set())
		size = style.size;
	// if(style.weight.set())
	//   weight = style.weight;
	if(style.fore.set())
		fore = style.fore;
	if(style.back.set())
		back = style.back;
	if(style.italic.set())
		italic = style.italic;
	if(style.bold.set())
		bold = style.bold;
	if(style.underline.set())
		underline = style.underline;
	if(style.eolfilled.set())
		eolfilled = style.eolfilled;
	if(style.charcase.set())
		charcase = style.charcase;
}

wxFont StyleDef::getFont()const
{
	wxFont temp = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);

	if(font.set())
		temp.SetFaceName(*font);
	if(size.set())
		temp.SetPointSize(*size);
	if(italic.set())
		temp.SetStyle(*italic?wxFONTSTYLE_ITALIC:wxFONTSTYLE_NORMAL);
	if(bold.set())
		temp.SetWeight(*bold?wxFONTWEIGHT_BOLD:wxFONTWEIGHT_NORMAL);
	if(underline.set())
		temp.SetUnderlined(*underline);

	return temp;
}

wxString StyleDef::toString()const
{
	wxArrayString arr;
	wxString res;

	if(bold.set()){ if(*bold) arr.Add("bold"); else arr.Add("notbold"); }
	if(italic.set()){ if(*italic) arr.Add("italic"); else arr.Add("notitalic"); }
	if(underline.set()){ if(*underline) arr.Add("underline"); else arr.Add("notunderline"); }
	if(eolfilled.set()){ if(*eolfilled) arr.Add("eolfilled"); else arr.Add("noteolfilled"); }
	if(font){arr.Add(wxString("font:")+*font); }
	if(size){arr.Add(wxString::Format("size:%d", *size)); }
//	if(weight){arr.Add(wxString::Format("weight:%d", *weight)); }
	if(charcase){arr.Add(wxString("case:")+*charcase); }
	if(fore){arr.Add(wxString("fore:")+(*fore).GetAsString(wxC2S_HTML_SYNTAX));}
	if(back){arr.Add(wxString("back:")+(*back).GetAsString(wxC2S_HTML_SYNTAX));}

	if(arr.GetCount()>0)
	{
		res = arr[0];
		for(size_t n=1; n<arr.GetCount(); ++n)
		{
			res += ",";
			res += arr[n];
		}
	}
	return res;
}

StyleDef StyleDef::fromString(const wxString& str)
{
	StyleDef def;
	
	wxStringTokenizer tkz(str, ",");
	while(tkz.HasMoreTokens())
	{
		wxString token = tkz.GetNextToken();
		if(token=="bold")
			*def.bold = true;
		else if(token=="notbold")
			*def.bold = false;
		else if(token=="italic" || token=="italics")
			*def.italic = true;
		else if(token=="notitalic" || token=="notitalics")
			*def.italic = false;
		else if(token=="underline" || token=="underlines")
			*def.italic = true;
		else if(token=="notunderline" || token=="notunderlines")
			*def.italic = false;
		else if(token=="eolfilled")
			*def.eolfilled = true;
		else if(token=="noteolfilled")
			*def.eolfilled = false;
		else
		{
			wxString name = token.BeforeFirst(':');
			wxString value = token.AfterFirst(':');
			if(name=="font")
				def.font = value;
			else if(name=="size")
			{
				long val;
				if(value.ToLong(&val))
					def.size = (int)val;
			}
/*			else if(name=="weight")
			{
				long val;
				if(value.ToLong(&val))
					def.weight = (int)val;
			}*/
			else if(name=="case" || name=="charcase")
			{
				if(value.Length()>=1)
					def.charcase = value[0];
			}
			else if(name=="fore")
			{
				def.fore = wxColour(value);
			}
			else if(name=="back")
			{
				def.back = wxColour(value);
			}
		}
	}

	return def;
}

//
// Style
//
/*
wxFont Style::defaultFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);

Style::Style()
{
}

Style::Style(const Style& style):
font(style.font),
size(style.size),
italic(style.italic),
bold(style.bold),
underline(style.underline),
fore(style.fore),
back(style.back),
eolfilled(style.eolfilled),
charcase(style.charcase)
{
}

wxFont Style::getFont()const
{
	
}

void Style::override(const StyleDef& style)
{
	if(style.font.set())
	{
		wxString str = style.font;
		if(str.Length()>0 && str[0]=='!')
		{
			str = str.substr(1);
		}
		font.SetFaceName(str);
	}
	if(style.size.set())
		font.SetPointSize(style.size);

	if(style.italic.set())
		font.SetStyle(*style.italic?wxFONTSTYLE_ITALIC:wxFONTSTYLE_NORMAL);		
	if(style.bold.set())
		font.SetWeight(*style.bold?wxFONTWEIGHT_BOLD:wxFONTWEIGHT_NORMAL);
	if(style.underline.set())
		font.SetUnderlined(*style.underline);

	if(style.fore.set())
		fore = *style.fore;
	if(style.back.set())
		back = *style.back;
	
	if(style.eolfilled.set())
		eolfilled = *style.eolfilled;
	if(style.charcase.set())
		charcase = *style.charcase;
}

wxString Style::toString()const
{
	wxArrayString arr;
	wxString res;

	if(font.IsOk()){arr.Add(wxString("font:")+font.GetNativeFontInfoUserDesc()); }
	if(fore.IsOk()){arr.Add(wxString("fore:")+fore.GetAsString(wxC2S_HTML_SYNTAX));}
	if(back.IsOk()){arr.Add(wxString("back:")+back.GetAsString(wxC2S_HTML_SYNTAX));}
	
	if(arr.GetCount()>0)
	{
		res = arr[0];
		for(size_t n=1; n<arr.GetCount(); ++n)
		{
			res += ",";
			res += arr[n];
		}
	}
	return res;
}
*/



//
// EditorStyle
//

EditorStyle::EditorStyle(const EditorStyle& style)
{
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
		_styleDef[n] = style._styleDef[n];
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
		_styleName[n] = style._styleName[n];
}
/*
bool EditorStyle::has(size_t idx)const
{
	if(idx >= wxSTC_STYLE_LASTPREDEFINED)
		return false;
	return _styleDef[idx].set();
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
*/
EditorStyle EditorStyle::readFromConfig(wxFileConfig* config, const wxString& path)
{
	wxString oldPath = config->GetPath();
	config->SetPath(wxString(CONFPATH_EDITOR_STYLE_ROOT) + "/" + path);

	EditorStyle style;
	wxString str;

	// Style names
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		if(config->Read(wxString::Format("stylename.%lu", n), &str))
		{
			style.setStyleName(n, str);
		}
	}
	
	// Style definitions (0 - 39)
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		if(config->Read(wxString::Format("style.%lu", n), &str))
		{
			style.setStyle(n, str);
		}
	}
		
	config->SetPath(oldPath);
	return style;
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
		if(style.hasStyle(n))
		   style.setStyle(n, getThemeExpandedValue(style.getStyle(n)));
	}
}

EditorStyle EditorThemeManager::getExpandedStyle(const wxString& name)const
{
	EditorStyle res;

	EditorStyle style = getStyle(name);
	EditorStyle def   = getStyle("default");

	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		if(style.hasStyle(n))
		   res.setStyle(n, getThemeExpandedValue(style.getStyle(n)));
		else if(def.hasStyle(n))
		   res.setStyle(n, getThemeExpandedValue(def.getStyle(n)));
	}
	
	return res;
}

void EditorThemeManager::readFromConfig(wxFileConfig* config)
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
		_styles[name] = EditorStyle::readFromConfig(config, name);
		while(config->GetNextGroup(name, index))
		{
			_styles[name] = EditorStyle::readFromConfig(config, name);
		}
	}
	
	// Add a "default" if not found
	_styles["default"];	

	config->SetExpandEnvVars(expand);
	config->SetPath(oldPath);
}


