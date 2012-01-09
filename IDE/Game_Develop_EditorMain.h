/** \file
 *  Game Develop
 *  2008-2012 Florian Rival (Florian.Rival@gmail.com)
 */

#ifndef GAME_DEVELOP_EDITORMAIN_H
#define GAME_DEVELOP_EDITORMAIN_H

//(*Headers(Game_Develop_EditorFrame)
#include <wx/sizer.h>
#include <wx/menu.h>
#include <wx/aui/aui.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/infobar.h>
#include <wx/timer.h>
//*)
#include "wx/aui/aui.h"
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#ifdef __WXMSW__
#include <wx/msw/winundef.h>
#endif
#include <iostream>
#include <string>
#include <list>
#include <sstream>
#include <wx/fileconf.h>
#include <wx/artprov.h>
#include <wx/log.h>
#ifdef __WXMSW__
#include <wx/msw/winundef.h>
#endif
#include "GDL/IDE/Dialogs/ResourcesEditor.h"
#include "GDL/Game.h"
#include "GDL/CommonTools.h"
#include "EditorScene.h"
#include "RecentList.h"
class RuntimeGame;
class ProjectManager;
class StartHerePage;
class BuildToolsPnl;

using namespace std;

/**
 * Class representing the main editor
 */
class Game_Develop_EditorFrame: public wxFrame
{
    friend class ResourcesEditor;
    public:

        /**
         * Constructor.
         * \param Parent window
         * \param True to create an initial empty project
         */
        Game_Develop_EditorFrame(wxWindow* parent, bool createEmptyProject);
        virtual ~Game_Develop_EditorFrame();

        vector < boost::shared_ptr<RuntimeGame> > games; ///< All games opened
        unsigned int gameCurrentlyEdited; ///< Index of the current game ( "Current" means choosen in the project manager )

        /**
         * Get a shared pointer to the current game ( "Current" means choosen in the project manager )
         */
        inline boost::shared_ptr<RuntimeGame> GetCurrentGame()
        {
            if ( gameCurrentlyEdited >= games.size()) return boost::shared_ptr<RuntimeGame> ();

            return games[gameCurrentlyEdited];
        }

        /**
         * True if a game is currently edited
         */
        inline bool CurrentGameIsValid()
        {
            if ( gameCurrentlyEdited >= games.size()) return false;

            return true;
        }

        /**
         * Change the current game
         */
        void SetCurrentGame(unsigned int i);

        /**
         * Open a game from its filename
         */
        void Open(string FichierJeu);

        static void LoadSkin(wxRibbonBar * bar);
        static void LoadSkin(wxAuiManager * auiManager, wxAuiNotebook * notebook);

        /**
         * Get a pointer to notebook containing editors
         */
        inline const wxAuiNotebook * GetEditorsNotebook() const { return editorsNotebook; };
        inline wxAuiNotebook * GetEditorsNotebook() { return editorsNotebook; };

        /**
         * Get a lightweight interface to this class.
         */
        MainEditorCommand & GetMainEditorCommand() { return mainEditorCommand; };

        /**
         * Get a pointer to the ribbon
         */
        inline const wxRibbonBar * GetRibbon() const { return m_ribbon; };
        inline wxRibbonBar * GetRibbon() { return m_ribbon; };

        /**
         * Get a pointer to ribbon bar which can be changed by scene editors
         */
        inline wxRibbonButtonBar * GetRibbonSceneEditorButtonBar() const { return ribbonSceneEditorButtonBar; };

        /**
         * Get a pointer to the build tools panel
         */
        inline BuildToolsPnl * GetBuildToolsPanel() const { return buildToolsPnl; }

        /**
         * Get a pointer to the wxAUI pane manager
         */
        inline const wxAuiManager * GetAUIPaneManger() const { return &m_mgr; }
        /**
         * Get a pointer to the wxAUI pane manager
         */
        inline wxAuiManager * GetAUIPaneManger() { return &m_mgr; }

        /**
         * Get a pointer to scene locking shortcuts list.
         */
        inline std::vector<SceneCanvas*> * GetScenesLockingShortcutsList() { return &scenesLockingShortcuts; };

        //(*Handlers(Game_Develop_EditorFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnSaveMenuSelected(wxCommandEvent& event);
        void OnMenuNewSelected(wxCommandEvent& event);
        void OnMenuOpenSelected(wxCommandEvent& event);
        void OnMenuSaveSelected(wxCommandEvent& event);
        void OnMenuSaveAsSelected(wxCommandEvent& event);
        void OnMenuImagesEditorSelected(wxCommandEvent& event);
        void OnMenuObjetsEditorSelected(wxCommandEvent& event);
        void OnMenuEventsEditorSelected(wxCommandEvent& event);
        void OnMenuAideSelected(wxCommandEvent& event);
        void OnSceneTreeItemMenu(wxTreeEvent& event);
        void OnRefreshBtClick(wxCommandEvent& event);
        void OnStartBtClick(wxCommandEvent& event);
        void OnStopBtClick(wxCommandEvent& event);
        void OnPauseBtClick(wxCommandEvent& event);
        void OnEditionBtClick(wxCommandEvent& event);
        void OnApercuBtClick(wxCommandEvent& event);
        void OnAddObjCheckClick(wxCommandEvent& event);
        void OnDeplacerGCheckClick(wxCommandEvent& event);
        void OnDelObjetCheckClick(wxCommandEvent& event);
        void OnDeplacerDCheckClick(wxCommandEvent& event);
        void OnMenuPrefSelected(wxCommandEvent& event);
        void OnRefreshAnalyseBtClick(wxCommandEvent& event);
        void OnMenuCompilationSelected(wxCommandEvent& event);
        void OnMenuPortableSelected(wxCommandEvent& event);
        void OnMenuItem23Selected(wxCommandEvent& event);
        void OnMenuForumSelected(wxCommandEvent& event);
        void OnMenuSiteSelected(wxCommandEvent& event);
        void OnMenuFusionSelected(wxCommandEvent& event);
        void OnMenuItem36Selected(wxCommandEvent& event);
        void OnMenuTutoSelected(wxCommandEvent& event);
        void OnDecomposeGIFSelected(wxCommandEvent& event);
        void OnDecomposeRPGSelected(wxCommandEvent& event);
        void OnMenuWikiSelected(wxCommandEvent& event);
        void OnSceneTreeItemActivated(wxTreeEvent& event);
        void OnextensionsEditBtClick(wxCommandEvent& event);
        void OnDecomposeSSSelected(wxCommandEvent& event);
        void OnNotebook1PageChanged(wxAuiNotebookEvent& event);
        void OnmodVarSceneMenuISelected(wxCommandEvent& event);
        void OnglobalVarBtClick(wxCommandEvent& event);
        void OneditorsNotebookPageClose(wxAuiNotebookEvent& event);
        void OnOpenExampleSelected(wxCommandEvent& event);
        void OnautoSaveTimerTrigger(wxTimerEvent& event);
        void OnKeyDown(wxKeyEvent& event);
        void OnMenuSaveAllSelected(wxCommandEvent& event);
        void OnCloseCurrentProjectSelected(wxCommandEvent& event);
        void OnResize(wxSizeEvent& event);
        //*)
        void OnRibbonNewClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonOpenClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonOpenDropDownClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonSaveClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonSaveAllClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonSaveDropDownClicked(wxRibbonButtonBarEvent& evt);
        void OnProjectsManagerClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonDecomposerDropDownClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonStartPageClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonCppToolsClicked(wxRibbonButtonBarEvent& evt);
        void OnRibbonFileBtClick(wxMouseEvent& event);
        void OnRibbonFileBtLeave(wxMouseEvent& event);
        void OnRibbonFileBtEnter(wxMouseEvent& event);
        void OnRibbonHelpBtClick(wxMouseEvent& event);
        void OnRibbonHelpBtLeave(wxMouseEvent& event);
        void OnRibbonHelpBtEnter(wxMouseEvent& event);
        void SaveAs();
        void OnRecentClicked(wxCommandEvent& event );
        void UpdateNotebook();
        void MakeImagesEditorRibbon();
        void PrepareAutosave();
        void RealizeRibbonCustomButtons();

    private:

        //(*Identifiers(Game_Develop_EditorFrame)
        static const long ID_CUSTOM1;
        static const long ID_AUINOTEBOOK1;
        static const long ID_PANEL1;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM8;
        static const long ID_MENUITEM2;
        static const long ID_MENUITEM3;
        static const long ID_MENUITEM4;
        static const long ID_MENUITEM5;
        static const long ID_MENUITEM6;
        static const long ID_TIMER1;
        static const long ID_MENUITEM7;
        static const long ID_MENUITEM9;
        static const long ID_MENUITEM10;
        static const long ID_MENUITEM11;
        static const long ID_MENUITEM12;
        static const long ID_MENUITEM18;
        static const long ID_MENUITEM13;
        static const long ID_MENUITEM16;
        static const long ID_MENUITEM15;
        static const long ID_MENUITEM19;
        static const long ID_MENUITEM17;
        static const long ID_MENUITEM14;
        static const long ID_MENUITEM20;
        static const long ID_MENUITEM23;
        static const long ID_MENUITEM22;
        static const long ID_MENUITEM25;
        static const long ID_MENUITEM24;
        static const long ID_MENUITEM21;
        //*)
        static const long IDM_RECENTS;
        static const long idRibbonNew;
        static const long idRibbonOpen;
        static const long idRibbonSave;
        static const long idRibbonSaveAll;
        static const long idRibbonSaveAs;
        static const long idRibbonPortable;
        static const long idRibbonCompil;
        static const long idRibbonOptions;
        static const long idRibbonHelp;
        static const long idRibbonTuto;
        static const long idRibbonWiki;
        static const long idRibbonForum;
        static const long idRibbonUpdate;
        static const long idRibbonWebSite;
        static const long idRibbonCredits;
        static const long idRibbonFileBt;
        static const long idRibbonHelpBt;

        //(*Declarations(Game_Develop_EditorFrame)
        wxMenuItem* MenuItem8;
        wxMenuItem* MenuItem7;
        wxMenuItem* MenuItem5;
        wxMenuItem* MenuItem2;
        wxTimer autoSaveTimer;
        wxMenuItem* MenuItem4;
        wxMenuItem* MenuItem14;
        wxMenuItem* MenuItem15;
        wxPanel* Panel1;
        wxMenuItem* MenuItem17;
        wxAuiNotebook* editorsNotebook;
        wxMenuItem* MenuItem13;
        wxMenu openContextMenu;
        wxMenuItem* MenuItem10;
        wxMenu decomposerContextMenu;
        wxMenuItem* MenuItem12;
        wxMenuItem* MenuItem3;
        wxMenuItem* MenuItem20;
        wxMenuItem* MenuItem6;
        wxInfoBar* infoBar;
        wxMenu helpMenu;
        wxMenuItem* MenuItem21;
        wxMenuItem* MenuItem16;
        wxMenuItem* MenuItem43;
        wxMenuItem* MenuItem9;
        wxMenuItem* MenuItem18;
        wxMenu saveContextMenu;
        wxMenuItem* MenuItem19;
        wxMenu fileMenu;
        //*)
        wxAuiManager m_mgr;
        wxRibbonBar * m_ribbon; ///< Pointer to the ribbon
        wxStaticBitmap * ribbonFileBt; ///< Pointer to the ribbon file custom button
        wxStaticBitmap * ribbonHelpBt; ///< Pointer to the ribbon file custom button
        wxRibbonButtonBar * ribbonSceneEditorButtonBar; ///Pointer to the ribbon bar which can be changed by scene editors
        BuildToolsPnl * buildToolsPnl;
        std::vector<SceneCanvas*> scenesLockingShortcuts;
        MainEditorCommand mainEditorCommand;

        StartHerePage * startPage;
        ProjectManager * projectManager;

        RecentList m_recentlist; ///<Inventory and manage recent files

        wxBitmap ribbonFileNormalBitmap;
        wxBitmap ribbonFileHoveredBitmap;
        wxBitmap ribbonHelpNormalBitmap;
        wxBitmap ribbonHelpHoveredBitmap;

        DECLARE_EVENT_TABLE()
};

#endif // GAME_DEVELOP_EDITORMAIN_H
