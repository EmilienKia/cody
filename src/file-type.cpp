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

#include <wx/filename.h>
#include <wx/stc/stc.h>
#include <wx/tokenzr.h>

#include "file-type.hpp"

#include "decls.hpp"

//
// StyleDef
//

/*
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
		if(token=="eolfilled")
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
			else if(name=="weight")
			{
				long val;
				if(value.ToLong(&val))
					def.weight = (int)val;
			}
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
}*/

//
// FileType
//

FileType FileType::nullFileType;

FileTypeMap FileType::fromConf(wxConfig* config)
{
	FileTypeMap res;
	wxString oldPath = config->GetPath();
	config->SetPath(CONFPATH_FILETYPE_ROOT);
	
	wxString name;
	long index;
	if(config->GetFirstGroup(name, index))
	{
		FileType filetype;
		if(fromConf(config, wxString(CONFPATH_FILETYPE_ROOT) + "/" + name, filetype))
			res[filetype.getID()] = filetype;
		
		while(config->GetNextGroup(name, index))
		{
			FileType filetype;
			if(fromConf(config, wxString(CONFPATH_FILETYPE_ROOT) + "/" + name, filetype))
				res[filetype.getID()] = filetype;
		}
	}
	
	config->SetPath(oldPath);

	return res;
}

bool FileType::fromConf(wxConfig* config, wxString absPath, FileType& filetype)
{
	wxString oldPath = config->GetPath();
	config->SetPath(absPath);

	wxString str;

	// Read ID
	if(!config->Read("id", &str))
		return false;
	filetype.setID(str);

	// Read name
	if(!config->Read("name", &str))
		return false;
	filetype.setName(str);
	
	// Read lexer name
	if(!config->Read("lexer", &str))
		return false;
	filetype.setLexerName(str);

	// Read file pattern
	if(!config->Read("patterns", &str))
		return false;
	wxStringTokenizer tkzPattern(str, ";");
	while ( tkzPattern.HasMoreTokens() )
	{
		filetype._patterns.Add(tkzPattern.GetNextToken());
	}

	// File filter
	if(!config->Read("filter", &str))
		return false;
	filetype.setFileFilter(str);

	// Style definitions (0 - 39)
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		if(config->Read(wxString::Format("style.%lu", n), &str))
		{
			filetype._styleDef[n] = str;
		}
	}

	// Keyword lists (0-8)
	for(size_t n=0; n<=wxSTC_KEYWORDSET_MAX; ++n)
	{
		if(config->Read(wxString::Format("keywords.%lu", n), &str))
		{
			filetype._keywords[n] = str;
		}		
	}

	// Iterate for all other properties
/*	long index;
	if(config->GetFirstEntry(str, index))
	{
		std::cout << " - " << str << " : " << config->Read(str, "<<none>>") << std::endl;
		
		while(config->GetNextEntry(str, index))
		{
			std::cout << " - " << str << " : " << config->Read(str, "<<none>>") << std::endl;
		}
	}*/

	config->SetPath(oldPath);
	return true;
}

int FileType::lexerFromName(const wxString& lexerName)
{
	if(lexerName == "txt")
		return wxSTC_LEX_NULL;  // 1
	if(lexerName == "python")
		return wxSTC_LEX_PYTHON;	// 2
	if(lexerName == "props")
		return wxSTC_LEX_PROPERTIES;	// 9
	if(lexerName == "error")
		return wxSTC_LEX_ERRORLIST;	// 10
	if(lexerName == "make")
		return wxSTC_LEX_MAKEFILE;	// 11
	if(lexerName == "winbatch")
		return wxSTC_LEX_BATCH;	// 12
	if(lexerName == "diff")
		return wxSTC_LEX_DIFF;	// 16

	return 0;
}

wxString FileType::lexerToName(int lexer)
{
	switch(lexer)
	{
		case wxSTC_LEX_NULL: // 1
			return "txt";
		case wxSTC_LEX_PYTHON:  // 2
			return "python";
		case wxSTC_LEX_PROPERTIES:  // 9
			return "props";
		case wxSTC_LEX_ERRORLIST: // 10
			return "error";
		case wxSTC_LEX_MAKEFILE: // 11
			return "make";
		case wxSTC_LEX_BATCH: // 12
			return "winbatch";
		case wxSTC_LEX_DIFF: // 16
			return "diff";
		default:
			return "";
	}
}

FileType::FileType()
{
}

FileType::FileType(const FileType& type):
_name(type._name),
_id(type._id),
_fileFilter(type._fileFilter),
_patterns(type._patterns),
_lexer(type._lexer)
{
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
		_styleDef[n] = type._styleDef[n];
	for(size_t n=0; n<wxSTC_KEYWORDSET_MAX; ++n)
		_keywords[n] = type._keywords[n];
}

