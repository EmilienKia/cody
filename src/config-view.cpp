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

#include <wx/statbox.h>

#include "config-view.hpp"

#include "cody-app.hpp"
#include "decls.hpp"
#include "text-document.hpp"
#include "text-frame.hpp"

enum ConfigViewID
{
	ConfigView_ShowCaretLine = 100,
	ConfigView_ShowWhiteSpaces,
	ConfigView_ShowIndentGuides,
	ConfigView_ShowEndOfLines,
	ConfigView_WrapLongLines
	
};


wxBEGIN_EVENT_TABLE(ConfigView, wxPanel)
	EVT_CHECKBOX(ConfigView_ShowCaretLine, ConfigView::onCheckShowCaretLine)
	EVT_CHECKBOX(ConfigView_ShowWhiteSpaces, ConfigView::onCheckShowWhiteSpaces)
	EVT_CHECKBOX(ConfigView_ShowIndentGuides, ConfigView::onCheckShowIndentGuides)
	EVT_CHECKBOX(ConfigView_ShowEndOfLines, ConfigView::onCheckShowEndOfLines)
	EVT_CHECKBOX(ConfigView_WrapLongLines, ConfigView::onCheckWrapLongLines)
wxEND_EVENT_TABLE()

ConfigView::ConfigView(wxWindow* parent, wxWindowID id):
wxPanel(parent, id)
{
	Initialize();
}

void ConfigView::Initialize()
{
	wxSizer* gsz = new wxBoxSizer(wxVERTICAL);

	wxConfig* conf = wxGetApp().getConfig();
	wxCheckBox *cb;

	{
		wxStaticBoxSizer *bsz = new wxStaticBoxSizer(wxVERTICAL, this, "Decorations");		

		// Caret line
		cb = new wxCheckBox(bsz->GetStaticBox(), ConfigView_ShowCaretLine, "Caret line");
		cb->SetValue(conf->ReadBool(CONFPATH_EDITOR_SHOWCARETLINE, CONFDEFAULT_EDITOR_SHOWCARETLINE));
		bsz->Add(cb, 0, wxALL, 4);

		// White spaces
		cb = new wxCheckBox(bsz->GetStaticBox(), ConfigView_ShowWhiteSpaces, "White space");
		cb->SetValue(conf->ReadBool(CONFPATH_EDITOR_SHOWWHITESPACES, CONFDEFAULT_EDITOR_SHOWWHITESPACES));
		bsz->Add(cb, 0, wxALL, 4);
		
		// Indentation guides
		cb = new wxCheckBox(bsz->GetStaticBox(), ConfigView_ShowIndentGuides, "Indentation guides");
		cb->SetValue(conf->ReadBool(CONFPATH_EDITOR_SHOWINDENTGUIDES, CONFDEFAULT_EDITOR_SHOWINDENTGUIDES));
		bsz->Add(cb, 0, wxALL, 4);

		// EOL
		cb = new wxCheckBox(bsz->GetStaticBox(), ConfigView_ShowEndOfLines, "End of lines");
		cb->SetValue(conf->ReadBool(CONFPATH_EDITOR_SHOWENDOFLINES, CONFDEFAULT_EDITOR_SHOWENDOFLINES));
		bsz->Add(cb, 0, wxALL, 4);

		// Wrap long lines
		cb = new wxCheckBox(bsz->GetStaticBox(), ConfigView_WrapLongLines, "Wrap long lines");
		cb->SetValue(conf->ReadBool(CONFPATH_EDITOR_WRAPLONGLINES, CONFDEFAULT_EDITOR_WRAPLONGLINES));
		bsz->Add(cb, 0, wxALL, 4);

		bsz->RecalcSizes();		
		gsz->Add(bsz, 0, wxEXPAND|wxALL, 8);
	}

	SetSizer(gsz);
}

void ConfigView::onCheckShowCaretLine(wxCommandEvent& event)
{
	bool checked = event.IsChecked();
	wxGetApp().getConfig()->Write(CONFPATH_EDITOR_SHOWCARETLINE, checked);
	for(std::set<TextDocument*>::iterator it=wxGetApp().getDocuments().begin(); it!=wxGetApp().getDocuments().end(); ++it)
	{
		TextDocument* txt = *it;
		txt->getFrame()->showCaretLine(checked);
	}	
}

void ConfigView::onCheckShowWhiteSpaces(wxCommandEvent& event)
{
	bool checked = event.IsChecked();
	wxGetApp().getConfig()->Write(CONFPATH_EDITOR_SHOWWHITESPACES, checked);
	for(std::set<TextDocument*>::iterator it=wxGetApp().getDocuments().begin(); it!=wxGetApp().getDocuments().end(); ++it)
	{
		TextDocument* txt = *it;
		txt->getFrame()->showWhiteSpaces(checked);
	}	
}

void ConfigView::onCheckShowIndentGuides(wxCommandEvent& event)
{
	bool checked = event.IsChecked();
	wxGetApp().getConfig()->Write(CONFPATH_EDITOR_SHOWINDENTGUIDES, checked);
	for(std::set<TextDocument*>::iterator it=wxGetApp().getDocuments().begin(); it!=wxGetApp().getDocuments().end(); ++it)
	{
		TextDocument* txt = *it;
		txt->getFrame()->showIndentationGuides(checked);
	}	
}

void ConfigView::onCheckShowEndOfLines(wxCommandEvent& event)
{
	bool checked = event.IsChecked();
	wxGetApp().getConfig()->Write(CONFPATH_EDITOR_SHOWENDOFLINES, checked);
	for(std::set<TextDocument*>::iterator it=wxGetApp().getDocuments().begin(); it!=wxGetApp().getDocuments().end(); ++it)
	{
		TextDocument* txt = *it;
		txt->getFrame()->showEOL(checked);
	}	
}

void ConfigView::onCheckWrapLongLines(wxCommandEvent& event)
{
	bool checked = event.IsChecked();
	wxGetApp().getConfig()->Write(CONFPATH_EDITOR_WRAPLONGLINES, checked);
	for(std::set<TextDocument*>::iterator it=wxGetApp().getDocuments().begin(); it!=wxGetApp().getDocuments().end(); ++it)
	{
		TextDocument* txt = *it;
		txt->getFrame()->wrapLongLines(checked);
	}	
}

