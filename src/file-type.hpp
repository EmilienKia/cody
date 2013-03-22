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

#ifndef _FILE_TYPE_HPP_
#define _FILE_TYPE_HPP_

#include <map>

#include <wx/config.h>

#include <wx/stc/stc.h>

#include "template-tools.hpp"
#include "editor-theme.hpp"

class FileType;
typedef std::map<wxString, FileType> FileTypeMap;

/*
class StyleDef
{
public:
	Optional<wxString> font; // Font name
	Optional<int>	   size; // Font size (in point ?)
	Optional<int>	   weight; // Font weight (1..999: 100=light, 400=normal, 700=bold)
	Optional<wxColour> fore; // Foreground color
	Optional<wxColour> back; // Background color
	Optional<bool>	   italic; // Is font in italic
	Optional<bool>	   bold; // Is font in bold
	Optional<bool>	   eolfilled; // Are EOL filled
	Optional<char>	   charcase; // Case modification ('m', 'u', or 'l' for mixed, upper or lower case)

	static StyleDef fromString(const wxString& str);
};
*/

class FileStyle
{
public:
	
};

class FileType
{
public:
	static FileTypeMap fromConf(wxConfig* config);
	static bool fromConf(wxConfig* config, wxString absPath, FileType& filetype);
	static FileType nullFileType;

	FileType();
	FileType(const FileType& type);

	bool Ok()const{return !getID().IsEmpty();}

	wxString getID()const{return _id;}
	void setID(const wxString& id){_id = id;}
	
	wxString getName()const{return _name;}
	void setName(const wxString& name){_name = name;}

	wxString getFileFilter()const{return _fileFilter;}
	void setFileFilter(const wxString& fileFilter){_fileFilter = fileFilter;}

	const wxArrayString& getPatterns()const{return _patterns;}
	void setPatterns(const wxArrayString& patterns){_patterns = patterns;}

	int getLexer()const{return _lexer;}
	void setLexer(int lexer){_lexer = lexer;}
	wxString getLexerName()const{return lexerToName(_lexer);}
	void setLexerName(const wxString& lexerName){setLexer(lexerFromName(lexerName));}

	const Optional<wxString>& getStyleDef(size_t n)const{return _styleDef[n];}
	const Optional<wxString>& getKeywords(size_t n)const{return _keywords[n];}

protected:
	wxString _name, _id, _fileFilter;
	wxArrayString _patterns;
	int _lexer;

	Optional<wxString> _styleDef[wxSTC_STYLE_LASTPREDEFINED];
	Optional<wxString> _keywords[wxSTC_KEYWORDSET_MAX];
	
	static int lexerFromName(const wxString& lexerName);
	static wxString lexerToName(int lexer);

};

#endif // _FILE_TYPE_HPP_

