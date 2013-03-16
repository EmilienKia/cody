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

#include "config-view.hpp"

#include "cody-app.hpp"
#include "decls.hpp"
#include "text-document.hpp"
#include "text-frame.hpp"

enum ConfigViewID
{
	ConfigView_CaretLine = 100
};


wxBEGIN_EVENT_TABLE(ConfigView, wxPanel)
	EVT_CHECKBOX(ConfigView_CaretLine, ConfigView::onCheckCaretLine)
wxEND_EVENT_TABLE()


ConfigView::ConfigView(wxWindow* parent, wxWindowID id):
wxPanel(parent, id)
{
	Initialize();
}

void ConfigView::Initialize()
{
	wxSizer* sz = new wxBoxSizer(wxVERTICAL);

	wxConfig* conf = wxGetApp().getConfig();
	wxCheckBox *cb;
	
	{
		sz->Add(new wxStaticText(this, wxID_ANY, "Decorations"), 0, wxALL, 4);

		cb = new wxCheckBox(this, ConfigView_CaretLine, "Caret line");
		cb->SetValue(conf->ReadBool(CONFPATH_EDITOR_SHOWCARETLINE, CONFDEFAULT_EDITOR_SHOWCARETLINE));
		sz->Add(cb, 0, wxALL, 4);
		        
	}
	
	SetSizer(sz);
}

void ConfigView::onCheckCaretLine(wxCommandEvent& event)
{
	bool checked = event.IsChecked();
	wxGetApp().getConfig()->Write(CONFPATH_EDITOR_SHOWCARETLINE, checked);
	for(std::set<TextDocument*>::iterator it=wxGetApp().getDocuments().begin(); it!=wxGetApp().getDocuments().end(); ++it)
	{
		TextDocument* txt = *it;
		txt->getFrame()->showCaretLine(checked);
	}
	
}
