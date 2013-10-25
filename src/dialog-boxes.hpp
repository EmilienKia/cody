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

#ifndef _DIALOG_BOXES_HPP_
#define _DIALOG_BOXES_HPP_

class IndentationDialog : public wxDialog
{
public:
  IndentationDialog(wxWindow* parent);

  bool useTabs() const;
  void useTabs(bool useTabs);

  int indents() const;
  void indents(int indents);

protected:

  wxCheckBox* _useTabs;
  wxSpinCtrl* _indents;

//  bool _useTabs;
//  int  _indents;

};


#endif // _DIALOG_BOXES_HPP_

