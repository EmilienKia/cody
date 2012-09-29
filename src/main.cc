/*
 * main.cc
 * Copyright (C) 2012 Émilien Kia <emilien.kia@gmail.com>
 * 
 * cody is free software: you can redistribute it and/or modify it
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

#include "main-frame.hpp"

class CodyApp : public wxApp
{
	DECLARE_EVENT_TABLE();
  public:
    virtual bool OnInit();
};

IMPLEMENT_APP(CodyApp)

BEGIN_EVENT_TABLE(CodyApp, wxApp)
END_EVENT_TABLE()

bool CodyApp::OnInit()
{
	MainFrame* frame = new MainFrame();
	frame->Show(TRUE);
	return TRUE;
}
