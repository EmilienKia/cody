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
#include <wx/xrc/xmlres.h>
#include <wx/ribbon/buttonbar.h>

#include "cody-app.hpp"

#include "main-frame.hpp"
#include "text-document.hpp"
#include "text-frame.hpp"

IMPLEMENT_APP(CodyApp)

BEGIN_EVENT_TABLE(CodyApp, wxApp)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_ABOUT, CodyApp::onAbout)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_EXIT, CodyApp::onExit)
END_EVENT_TABLE()


bool CodyApp::OnInit()
{
	_config = new wxConfig("Cody");
	_fileHistory.Load(*_config);
	
	_frame = new MainFrame();
	_frame->Show(TRUE);

	createEmptyDocument(_frame);
	
	return TRUE;
}

int CodyApp::OnExit()
{
	delete _config;
	_config = NULL;
	
	return wxApp::OnExit();
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
	// TODO absolutize file path and verify existance
	if(doc->loadFile(path))
	{
		_fileHistory.AddFileToHistory(path);
		_fileHistory.Save(*_config);
	}
	return doc;
}

TextDocument* CodyApp::queryLoadFile(MainFrame* mainFrame)
{
	wxString file = wxLoadFileSelector("Open a file", "*", wxEmptyString, mainFrame);
	if(file.IsEmpty())
		return NULL;
	return loadDocument(file, mainFrame);
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
	aboutInfo.SetWebSite("http://emilienkia.net");
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
