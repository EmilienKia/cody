/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cody
 * Copyright (C) Émilien Kia 2012 <emilien@emilien-vm-lm-maya>
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

#include <wx/artprov.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>

#include "main-frame.hpp"
#include "cody-app.hpp"
#include "text-frame.hpp"
#include "text-document.hpp"
#include "bookmark.hpp"
#include "cody-app.hpp"


MainFrame::MainFrame():
wxFrame(NULL, wxID_ANY, "Cody")
{
	CommonInit();

	_recentFileMenu = new wxMenu;
	wxGetApp().getFileHistory().UseMenu(_recentFileMenu);
	wxGetApp().getFileHistory().AddFilesToMenu(_recentFileMenu);
}

void MainFrame::CommonInit()
{
	wxSizer* gsz = new wxBoxSizer(wxVERTICAL);
	_ribbon = new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxSize(640, 480), wxRIBBON_BAR_FLOW_HORIZONTAL
                                | wxRIBBON_BAR_SHOW_PAGE_LABELS
                                | wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS
                                | wxRIBBON_BAR_SHOW_TOGGLE_BUTTON
                                //| wxRIBBON_BAR_SHOW_HELP_BUTTON
                                );
	InitRibbon();
	_panel = new wxPanel(this, wxID_ANY);
	_manager.SetManagedWindow(_panel);
	
	gsz->Add(_ribbon, 0, wxEXPAND);
	gsz->Add(_panel, 1, wxEXPAND);
	SetSizer(gsz);

	_notebook = new wxAuiNotebook(_panel, wxID_ANY);
	_manager.AddPane(_notebook, wxAuiPaneInfo().CenterPane().PaneBorder(false));

	_bookmark = new BookmarkPanel(_panel, wxID_ANY);
	_manager.AddPane(_bookmark, wxAuiPaneInfo().Left().Caption("Bookmarks").MinSize(48, 48).Hide());
	
	_manager.Update();

	Layout();
}

void MainFrame::InitRibbon()
{
	{// File page
		wxRibbonPage* page = new wxRibbonPage(_ribbon, wxID_ANY, "File");
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Load");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(wxID_NEW, "New", wxArtProvider::GetBitmap(wxART_NEW, wxART_BUTTON, wxSize(24, 24)));
			bar->AddHybridButton(wxID_OPEN, "Open", wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_REVERT_TO_SAVED, "Revert", wxArtProvider::GetBitmap(wxART_UNDO, wxART_BUTTON, wxSize(24, 24)));
		}
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Save");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(wxID_SAVE, "Save", wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_SAVEAS, "Save as", wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(XRCID("Save all"), "Save all", wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_BUTTON, wxSize(24, 24)));
		}
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Close");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(wxID_CLOSE, "Close", wxArtProvider::GetBitmap(wxART_CLOSE, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(XRCID("Close all"), "Close all", wxArtProvider::GetBitmap(wxART_CLOSE, wxART_BUTTON, wxSize(24, 24)));
		}
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Help");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(wxID_HELP, "Help", wxArtProvider::GetBitmap(wxART_HELP, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_ABOUT, "About Cody", wxArtProvider::GetBitmap(wxART_INFORMATION, wxART_BUTTON, wxSize(24, 24)));
		}
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(wxID_EXIT, "Quit", wxArtProvider::GetBitmap(wxART_QUIT, wxART_BUTTON, wxSize(24, 24)));
		}
	}
	{// Edit page
		wxRibbonPage* page = new wxRibbonPage(_ribbon, wxID_ANY, "Edit");
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "History");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(wxID_UNDO, "Undo", wxArtProvider::GetBitmap(wxART_UNDO, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_REDO, "Redo", wxArtProvider::GetBitmap(wxART_REDO, wxART_BUTTON, wxSize(24, 24)));
		}
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Clipboard");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(wxID_DELETE, "Delete", wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_CUT, "Cut", wxArtProvider::GetBitmap(wxART_CUT, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_COPY, "Copy", wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_PASTE, "Paste", wxArtProvider::GetBitmap(wxART_PASTE, wxART_BUTTON, wxSize(24, 24)));
		}
	}
	{// Navigate page
		wxRibbonPage* page = new wxRibbonPage(_ribbon, wxID_ANY, "Navigate");
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Search");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(wxID_FIND, "Find", wxArtProvider::GetBitmap(wxART_FIND, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_BACKWARD, "Find previous", wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_FORWARD, "Find next", wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(XRCID("Go to line"), "Go to line", wxArtProvider::GetBitmap(wxART_GOTO_LAST, wxART_BUTTON, wxSize(24, 24)));
		}
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, XRCID("Bookmark panel"), "Bookmarks", wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_DEFAULT_STYLE|wxRIBBON_PANEL_EXT_BUTTON);
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddButton(XRCID("Toggle bookmark"), "Toggle", wxArtProvider::GetBitmap(wxART_ADD_BOOKMARK, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_UP, "Previous", wxArtProvider::GetBitmap(wxART_GO_UP, wxART_BUTTON, wxSize(24, 24)));
			bar->AddButton(wxID_DOWN, "Next", wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_BUTTON, wxSize(24, 24)));
		}
	}
	{// View page
		wxRibbonPage* page = new wxRibbonPage(_ribbon, wxID_ANY, "View");
		{
			wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Decorations");
			wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
			bar->AddToggleButton(XRCID("Display line numbers"), "Line numbers", wxArtProvider::GetBitmap(wxART_GOTO_LAST, wxART_BUTTON, wxSize(24, 24)));
		}
	}
	_ribbon->Realise();
}

bool MainFrame::Destroy()
{
	if(!wxFrame::Destroy())
		return false;

	wxGetApp().getFileHistory().RemoveMenu(_recentFileMenu);
	delete _recentFileMenu;

	_manager.UnInit();

	return true;
}


wxStyledTextCtrl* MainFrame::getCurrentTextCtrl()
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame!=NULL)
	{
		return frame->getCurrentTextCtrl();
	}
	return NULL;
}

TextFrame* MainFrame::getCurrentTextFrame()
{
	wxWindow* window = _notebook->GetCurrentPage();
	if(!window)
		return NULL;
	return dynamic_cast<TextFrame*>(window);
}

TextDocument* MainFrame::getCurrentDocument()
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame)
	{
		return frame->getDocument();
	}
	return NULL;
}

void MainFrame::toggleBookmarkPanel()
{
	wxAuiPaneInfo& paneinfo = _manager.GetPane(_bookmark);
	paneinfo.Show(!paneinfo.IsShown());
	_manager.Update();
}

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_UPDATE_UI(wxID_SAVE, MainFrame::onUpdateHasOpenDocument)
	EVT_UPDATE_UI(wxID_SAVEAS, MainFrame::onUpdateHasOpenDocument)
	EVT_UPDATE_UI(XRCID("Save all"), MainFrame::onUpdateHasOpenDocument)
	EVT_UPDATE_UI(wxID_CLOSE, MainFrame::onUpdateHasOpenDocument)
	EVT_UPDATE_UI(XRCID("Close all"), MainFrame::onUpdateHasOpenDocument)

	EVT_UPDATE_UI(wxID_UNDO, MainFrame::onUpdateCanUndo)
	EVT_UPDATE_UI(wxID_REDO, MainFrame::onUpdateCanRedo)
	EVT_UPDATE_UI(wxID_DELETE, MainFrame::onUpdateHasOpenDocument)

	EVT_UPDATE_UI(wxID_CUT, MainFrame::onUpdateHasSelection)
	EVT_UPDATE_UI(wxID_COPY, MainFrame::onUpdateHasSelection)
	EVT_UPDATE_UI(wxID_PASTE, MainFrame::onUpdateCanPaste)


	EVT_RIBBONBUTTONBAR_CLICKED(wxID_NEW, MainFrame::onNewDocument)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_OPEN, MainFrame::onOpenDocument)
	EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(wxID_OPEN, MainFrame::onRecentDocumentMenu)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_REVERT_TO_SAVED, MainFrame::onRevertDocument)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_SAVE, MainFrame::onSaveDocument)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_SAVEAS, MainFrame::onSaveDocumentAs)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_CLOSE, MainFrame::onCloseDocument)
	EVT_RIBBONBUTTONBAR_CLICKED(XRCID("Close all"), MainFrame::onCloseAllDocuments)

	EVT_RIBBONBUTTONBAR_CLICKED(wxID_UNDO, MainFrame::onUndo)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_REDO, MainFrame::onRedo)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_DELETE, MainFrame::onDelete)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_CUT, MainFrame::onCut)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_COPY, MainFrame::onCopy)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_PASTE, MainFrame::onPaste)

	EVT_RIBBONBUTTONBAR_CLICKED(wxID_FIND, MainFrame::onFind)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_BACKWARD, MainFrame::onFindPrev)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_FORWARD, MainFrame::onFindNext)
	EVT_RIBBONBUTTONBAR_CLICKED(XRCID("Go to line"), MainFrame::onGoToLine)

	EVT_RIBBONPANEL_EXTBUTTON_ACTIVATED(XRCID("Bookmark panel"), MainFrame::onBookmarkRibbonBarExtActivated)
	EVT_RIBBONBUTTONBAR_CLICKED(XRCID("Toggle bookmark"), MainFrame::onToggleBookmark)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_UP, MainFrame::onPreviousBookmark)
	EVT_RIBBONBUTTONBAR_CLICKED(wxID_DOWN, MainFrame::onNextBookmark)

	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, MainFrame::onPageClosing)
	EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, MainFrame::onPageChanged)
END_EVENT_TABLE()

void MainFrame::onNewDocument(wxRibbonButtonBarEvent& WXUNUSED(event))
{
	wxGetApp().createEmptyDocument(this);
}

void MainFrame::onOpenDocument(wxRibbonButtonBarEvent& event)
{
	wxGetApp().queryLoadFile(this);
}

void MainFrame::onRecentDocumentMenu(wxRibbonButtonBarEvent& event)
{
	event.PopupMenu(_recentFileMenu);
}

void MainFrame::onRevertDocument(wxRibbonButtonBarEvent& event)
{
	TextDocument* doc = getCurrentDocument();
	if(doc)
	{
		// TODO message box
		doc->reloadFile();
	}
}

void MainFrame::onSaveDocument(wxRibbonButtonBarEvent& event)
{
	TextDocument* doc = getCurrentDocument();
	if(doc)
	{
		if(!doc->getFile().IsEmpty())
			doc->saveFile();
		else
		{
			wxGetApp().saveDocumentAs(doc, this);
		}
	}
}

void MainFrame::onSaveDocumentAs(wxRibbonButtonBarEvent& event)
{
	TextDocument* doc = getCurrentDocument();
	if(doc)
	{
		wxGetApp().saveDocumentAs(doc, this);
	}
}

void MainFrame::onCloseDocument(wxRibbonButtonBarEvent& event)
{
	TextDocument* doc = getCurrentDocument();
	if(doc)
	{
		wxGetApp().closeDocument(doc);
	}
}

void MainFrame::onCloseAllDocuments(wxRibbonButtonBarEvent& event)
{
	wxGetApp().closeAllFrameDocuments(this);
}


void MainFrame::onPageClosing(wxAuiNotebookEvent& event)
{
	TextDocument* doc = getCurrentDocument();
	if(doc)
	{
		wxGetApp().closeDocument(doc);
		event.Veto();
	}
}

void MainFrame::onPageChanged(wxAuiNotebookEvent& event)
{
	if(getBookmarkPanel())
	{
		getBookmarkPanel()->setView(getCurrentTextFrame());
	}
}

void MainFrame::onUndo(wxRibbonButtonBarEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt)
	{
		txt->Undo();
	}
}

void MainFrame::onRedo(wxRibbonButtonBarEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt)
	{
		txt->Redo();
	}
}

void MainFrame::onDelete(wxRibbonButtonBarEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt)
	{
		txt->Clear();
	}
}

void MainFrame::onCut(wxRibbonButtonBarEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt)
	{
		txt->Cut();
	}
}

void MainFrame::onCopy(wxRibbonButtonBarEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt)
	{
		txt->Copy();
	}
}

void MainFrame::onPaste(wxRibbonButtonBarEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt)
	{
		txt->Paste();
	}
}

void MainFrame::onFind(wxRibbonButtonBarEvent& event)
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame)
	{
		frame->showFastFindText();
	}
}

void MainFrame::onFindNext(wxRibbonButtonBarEvent& event)
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame)
	{
		frame->findNext();
	}
}

void MainFrame::onFindPrev(wxRibbonButtonBarEvent& event)
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame)
	{
		frame->findPrev();
	}
}

void MainFrame::onGoToLine(wxRibbonButtonBarEvent& event)
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame)
	{
		frame->showFastFindGoToLine();
	}
}

void MainFrame::onUpdateHasOpenDocument(wxUpdateUIEvent& event)
{
	event.Enable(getCurrentDocument()!=NULL);
}

void MainFrame::onUpdateCanUndo(wxUpdateUIEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt!=NULL)
	{
		event.Enable(txt->CanUndo());
	}
	else
	{
		event.Enable(false);
	}
}

void MainFrame::onUpdateCanRedo(wxUpdateUIEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt!=NULL)
	{
		event.Enable(txt->CanRedo());
	}
	else
	{
		event.Enable(false);
	}
}

void MainFrame::onUpdateCanPaste(wxUpdateUIEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt!=NULL)
	{
		event.Enable(txt->CanPaste());
	}
	else
	{
		event.Enable(false);
	}
}

void MainFrame::onUpdateHasSelection(wxUpdateUIEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
	if(txt!=NULL)
	{
		int begin, end;
		txt->GetSelection(&begin, &end);
		event.Enable(end > begin);
	}
	else
	{
		event.Enable(false);
	}	
}

void MainFrame::onBookmarkRibbonBarExtActivated(wxRibbonPanelEvent& event)
{
	toggleBookmarkPanel();
}

void MainFrame::onToggleBookmark(wxRibbonButtonBarEvent& event)
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame)
	{
		frame->toggleBookmark();
	}
}

void MainFrame::onPreviousBookmark(wxRibbonButtonBarEvent& event)
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame)
	{
		frame->gotoPrevBookmark();
	}
}

void MainFrame::onNextBookmark(wxRibbonButtonBarEvent& event)
{
	TextFrame* frame = getCurrentTextFrame();
	if(frame)
	{
		frame->gotoNextBookmark();
	}
}

