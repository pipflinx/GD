/** \file
 *  Game Develop
 *  2008-2012 Florian Rival (Florian.Rival@gmail.com)
 */
//The first part of the implementation is available in SceneEditorCanvas.cpp
#if defined(GD_IDE_ONLY)
#include <SFML/Graphics.hpp>
#include <wx/aui/aui.h>
#include <wx/scrolbar.h>
#include <wx/config.h>
#include <wx/log.h>
#include "SceneEditorCanvas.h"
#include "GDCore/Tools/HelpFileAccess.h"
#include "GDCore/IDE/CommonBitmapManager.h"
#include "GDCore/IDE/Dialogs/MainFrameWrapper.h"
#include "GDL/IDE/Dialogs/DebuggerGUI.h"
#include "GDL/IDE/Dialogs/ProfileDlg.h"
#include "GDL/Object.h"
#include "GDL/ObjectHelpers.h"

void SceneEditorCanvas::RenderCompilationScreen()
{
    sf::Event event;
    while ( pollEvent( event ) )
        ;

    clear(sf::Color(255,255,255));

    pushGLStates();
    setView(sf::View(sf::Vector2f(getSize().x/2, getSize().y/2), sf::Vector2f(getSize().x, getSize().y)));

    reloadingIconSprite.setTexture(reloadingIconImage);
    reloadingIconSprite.setColor(sf::Color(255,255,255,128));
    reloadingIconSprite.setPosition(getSize().x/2-reloadingIconSprite.getLocalBounds().width/2, getSize().y/2-reloadingIconSprite.getLocalBounds().height/2);
    reloadingText.setPosition(getSize().x/2-reloadingText.getLocalBounds().width/2, reloadingIconSprite.getPosition().y+reloadingIconSprite.getLocalBounds().height+10);

    draw(reloadingIconSprite);
    draw(reloadingText);

    popGLStates();
    display();
}

void SceneEditorCanvas::SetParentAuiManager(wxAuiManager * manager)
{
    parentAuiManager = manager;

    //Create now subeditors if needed
    if ( manager && parentControl )
    {
        if ( !debugger )
        {
            debugger = boost::shared_ptr<DebuggerGUI>(new DebuggerGUI(parentControl, previewScene) );
            parentAuiManager->AddPane( debugger.get(), wxAuiPaneInfo().Name( wxT( "DBG" ) ).Float().CloseButton( true ).Caption( _( "Debugger" ) ).MaximizeButton( true ).MinimizeButton( false ).CaptionVisible(true).MinSize(200, 100).Show(false) );
        }
        if ( !profiler )
        {
            profiler = boost::shared_ptr<ProfileDlg>(new ProfileDlg(parentControl, *this));
            parentAuiManager->AddPane( profiler.get(), wxAuiPaneInfo().Name( wxT( "PROFILER" ) ).Float().CloseButton( true ).Caption( _( "Profiling" ) ).MaximizeButton( true ).MinimizeButton( false ).CaptionVisible(true).MinSize(50, 50).BestSize(230,100).Show(false) );
        }
    }
}

void SceneEditorCanvas::UpdateMouseResizeCursor(const std::string & currentResizeBt)
{
    if ( currentResizeBt == "resizeUp" || currentResizeBt == "resizeDown"  )
        wxSetCursor(wxCursor(wxCURSOR_SIZENS));
    if ( currentResizeBt == "resizeLeft" || currentResizeBt == "resizeRight"  )
        wxSetCursor(wxCursor(wxCURSOR_SIZEWE));
    if ( currentResizeBt == "resizeLeftUp" || currentResizeBt == "resizeRightDown"  )
        wxSetCursor(wxCursor(wxCURSOR_SIZENWSE));
    if ( currentResizeBt == "resizeRightUp" || currentResizeBt == "resizeLeftDown"  )
        wxSetCursor(wxCursor(wxCURSOR_SIZENESW));
}

double SceneEditorCanvas::GetMouseXOnLayout()
{
    return convertCoords(sf::Mouse::getPosition(*this), editionView).x;
}

double SceneEditorCanvas::GetMouseYOnLayout()
{
    return convertCoords(sf::Mouse::getPosition(*this), editionView).y;
}

boost::shared_ptr<Object> SceneEditorCanvas::GetObjectLinkedToInitialInstance(gd::InitialInstance & instance)
{
    if ( initialInstancesAndObjectsBimap.left.find(dynamic_cast<InitialPosition*>(&instance)) == initialInstancesAndObjectsBimap.left.end() )
    {
        std::cout << "ERROR: Object associated to initial instance \""+instance.GetObjectName()+"\" not found!";
        return boost::shared_ptr<Object> ();
    }

    return initialInstancesAndObjectsBimap.left.find(dynamic_cast<InitialPosition*>(&instance))->second;
}

double SceneEditorCanvas::GetWidthOfInitialInstance(gd::InitialInstance & instance)
{
    boost::shared_ptr<Object> object = GetObjectLinkedToInitialInstance(instance);
    if ( object ) return object->GetWidth();

    return 0;
}

double SceneEditorCanvas::GetHeightOfInitialInstance(gd::InitialInstance & instance)
{
    boost::shared_ptr<Object> object = GetObjectLinkedToInitialInstance(instance);
    if ( object ) return object->GetHeight();

    return 0;
}

double SceneEditorCanvas::GetRealXPositionOfInitialInstance(gd::InitialInstance & instance)
{
    boost::shared_ptr<Object> object = GetObjectLinkedToInitialInstance(instance);
    if ( object ) return object->GetDrawableX();

    return instance.GetX();
}

double SceneEditorCanvas::GetRealYPositionOfInitialInstance(gd::InitialInstance & instance)
{
    boost::shared_ptr<Object> object = GetObjectLinkedToInitialInstance(instance);
    if ( object ) return object->GetDrawableY();

    return instance.GetY();
}

void SceneEditorCanvas::OnInitialInstanceMoved(gd::InitialInstance & instance)
{
    boost::shared_ptr<Object> object = GetObjectLinkedToInitialInstance(instance);
    if ( object )
    {
        object->SetX(instance.GetX());
        object->SetY(instance.GetY());
    }
}

void SceneEditorCanvas::OnInitialInstanceDeleted(gd::InitialInstance & instance)
{
    previewScene.objectsInstances.RemoveObject(GetObjectLinkedToInitialInstance(instance));
    initialInstancesAndObjectsBimap.left.erase(dynamic_cast<InitialPosition*>(&instance));
}

void SceneEditorCanvas::OnInitialInstanceAdded(gd::InitialInstance & gdInstance)
{
    try
    {
        InitialPosition & instance = dynamic_cast<InitialPosition &>(gdInstance);

        std::vector<ObjSPtr>::iterator sceneObject = std::find_if(scene.GetInitialObjects().begin(), scene.GetInitialObjects().end(), std::bind2nd(ObjectHasName(), instance.GetObjectName()));
        std::vector<ObjSPtr>::iterator globalObject = std::find_if(game.GetGlobalObjects().begin(), game.GetGlobalObjects().end(), std::bind2nd(ObjectHasName(), instance.GetObjectName()));

        ObjSPtr newObject = boost::shared_ptr<Object> ();

        if ( sceneObject != scene.GetInitialObjects().end() ) //We check first scene's objects' list.
            newObject = boost::shared_ptr<Object>((*sceneObject)->Clone());
        else if ( globalObject != game.GetGlobalObjects().end() ) //Then the global object list
            newObject = boost::shared_ptr<Object>((*globalObject)->Clone());

        if ( newObject == boost::shared_ptr<Object> () )
        {
            wxLogError(_("The object to add does not exist ( anymore ) in the object list.\nDrag and drop objects on the scene from the objects list."));
            return;
        }

        //Create the object to be displayed during edition
        newObject->SetX( instance.GetX() );
        newObject->SetY( instance.GetY() );
        newObject->SetZOrder( instance.GetZOrder() );
        newObject->SetLayer( instance.GetLayer() );
        newObject->InitializeFromInitialPosition(instance);
        newObject->LoadRuntimeResources( previewScene, *previewGame.imageManager );

        previewScene.objectsInstances.AddObject(newObject);
        initialInstancesAndObjectsBimap.insert(InstanceAndObjectPair(&instance, newObject));

        newObject->LoadResources(previewScene, *previewGame.imageManager); //Global objects images are curiously not displayed if we don't reload resources..

    }
    catch(...)
    {
        std::cout << "ERROR: IDE probably sent an object which is not a GD C++ Platform object to OnInitialInstanceAdded" << std::endl;
    }

}

void SceneEditorCanvas::CreatePreviewRibbonTools()
{
    bool hideLabels = false;
    wxConfigBase::Get()->Read( _T( "/Skin/HideLabels" ), &hideLabels );
    gd::CommonBitmapManager * bitmapManager = gd::CommonBitmapManager::GetInstance();

    wxRibbonButtonBar * ribbonToolbar = mainFrameWrapper.GetRibbonSceneEditorButtonBar();
    ribbonToolbar->AddButton(idRibbonRefresh, !hideLabels ? _("Refresh") : "", bitmapManager->refreshicon24);
    ribbonToolbar->AddButton(idRibbonPlay, !hideLabels ? _("Play") : "", bitmapManager->starticon24);
    ribbonToolbar->AddButton(idRibbonPlayWin, !hideLabels ? _("Play in a window") : "", bitmapManager->startwindow24);
    ribbonToolbar->AddButton(idRibbonPause, !hideLabels ? _("Pause") : "", bitmapManager->pauseicon24);
    ribbonToolbar->AddButton(idRibbonDebugger, !hideLabels ? _("Debugger") : "", bitmapManager->bug24);
    ribbonToolbar->AddButton(idRibbonProfiler, !hideLabels ? _("Profiling") : "", bitmapManager->profiler24);

    LayoutEditorCanvas::CreatePreviewRibbonTools();
}

void SceneEditorCanvas::CreateEditionRibbonTools()
{
    bool hideLabels = false;
    wxConfigBase::Get()->Read( _T( "/Skin/HideLabels" ), &hideLabels );
    gd::CommonBitmapManager * bitmapManager = gd::CommonBitmapManager::GetInstance();

    LayoutEditorCanvas::CreateEditionRibbonTools();

    wxRibbonButtonBar * ribbonToolbar = mainFrameWrapper.GetRibbonSceneEditorButtonBar();
    ribbonToolbar->AddButton(idRibbonOrigine, !hideLabels ? _("Return to the initial position ( 0;0 )") : "", bitmapManager->center24);
    ribbonToolbar->AddHybridButton(idRibbonOriginalZoom, !hideLabels ? _("Initial zoom") : "", bitmapManager->zoom24);
}

void SceneEditorCanvas::DoConnectEvents()
{
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonOrigine, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnOrigineBtClick, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonOriginalZoom, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnZoomInitBtClick, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonOriginalZoom, wxEVT_COMMAND_RIBBONBUTTON_DROPDOWN_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnZoomMoreBtClick, NULL, this);
	mainFrameWrapper.GetMainEditor()->Connect(ID_CUSTOMZOOMMENUITEM,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&SceneEditorCanvas::OnCustomZoomSelected, NULL, this);

    mainFrameWrapper.GetMainEditor()->Connect(idRibbonRefresh, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnPreviewRefreshBtClick, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonPlay, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnPreviewPlayBtClick, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonPlayWin, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnPreviewPlayWindowBtClick, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonPause, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnPreviewPauseBtClick, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonDebugger, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnPreviewDebugBtClick, NULL, this);
    mainFrameWrapper.GetMainEditor()->Connect(idRibbonProfiler, wxEVT_COMMAND_RIBBONBUTTON_CLICKED, (wxObjectEventFunction)&SceneEditorCanvas::OnPreviewProfilerBtClick, NULL, this);
}

void SceneEditorCanvas::OnZoomInitBtClick( wxCommandEvent & event )
{
    options.zoomFactor = 1;
    UpdateViewAccordingToZoomFactor();
}

void SceneEditorCanvas::OnZoomMoreBtClick(wxRibbonButtonBarEvent& evt)
{
    evt.PopupMenu(&zoomMenu);
}

void SceneEditorCanvas::OnMouseWheel( wxMouseEvent &event )
{
    if (!editing) return;

    float rotation = event.GetWheelRotation()*3;
    float newheight = editionView.getSize().y + ( rotation / 25 );
    float newZoomFactor = static_cast<float>(getSize().y)/newheight;
    if ( newZoomFactor > 0 ) options.zoomFactor = newZoomFactor;
    UpdateViewAccordingToZoomFactor();
}

void SceneEditorCanvas::OnCustomZoomSelected(wxCommandEvent& event)
{
    std::cout << event.GetId();
    UpdateViewAccordingToZoomFactor();
}

void SceneEditorCanvas::OnOrigineBtClick(wxCommandEvent & event )
{
    editionView.setCenter( (game.GetMainWindowDefaultWidth()/2),(game.GetMainWindowDefaultHeight()/2));
}

void SceneEditorCanvas::UpdateSize()
{
    if (parentControl == NULL) return;

    if ( editing )
    {
        //Scene takes all the space available in edition mode.

        //This line is unnecessary and create a crash related to X on Linux.
        //Window::SetSize(parentControl->GetSize().GetWidth()-vScrollbar->GetSize().GetWidth(), parentControl->GetSize().GetHeight()-hScrollbar->GetSize().GetHeight());
        wxWindowBase::SetPosition(wxPoint(0,0));
        wxWindowBase::SetSize(parentControl->GetSize().GetWidth() - (vScrollbar ? vScrollbar->GetSize().GetWidth() : 0),
                              parentControl->GetSize().GetHeight()- (hScrollbar ? hScrollbar->GetSize().GetHeight() : 0));

        UpdateViewAccordingToZoomFactor();
    }
    else
    {
        //Scene has the size of the game's window size in preview mode.
        Window::setSize(sf::Vector2u(game.GetMainWindowDefaultWidth(), game.GetMainWindowDefaultHeight()));
        wxWindowBase::SetClientSize(game.GetMainWindowDefaultWidth(), game.GetMainWindowDefaultHeight());

        //TODO : if ( externalWindow ) externalWindow->SetSizeOfRenderingZone(game.GetMainWindowDefaultWidth(), game.GetMainWindowDefaultHeight());

        //Scene is centered in preview mode
        wxWindowBase::SetPosition(wxPoint((parentControl->GetSize().GetWidth()-wxWindowBase::GetSize().GetX())/2,
                              (parentControl->GetSize().GetHeight()-wxWindowBase::GetSize().GetY())/2));
    }
}

void SceneEditorCanvas::OnvScrollbarScroll(wxScrollEvent& event)
{
    if ( vScrollbar == NULL )
        return;

    int newY = event.GetPosition()-(vScrollbar->GetRange()/2)+(getSize().y/2);
    editionView.setCenter( editionView.getCenter().x, newY);

    OnUpdate();
}

void SceneEditorCanvas::OnhScrollbarScroll(wxScrollEvent& event)
{
    if ( hScrollbar == NULL )
        return;

    int newX = event.GetPosition()-(hScrollbar->GetRange()/2)+(getSize().x/2);
    editionView.setCenter( newX, editionView.getCenter().y );

    OnUpdate();
}

void SceneEditorCanvas::UpdateScrollbars()
{
    if ( hScrollbar == NULL || vScrollbar == NULL )
        return;

    //Compute the thumb position
    int thumbY = editionView.getCenter().y+vScrollbar->GetRange()/2-getSize().y/2;
    vScrollbar->SetScrollbar(thumbY, getSize().y, vScrollbar->GetRange(), getSize().y);

    int thumbX = editionView.getCenter().x+hScrollbar->GetRange()/2-getSize().x/2;
    hScrollbar->SetScrollbar(thumbX, getSize().x, hScrollbar->GetRange(), getSize().x);

    //Update the size if needed
    if ( thumbY <= 0 || static_cast<int>(thumbY+getSize().y) >= vScrollbar->GetRange())
    {
        int ajout = getSize().y;
        vScrollbar->SetScrollbar(thumbY+ajout/2, getSize().y, vScrollbar->GetRange()+ajout, getSize().y);
    }

    if ( thumbX <= 0 || static_cast<int>(thumbX+getSize().x) >= hScrollbar->GetRange())
    {
        int ajout = getSize().x;
        hScrollbar->SetScrollbar(thumbX+ajout/2, getSize().x, hScrollbar->GetRange()+ajout, getSize().x);
    }
}
void SceneEditorCanvas::UpdateViewAccordingToZoomFactor()
{
    editionView.setSize(GetClientSize().GetWidth()/options.zoomFactor, GetClientSize().GetHeight()/options.zoomFactor);
}

void SceneEditorCanvas::OnHelpBtClick( wxCommandEvent & event )
{
    gd::HelpFileAccess::GetInstance()->OpenURL(_("http://www.wiki.compilgames.net/doku.php/en/game_develop/documentation/manual/edit_scene"));
}

#endif