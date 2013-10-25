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

#ifndef _MAIN_FRAME_HPP_
#define _MAIN_FRAME_HPP_

#include <wx/aui/auibook.h>
#include <wx/ribbon/bar.h>

class CodyApp;
class TextFrame;
class TextDocument;
class wxRibbonButtonBarEvent;
class wxStyledTextCtrl;
class BookmarkPanel;

/**
 * Top-level Cody window (aka main frame) class.
 * Embed a ribbon, a notebook for document frames and any tool panels.
 */
class MainFrame: public wxFrame
{
    friend class CodyApp;
    DECLARE_EVENT_TABLE();

public:
    /**
     * Default constructor.
     * No need to pass any parameter as all is specified directly.
     */
    MainFrame();

    /**
     * Get the current text control.
     * Aka the most recently focused text control of current text frame.
     * @return The current text control, @null if none.
     */
    wxStyledTextCtrl* getCurrentTextCtrl();

    /**
     * Get the currently focused text frame.
     * @return The current text frame, @null if none.
     */
    TextFrame* getCurrentTextFrame();

    /**
     * Get the document attached to the current text frame.
     * @return The document attached to the current text frame, @null if no current text frame.
     */
    TextDocument* getCurrentDocument();

    /**
     * Retrieve the bookmark panel of this frame.
     * @return The bookmark panel.
     */
    BookmarkPanel* getBookmarkPanel()
    {
        return _bookmark;
    }

    /**
     * Function called when the frame is to be deleted.
     * Part of wxWidgets framework.
     * @return @true if the frame will be destroyed correctly.
     */
    virtual bool Destroy();

    /**
     * Toggle (on/off - show/hide) the bookmark panel.
     */
    void toggleBookmarkPanel();

protected:

    /**
     * Initialize content of frame.
     * Create children widgets and initialize them.
     * Call other Init...() functions.
     */
    void CommonInit();

    /**
     * Init ribbon bar.
     * Fill it with all tabs and buttons.
     */
    void InitRibbon();

    /**
     * Setup the accelerator table with all shortcuts.
     * Needed to be done manually because wxWidgets RibbonBar does not support accelerators yet.
     */
    void InitAcceleratorTable();

    /**
     * Create and fill "file type" menu from file type database.
     * Connect all related handlers.
     */
    void InitFileTypeMenu();

    /**
     * Update the frame title bar content from currently selected document.
     */
    void UpdateTitle();

    /**
     * Retrieve the notebook which embed text frames.
     * @return The text frame notebook.
     */
    wxAuiNotebook* getNotebook()
    {
        return _notebook;
    }

private:
    wxPanel*	   _panel;    ///< Unique frame child. Used to embed all widgets managed by AuiManager.
    wxAuiManager   _manager;  ///< Aui manager, used to manage widgets positions and decorations.
    wxRibbonBar*   _ribbon;   ///< Ribbon widget.
    wxAuiNotebook* _notebook; ///< Notebook which embed text frames.
    BookmarkPanel* _bookmark; ///< Bookmark panel.

    wxMenu*        _recentFileMenu; ///< More recent file list menu.
    wxMenu*        _fileTypeMenu;   ///< File type list menu.
    wxMenu*        _eolMenu;        ///< EOL convert menu
    wxMenu*        _indentMenu;     ///< Indentation convert menu

    void onRibbonButtonClicked(wxEvent& event);

    void onClose(wxCloseEvent& event);

    void onPageClosing(wxAuiNotebookEvent& event);
    void onPageChanged(wxAuiNotebookEvent& event);

    void onNewDocument(wxCommandEvent& event);
    void onOpenDocument(wxCommandEvent& event);
    void onRecentDocumentMenu(wxRibbonButtonBarEvent& event);
    void onRecentDocument(wxCommandEvent& event);
    void onRevertDocument(wxCommandEvent& event);
    void onSaveDocument(wxCommandEvent& event);
    void onSaveDocumentAs(wxCommandEvent& event);
    void onCloseDocument(wxCommandEvent& event);
    void onCloseAllDocuments(wxCommandEvent& event);

    void onUndo(wxCommandEvent& event);
    void onRedo(wxCommandEvent& event);
    void onDelete(wxCommandEvent& event);
    void onCut(wxCommandEvent& event);
    void onCopy(wxCommandEvent& event);
    void onPaste(wxCommandEvent& event);
    void onEOLConvert(wxCommandEvent& event);
    void onEOLMenu(wxRibbonButtonBarEvent& event);
    void onEOLCR(wxCommandEvent& event);
    void onEOLLF(wxCommandEvent& event);
    void onEOLCRLF(wxCommandEvent& event);
    void onUpdateEOLCRLF(wxUpdateUIEvent& event);
    void onUpdateEOLCR(wxUpdateUIEvent& event);
    void onUpdateEOLLF(wxUpdateUIEvent& event);
    void onIndentConvert(wxCommandEvent& event);
    void onIndentMenu(wxRibbonButtonBarEvent& event);
    void onIndentUseTabs(wxCommandEvent& event);
    void onUpdateIndentUseTabs(wxUpdateUIEvent& event);
    void onIndent2(wxCommandEvent& event);
    void onUpdateIndent2(wxUpdateUIEvent& event);
    void onIndent3(wxCommandEvent& event);
    void onUpdateIndent3(wxUpdateUIEvent& event);
    void onIndent4(wxCommandEvent& event);
    void onUpdateIndent4(wxUpdateUIEvent& event);
    void onIndent8(wxCommandEvent& event);
    void onUpdateIndent8(wxUpdateUIEvent& event);
    void onIndentOther(wxCommandEvent& event);
    void onUpdateIndentOther(wxUpdateUIEvent& event);


    void onFindRibbonBarExtActivated(wxRibbonPanelEvent& event);
    void onFind(wxCommandEvent& event);
    void onFindNext(wxCommandEvent& event);
    void onFindPrev(wxCommandEvent& event);
    void onGoToLine(wxCommandEvent& event);

    void onUpdateHasOpenDocument(wxUpdateUIEvent& event);
    void onUpdateCanUndo(wxUpdateUIEvent& event);
    void onUpdateCanRedo(wxUpdateUIEvent& event);
    void onUpdateCanPaste(wxUpdateUIEvent& event);
    void onUpdateHasSelection(wxUpdateUIEvent& event);

    void onBookmarkRibbonBarExtActivated(wxRibbonPanelEvent& event);
    void onToggleBookmark(wxCommandEvent& event);
    void onPreviousBookmark(wxCommandEvent& event);
    void onNextBookmark(wxCommandEvent& event);

    void onDisplayLineNumber(wxCommandEvent& event);
    void onUpdateDisplayLineNumber(wxUpdateUIEvent& event);
    void onDisplayMarkers(wxCommandEvent& event);
    void onUpdateDisplayMarkers(wxUpdateUIEvent& event);
    void onDisplayFolders(wxCommandEvent& event);
    void onUpdateDisplayFolders(wxUpdateUIEvent& event);

    void onDisplayCaretLine(wxCommandEvent& event);
    void onUpdateDisplayCaretLine(wxUpdateUIEvent& event);
    void onDisplayWhiteSpaces(wxCommandEvent& event);
    void onUpdateDisplayWhiteSpaces(wxUpdateUIEvent& event);
    void onDisplayIndentationGuides(wxCommandEvent& event);
    void onUpdateDisplayIndentationGuides(wxUpdateUIEvent& event);
    void onDisplayEOL(wxCommandEvent& event);
    void onUpdateDisplayEOL(wxUpdateUIEvent& event);
    void onDisplayLongLines(wxCommandEvent& event);
    void onUpdateDisplayLongLines(wxUpdateUIEvent& event);
    void onDisplayWrapLongLines(wxCommandEvent& event);
    void onUpdateDisplayWrapLongLines(wxUpdateUIEvent& event);

    void onSelectLanguages(wxRibbonButtonBarEvent& event);
    void onSelectLanguage(wxCommandEvent& event);
    void onUpdateSelectLanguage(wxUpdateUIEvent& event);

    void onSplitView(wxCommandEvent& event);
    void onUpdateSplitView(wxUpdateUIEvent& event);
    void onSwapView(wxCommandEvent& event);
    void onUpdateSwapView(wxUpdateUIEvent& event);

    void onZoomIn(wxCommandEvent& event);
    void onZoomOut(wxCommandEvent& event);
    void onZoomReset(wxCommandEvent& event);

};

#endif // _MAIN_FRAME_HPP_
