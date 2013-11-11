/* -*- Mode: C++; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cody
 * Copyright (C) 2012-2013 Émilien KIA <emilien.kia@gmail.com>
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
#include "dialog-boxes.hpp"


#define wxArtIcon(artid, sz) (wxArtProvider::GetBitmap(artid, wxART_OTHER, wxSize(sz,sz)))
#define RibbonIcon(artid) wxArtIcon(artid, 32)



//
// MainFrameDropTarget
//
class MainFrameDropTarget : public wxDropTarget
{
public:
	MainFrameDropTarget(MainFrame* frame):
	_frame(frame)
	{
		wxDataObjectComposite* dataobj = new wxDataObjectComposite();
		dataobj->Add(new wxFileDataObject(), true);
		SetDataObject(dataobj);
	}

	wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult defaultDragResult);

private:
	MainFrame* _frame;
};

wxDragResult MainFrameDropTarget::OnData(wxCoord x, wxCoord y, wxDragResult defaultDragResult)
{
    if( !GetData() )
	{
		wxMessageBox("Nothing to drop.");
        return wxDragNone;
	}

	wxDataObjectComposite * dataObjs = dynamic_cast<wxDataObjectComposite *>(GetDataObject());
	if(dataObjs)
	{
		wxDataFormat format = dataObjs->GetReceivedFormat();
		wxDataObject *dataObj = dataObjs->GetObject(format);
		switch( format.GetType() )
		{
			case wxDF_FILENAME:
			{
				wxFileDataObject *dataObjFile = static_cast<wxFileDataObject *>(dataObj);
				wxArrayString files = dataObjFile->GetFilenames();
				for(size_t n=0; n<files.GetCount(); ++n)
				{
					wxGetApp().loadDocument(files[n], _frame);
				}
				return wxDragCopy;
			}
			break;
			default:
				wxFAIL_MSG( "Unexpected data object format to drop." );
				return wxDragNone;
		}
	}
	return wxDragNone;
}

//
// MainFrame
//

MainFrame::MainFrame():
    wxFrame(NULL, wxID_ANY, "Cody")
{
    CommonInit();

    _recentFileMenu = new wxMenu;
    wxGetApp().getFileHistory().UseMenu(_recentFileMenu);
    wxGetApp().getFileHistory().AddFilesToMenu(_recentFileMenu);

    _eolMenu = new wxMenu;
    _eolMenu->AppendCheckItem(XRCID("EOL_CR"), "CR");
    _eolMenu->AppendCheckItem(XRCID("EOL_LF"), "LF (Unix)");
    _eolMenu->AppendCheckItem(XRCID("EOL_CRLF"), "CR-LF (Windows)");

    _indentMenu = new wxMenu;
    _indentMenu->AppendCheckItem(XRCID("Indent use tabs"), "Use tabs");
    _indentMenu->AppendSeparator();
    _indentMenu->AppendCheckItem(XRCID("Indent tab 2"), "2");
    _indentMenu->AppendCheckItem(XRCID("Indent tab 3"), "3");
    _indentMenu->AppendCheckItem(XRCID("Indent tab 4"), "4");
    _indentMenu->AppendCheckItem(XRCID("Indent tab 8"), "8");
    _indentMenu->AppendCheckItem(XRCID("Indent tab other"), "Other ...");
}

void MainFrame::CommonInit()
{
    wxSizer* gsz = new wxBoxSizer(wxVERTICAL);
    _ribbon = new wxRibbonBar(this, wxID_ANY, wxDefaultPosition, wxSize(800, 600), wxRIBBON_BAR_FLOW_HORIZONTAL
                              | wxRIBBON_BAR_SHOW_PAGE_LABELS
                              | wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS
                              //| wxRIBBON_BAR_SHOW_TOGGLE_BUTTON
                              //| wxRIBBON_BAR_SHOW_HELP_BUTTON
                             );
    InitFileTypeMenu();
    InitRibbon();
    InitAcceleratorTable();

    _panel = new wxPanel(this, wxID_ANY);
    _manager.SetManagedWindow(_panel);

    gsz->Add(_ribbon, 0, wxEXPAND);
    gsz->Add(_panel, 1, wxEXPAND);
    SetSizer(gsz);

    _notebook = new wxAuiNotebook(_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE|wxAUI_NB_TAB_SPLIT);
    _manager.AddPane(_notebook, wxAuiPaneInfo().CenterPane().PaneBorder(false));

    _bookmark = new BookmarkPanel(_panel, wxID_ANY);
    _manager.AddPane(_bookmark, wxAuiPaneInfo().Left().Caption("Bookmarks").MinSize(48, 48).Hide());

    _manager.Update();

    Layout();

    SetSize(800, 600);

	SetIcons(wxArtProvider::GetIconBundle("cody", wxART_FRAME_ICON));

	// Setup drop target
	SetDropTarget(new MainFrameDropTarget(this));
}

void MainFrame::InitRibbon()
{
    {
        // File page
        wxRibbonPage* page = new wxRibbonPage(_ribbon, wxID_ANY, "File");
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Load");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_NEW, "New", RibbonIcon("document-new"));
            bar->AddHybridButton(wxID_OPEN, "Open", RibbonIcon("document-open"));
            bar->AddButton(wxID_REVERT_TO_SAVED, "Reload", RibbonIcon("document-reload"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Save");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_SAVE, "Save", RibbonIcon("document-save"));
            bar->AddButton(wxID_SAVEAS, "Save as", RibbonIcon("document-save-as"));
            bar->AddButton(XRCID("Save all"), "Save all", RibbonIcon("document-save-all"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Close");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_CLOSE, "Close", RibbonIcon("document-close"));
            bar->AddButton(XRCID("Close all"), "Close all", RibbonIcon("document-close-all"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_PREFERENCES, "Preferences", RibbonIcon("application-preferences"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Help");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_HELP, "Help", RibbonIcon("help-contents"));
            bar->AddButton(wxID_ABOUT, "About Cody", RibbonIcon("help-about"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_EXIT, "Quit", RibbonIcon("application-exit"));
        }
    }
    {
        // Edit page
        wxRibbonPage* page = new wxRibbonPage(_ribbon, wxID_ANY, "Edit");
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "History");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_UNDO, "Undo", RibbonIcon("edit-undo"));
            bar->AddButton(wxID_REDO, "Redo", RibbonIcon("edit-redo"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Clipboard");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_DELETE, "Delete",RibbonIcon("edit-delete"));
            bar->AddButton(wxID_CUT, "Cut", RibbonIcon("edit-cut"));
            bar->AddButton(wxID_COPY, "Copy", RibbonIcon("edit-copy"));
            bar->AddButton(wxID_PASTE, "Paste", RibbonIcon("edit-paste"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Content flow");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddHybridButton(XRCID("CONVERT_EOL"), "End of lines", RibbonIcon("edit-eol"));
            bar->AddHybridButton(XRCID("Indent convert"), "Indentations", RibbonIcon("edit-indent"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Selection");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(XRCID("Select all"), "Select all", RibbonIcon("edit-select-all"));
            //bar->AddButton(XRCID("Select line"), "Select line", RibbonIcon("edit-select-line"));
        }
    }
    {
        // Navigate page
        wxRibbonPage* page = new wxRibbonPage(_ribbon, wxID_ANY, "Navigate");
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, XRCID("Search panel"), "Search", wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_DEFAULT_STYLE|wxRIBBON_PANEL_EXT_BUTTON);
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(wxID_FIND, "Find", RibbonIcon("edit-find"));
            bar->AddButton(wxID_BACKWARD, "Find previous", RibbonIcon("go-previous"));
            bar->AddButton(wxID_FORWARD, "Find next", RibbonIcon("go-next"));
            bar->AddButton(XRCID("Go to line"), "Go to line", RibbonIcon("go-jump"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, XRCID("Bookmark panel"), "Bookmarks", wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_DEFAULT_STYLE|wxRIBBON_PANEL_EXT_BUTTON);
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(XRCID("Toggle bookmark"), "Toggle", RibbonIcon("bookmark-new"));
            bar->AddButton(wxID_UP, "Previous", RibbonIcon("bookmark-prev"));
            bar->AddButton(wxID_DOWN, "Next", RibbonIcon("bookmark-next"));
        }
    }
    {
        // View page
        wxRibbonPage* page = new wxRibbonPage(_ribbon, wxID_ANY, "View");
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Zoom");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddButton(XRCID("Zoom in"), "Zoom in", RibbonIcon("view-zoom-in"));
            bar->AddButton(XRCID("Zoom reset"), "Reset zoom", RibbonIcon("view-zoom-0"));
            bar->AddButton(XRCID("Zoom out"), "Zoom out", RibbonIcon("view-zoom-out"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Decorations");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddToggleButton(XRCID("Display caret line"), "Caret line", RibbonIcon("view-caret-line"));
            bar->AddToggleButton(XRCID("Display white spaces"), "White spaces", RibbonIcon("view-white-spaces"));
            bar->AddToggleButton(XRCID("Display indentation guides"), "Indentation guides", RibbonIcon("view-indent-lines"));
            bar->AddToggleButton(XRCID("Display end of lines"), "End of lines", RibbonIcon("view-end-of-lines"));
            bar->AddToggleButton(XRCID("Display wrap long lines"), "Wrap long lines", RibbonIcon("view-wrap-mode"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Margins");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddToggleButton(XRCID("Display line numbers"), "Line numbers", RibbonIcon("view-line-numbers"));
            bar->AddToggleButton(XRCID("Display markers"), "Markers", RibbonIcon("view-marker-margin"));
            bar->AddToggleButton(XRCID("Display folders"), "Folders", RibbonIcon("view-folder-margin"));
            bar->AddToggleButton(XRCID("Display long lines"), "Long line", RibbonIcon("view-long-lines"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "Language");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddDropdownButton(XRCID("Select language"), "Languages", RibbonIcon("view-long-lines"));
        }
        {
            wxRibbonPanel* panel = new wxRibbonPanel(page, wxID_ANY, "View");
            wxRibbonButtonBar* bar = new wxRibbonButtonBar(panel, wxID_ANY);
            bar->AddToggleButton(XRCID("Split view"), "Split", RibbonIcon("view-split"));
            bar->AddButton(XRCID("Swap views"), "Swap", RibbonIcon("view-split-swap"));
        }
    }
    _ribbon->Realise();
}

void MainFrame::InitAcceleratorTable()
{
    static const size_t number = 17;
    wxAcceleratorEntry entries[number];

    // File/Load
    entries[0].Set(wxACCEL_CTRL, (int) 'N', wxID_NEW);
    entries[1].Set(wxACCEL_CTRL, (int) 'O', wxID_OPEN);
    entries[2].Set(wxACCEL_CTRL, (int) 'R', wxID_REVERT_TO_SAVED);
    // File/Save
    entries[3].Set(wxACCEL_CTRL, (int) 'S', wxID_SAVE);
    entries[4].Set(wxACCEL_CTRL|wxACCEL_SHIFT, (int) 'S', XRCID("Save all"));
    // File/Close
    entries[5].Set(wxACCEL_CTRL, (int) 'W', wxID_CLOSE);
    entries[6].Set(wxACCEL_CTRL|wxACCEL_SHIFT, (int) 'W', XRCID("Close all"));
    // File/Help
    entries[7].Set(wxACCEL_NORMAL, WXK_F1, wxID_HELP);
    entries[8].Set(wxACCEL_SHIFT, WXK_F1, wxID_ABOUT);
    // File / ""
    entries[9].Set(wxACCEL_CTRL, (int) 'Q', wxID_EXIT);

    // Navigate / Search
    entries[10].Set(wxACCEL_CTRL, (int) 'F', wxID_FIND);
    entries[11].Set(wxACCEL_NORMAL, WXK_F3, wxID_FORWARD);
    entries[12].Set(wxACCEL_SHIFT, WXK_F3, wxID_BACKWARD);
    entries[13].Set(wxACCEL_CTRL, (int) 'G', XRCID("Go to line"));
    // Navigate / Bookmarks
    entries[14].Set(wxACCEL_CTRL, WXK_F2, XRCID("Toggle bookmark"));
    entries[15].Set(wxACCEL_NORMAL, WXK_F2, wxID_DOWN);
    entries[16].Set(wxACCEL_SHIFT, WXK_F2, wxID_UP);

    wxAcceleratorTable accel(number, entries);
    SetAcceleratorTable(accel);
}

void MainFrame::InitFileTypeMenu()
{
    _fileTypeMenu = new wxMenu;

    wxWindowID baseid = FileTypeManager::get().getFirstWindowID();

    for(int i=0; i<FT_COUNT; ++i)
    {
        _fileTypeMenu->AppendCheckItem(baseid+i, FileTypeManager::get().getFileType(i).getName());
    }

    Connect(baseid, baseid+FT_COUNT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::onSelectLanguage));
    Connect(baseid, baseid+FT_COUNT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler(MainFrame::onUpdateSelectLanguage));
}

void MainFrame::UpdateTitle()
{
    TextDocument* doc = getCurrentDocument();
    wxString title = "Cody";
    if(doc)
    {
        title += " - " + doc->getTitle();
    }
    SetTitle(title);
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


void MainFrame::onRibbonButtonClicked(wxEvent/*wxRibbonButtonBarEvent*/& event)
{
    wxRibbonButtonBarEvent *ribbonevent = dynamic_cast<wxRibbonButtonBarEvent*>(&event);

    // Process an equivalent wxEVT_COMMAND_MENU_SELECTED event for simulating menu events.
    // Used to unify RibbonButton click and accelerator entries.

    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, event.GetId());
    if(ribbonevent)
        evt.SetInt(ribbonevent->GetInt());
    ProcessEvent(evt);

    event.Skip();

}

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CUSTOM(wxEVT_COMMAND_RIBBONBUTTON_CLICKED, wxID_ANY, MainFrame::onRibbonButtonClicked)

    EVT_UPDATE_UI(wxID_REVERT_TO_SAVED, MainFrame::onUpdateHasOpenDocument)
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

    EVT_MENU(wxID_NEW, MainFrame::onNewDocument)
    EVT_MENU(wxID_OPEN, MainFrame::onOpenDocument)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(wxID_OPEN, MainFrame::onRecentDocumentMenu)
    EVT_MENU_RANGE(wxID_FILE1, wxID_FILE9, MainFrame::onRecentDocument)
    EVT_MENU(wxID_REVERT_TO_SAVED, MainFrame::onRevertDocument)
    EVT_MENU(wxID_SAVE, MainFrame::onSaveDocument)
    EVT_MENU(wxID_SAVEAS, MainFrame::onSaveDocumentAs)
    EVT_MENU(wxID_CLOSE, MainFrame::onCloseDocument)
    EVT_MENU(XRCID("Close all"), MainFrame::onCloseAllDocuments)

    EVT_MENU(wxID_UNDO, MainFrame::onUndo)
    EVT_MENU(wxID_REDO, MainFrame::onRedo)
    EVT_MENU(wxID_DELETE, MainFrame::onDelete)
    EVT_MENU(wxID_CUT, MainFrame::onCut)
    EVT_MENU(wxID_COPY, MainFrame::onCopy)
    EVT_MENU(wxID_PASTE, MainFrame::onPaste)
    EVT_MENU(XRCID("CONVERT_EOL"), MainFrame::onEOLConvert)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("CONVERT_EOL"), MainFrame::onEOLMenu)
    EVT_MENU(XRCID("EOL_CR"), MainFrame::onEOLCR)
    EVT_UPDATE_UI(XRCID("EOL_CR"), MainFrame::onUpdateEOLCR)
    EVT_MENU(XRCID("EOL_LF"), MainFrame::onEOLLF)
    EVT_UPDATE_UI(XRCID("EOL_LF"), MainFrame::onUpdateEOLLF)
    EVT_MENU(XRCID("EOL_CRLF"), MainFrame::onEOLCRLF)
    EVT_UPDATE_UI(XRCID("EOL_CRLF"), MainFrame::onUpdateEOLCRLF)
    EVT_MENU(XRCID("Indent convert"), MainFrame::onIndentConvert)
    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("Indent convert"), MainFrame::onIndentMenu)
    EVT_MENU(XRCID("Indent use tabs"), MainFrame::onIndentUseTabs)
    EVT_UPDATE_UI(XRCID("Indent use tabs"), MainFrame::onUpdateIndentUseTabs)
    EVT_MENU(XRCID("Indent tab 2"), MainFrame::onIndent2)
    EVT_UPDATE_UI(XRCID("Indent tab 2"), MainFrame::onUpdateIndent2)
    EVT_MENU(XRCID("Indent tab 3"), MainFrame::onIndent3)
    EVT_UPDATE_UI(XRCID("Indent tab 3"), MainFrame::onUpdateIndent3)
    EVT_MENU(XRCID("Indent tab 4"), MainFrame::onIndent4)
    EVT_UPDATE_UI(XRCID("Indent tab 4"), MainFrame::onUpdateIndent4)
    EVT_MENU(XRCID("Indent tab 8"), MainFrame::onIndent8)
    EVT_UPDATE_UI(XRCID("Indent tab 8"), MainFrame::onUpdateIndent8)
    EVT_MENU(XRCID("Indent tab other"), MainFrame::onIndentOther)
    EVT_UPDATE_UI(XRCID("Indent tab other"), MainFrame::onUpdateIndentOther)

	EVT_MENU(XRCID("Select all"), MainFrame::onSelectAll)
	EVT_UPDATE_UI(XRCID("Select all"), MainFrame::onUpdateHasOpenDocument)

    EVT_RIBBONPANEL_EXTBUTTON_ACTIVATED(XRCID("Search panel"), MainFrame::onFindRibbonBarExtActivated)
    EVT_MENU(wxID_FIND, MainFrame::onFind)
    EVT_MENU(wxID_BACKWARD, MainFrame::onFindPrev)
    EVT_MENU(wxID_FORWARD, MainFrame::onFindNext)
    EVT_MENU(XRCID("Go to line"), MainFrame::onGoToLine)
    EVT_UPDATE_UI(wxID_FIND, MainFrame::onUpdateHasOpenDocument)
    EVT_UPDATE_UI(wxID_BACKWARD, MainFrame::onUpdateHasOpenDocument)
    EVT_UPDATE_UI(wxID_FORWARD, MainFrame::onUpdateHasOpenDocument)
    EVT_UPDATE_UI(XRCID("Go to line"), MainFrame::onUpdateHasOpenDocument)

    EVT_RIBBONPANEL_EXTBUTTON_ACTIVATED(XRCID("Bookmark panel"), MainFrame::onBookmarkRibbonBarExtActivated)
    EVT_MENU(XRCID("Toggle bookmark"), MainFrame::onToggleBookmark)
    EVT_MENU(wxID_UP, MainFrame::onPreviousBookmark)
    EVT_MENU(wxID_DOWN, MainFrame::onNextBookmark)
    EVT_UPDATE_UI(XRCID("Toggle bookmark"), MainFrame::onUpdateHasOpenDocument)
    EVT_UPDATE_UI(wxID_UP, MainFrame::onUpdateHasOpenDocument)
    EVT_UPDATE_UI(wxID_DOWN, MainFrame::onUpdateHasOpenDocument)

    EVT_MENU(XRCID("Display line numbers"), MainFrame::onDisplayLineNumber)
    EVT_UPDATE_UI(XRCID("Display line numbers"), MainFrame::onUpdateDisplayLineNumber)
    EVT_MENU(XRCID("Display markers"), MainFrame::onDisplayMarkers)
    EVT_UPDATE_UI(XRCID("Display markers"), MainFrame::onUpdateDisplayMarkers)
    EVT_MENU(XRCID("Display folders"), MainFrame::onDisplayFolders)
    EVT_UPDATE_UI(XRCID("Display folders"), MainFrame::onUpdateDisplayFolders)

    EVT_MENU(XRCID("Display caret line"), MainFrame::onDisplayCaretLine)
    EVT_UPDATE_UI(XRCID("Display caret line"), MainFrame::onUpdateDisplayCaretLine)
    EVT_MENU(XRCID("Display white spaces"), MainFrame::onDisplayWhiteSpaces)
    EVT_UPDATE_UI(XRCID("Display white spaces"), MainFrame::onUpdateDisplayWhiteSpaces)
    EVT_MENU(XRCID("Display indentation guides"), MainFrame::onDisplayIndentationGuides)
    EVT_UPDATE_UI(XRCID("Display indentation guides"), MainFrame::onUpdateDisplayIndentationGuides)
    EVT_MENU(XRCID("Display end of lines"), MainFrame::onDisplayEOL)
    EVT_UPDATE_UI(XRCID("Display end of lines"), MainFrame::onUpdateDisplayEOL)
    EVT_MENU(XRCID("Display long lines"), MainFrame::onDisplayLongLines)
    EVT_UPDATE_UI(XRCID("Display long lines"), MainFrame::onUpdateDisplayLongLines)
    EVT_MENU(XRCID("Display wrap long lines"), MainFrame::onDisplayWrapLongLines)
    EVT_UPDATE_UI(XRCID("Display wrap long lines"), MainFrame::onUpdateDisplayWrapLongLines)

    EVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED(XRCID("Select language"), MainFrame::onSelectLanguages)

    EVT_MENU(XRCID("Split view"), MainFrame::onSplitView)
    EVT_UPDATE_UI(XRCID("Split view"), MainFrame::onUpdateSplitView)
    EVT_MENU(XRCID("Swap views"), MainFrame::onSwapView)
    EVT_UPDATE_UI(XRCID("Swap views"), MainFrame::onUpdateSwapView)

    EVT_MENU(XRCID("Zoom in"), MainFrame::onZoomIn)
    EVT_UPDATE_UI(XRCID("Zoom in"), MainFrame::onUpdateHasOpenDocument)
    EVT_MENU(XRCID("Zoom out"), MainFrame::onZoomOut)
    EVT_UPDATE_UI(XRCID("Zoom out"), MainFrame::onUpdateHasOpenDocument)
    EVT_MENU(XRCID("Zoom reset"), MainFrame::onZoomReset)
    EVT_UPDATE_UI(XRCID("Zoom reset"), MainFrame::onUpdateHasOpenDocument)

    EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, MainFrame::onPageClosing)
    EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, MainFrame::onPageChanged)

    EVT_CLOSE(MainFrame::onClose)
END_EVENT_TABLE()

void MainFrame::onClose(wxCloseEvent& event)
{
    if(!wxGetApp().closeAllFrameDocuments(this) && event.CanVeto())
        event.Veto();
    else
        event.Skip();
}

void MainFrame::onNewDocument(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().createEmptyDocument(this);
}

void MainFrame::onOpenDocument(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().queryLoadFile(this);
}

void MainFrame::onRecentDocumentMenu(wxRibbonButtonBarEvent& event)
{
    event.PopupMenu(_recentFileMenu);
}

void MainFrame::onRecentDocument(wxCommandEvent& event)
{
    int id = event.GetId() - wxID_FILE1;
    if(id>=0 && id<(int)wxGetApp().getFileHistory().GetCount())
    {
        wxString name = wxGetApp().getFileHistory().GetHistoryFile(id);
        if(!name.IsEmpty())
        {
            wxGetApp().loadDocument(name, this);
        }
    }
}

void MainFrame::onRevertDocument(wxCommandEvent& WXUNUSED(event))
{
    TextDocument* doc = getCurrentDocument();
    if(doc)
    {
        // TODO message box
        doc->reloadFile();
    }
}

void MainFrame::onSaveDocument(wxCommandEvent& WXUNUSED(event))
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

void MainFrame::onSaveDocumentAs(wxCommandEvent& WXUNUSED(event))
{
    TextDocument* doc = getCurrentDocument();
    if(doc)
    {
        wxGetApp().saveDocumentAs(doc, this);
    }
}

void MainFrame::onCloseDocument(wxCommandEvent& WXUNUSED(event))
{
    TextDocument* doc = getCurrentDocument();
    if(doc)
    {
        wxGetApp().closeDocument(doc);
    }
}

void MainFrame::onCloseAllDocuments(wxCommandEvent& WXUNUSED(event))
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

void MainFrame::onPageChanged(wxAuiNotebookEvent& WXUNUSED(event))
{
    if(getBookmarkPanel())
    {
        getBookmarkPanel()->setView(getCurrentTextFrame());
    }
    UpdateTitle();
}

void MainFrame::onUndo(wxCommandEvent& WXUNUSED(event))
{
    wxStyledTextCtrl* txt = getCurrentTextCtrl();
    if(txt)
    {
        txt->Undo();
    }
}

void MainFrame::onRedo(wxCommandEvent& WXUNUSED(event))
{
    wxStyledTextCtrl* txt = getCurrentTextCtrl();
    if(txt)
    {
        txt->Redo();
    }
}

void MainFrame::onDelete(wxCommandEvent& WXUNUSED(event))
{
    wxStyledTextCtrl* txt = getCurrentTextCtrl();
    if(txt)
    {
        txt->Clear();
    }
}

void MainFrame::onCut(wxCommandEvent& WXUNUSED(event))
{
    wxStyledTextCtrl* txt = getCurrentTextCtrl();
    if(txt)
    {
        txt->Cut();
    }
}

void MainFrame::onCopy(wxCommandEvent& WXUNUSED(event))
{
    wxStyledTextCtrl* txt = getCurrentTextCtrl();
    if(txt)
    {
        txt->Copy();
    }
}

void MainFrame::onPaste(wxCommandEvent& WXUNUSED(event))
{
    wxStyledTextCtrl* txt = getCurrentTextCtrl();
    if(txt)
    {
        txt->Paste();
    }
}

void MainFrame::onEOLConvert(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->convertEOL(doc->getEOLMode());
  }
}

void MainFrame::onEOLMenu(wxRibbonButtonBarEvent& event)
{
  event.PopupMenu(_eolMenu);
}

void MainFrame::onEOLCR(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->setEOLMode(TextDocument::EOL_CR);
  }
}

void MainFrame::onEOLLF(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->setEOLMode(TextDocument::EOL_LF);
  }
}

void MainFrame::onEOLCRLF(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->setEOLMode(TextDocument::EOL_CRLF);
  }
}

void MainFrame::onUpdateEOLCRLF(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    event.Check(doc->getEOLMode()==TextDocument::EOL_CRLF);
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}

void MainFrame::onIndentConvert(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->convertIndent();
  }
}

void MainFrame::onIndentMenu(wxRibbonButtonBarEvent& event)
{
  event.PopupMenu(_indentMenu);
}

void MainFrame::onIndentUseTabs(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->useTabs(!doc->useTabs());
  }
}

void MainFrame::onUpdateIndentUseTabs(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    event.Check(doc->useTabs());
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}

void MainFrame::onIndent2(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->setIndent(2);
  }
}

void MainFrame::onUpdateIndent2(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    event.Check(doc->getIndent()==2);
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}

void MainFrame::onIndent3(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->setIndent(3);
  }
}

void MainFrame::onUpdateIndent3(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    event.Check(doc->getIndent()==3);
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}

void MainFrame::onIndent4(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->setIndent(4);
  }
}

void MainFrame::onUpdateIndent4(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    event.Check(doc->getIndent()==4);
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}

void MainFrame::onIndent8(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    doc->setIndent(8);
  }
}

void MainFrame::onUpdateIndent8(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    event.Check(doc->getIndent()==8);
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}

void MainFrame::onIndentOther(wxCommandEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    IndentationDialog dialog(this);
    dialog.useTabs(doc->useTabs());
    dialog.indents(doc->getIndent());
    if(dialog.ShowModal()==wxID_OK)
    {
      doc->useTabs(dialog.useTabs());
      doc->setIndent(dialog.indents());
    }
  }
}


void MainFrame::onUpdateIndentOther(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    int indent = doc->getIndent();
    event.Check(indent!=2 && indent!=3 && indent!=4 && indent!=8);
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}


void MainFrame::onUpdateEOLCR(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    event.Check(doc->getEOLMode()==TextDocument::EOL_CR);
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}

void MainFrame::onUpdateEOLLF(wxUpdateUIEvent& event)
{
  TextDocument* doc = getCurrentDocument();
  if(doc)
  {
    event.Enable(true);
    event.Check(doc->getEOLMode()==TextDocument::EOL_LF);
  }
  else
  {
    event.Check(false);
    event.Enable(false);
  }
}

void MainFrame::onSelectAll(wxCommandEvent& event)
{
	wxStyledTextCtrl* txt = getCurrentTextCtrl();
    if(txt)
    {
        txt->SelectAll();
    }
}

void MainFrame::onFindRibbonBarExtActivated(wxRibbonPanelEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showFastFind(!frame->fastFindShown());
    }
}

void MainFrame::onFind(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showFastFindText();
    }
}

void MainFrame::onFindNext(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->findNext();
    }
}

void MainFrame::onFindPrev(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->findPrev();
    }
}

void MainFrame::onGoToLine(wxCommandEvent& WXUNUSED(event))
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

void MainFrame::onBookmarkRibbonBarExtActivated(wxRibbonPanelEvent& WXUNUSED(event))
{
    toggleBookmarkPanel();
}

void MainFrame::onToggleBookmark(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->toggleBookmark();
    }
}

void MainFrame::onPreviousBookmark(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->gotoPrevBookmark();
    }
}

void MainFrame::onNextBookmark(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->gotoNextBookmark();
    }
}

void MainFrame::onDisplayLineNumber(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showLineNumbers(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayLineNumber(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->lineNumbersShown());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onDisplayMarkers(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showMarkers(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayMarkers(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->markersShown());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onDisplayFolders(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showFolders(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayFolders(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->foldersShown());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onDisplayCaretLine(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showCaretLine(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayCaretLine(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->caretLineShown());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onDisplayWhiteSpaces(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showWhiteSpaces(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayWhiteSpaces(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->whiteSpacesShown());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onDisplayIndentationGuides(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showIndentationGuides(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayIndentationGuides(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->indentationGuidesShown());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onDisplayEOL(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showEOL(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayEOL(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->EOLShown());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onDisplayLongLines(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->showLongLines(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayLongLines(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->longLinesShown());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onDisplayWrapLongLines(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->wrapLongLines(event.IsChecked());
    }
}

void MainFrame::onUpdateDisplayWrapLongLines(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->longLinesWrapped());
    }
    else
    {
        event.Enable(false);
    }
}


void MainFrame::onZoomIn(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->zoomIn();
    }
}

void MainFrame::onZoomOut(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->zoomOut();
    }
}

void MainFrame::onZoomReset(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->setZoom(0);
    }
}

void MainFrame::onSplitView(wxCommandEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->splitView(event.IsChecked());
    }
}

void MainFrame::onUpdateSplitView(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(true);
        event.Check(frame->viewSplitted());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onSwapView(wxCommandEvent& WXUNUSED(event))
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        frame->swapViews();
    }
}

void MainFrame::onUpdateSwapView(wxUpdateUIEvent& event)
{
    TextFrame* frame = getCurrentTextFrame();
    if(frame)
    {
        event.Enable(frame->viewSplitted());
    }
    else
    {
        event.Enable(false);
    }
}

void MainFrame::onSelectLanguages(wxRibbonButtonBarEvent& event)
{
    event.PopupMenu(_fileTypeMenu);
}

void MainFrame::onSelectLanguage(wxCommandEvent& event)
{
    int id = event.GetId() - FileTypeManager::get().getFirstWindowID();
    TextDocument* doc = getCurrentDocument();
    if(doc)
    {
        doc->setDocumentType(id);
    }
}

void MainFrame::onUpdateSelectLanguage(wxUpdateUIEvent& event)
{
    int id = event.GetId() - FileTypeManager::get().getFirstWindowID();
    TextDocument* doc = getCurrentDocument();
    if(doc)
    {
        event.Check(doc->getDocumentType()==id);
    }
}
