/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cody
 * Copyright (C) Émilien Kia 2012 <emilien.kia@gmail.com>
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

#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/xrc/xmlres.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/stdpaths.h>

#include "cody-app.hpp"

#include "config-view.hpp"
#include "editor-theme.hpp"
#include "fdartprov.hpp"
#include "file-type.hpp"
#include "main-frame.hpp"
#include "template-tools.hpp"
#include "text-document.hpp"
#include "text-frame.hpp"

IMPLEMENT_APP(CodyApp)

BEGIN_EVENT_TABLE(CodyApp, wxApp)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_ABOUT, CodyApp::onAbout)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_EXIT, CodyApp::onExit)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_PREFERENCES, CodyApp::onPreferences)
END_EVENT_TABLE()


bool CodyApp::OnInit()
{
	// Init all image handlers
	wxInitAllImageHandlers();

	// Plug additionnal art providers
	wxArtProvider::Push(new wxFreedesktopArtProvider("/usr/share/icons/gnome"));
	wxArtProvider::Push(new wxFreedesktopArtProvider(wxStandardPaths::Get().GetDataDir()+"/icons/hicolor"));
	
	// Ensure that user local data dir is created
	if(!wxDir::Exists(wxStandardPaths::Get().GetUserLocalDataDir()))
	   wxDir::Make(wxStandardPaths::Get().GetUserLocalDataDir());

	// Load config
 	_config = new wxFileConfig("Cody", "Cody", 
		wxStandardPaths::Get().GetUserLocalDataDir() + wxFileName::GetPathSeparator() + "cody.conf",
	    wxStandardPaths::Get().GetDataDir() + wxFileName::GetPathSeparator() + "cody.conf");

	// Load theme and style
	EditorThemeManager::get().readFromConfig(_config);
	
	// Load file type descriptions
	FileTypeManager::get().readFromConfig(_config);
	
	// Load history from conf
	_fileHistory.Load(*_config);

	// Create the first main frame
	_frame = new MainFrame();
	_frame->Show(TRUE);

	// Create an empty document
	createEmptyDocument(_frame);

	return TRUE;
}

int CodyApp::OnExit()
{
	delete _config;
	_config = NULL;
	
	return wxApp::OnExit();
}

wxConfig* CodyApp::getConfig()
{
	return _config;
}

TextDocument* CodyApp::createEmptyDocument(MainFrame* mainFrame)
{
	if(mainFrame==NULL)
		mainFrame = _frame;

	TextDocument* doc = new TextDocument("<unnamed>");
	_documents.insert(doc);
	
	TextFrame* frame = doc->createFrame(mainFrame->getNotebook());

	return doc;
}

TextDocument* CodyApp::loadDocument(const wxString& path, MainFrame* mainFrame)
{
	TextDocument* doc = createEmptyDocument(mainFrame);

	wxFileName filename = path;
	filename.MakeAbsolute();
	wxString filepath = filename.GetFullPath();
	
	if(doc->loadFile(filepath))
	{
		_fileHistory.AddFileToHistory(filepath);
		_fileHistory.Save(*_config);

		wxString type = FileTypeManager::get().deduceFileTypeFromName(filename.GetFullName());
		if(!type.IsEmpty())
		{
			FileType ftype = FileTypeManager::get().getFileType(type);
			doc->setDocumentType(ftype);
		}
	}

	if(mainFrame)
		mainFrame->UpdateTitle();
	
	return doc;
}

std::list<TextDocument*> CodyApp::queryLoadFile(MainFrame* mainFrame)
{
	std::list<TextDocument*> list;
	
	wxFileDialog openDialog(mainFrame, "Open files", wxEmptyString, wxEmptyString,
	"All files (*)|*", wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);
	
	if(openDialog.ShowModal() == wxID_OK)
	{
		wxArrayString filenames;
		openDialog.GetPaths(filenames);

		for(size_t n=0; n<filenames.size(); ++n)
		{
			TextDocument* doc = loadDocument(filenames[n], mainFrame);
			if(doc)
				list.push_back(doc);
		}
	}
	return list;
}

bool CodyApp::saveDocumentAs(TextDocument* doc, MainFrame* mainFrame)
{
	if(doc==NULL)
		return false;
	wxString file = wxSaveFileSelector("Save a file", "*", doc->getFile(), mainFrame);
	if(!file.IsEmpty())
	{
		return doc->saveFileAs(file);
	}
	return false;
}

void CodyApp::closeDocument(TextDocument* doc)
{
	if(doc)
	{
		wxAuiNotebook* parent = doc->getParent();
		TextFrame*     frame  = doc->getFrame();

		if(parent && frame)
		{
			if(doc->isMosified())
			{
				// TODO Query then save it.
			}
			
			int idx = parent->GetPageIndex(frame);
			if(idx>=0)
			{
				parent->DeletePage(idx);
				frame = NULL;
				parent = NULL;
			}
		}

		_documents.erase(doc);
		delete doc;
	}
}

void CodyApp::closeAllFrameDocuments(MainFrame* mainFrame)
{
	if(!mainFrame)
		mainFrame = _frame;

	wxAuiNotebook* notebook = mainFrame->getNotebook();

	typedef std::set<TextDocument*>::iterator iter;
	for(iter it=_documents.begin(); it!=_documents.end(); ++it)
	{
		TextDocument* doc = *it;
		if(doc->getParent()==notebook)
		{
			closeDocument(doc);
		}
	}
}

void CodyApp::onAbout(wxRibbonButtonBarEvent& event)
{
	wxAboutDialogInfo aboutInfo;
	aboutInfo.SetName("Cody");
	aboutInfo.SetVersion("0.1", "0.1 dev");
	aboutInfo.SetDescription(_("A simple code/script/text editor"));
	aboutInfo.SetCopyright("(C) 2012");
	aboutInfo.SetWebSite("https://github.com/balooloo/cody");
	aboutInfo.AddDeveloper("Émilien Kia <emilien.kia@gmail.com>");

	wxAboutBox(aboutInfo);
}

void CodyApp::onExit(wxRibbonButtonBarEvent& event)
{
	_frame->Close();
}

wxFileHistory& CodyApp::getFileHistory()
{
	return _fileHistory;
}

void CodyApp::onPreferences(wxRibbonButtonBarEvent& event)
{
	preferences();
}

void CodyApp::preferences()
{
	wxDialog dialog(_frame, wxID_ANY, "Cody - Preferences");
	wxSizer* gsz = new wxBoxSizer(wxVERTICAL);
	wxNotebook* notebook = new wxNotebook(&dialog, wxID_ANY);

	notebook->AddPage(new ConfigView(notebook), "View");
	
	gsz->Add(notebook, 1, wxEXPAND|wxALL, 4);
	gsz->Add(dialog.CreateButtonSizer(wxCLOSE), 0, wxEXPAND|wxALL, 4);
	dialog.SetSizer(gsz);
	dialog.Fit();
	dialog.AddMainButtonId(wxID_CLOSE);
	dialog.SetAffirmativeId(wxID_CLOSE);
	dialog.ShowModal();
}
