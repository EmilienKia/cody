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
// FileType
//


int FileType::lexerFromName(const wxString& lexerName)
{
	if(lexerName == "txt")
		return wxSTC_LEX_NULL;  // 1
	if(lexerName == "python")
		return wxSTC_LEX_PYTHON;	// 2
	if(lexerName == "cpp")
		return wxSTC_LEX_CPP;	// 3
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
		case wxSTC_LEX_CPP:  // 3
			return "cpp";
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
_defStyle(type._defStyle),
_patterns(type._patterns),
_lexer(type._lexer),
_styleDef(type._styleDef)
{
	for(size_t n=0; n<wxSTC_KEYWORDSET_MAX; ++n)
		_keywords[n] = type._keywords[n];
}

const EditorStyle& FileType::getEditorStyle()const
{
	return EditorThemeManager::get().getStyle(_defStyle);
}

const Optional<wxString>& FileType::getEditorStyle(size_t n)const
{
	return getEditorStyle().getStyle(n);
}

const Optional<wxString>& FileType::getEditorStyleName(size_t n)const
{
	return getEditorStyle().getStyleName(n);
}

void FileType::expandFileTypeStyles()
{
	const EditorStyle &defStyle = EditorThemeManager::get().getStyle("default");
	const EditorStyle &refStyle = EditorThemeManager::get().getStyle(getDefaultStyle());

	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		wxString res;
	
		if(defStyle.hasStyle(n))
			res = *defStyle.getStyle(n);
		if(refStyle.hasStyle(n))
			res += "," + *refStyle.getStyle(n);

		if(_styleDef[n].set())
			res += "," + *_styleDef[n];
	
		_appliedStyle[n] = EditorThemeManager::get().getThemeExpandedValue(res);
	}
}

void FileType::expandFileTypeStyle(size_t n)
{
	const EditorStyle &defStyle = EditorThemeManager::get().getStyle("default");
	const EditorStyle &refStyle = EditorThemeManager::get().getStyle(getDefaultStyle());

	wxString res;
	
	if(defStyle.hasStyle(n))
		res = *defStyle.getStyle(n);
	if(refStyle.hasStyle(n))
		res += "," + *defStyle.getStyle(n);

	if(_styleDef[n].set())
		res += "," + *_styleDef[n];

	_appliedStyle[n] = EditorThemeManager::get().getThemeExpandedValue(res);
}


//
// FileTypeManager
//

FileTypeManager FileTypeManager::s_manager;
FileType        FileTypeManager::s_nullFileType;

wxWindowID	    FileTypeManager::s_firstFileTypeWindowID = 0;

wxString        FileTypeManager::s_fileTypeID[FT_COUNT] = 
{
	/* FT_NONE */       "txt",
	/* FT_PYTHON */     "python",
	/* FT_SCONS */      "scons",
	/* FT_CPP */        "cpp",
	/* FT_CSHARP */     "csharp",
	/* FT_RC */         "rc",
	/* FT_VALA */       "vala",
	/* FT_JAVA */       "java",
	/* FT_JAVAFX */     "javafx",
	/* FT_JAVASCRIPT */ "javascript",
	/* FT_IDL */        "idl",
	/* FT_FLASH */      "flash",
	/* FT_CH */         "ch",
	/* FT_GO */         "go",
	/* FT_PIKE */       "pike",
	/* FT_PROPERTIES */ "props",
	/* FT_ERROR */      "error",
	/* FT_MAKEFILE */   "make",
	/* FT_WINBATCH */   "winbatch",
	/* FT_DIFF */       "diff"	
};

FileTypeManager& FileTypeManager::get()
{
	return s_manager;
}


wxString FileTypeManager::fileTypeIDFromNum(int num)
{
	if(num>=0 && num<FT_COUNT)
		return s_fileTypeID[num];
	else
		return "";
}

int FileTypeManager::numFromFileTypeID(const wxString& type)
{
	for(int n=0; n<FT_COUNT; ++n)
	{
		if(type==s_fileTypeID[n])
			return n;
	}
	return FT_UNKNOWN;
}


bool FileTypeManager::readFromConfig(wxFileConfig* config, wxString absPath, FileType& filetype)
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
	filetype.setFilePattern(str);
	wxStringTokenizer tkzPattern(str, ";");
	while ( tkzPattern.HasMoreTokens() )
	{
		filetype._patterns.Add(tkzPattern.GetNextToken());
	}

	// File filter
	if(!config->Read("filter", &str))
		return false;
	filetype.setFileFilter(str);

	// Keyword lists (0-8)
	for(size_t n=0; n<=wxSTC_KEYWORDSET_MAX; ++n)
	{
		if(config->Read(wxString::Format("keywords.%lu", n), &str))
		{
			filetype._keywords[n] = str;
		}		
	}

	// Default style (if any)
	filetype._defStyle = config->Read("style", "");
	
	// Style definitions (0 - 39)
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		if(config->Read(wxString::Format("style.%lu", n), &str))
		{
			filetype._styleDef[n] = str;
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

void FileTypeManager::readFromConfig(wxFileConfig* config)
{
	wxString oldPath = config->GetPath();
	config->SetPath(CONFPATH_FILETYPE_ROOT);
	bool expand = config->IsExpandingEnvVars();
	config->SetExpandEnvVars(false);

	// Read all types
	for(int num=0; num<FT_COUNT; ++num)
	{
		_fileTypes[num] = FileType();
		wxString type = fileTypeIDFromNum(num);
		if(!readFromConfig(config, wxString(CONFPATH_FILETYPE_ROOT) + "/" + type, _fileTypes[num]))
		{
			std::cerr << "Error while loading file type '" << type << "'" << std::endl;
		}
	}
	
	expandFileTypeStyles();
	
	config->SetPath(oldPath);
	config->SetExpandEnvVars(expand);
}

const FileType& FileTypeManager::getFileType(const wxString& type)const
{
	return getFileType(numFromFileTypeID(type));
}

const FileType& FileTypeManager::getFileType(int type)const
{
	if(type>=0 && type<FT_COUNT)
		return _fileTypes[type];
	return s_nullFileType;
}

int FileTypeManager::deduceFileTypeFromName(const wxString& name)const
{
	for(int i=0; i<FT_COUNT; ++i)
	{
		const FileType& type = getFileType(i);
		for(size_t n=0; n<type.getPatterns().GetCount(); ++n)
		{
			if(name.Matches(type.getPatterns()[n]))
			   return i;
		}
	}
	return -1;
}
/*
FileType FileTypeManager::expandFileTypeStyle(const FileType& type)const
{
	FileType res;
	EditorStyle style;

	// Copy current type to result
	res = type;	
	
	// Get its default style if any. 
	if(!res.getDefaultStyle().IsEmpty())
	{
		style = EditorThemeManager::get().getStyle(res.getDefaultStyle());
	}
	else
	{
		style = EditorThemeManager::get().getStyle("default");
	}

	// Override styles with file-specific styles
	for(size_t n=0; n<wxSTC_STYLE_LASTPREDEFINED; ++n)
	{
		Optional<wxString>& st = res.getStyleDef(n);
		if(st)
		{
			style[n] = *style[n] + "," + *st;
		}
	}

	// Substitue variables variables
	EditorThemeManager::get().expandStyle(style);
	
	// Apply expanded theme to file type.
	res._styleDef = style;
	
	return res;
}
*/
void FileTypeManager::expandFileTypeStyles()
{
	for(int i=0; i<FT_COUNT; ++i)
    {
		FileType& type = _fileTypes[i];
		type.expandFileTypeStyles();
	}
}


wxString FileTypeManager::getWildcard()const
{
	wxString wildcard = "All files (autodetect)|*";

	for(int i=0; i<FT_COUNT; ++i)
    {
		const FileType& type = getFileType(i);
		wildcard += "|" + type.getFileFilter() + "|" + type.getFilePattern();
	}

	return wildcard;
}

wxWindowID FileTypeManager::getFirstWindowID()const
{
	if(s_firstFileTypeWindowID==0)
	{	
		s_firstFileTypeWindowID = wxIdManager::ReserveId(FT_COUNT);
	}
	return s_firstFileTypeWindowID;
}

