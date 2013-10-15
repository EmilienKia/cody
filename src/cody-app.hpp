/* -*- Mode: C++; indent-tabs-mode: s; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * cody
 * Copyright (C) 2012-2013 Émilien Kia <emilien.kia@gmail.com>
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

#ifndef _CODY_APP_HPP_
#define _CODY_APP_HPP_

#include <set>
#include <list>
#include <map>

#include <wx/filehistory.h>
#include <wx/fileconf.h>

#include "file-type.hpp"

class MainFrame;
class TextDocument;

class wxRibbonButtonBarEvent;


/**
 * This is the Cody application class.
 * It is responsible to manage documents and main frames.
 *
 * Unique instantiation from wxWidgets framework.
 * @see wxApp.
 */
class CodyApp : public wxApp
{
    DECLARE_EVENT_TABLE();
public:

    /**
     * Application initialization function.
     * Called by wxWidgets framework at startup.
     * @return @true if initialization if OK and the application can continue or @false otherwise.
     */
    virtual bool OnInit();

    /**
     * Application finalization function.
     * Called by wxWidgets framework at exit.
     */
    virtual int OnExit();

    /**
     * Create an empty document, of default text type, and its view.
     * @param mainFrame Frame to which attach document view, default to current.
     * @return The newly created document.
     */
    TextDocument* createEmptyDocument(MainFrame* mainFrame = NULL);

    /**
     * Intend to load a document from a specified path, of specified type, and create its view.
     * @param path Path of file from which load document.
     * @param mainFrame Frame to which attach document view, default to current.
     * @param filetype File type identifier, -1 to try to utodetect it from document.
     * @return The newly created document.
     */
    TextDocument* loadDocument(const wxString& path, MainFrame* mainFrame = NULL, int filetype = -1);

    /**
     * Query the user (UI) to load many documents, and create their views.
     * File types are automatically deduced.
     * @param mainFrame Frame to which attach document view, default to current.
     * @return List of newly create documents.
     */
    std::list<TextDocument*> queryLoadFile(MainFrame* mainFrame = NULL);

    /**
     * Query the user to select/input a path to which save a document, and save it.
     * @param doc Document to save as.
     * @param mainFrame Frame to which attach file dialog box.
     * @return @true if document has been correctly saved or @false if user cancels or anything goes wrong.
     */
    bool saveDocumentAs(TextDocument* doc, MainFrame* mainFrame = NULL);

    /**
     * Intend to close a document and all its views.
     * Query user to save document if it is modified.
     * @param doc Document to close.
     * @return @true if document has been correctly closed.
     */
    bool closeDocument(TextDocument* doc);

    /**
     * Intend to close all documents attached to a frame.
     * @param mainFrame Frame from which intend to save documents (default to current).
     * @return @true if all documents from the frame have been closed.
     */
    bool closeAllFrameDocuments(MainFrame* mainFrame = NULL);

    /**
     * Retrieve the configuration helper of application.
     * @return Configuration helper.
     */
    wxFileConfig* getConfig();

    /**
     * Return the more recent file history.
     * @return The more recent file history.
     */
    wxFileHistory& getFileHistory();

    /**
     * Intend to reload config and apply changes.
     */
    void reloadConfig();

    /**
     * Display preference dialog box.
     */
    void preferences();

    /**
     * Retrieve the set of all opened documents.
     * @return The set of all opened documents.
     */
    std::set<TextDocument*>& getDocuments()
    {
        return _documents;
    }

protected:
    /**
     * Initialize command line parser options.
     * Part of wxWidgets framework.
     * @see wxApp::OnInitCmdLine
     */
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    /**
     * Apply parsed command line.
     * Part of wxWidgets framework.
     * @see wxApp::OnCmdLineParsed
     */
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

    /**
     * The main frame instance.
     */
    MainFrame* _frame;

    /**
     * The set of opened documents.
     */
    std::set<TextDocument*> _documents;

    /**
     * The more recent file history.
     */
    wxFileHistory _fileHistory;

    /**
     * The application configuration helper.
     */
    wxFileConfig* _config;

private:
    void onAbout(wxRibbonButtonBarEvent& event);          ///< Handler for File/Help/About ribbon button
    void onExit(wxRibbonButtonBarEvent& event);           ///< Handler for File//Exit ribbon button
    void onPreferences(wxRibbonButtonBarEvent& event);    ///< Handler for File//Preferences ribbon button
};

DECLARE_APP(CodyApp)

#endif // _CODY_APP_HPP_
