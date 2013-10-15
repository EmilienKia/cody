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

#ifndef _TEXT_FRAME_HPP_
#define _TEXT_FRAME_HPP_

#include <wx/stc/stc.h>


class TextDocument;
class MainFrame;
class FileType;
class StyleDef;

class wxAuiNotebook;
class wxMarkBar;
class wxMarkBarEvent;
class wxSearchCtrl;
class wxSpinCtrl;
class wxSplitterWindow;

/**
 * Text frame class.
 * Panel which embed two text edition control, the fast find panel and the mark bar panel.
 * Each text frame (and all embeded widgets) are related to one document.
 */
class TextFrame: public wxPanel
{
    DECLARE_EVENT_TABLE()
public:
    /**
     * Constructor.
     * @param parent Parent window, generally a wxAuiNotebook.
     * @param doc Document to which the frame is attached.
     */
    TextFrame(wxWindow* parent, TextDocument* doc);

    /**
     * Retrieve the document to which the frame is attached.
     * @return The attached document.
     */
    TextDocument* getDocument()
    {
        return _document;
    }

    /**
     * Set the document to which the frame is attached.
     * Must not be used as anything is refreshed if document is changed.
     * @todo To be deleted ?
     */
    void setDocument(TextDocument* doc)
    {
        _document = doc;
    }

    /**
     * Initialize frame after document loading.
     * Setup bookmarks and markers.
     */
    void initAfterLoading();

    /**
     * Apply all config related to file type.
     * Update text style and keywords.
     */
    void applyFileType();

    /**
     * Apply text style according to file type.
     * Update all text styles.
     */
    void applyFileTypeStyle();

    /**
     * Apply text style according to file type.
     * Update only specified text style.
     * @param stylenum Text style identifier (0 to wxSTC_STYLE_DEFAULT).
     */
    void applyFileTypeStyle(unsigned short stylenum);

    /**
     * Apply text keywords for syntaxic coloration according to file type.
     * Update all text keywords.
     */
    void applyFileTypeKeywords();

    /**
     * Apply text keywords for syntaxic coloration according to file type.
     * Update only specified text keywords.
     * @param keywordnum Keyword identifier (0 to wxSTC_KEYWORDSET_MAX)
     */
    void applyFileTypeKeywords(unsigned short keywordnum);

    /**
     * Apply file type named properties.
     * Update all properties.
     */
    void applyFileTypeProperties();

    /**
     * Apply file type named properties.
     * Update only specified property.
     * @param propname Name of property to update.
     */
    void applyFileTypeProperty(const wxString& propname);

    /**
     * Retrieve primary text control widget.
     * @return Primary text control.
     */
    wxStyledTextCtrl* getMainTextCtrl()const
    {
        return _mainText;
    }

    /**
     * Retrieve currently selected text control widget.
     * @return Current text control.
     */
    wxStyledTextCtrl* getCurrentTextCtrl()const
    {
        return _currentText!=NULL?_currentText:_mainText;
    }

    /**
     * Retrieve the main frame to which this frame is attached.
     * @return Attached main frame.
     */
    MainFrame* getMainFrame();

    /**
     * Show or hide fast-find panel.
     * @param shown @true to show and @false to hide panel.
     */
    void showFastFind(bool shown=true);

    /**
     * Retrieve if fast-find panel is shown.
     * @return @true if fast-find panel is shown, @false is hidden.
     */
    bool fastFindShown()const
    {
        return _fastFindShown;
    }

    /**
     * Show the fast-find panel and set selected text as find entry.
     * Set the focus to the find entry.
     */
    void showFastFindText();

    /**
     * Show the fast-find panel and set focus to line-number entry.
     */
    void showFastFindGoToLine();

    /**
     * Find a next occurence of fast-find entry.
     * @param offset The index of next occurence.
     */
    void findNext(int offset = 1);

    /**
     * Find a previous occurence of fast-find entry.
     * @param offset The index of previous occurence.
     */
    void findPrev(int offset = -1);

    /**
     * Set the title of frame.
     * Update the tab title too.
     */
    void setTitle(const wxString& title);

    /**
     * Show or hide the line-number margin.
     * @param show @true to show, @false to hide.
     */
    void showLineNumbers(bool show = true);

    /**
     * Return if the line-number margin is shown.
     * @return @true if shown, @false if hidden.
     */
    bool lineNumbersShown()const;

    /**
     * Show or hide the marker margin.
     * @param show @true to show, @false to hide.
     */
    void showMarkers(bool show = true);

    /**
     * Return if the marker margin is shown.
     * @return @true if shown, @false if hidden.
     */
    bool markersShown()const;

    /**
     * Show or hide the folding margin.
     * @param show @true to show, @false to hide.
     */
    void showFolders(bool show = true);

    /**
     * Return if the folding margin is shown.
     * @return @true if shown, @false if hidden.
     */
    bool foldersShown()const;

    /**
     * Hilight or not the caret line.
     * @param show @true to hilight, @false to hide.
     */
    void showCaretLine(bool show = true);

    /**
     * Return if the current line is hilighted.
     * @return @true if hilighted, @false if hidden.
     */
    bool caretLineShown()const;

    /**
     * Show or hide whitespace indicators.
     * @param show @true to show, @false to hide.
     */
    void showWhiteSpaces(bool show = true);

    /**
     * Return if whitespace indicators are shown.
     * @return @true if shown, @false if hidden.
     */
    bool whiteSpacesShown()const;

    /**
     * Show or hide indentation guides.
     * @param show @true to show, @false to hide.
     */
    void showIndentationGuides(bool show = true);

    /**
     * Return if indentationguides are shown.
     * @return @true if shown, @false if hidden.
     */
    bool indentationGuidesShown()const;

    /**
     * Show or hide end-of-line indicators.
     * @param show @true to show, @false to hide.
     */
    void showEOL(bool show = true);

    /**
     * Return if end-of-line indicators are shown.
     * @return @true if shown, @false if hidden.
     */
    bool EOLShown()const;

    /**
     * Show or hide long-line indicators.
     * @param show @true to show, @false to hide.
     */
    void showLongLines(bool show = true);

    /**
     * Return if long-line indicators are shown.
     * @return @true if shown, @false if hidden.
     */
    bool longLinesShown()const;

    /**
     * Set if long-lines are wrapped.
     * @param wrap @true to wrap, @false to not wrap (and horizontally scroll).
     */
    void wrapLongLines(bool wrap = true);

    /**
     * Return if long-line are warpped.
     * @return @true if wrapped, @false if not.
     */
    bool longLinesWrapped()const;

    /**
     * Set character zoom factor.
     * @param scale Zoom factor.
     */
    void setZoom(int scale=0);

    /**
     * Return the current zoom factor.
     * @return Current zoom factor.
     * @todo Rename it according to setZoom() function.
     */
    int getScale()const;

    /**
     * Zoom in by one step.
     */
    void zoomIn();

    /**
     * Zoom out by one step.
     */
    void zoomOut();

    /**
     * Split or unspit views.
     * Split view make the secondary text control visible.
     * @param split @true to split, @false to unsplit.
     */
    void splitView(bool split = true);

    /**
     * Return if views are splitted, and secondary text control is visible.
     * @return @true if splitted, @false if not.
     */
    bool viewSplitted()const;

    /**
     * Swap caret position between primary and secondary text control.
     * Possible only if view are splitted.
     */
    void swapViews();

    /**
     * Toggle a bookmark on the current line.
     */
    void toggleBookmark();

    /**
     * Add a bookmark to the current line with the specified bookmark name.
     * If the name is empty, intend to use line content and query it to user (UI).
     * @param line Line to which add the bookmark.
     * @param name Bookmark name.
     */
    void addBookmark(int line=wxNOT_FOUND, wxString name=wxT(""));

    /**
     * Remove bookmark from the specified line.
     * @param line Line number to which remove bookmark.
     */
    void remBookmark(int line=wxNOT_FOUND);

    /**
     * Remove all bookmarks of the attached document.
     */
    void clearBookmarks();

    /**
     * Add bookmarks saved from last session.
     */
    void addBookmarksFromProvider();

    /**
     * Move caret and scroll cyclicly to the next bookmark, if any.
     */
    void gotoPrevBookmark();

    /**
     * Move caret and scroll cyclicly to the previous bookmark, if any.
     */
    void gotoNextBookmark();

    /**
     * Move caret and scroll to the specified line.
     * @param line Line number where move to.
     */
    void gotoLine(int line);

    /**
     * Force to set focus to current text control.
     * Usefull to force user input.
     */
    void setFocusToTextCtrl();


protected:
    void CommonInit();
    void InitTextCtrl(wxStyledTextCtrl* txt);
    void ApplyGlobalConfig();

    void applyFileTypeStyle(unsigned int stylenum, const StyleDef& style);

    wxAuiNotebook* getNotebook();

    enum MARGIN_ID
    {
        MARGIN_LINE_NB,
        MARGIN_MARKER,
        MARGIN_FOLD
    };

    void showMargin(unsigned id, bool shown=true);
    bool marginShown(unsigned id)const;

    enum MARKER_ID
    {
        TEXT_MARKER_BOOKMARK,
        TEXT_MARKER_SEARCH
    };

    void UpdateBookmarkPanel();

    void setMarkerStyle(int id, const wxBitmap &bmp, const wxColour& fore, const wxColour& back);
    void setMarkerStyle(int id, int predefStyle, const wxColour& fore, const wxColour& back);
    void addMarker(int id, const wxString& name, int line=wxNOT_FOUND);
    void remMarker(int id, int line=wxNOT_FOUND);
    void remMarkers(int id);

    void UpdateMarkerPages();

private:
    TextDocument*     _document;
    wxStyledTextCtrl* _mainText;
    wxStyledTextCtrl* _secondText;
    wxStyledTextCtrl* _currentText;
    wxSplitterWindow* _splitter;

    wxPanel *_firstPanel;
    wxPanel *_secondPanel;

    bool _fastFindShown;
    wxSizer* _fastFindSizer;
    wxSearchCtrl* _fastFindText;
    wxSpinCtrl*   _fastFindLine;
    wxBitmapButton* _fastFindClose;
    wxMarkBar*  _markbar;

    unsigned int _marginShown;

    void updateLineNbMargin();

    void onCloseFastFind(wxCommandEvent& event);
    void onFastFindText(wxCommandEvent& event);
    void onFastFindNext(wxCommandEvent& event);
    void onFastFindPrev(wxCommandEvent& event);
    void onFastFindLine(wxCommandEvent& event);

    void OnTextModified(wxStyledTextEvent& event);
    void onUpdateUI(wxStyledTextEvent& event);

    void onMarkerActivated(wxMarkBarEvent& event);

    void onSelectionChanged();

    void onChildFocus(wxChildFocusEvent& event);
};

#endif // _TEXT_FRAME_HPP_
