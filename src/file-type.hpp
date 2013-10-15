/* -*- Mode: C++; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*-  */
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
#include <wx/fileconf.h>
#include <wx/stc/stc.h>

#include "template-tools.hpp"
#include "editor-theme.hpp"

class CodyApp;

class FileType;
typedef std::map<wxString, FileType> FileTypeMap;

class FileTypeManager;



enum FILE_TYPE
{
	FT_UNKNOWN = -1,
	FT_NONE = 0,
	FT_DEFAULT = FT_NONE,
	FT_TEXT = FT_NONE,
	FT_PYTHON,
	FT_SCONS,
	FT_CPP,
	FT_CSHARP,
	FT_RC,
	FT_VALA,
	FT_JAVA,
	FT_JAVAFX,
	FT_JAVASCRIPT,
	FT_IDL,
	FT_FLASH,
	FT_CH,
	FT_GO,
	FT_PIKE,
	FT_PROPERTIES,
	FT_ERROR,
	FT_MAKEFILE,
	FT_WINBATCH,
	FT_DIFF,

	
	FT_COUNT
};


class FileType
{
	friend class CodyApp;
	friend class FileTypeManager;
public:
	FileType();
	FileType(const FileType& type);

	bool Ok()const{return !getID().IsEmpty();}

	wxString getID()const{return _id;}
	void setID(const wxString& id){_id = id;}
	
	wxString getName()const{return _name;}
	void setName(const wxString& name){_name = name;}

	wxString getFileFilter()const{return _fileFilter;}
	void setFileFilter(const wxString& fileFilter){_fileFilter = fileFilter;}

	wxString getDefaultStyle()const{return _defStyle;}
	void setDefaultStyle(const wxString& defStyle){_defStyle = defStyle;}

	wxString getFilePattern()const{return _filePattern;}
	void setFilePattern(const wxString& filePattern){_filePattern = filePattern;}
	
	const wxArrayString& getPatterns()const{return _patterns;}
	void setPatterns(const wxArrayString& patterns){_patterns = patterns;}

	int getLexer()const{return _lexer;}
	void setLexer(int lexer){_lexer = lexer;}
	wxString getLexerName()const{return lexerToName(_lexer);}
	void setLexerName(const wxString& lexerName){setLexer(lexerFromName(lexerName));}


	
	const Optional<wxString>& getStyleDef(size_t n)const{return _styleDef[n];}
	Optional<wxString>& getStyleDef(size_t n){return _styleDef[n];}

	const StyleDef& getAppliedStyle(size_t n)const{return _appliedStyle[n];}
	StyleDef& getAppliedStyle(size_t n){return _appliedStyle[n];}
	
	const Optional<wxString>& getKeywords(size_t n)const{return _keywords[n];}
	Optional<wxString>& getKeywords(size_t n){return _keywords[n];}

	const EditorStyle& getEditorStyle()const;
	const Optional<wxString>& getEditorStyle(size_t n)const;
	const Optional<wxString>& getEditorStyleName(size_t n)const;

	wxString getProperty(const wxString& name)const;
	void setProperty(const wxString& name, const wxString& value);
	void resetProperty(const wxString& name);
	void getPropertyNames(wxArrayString& arr)const;

	void expandFileTypeStyles();
	void expandFileTypeStyle(size_t n);
protected:
	wxString _name, _id, _fileFilter, _defStyle, _filePattern;
	wxArrayString _patterns;
	int _lexer;

	Optional<wxString> _styleDef[wxSTC_STYLE_LASTPREDEFINED];
	StyleDef           _appliedStyle[wxSTC_STYLE_LASTPREDEFINED];
	
	Optional<wxString> _keywords[wxSTC_KEYWORDSET_MAX];

	std::map<wxString, wxString> _properties;
	
	static int lexerFromName(const wxString& lexerName);
	static wxString lexerToName(int lexer);
};


class FileTypeManager
{
private:
	static FileTypeManager s_manager;
	static FileType        s_nullFileType;

	static wxString        s_fileTypeID[FT_COUNT];
	static wxWindowID	   s_firstFileTypeWindowID;
	
public:
	static FileTypeManager& get();

	static wxString fileTypeIDFromNum(int num);
	static int numFromFileTypeID(const wxString& type);
	
	const FileType& getFileType(const wxString& type)const;
	const FileType& getFileType(int type)const;

	void resetFileTypeStyle(int type, int style);
	void setFileTypeStyle(int type, int style, const wxString& stylestr);

	void setFileTypeKeywords(int type, int kw, const wxString& keywords);
	
	void setFileTypeProperty(int type, const wxString& name, const wxString& value);

	int deduceFileTypeFromName(const wxString& name)const;

//	FileType expandFileTypeStyle(const FileType& type)const;

	wxString getWildcard()const;

	wxWindowID getFirstWindowID()const;
	
	void readFromConfig(wxFileConfig* config);
	bool readFromConfig(wxFileConfig* config, wxString absPath, FileType& filetype);

protected:
	void expandFileTypeStyles();
	void applyStyleToAllDocuments();
	void applyKeywordsToAllDocuments();
	void applyPropertiesToAllDocuments();
	
	FileType    _fileTypes[FT_COUNT];
};



#endif // _FILE_TYPE_HPP_

