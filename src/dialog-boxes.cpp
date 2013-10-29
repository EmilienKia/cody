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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "dialog-boxes.hpp"

//
// IndentationDialog
//
IndentationDialog::IndentationDialog(wxWindow* parent):
wxDialog(parent, wxID_ANY, "Indentations", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
{
  wxSizer* gsz = new wxBoxSizer(wxVERTICAL);

  wxFlexGridSizer* sz = new wxFlexGridSizer(2, 8, 8);

  sz->Add(new wxStaticText(this, wxID_ANY, "Use tabs:"), 0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL);
  sz->Add(_useTabs = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator), 0, wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL);

  sz->Add(new wxStaticText(this, wxID_ANY, "Tab size:"), 0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL);
  sz->Add(_indents = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 64, 4), 0, wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL);

  gsz->AddStretchSpacer(1);
  gsz->Add(sz, 0, wxALIGN_CENTRE_HORIZONTAL);
  gsz->AddStretchSpacer(1);
  gsz->Add(CreateStdDialogButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND);
  SetSizer(gsz);

  gsz->FitInside(this);
}

bool IndentationDialog::useTabs() const
{
  return _useTabs->GetValue();
}

void IndentationDialog::useTabs(bool useTabs)
{
  _useTabs->SetValue(useTabs);
}

int IndentationDialog::indents() const
{
  return _indents->GetValue();
}

void IndentationDialog::indents(int indents)
{
  _indents->SetValue(indents);
}
