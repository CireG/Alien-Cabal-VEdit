/*******************************************************************

    frame.c

    Routines for displaying and managing the MDI frame window.

********************************************************************/


#include "vedit.h"
#include <mmsystem.h>


//
//  Private definitions.
//


//
//  Private prototypes.
//

VOID Frame_OnCommand( HWND hwnd,
                      INT  id,
                      HWND hwndCtl,
                      UINT codeNotify );

BOOL Frame_OnCreate( HWND               hwnd,
                     CREATESTRUCT FAR * pCreateStruct );

VOID Frame_OnInitMenu( HWND hwnd, HMENU hMenu );

VOID Frame_OnClose( HWND hwnd );

VOID Frame_OnDestroy( HWND hwnd );

BOOL Frame_OnQueryEndSession( HWND hwnd );

LRESULT Frame_MDIMessageForwarder( HWND   hwnd,
                                   UINT   nMessage,
                                   WPARAM wParam,
                                   LPARAM lParam );

VOID Frame_FileNew( VOID );

VOID Frame_FileOpen( VOID );

VOID Frame_FileClose( VOID );

VOID Frame_FileRebuild( VOID );

VOID Frame_FileSaveAs( VOID );

VOID Frame_FilePrint( VOID );

VOID Frame_FilePrintSetup( VOID );

VOID Frame_EditUndo( VOID );

VOID Frame_EditCut( VOID );

VOID Frame_EditCopy( VOID );

VOID Frame_EditPaste( VOID );

VOID Frame_EditDelete( VOID );

VOID Frame_EditHide( VOID );

VOID Frame_EditHideAll( VOID );

VOID Frame_EditShowAll( VOID );

VOID Frame_EditClearSelections( VOID );

VOID Frame_EditVertexPosition( VOID );

VOID Frame_EditVertexProperties( VOID );

VOID Frame_EditPolygonProperties( VOID );

VOID Frame_EditGroupAssignments( VOID );

VOID Frame_ViewHome( VOID );

VOID Frame_ViewOptions( VOID );

VOID Frame_ViewStatistics( VOID );

VOID Frame_ViewFileInfo( VOID );

VOID Frame_ViewLevels( VOID );

VOID Frame_ViewBitmaps( VOID );

VOID Frame_ViewTextures( VOID );

VOID Frame_ViewSounds( VOID );

VOID Frame_ViewAnimations( VOID );

VOID Frame_ViewLifeForms( VOID );

VOID Frame_ViewItems( VOID );

VOID Frame_ViewSprites( VOID );

VOID Frame_ViewNoises( VOID );

VOID Frame_ViewGroups( VOID );

VOID Frame_ViewTriggers( VOID );

VOID Frame_ViewMotions( VOID );

VOID Frame_ViewMarks( VOID );

WND_DATA *Frame_CreateWindowData( WORD cid );


//
//  Private data.
//

static WORD wNextCid;


//
//  Public functions.
//

/*******************************************************************

    NAME:       Frame_WndProc

    SYNOPSIS:   Window procedure for the MDI frame window.

    ENTRY:      hwnd - Window handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    LRESULT - Depends on the actual message.

********************************************************************/

LRESULT CALLBACK __export Frame_WndProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG( hwnd, WM_COMMAND,         Frame_OnCommand         );
    HANDLE_MSG( hwnd, WM_CREATE,          Frame_OnCreate          );
    HANDLE_MSG( hwnd, WM_INITMENU,        Frame_OnInitMenu        );
    HANDLE_MSG( hwnd, WM_CLOSE,           Frame_OnClose           );
    HANDLE_MSG( hwnd, WM_DESTROY,         Frame_OnDestroy         );
    HANDLE_MSG( hwnd, WM_QUERYENDSESSION, Frame_OnQueryEndSession );
  }

  return DefFrameProc( hwnd, hwndMDIClient, nMessage, wParam, lParam );

}   // Frame_WndProc


//
//  Private functions.
//

/*******************************************************************

    NAME:       Frame_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages.

    ENTRY:      hwnd - Window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/

VOID Frame_OnCommand( HWND hwnd,
                      INT  id,
                      HWND hwndCtl,
                      UINT codeNotify )
{
  switch( id )
  {
    case IDM_FILE_NEW:
      Frame_FileNew();
      break;

    case IDM_FILE_OPEN:
      Frame_FileOpen();
      break;

    case IDM_FILE_CLOSE:
      Frame_FileClose();
      break;

    case IDM_FILE_REBUILD:
      Frame_FileRebuild();
      break;

    case IDM_FILE_SAVE:
      Frame_FileSave( NULL );
      break;

    case IDM_FILE_SAVE_AS:
      Frame_FileSaveAs();
      break;

    case IDM_FILE_PRINT:
      Frame_FilePrint();
      break;

    case IDM_FILE_PRINT_SETUP:
      Frame_FilePrintSetup();
      break;

    case IDM_FILE_EXIT:
      SendMessage( hwnd, WM_CLOSE, 0, 0 );
      break;

    case IDM_EDIT_UNDO:
      Frame_EditUndo();
      break;

    case IDM_EDIT_CUT:
      Frame_EditCut();
      break;

    case IDM_EDIT_COPY:
      Frame_EditCopy();
      break;

    case IDM_EDIT_PASTE:
      Frame_EditPaste();
      break;

    case IDM_EDIT_DELETE:
      Frame_EditDelete();
      break;

    case IDM_EDIT_HIDE:
      Frame_EditHide();
      break;

    case IDM_EDIT_HIDE_ALL:
      Frame_EditHideAll();
      break;

    case IDM_EDIT_SHOW_ALL:
      Frame_EditShowAll();
      break;

    case IDM_EDIT_CLEAR_SELECTIONS:
      Frame_EditClearSelections();
      break;

    case IDM_EDIT_VERTEX_POSITION:
      Frame_EditVertexPosition();
      break;

    case IDM_EDIT_VERTEX_PROPERTIES:
      Frame_EditVertexProperties();
      break;

    case IDM_EDIT_POLYGON_PROPERTIES:
      Frame_EditPolygonProperties();
      break;

    case IDM_EDIT_GROUP_ASSIGNMENTS:
      Frame_EditGroupAssignments();
      break;

    case IDM_VIEW_HOME:
      Frame_ViewHome();
      break;

    case IDM_VIEW_OPTIONS:
      Frame_ViewOptions();
      break;

    case IDM_VIEW_STATISTICS:
      Frame_ViewStatistics();
      break;

    case IDM_VIEW_FILE_INFO:
      Frame_ViewFileInfo();
      break;

    case IDM_VIEW_LEVELS:
      Frame_ViewLevels();
      break;

    case IDM_VIEW_BITMAPS:
      Frame_ViewBitmaps();
      break;

    case IDM_VIEW_TEXTURES:
      Frame_ViewTextures();
      break;

    case IDM_VIEW_SOUNDS:
      Frame_ViewSounds();
      break;

    case IDM_VIEW_ANIMATIONS:
      Frame_ViewAnimations();
      break;

    case IDM_VIEW_LIFEFORMS:
      Frame_ViewLifeForms();
      break;

    case IDM_VIEW_ITEMS:
      Frame_ViewItems();
      break;

    case IDM_VIEW_SPRITES:
      Frame_ViewSprites();
      break;

    case IDM_VIEW_NOISES:
      Frame_ViewNoises();
      break;

    case IDM_VIEW_GROUPS:
      Frame_ViewGroups();
      break;

    case IDM_VIEW_TRIGGERS:
      Frame_ViewTriggers();
      break;

    case IDM_VIEW_MOTIONS:
      Frame_ViewMotions();
      break;

    case IDM_VIEW_MARKS:
      Frame_ViewMarks();
      break;
      
    case IDM_WINDOW_CASCADE:
      FORWARD_WM_MDICASCADE( hwndMDIClient, 0, SendMessage );
      break;

    case IDM_WINDOW_TILE_VERTICALLY:
      FORWARD_WM_MDITILE( hwndMDIClient, MDITILE_VERTICAL, SendMessage );
      break;

    case IDM_WINDOW_TILE_HORIZONTALLY:
      FORWARD_WM_MDITILE( hwndMDIClient, MDITILE_HORIZONTAL, SendMessage );
      break;

    case IDM_WINDOW_ARRANGE_ICONS:
      FORWARD_WM_MDIICONARRANGE( hwndMDIClient, SendMessage );
      break;

    case IDM_HELP_ABOUT:
      AboutDialog( hwnd );
      break;

    default:
      FORWARD_WM_COMMAND( hwnd,
                          id,
                          hwndCtl,
                          codeNotify,
                          Frame_MDIMessageForwarder );
      break;
  }

}   // Frame_OnCommand


/*******************************************************************

    NAME:       Frame_OnCreate

    SYNOPSIS:   Handles WM_CREATE messages.

    ENTRY:      hwnd - Window handle.

                pCreateStruct - Contains window creation parameters.

    RETURNS:    BOOL - TRUE if window created OK, FALSE otherwise.

********************************************************************/

BOOL Frame_OnCreate( HWND               hwnd,
                     CREATESTRUCT FAR * pCreateStruct )
{
  CLIENTCREATESTRUCT ccs;

  //
  //  Create the MDI client window.
  //

  ccs.hWindowMenu  = GetSubMenu( GetMenu( hwnd ), 3 );
  ccs.idFirstChild = IDM_WINDOW_FIRST_CHILD;

  hwndMDIClient = CreateWindow( pszMDIClientClass,
                                NULL,
                                WS_CHILD | WS_CLIPCHILDREN,
                                0, 0,
                                0, 0,
                                hwnd,
                                0,
                                hInst,
                                (LPSTR)&ccs );

  if ( hwndMDIClient == NULL )
  {
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Cannot create client window" );

    return FALSE;
  }

  ShowWindow( hwndMDIClient, SW_SHOW );

  return TRUE;

} // Frame_OnCreate


/*******************************************************************

    NAME:       Frame_OnInitMenu

    SYNOPSIS:   Setup the menu based on the current child window.

    ENTRY:      hwnd - Window handle.
                
                hMenu - Menu handle.
                
********************************************************************/

VOID Frame_OnInitMenu( HWND hwnd, HMENU hMenu )
{
  HWND hwndChild;
  UINT uEnable;
  WND_DATA *pWndData;
  LEVEL_DATA *pLevel;

  if ( hMenu == NULL )
  {
    return;
  }

  pWndData = NULL;
  pLevel = NULL;

  //
  //  If there are no active windows then disable the applicable commands.
  //

  hwndChild = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwndChild )
  {
    pWndData = WNDPTR( hwndChild );
  }
  
  if ( pWndData )
  {
    pLevel = &pWndData->pLevelData[pWndData->level_index];

    if ( !pWndData->loaded )
    {
      pWndData = NULL;
    }
  }
  
  uEnable = MF_BYCOMMAND | MF_GRAYED;

  if ( pWndData )
  {
    uEnable = MF_BYCOMMAND | MF_ENABLED;
  }

  EnableMenuItem( hMenu, IDM_FILE_CLOSE,      uEnable );
  EnableMenuItem( hMenu, IDM_FILE_REBUILD,    uEnable );
  EnableMenuItem( hMenu, IDM_FILE_SAVE,       uEnable );
  EnableMenuItem( hMenu, IDM_FILE_SAVE_AS,    uEnable );
  EnableMenuItem( hMenu, IDM_FILE_PRINT,      uEnable );
  EnableMenuItem( hMenu, IDM_EDIT_HIDE_ALL,   uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_HOME,       uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_OPTIONS,    uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_STATISTICS, uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_FILE_INFO,  uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_LEVELS,     uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_BITMAPS,    uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_TEXTURES,   uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_SOUNDS,     uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_ANIMATIONS, uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_LIFEFORMS,  uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_ITEMS,      uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_SPRITES,    uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_NOISES,     uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_GROUPS,     uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_TRIGGERS,   uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_MOTIONS,    uEnable );
  EnableMenuItem( hMenu, IDM_VIEW_MARKS,      uEnable );

  //
  //  If undo data is available then enable the Undo command.
  //

  uEnable = MF_BYCOMMAND | MF_GRAYED;

  if ( pWndData )
  {
    if ( pWndData->pUndoData )
    {
      uEnable = MF_BYCOMMAND | MF_ENABLED;
    }
  }

  EnableMenuItem( hMenu, IDM_EDIT_UNDO, uEnable );

  //
  //  If our data is on the clipboard then enable the Paste command.
  //
  
  uEnable = MF_BYCOMMAND | MF_GRAYED;

  if ( pWndData )
  {
    if ( IsClipboardFormatAvailable(uFormatCode) )
    {
      uEnable = MF_BYCOMMAND | MF_ENABLED;
    }
  }

  EnableMenuItem( hMenu, IDM_EDIT_PASTE, uEnable );

  //
  //  If anything is selected then enable the Cut, Copy, Hide, Delete
  //  and Clear Selections commands.
  //
  
  uEnable = MF_BYCOMMAND | MF_GRAYED;

  if ( pWndData )
  {
    if ( GetOpenClipboardWindow() == NULL )
    {
      if ( CountAllSelections( pLevel ) > 0 )
      {
        uEnable = MF_BYCOMMAND | MF_ENABLED;
      }
    }
  }

  EnableMenuItem( hMenu, IDM_EDIT_CUT,               uEnable );
  EnableMenuItem( hMenu, IDM_EDIT_COPY,              uEnable );
  EnableMenuItem( hMenu, IDM_EDIT_HIDE,              uEnable );
  EnableMenuItem( hMenu, IDM_EDIT_DELETE,            uEnable );
  EnableMenuItem( hMenu, IDM_EDIT_CLEAR_SELECTIONS,  uEnable );
  EnableMenuItem( hMenu, IDM_EDIT_GROUP_ASSIGNMENTS, uEnable );

  //
  //  If there are any selected polygons enable "Polygon Properties..."
  //
  
  uEnable = MF_BYCOMMAND | MF_GRAYED;

  if ( pWndData )
  {
    if ( GetOpenClipboardWindow() == NULL )
    {
      if ( CountSelectedPolygons( pLevel ) > 0 )
      {
        uEnable = MF_BYCOMMAND | MF_ENABLED;
      }
    }
  }

  EnableMenuItem( hMenu, IDM_EDIT_POLYGON_PROPERTIES, uEnable );

  //
  //  If there are any selected vertices enable "Vertices..."
  //
  
  uEnable = MF_BYCOMMAND | MF_GRAYED;

  if ( pWndData )
  {
    if ( GetOpenClipboardWindow() == NULL )
    {
      if ( CountSelectedVertices( pLevel ) > 0 )
      {
        uEnable = MF_BYCOMMAND | MF_ENABLED;
      }
    }
  }

  EnableMenuItem( hMenu, IDM_EDIT_VERTEX_POSITION, uEnable );
  EnableMenuItem( hMenu, IDM_EDIT_VERTEX_PROPERTIES, uEnable );

  //
  //  If nothing is hidden then disable the Show All command.
  //
  
  uEnable = MF_BYCOMMAND | MF_GRAYED;

  if ( pWndData )
  {
    if ( GetOpenClipboardWindow() == NULL )
    {
      if ( CountAllHidden( pLevel ) > 0 )
      {
        uEnable = MF_BYCOMMAND | MF_ENABLED;
      }
    }
  }

  EnableMenuItem( hMenu, IDM_EDIT_SHOW_ALL, uEnable );

} // Frame_OnInitMenu


/*******************************************************************

    NAME:       Frame_OnClose

    SYNOPSIS:   Handles WM_CLOSE message. If all children say its
                ok to close, then shut down the app.

    ENTRY:      hwnd - Window handle.

********************************************************************/

VOID Frame_OnClose( HWND hwnd )
{
  if ( Frame_OnQueryEndSession( hwnd ) )
  {
    FORWARD_WM_CLOSE( hwnd, Frame_MDIMessageForwarder );
  }
  
} // Frame_OnClose


/*******************************************************************

    NAME:       Frame_OnDestroy

    SYNOPSIS:   Handles WM_DESTROY messages.

    ENTRY:      hwnd - Window handle.

********************************************************************/

VOID Frame_OnDestroy( HWND hwnd )
{
  if (pSoundPlayData)
  {
    sndPlaySound(NULL, 0);
    GlobalFreePtr(pSoundPlayData);
    pSoundPlayData = NULL;
  }

  PostQuitMessage( 0 );

} // Frame_OnDestroy


/*******************************************************************

    NAME:       Frame_OnQueryEndSession

    SYNOPSIS:   Passes WM_QUERYENDSESSION message to all the children.

    ENTRY:      hwnd - Window handle.

    RETURN:     TRUE if ok to end windows session.
                FALSE if not.
                
********************************************************************/

BOOL Frame_OnQueryEndSession( HWND hwnd )
{
  HWND hwndChild;

  hwndChild = GetWindow( hwndMDIClient, GW_CHILD );

  while( hwndChild != NULL )
  {
    //
    //  Make sure we aren't looking at an icon title of a minimized
    //  MDI child window. Note that in Windows 95 there are no icon
    //  title windows created when minimizing an MDI child window.
    //
    
    if ( GetWindow( hwndChild, GW_OWNER ) == NULL )
    {
      if ( FORWARD_WM_QUERYENDSESSION( hwndChild, SendMessage ) == FALSE )
      {
        return FALSE;
      }
    }

    hwndChild = GetWindow( hwndChild, GW_HWNDNEXT );
  }

  return TRUE;

} // Frame_OnQueryEndSession


/*******************************************************************

    NAME:       Frame_MDIMessageForwarder

    SYNOPSIS:   Forwards messages to DefFrameProc.  This is usually
                used as the last parameter to a FORWARD_WM_* macro.

    ENTRY:      hwnd - Window handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    LRESULT - Depends on the actual message.

********************************************************************/

LRESULT Frame_MDIMessageForwarder( HWND   hwnd,
                                   UINT   nMessage,
                                   WPARAM wParam,
                                   LPARAM lParam )
{
  return DefFrameProc( hwnd, hwndMDIClient, nMessage, wParam, lParam );

} // Frame_MDIMessageForwarder


/*******************************************************************

    NAME:       Frame_FileNew

    SYNOPSIS:   Create a new GOB file.

********************************************************************/

VOID Frame_FileNew( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  //
  //  Allocate memory for the window data.
  //

  pWndData = Frame_CreateWindowData( 0 );
  
  if ( pWndData == NULL )
  {
    return;
  }

  //
  //  Create an empty level to display.
  //

  if (Frame_NewLevel(pWndData, 0) == FALSE)
  {
    Frame_FreeWindowData( pWndData );
    return;
  }

  pWndData->nLevels = 1;

  //
  //  Record the path and name in the window data.
  //
  
  _fstrcpy( pWndData->path, szPath );
  wsprintf( pWndData->name, "NEW-%u.GOB", wNextCid );
  wsprintf( pWndData->file, "%s%s", pWndData->path, pWndData->name );
      
  //
  //  Create a new window.
  //
  
  hwnd = Object_CreateNew(pWndData);

  if ( hwnd == NULL )
  {
    Frame_FreeWindowData( pWndData );

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to create window" );

    return;
  }

  //
  //  Success.
  //

  InvalidateRect( hwnd, NULL, TRUE );
  pWndData->loaded = TRUE;
  pWndData->need_to_save = FALSE;

  return;

} // Frame_FileNew


/*******************************************************************

    NAME:       Frame_FileOpen

    SYNOPSIS:   Prompts the user to select a file, then loads and
                displays it.

********************************************************************/

VOID Frame_FileOpen( VOID )
{
  INT n;
  BOOL bResult;
  OPENFILENAME ofn;
  char szDirName[256], szFile[256], szFileTitle[256];
  char drive[3], dir[64], fname[9], ext[5];
  static char *szFilter = "GOB files (*.gob)\000*.gob\000"
                          "All Files (*.*)\000*.*\000";

  _fstrcpy( szDirName, szPath );

  n = _fstrlen( szDirName );

  if (n > 0)
  {
    if (szDirName[n-1] == '\\' )
    {
      szDirName[n-1] = '\0';
    }
  }
  
  szFile[0] = '\0';

  _fmemset( &ofn, 0, sizeof(OPENFILENAME) );

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwndMDIClient;
  ofn.hInstance = hInst;
  ofn.lpstrFilter = szFilter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile= szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof(szFileTitle);
  ofn.lpstrInitialDir = szDirName;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  bResult = GetOpenFileName( &ofn );
    
  if ( !bResult )
  {
    DWORD dwError;
    
    dwError = CommDlgExtendedError();
    
    if ( dwError )
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Common dialog error %lu",
              dwError );
    }
              
    return;
  }

  //
  //  Check for a valid file name.
  //
  
  n = _fstrlen( szFileTitle );
  
  if ( n < 5 )
  {
    return;
  }
  
  //
  //  If the file is a GOB file then create a new window.
  //
  
  if ( !_fstricmp( &szFileTitle[n-4], ".gob" ) )
  {
    HWND hwnd;
    WND_DATA *pWndData;
      
    pWndData = Frame_CreateWindowData( 0 );

    if ( pWndData == NULL )
    {
      return;
    }

    //
    //  Record the path and name in the window data.
    //
  
    _splitpath( szFile, drive, dir, fname, ext );
    wsprintf( pWndData->path, "%s%s", drive, dir );
    _fstrcpy( pWndData->name, szFileTitle );
    _fstrcpy( pWndData->file, szFile );
  
    //
    //  Load the graphical object file.
    //

    if ( LoadObjectFile( szFile, pWndData ) == FALSE )
    {
      Frame_FreeWindowData( pWndData );
      return;
    }

    //
    //  Create a graphical object window.
    //

    hwnd = Object_CreateNew( (WND_DATA *)pWndData );

    if ( hwnd == NULL )
    {
      Frame_FreeWindowData( pWndData );
      return;
    }

    //
    //  Success.
    //

    InvalidateRect( hwnd, NULL, TRUE );
    pWndData->loaded = TRUE;
    return;
  }
  
  return;
  
} // Frame_FileOpen


/*******************************************************************

    NAME:       Frame_FileClose

    SYNOPSIS:   Close the active MDI child window.

********************************************************************/

VOID Frame_FileClose( VOID )
{
  HWND hwnd;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd != NULL )
  {
    SendMessage( hwnd, WM_CLOSE, (WPARAM)0, (LPARAM)0 );
  }
      
  return;

} // Frame_FileClose


/*******************************************************************

    NAME:       Frame_FileRebuild

    SYNOPSIS:   Rebuild the zone data. This would be done after
                a zone data format change or to erase all visibility
                lists.

********************************************************************/

VOID Frame_FileRebuild( VOID )
{
  HWND hwnd;
  INT i;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if (hwnd == NULL)
  {
    return;
  }
  
  pWndData = WNDPTR(hwnd);

  if (pWndData == NULL)
  {
    return;
  }
  
  if (!pWndData->loaded)
  {
    return;
  }

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pWndData->pLevelData[i].bRebuildZone = TRUE;
  }

  if (RebuildZoneData(pWndData) == FALSE)
  {
    Object_ViewChanged(hwnd, pWndData);
  }
      
  return;

} // Frame_FileRebuild


/*******************************************************************

    NAME:       Frame_FileSave

    SYNOPSIS:   Save the assay data indicated or the contents of
                the active window.

    ENTRY:      pAssayWnd - Pointer to the assay data (or NULL to
                  save the data associated with the active window).

********************************************************************/

VOID Frame_FileSave( WND_DATA *pWndData )
{
  HWND hwnd;
    
  if ( pWndData == NULL )
  {
    hwnd = GetWindow( hwndMDIClient, GW_CHILD );

    if ( hwnd == NULL )
    {
      return;
    }
  
    pWndData = WNDPTR( hwnd );

    if ( pWndData == NULL )
    {
      return;
    }
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  if ( SaveObjectFile( pWndData->file, pWndData ) == FALSE )
  {
    return;
  }

  pWndData->need_to_save = FALSE;
  
  return;

} // Frame_FileSave


/*******************************************************************

    NAME:       Frame_FileSaveAs

    SYNOPSIS:   Save the contents of the active MDI child window.

********************************************************************/

VOID Frame_FileSaveAs( VOID )
{
  INT i, j, n, nResult;
  HWND hwnd;
  BOOL bResult;
  OPENFILENAME ofn;
  WND_DATA *pWndData;
  char szDirName[128], szFile[128], szFileTitle[128];
  char drive[3], dir[128], fname[9], ext[5], path[128];
  char szBuf[128];
  
  static LPSTR szFilter = "GOB files (*.gob)\000*.gob\000";

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  _fstrcpy( szDirName, pWndData->path );

  n = _fstrlen( szDirName );

  if (n > 0)
  {
    if (szDirName[n-1] == '\\' )
    {
      szDirName[n-1] = '\0';
    }
  }
  
  _fstrcpy( szFile, pWndData->name );

  _fmemset( &ofn, 0, sizeof(OPENFILENAME) );

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwndMDIClient;
  ofn.hInstance = hInst;
  ofn.lpstrFilter = szFilter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile= szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof(szFileTitle);
  ofn.lpstrInitialDir = szDirName;
  ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
  
  bResult = GetSaveFileName( &ofn );
  
  if ( !bResult )
  {
    DWORD dwError;
    
    dwError = CommDlgExtendedError();
    
    if ( dwError )
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Common dialog error %lu",
              dwError );
    }
              
    return;
  }

  _splitpath( szFile, drive, dir, fname, ext );

  wsprintf( path, "%s%s", drive, dir );
  
  n = _fstrlen( path );

  if ( n > 0 )
  {
    if ( path[n-1] != '\\' )
    {
      path[n-1] = '\\';
    }
  }
  
  n = _fstrlen( szFileTitle );
  
  for( i = 0; i < n; ++i )
  {
    if ( !_fstrchr( pszValidDosChars, szFileTitle[i] ) && szFileTitle[i] != '.' )
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "The file name entered as \"%s\" contains invalid "
              "characters. Please use only those characters which are "
              "allowed in a DOS file name.",
              szFileTitle );

      return;
    }
  }

  j = 0;

  for( i = 0; i < n; ++i )
  {
    if ( szFileTitle[i] == '.' )
    {
      ++j;
    }
  }

  i = 0;
  
  while( i < n  && szFileTitle[i] != '.' )
  {
    ++i;
  }
  
  if ( i < 1 || i > 8 || (n - i) != 4 || j > 1 )
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "The file name \"%s\" is not properly formatted. "
            "DOS file names used with the %s should consist of from "
            "1 to 8 characters followed by a period then a 3 character "
            "extension. Spaces and some non-alphanumeric characters are "
            "not allowed.",
            szFileTitle,
            pszAppLongName );
            
    return;
  }

  //
  //  Make sure the file name has a GOB extension.
  //

  if ( _fstricmp( &szFileTitle[n-4], ".gob" ) )
  {
    nResult = MsgBox( hwndMDIClient,
                      MB_ICONEXCLAMATION | MB_YESNO,
                      "Graphical object files should be given a \"GOB\" "
                      "file extension. Do you want to save the file as "
                      "\"%s\" anyway?",
                      szFileTitle );

    if ( nResult != IDYES )
    {
      return;
    }
  }
  
  //
  //  Update the window data and window title with the new path and name.
  //
  
  _fstrcpy( pWndData->path, path );
  _fstrcpy( pWndData->name, szFileTitle );
  _fstrcpy( pWndData->file, szFile );

  GenWindowTitle( pWndData, szBuf, sizeof(szBuf) );
  SendMessage( hwnd, WM_SETTEXT, (WPARAM)0, (LPARAM)(LPSTR)szBuf );
  
  //
  //  Save the data.
  //
    
  Frame_FileSave( pWndData );
  
  return;

} // Frame_FileSaveAs


/*******************************************************************

    NAME:       Frame_FilePrint

    SYNOPSIS:   Print the contents of the active MDI child window.

********************************************************************/

VOID Frame_FilePrint( VOID )
{

#if 0

  HWND hwnd;
  BOOL bResult;
  RECT rc;
  DWORD dwResult;
  HCURSOR hcurSave;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  printDialog.hwndOwner = hwndMDIClient;
  printDialog.hInstance = hInst;
  printDialog.Flags = PD_RETURNDC;
  
  bResult = PrintDlg(&printDialog);
    
  if ( !bResult )
  {
    dwResult = CommDlgExtendedError();

    if (dwResult)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Print dialog error %lu",
              dwResult );
    }

    return;
  }

  hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

  Escape(printDialog.hDC, STARTDOC, 8, pWndData->name, NULL);

  rc.top = 100;                 // coordinates in 1/100 inch (MM_LOENGLISH)
  rc.bottom = 950;
  rc.left = 100;
  rc.right = 700;

  Object_DrawPage( printDialog.hDC,
                   &rc,
                   MM_LOENGLISH,
                   pWndData );

  Escape(printDialog.hDC, NEWFRAME, 0, NULL, NULL);

  Escape(printDialog.hDC, ENDDOC, 0, NULL, NULL);
    
  DeleteDC(printDialog.hDC);

  if (printDialog.hDevMode != NULL)
  {
    GlobalFree( printDialog.hDevMode );
    printDialog.hDevMode = NULL;
  }

  if (printDialog.hDevNames != NULL)
  {
    GlobalFree( printDialog.hDevNames );
    printDialog.hDevNames = NULL;
  }

  SetCursor(hcurSave);

#endif

  MsgBox( hwndMDIClient,
          MB_ICONEXCLAMATION,
          "Sorry, this function has not been implemented" );
      
  return;

} // Frame_FilePrint


/*******************************************************************

    NAME:       Frame_FilePrintSetup

    SYNOPSIS:   Print the contents of the active MDI child window.

********************************************************************/

VOID Frame_FilePrintSetup( VOID )
{
  BOOL bResult;
  DWORD dwResult;

  printDialog.Flags = PD_RETURNDC | PD_PRINTSETUP;
  bResult = PrintDlg((LPPRINTDLG)&printDialog);

  if ( bResult == 0 )
  {
    dwResult = CommDlgExtendedError();

    if (dwResult)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Print setup dialog error %lu",
              dwResult );
    }
  }
      
  return;

} // Frame_FilePrintSetup


/*******************************************************************

    NAME:       Frame_EditUndo

    SYNOPSIS:   Undo the last operation for the active MDI child window.

********************************************************************/

VOID Frame_EditUndo( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  if ( pWndData->pUndoData )
  {
    Object_EditUndo( hwnd, pWndData );
    Object_ViewChanged( hwnd, pWndData );
  }
      
  return;

} // Frame_EditUndo


/*******************************************************************

    NAME:       Frame_EditCut

    SYNOPSIS:   Copy the selected objects from the active MDI child
                window to the clipboard then delete them.

********************************************************************/

VOID Frame_EditCut( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_CreateUndoData( hwnd, pWndData );
  Object_EditCopy( hwnd, pWndData );
  DeleteSelections( pWndData->pLevelData + pWndData->level_index );
  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_EditCut


/*******************************************************************

    NAME:       Frame_EditCopy

    SYNOPSIS:   Copy the selected objects from the active MDI child
                window to the clipboard.

********************************************************************/

VOID Frame_EditCopy( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_EditCopy( hwnd, pWndData );
      
  return;

} // Frame_EditCopy


/*******************************************************************

    NAME:       Frame_EditPaste

    SYNOPSIS:   Paste the objects on the clipboard into the active
                MDI child window.

********************************************************************/

VOID Frame_EditPaste( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_CreateUndoData( hwnd, pWndData );
  ClearSelections( pWndData->pLevelData + pWndData->level_index );
  Object_EditPaste( hwnd, pWndData );
      
  return;

} // Frame_EditPaste


/*******************************************************************

    NAME:       Frame_EditDelete

    SYNOPSIS:   Delete the selected objects from the active MDI child
                window.

********************************************************************/

VOID Frame_EditDelete( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_CreateUndoData( hwnd, pWndData );
  DeleteSelections( pWndData->pLevelData + pWndData->level_index );
  Object_ViewChanged( hwnd, pWndData );
    
  return;

} // Frame_EditDelete


/*******************************************************************

    NAME:       Frame_EditHide

    SYNOPSIS:   Hide the selected objects in the active MDI child
                window.

********************************************************************/

VOID Frame_EditHide( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_CreateUndoData( hwnd, pWndData );
  HideSelections( pWndData->pLevelData + pWndData->level_index );
  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_EditHide


/*******************************************************************

    NAME:       Frame_EditHideAll

    SYNOPSIS:   Hide the all objects in the active MDI child window.

********************************************************************/

VOID Frame_EditHideAll( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_CreateUndoData( hwnd, pWndData );
  HideAll( pWndData->pLevelData + pWndData->level_index );
  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_EditHideAll


/*******************************************************************

    NAME:       Frame_EditShowAll

    SYNOPSIS:   Show all the objects in the active MDI child window
                (i.e. un-hide all objects).

********************************************************************/

VOID Frame_EditShowAll( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_CreateUndoData( hwnd, pWndData );
  ShowAll( pWndData->pLevelData + pWndData->level_index );
  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_EditShowAll


/*******************************************************************

    NAME:       Frame_EditClearSelections

    SYNOPSIS:   Clear the selection flag for all objects in the
                active MDI child window.

********************************************************************/

VOID Frame_EditClearSelections( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_CreateUndoData( hwnd, pWndData );
  ClearSelections( pWndData->pLevelData + pWndData->level_index );
  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_EditClearSelections


/*******************************************************************

    NAME:       Frame_EditVertexPosition

    SYNOPSIS:   Edit positions of all the selected vertices in
                the active MDI child window.

********************************************************************/

VOID Frame_EditVertexPosition( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  if ( EditVertexPositionDialog( hwnd, pWndData ) == IDOK )
  {
    Object_ResetPixelSize( hwnd, pWndData );
    Object_ViewChanged( hwnd, pWndData );
  }
    
  return;

} // Frame_EditVertexPosition


/*******************************************************************

    NAME:       Frame_EditVertexProperties

    SYNOPSIS:   Edit properties of all the selected vertices in
                the active MDI child window.

********************************************************************/

VOID Frame_EditVertexProperties( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  if ( EditVertexPropertiesDialog( hwnd, pWndData ) == IDOK )
  {
    Object_ViewChanged( hwnd, pWndData );
  }
    
  return;

} // Frame_EditVertexProperties


/*******************************************************************

    NAME:       Frame_EditPolygonProperties

    SYNOPSIS:   Assign properties for all the selected polygons
                in the active MDI child window. Only unlinked
                polygons will be affected.

********************************************************************/

VOID Frame_EditPolygonProperties( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  EditPolygonPropertiesDialog( hwnd, pWndData );

  Object_ResetPixelSize( hwnd, pWndData );
      
  return;

} // Frame_EditPolygonProperties


/*******************************************************************

    NAME:       Frame_EditGroupAssignments

    SYNOPSIS:   Assign the group for all the selected polygons and
                vertices in the active MDI child window.

********************************************************************/

VOID Frame_EditGroupAssignments( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  EditGroupAssignmentsDialog( hwnd, pWndData );

  Object_ResetPixelSize( hwnd, pWndData );
      
  return;

} // Frame_EditGroupAssignments


/*******************************************************************

    NAME:       Frame_ViewHome

    SYNOPSIS:   Change the view back to the home position for the
                active MDI child window.

********************************************************************/

VOID Frame_ViewHome( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  Object_ViewHome( hwnd, pWndData );
      
  return;

} // Frame_ViewHome


/*******************************************************************

    NAME:       Frame_ViewOptions

    SYNOPSIS:   Display the view options dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewOptions( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewOptionsDialog( hwnd, pWndData );

  Object_ViewStatusUpdate( hwnd, pWndData );
  Object_GridStatusUpdate( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewOptions


/*******************************************************************

    NAME:       Frame_ViewStatistics

    SYNOPSIS:   Display the "Statistics" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewStatistics( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewStatisticsDialog( hwnd, pWndData );
      
  return;

} // Frame_ViewStatistics


/*******************************************************************

    NAME:       Frame_ViewFileInfo

    SYNOPSIS:   Display the "File Info" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewFileInfo( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewFileInfoDialog( hwnd, pWndData );
      
  return;

} // Frame_ViewFileInfo


/*******************************************************************

    NAME:       Frame_ViewLevels

    SYNOPSIS:   Display the level dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewLevels( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewLevelsDialog( hwnd, pWndData );

  Object_ViewStatusUpdate( hwnd, pWndData );
  Object_GridStatusUpdate( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewLevels


/*******************************************************************

    NAME:       Frame_ViewBitmaps

    SYNOPSIS:   Display the "Bitmaps" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewBitmaps( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewBitmapsDialog( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewBitmaps


/*******************************************************************

    NAME:       Frame_ViewTextures

    SYNOPSIS:   Display the "Textures" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewTextures( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewTexturesDialog( hwnd, pWndData );

  Object_ResetPixelSize( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewTextures


/*******************************************************************

    NAME:       Frame_ViewSounds

    SYNOPSIS:   Display the "Sounds" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewSounds( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewSoundsDialog( hwnd, pWndData );
      
  return;

} // Frame_ViewSounds


/*******************************************************************

    NAME:       Frame_ViewAnimations

    SYNOPSIS:   Display the "Animations" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewAnimations( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewAnimationsDialog( hwnd, pWndData );

  Object_ResetPixelSize( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewAnimations


/*******************************************************************

    NAME:       Frame_ViewLifeForms

    SYNOPSIS:   Display the "Life Forms" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewLifeForms( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewLifeFormsDialog( hwnd, pWndData );

  Object_ResetPixelSize( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewLifeForms


/*******************************************************************

    NAME:       Frame_ViewItems

    SYNOPSIS:   Display the "Items" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewItems( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewItemsDialog( hwnd, pWndData );

  Object_ResetPixelSize( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewItems


/*******************************************************************

    NAME:       Frame_ViewSprites

    SYNOPSIS:   Display the "Sprites" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewSprites( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewSpritesDialog( hwnd, pWndData );
      
  return;

} // Frame_ViewSprites


/*******************************************************************

    NAME:       Frame_ViewNoises

    SYNOPSIS:   Display the "Noises" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewNoises( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewNoisesDialog( hwnd, pWndData );
      
  return;

} // Frame_ViewNoises


/*******************************************************************

    NAME:       Frame_ViewGroups

    SYNOPSIS:   Display the "Groups" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewGroups( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewGroupsDialog( hwnd, pWndData );

  return;

} // Frame_ViewGroups


/*******************************************************************

    NAME:       Frame_ViewTriggers

    SYNOPSIS:   Display the "Triggers" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewTriggers( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewTriggersDialog( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewTriggers


/*******************************************************************

    NAME:       Frame_ViewMotions

    SYNOPSIS:   Display the "Motions" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewMotions( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewMotionsDialog( hwnd, pWndData );

  Object_ViewChanged( hwnd, pWndData );
      
  return;

} // Frame_ViewMotions


/*******************************************************************

    NAME:       Frame_ViewMarks

    SYNOPSIS:   Display the "Marks" dialog for the active MDI
                child window.

********************************************************************/

VOID Frame_ViewMarks( VOID )
{
  HWND hwnd;
  WND_DATA *pWndData;

  hwnd = GetWindow( hwndMDIClient, GW_CHILD );

  if ( hwnd == NULL )
  {
    return;
  }
  
  pWndData = WNDPTR( hwnd );

  if ( pWndData == NULL )
  {
    return;
  }
  
  if ( !pWndData->loaded )
  {
    return;
  }

  ViewMarksDialog( hwnd, pWndData );
      
  return;

} // Frame_ViewMarks


/*******************************************************************

    NAME:       Frame_CreateWindowData

    SYNOPSIS:   Creates a new window data structure.

    ENTRY:      cid - If zero then assign the next cid to this
                  data structure, otherwise use the cid given.
                  A series of WND_DATA structures having the
                  same cid belong to the same GOB file and will
                  be updated togther.

    RETURNS:    Pointer to the new data structure or
                NULL if failed to create & initialize.

********************************************************************/

WND_DATA *Frame_CreateWindowData( WORD cid )
{
  DWORD dwSize;
  WND_DATA *pWndData;
  struct dosdate_t dos_date;
  
  //
  //  Create the window data.
  //

  pWndData = (WND_DATA *)GlobalAllocPtr(GPTR,(DWORD)sizeof(WND_DATA));

  if (pWndData == NULL)
  {
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  pWndData->pPalette = (PALETTE_DATA *)GlobalAllocPtr(GPTR,(DWORD)sizeof(PALETTE_DATA));

  if (pWndData->pPalette == NULL)
  {
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(BITMAP_DATA) * (DWORD)MAX_BITMAPS;

  pWndData->pBitmapData = (BITMAP_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pBitmapData == NULL)
  {
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(TEXTURE_DATA) * (DWORD)MAX_TEXTURES;

  pWndData->pTextureData = (TEXTURE_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pTextureData == NULL)
  {
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(SOUND_DATA) * (DWORD)MAX_SOUNDS;

  pWndData->pSoundData = (SOUND_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pSoundData == NULL)
  {
    GlobalFreePtr(pWndData->pTextureData);
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(ANIMATION_DATA) * (DWORD)MAX_ANIMATIONS;

  pWndData->pAnimationData = (ANIMATION_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pAnimationData == NULL)
  {
    GlobalFreePtr(pWndData->pSoundData);
    GlobalFreePtr(pWndData->pTextureData);
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(LIFEFORM_DATA) * (DWORD)MAX_LIFEFORMS;

  pWndData->pLifeFormData = (LIFEFORM_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pLifeFormData == NULL)
  {
    GlobalFreePtr(pWndData->pAnimationData);
    GlobalFreePtr(pWndData->pSoundData);
    GlobalFreePtr(pWndData->pTextureData);
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(ITEM_DATA) * (DWORD)MAX_ITEMS;

  pWndData->pItemData = (ITEM_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pItemData == NULL)
  {
    GlobalFreePtr(pWndData->pLifeFormData);
    GlobalFreePtr(pWndData->pAnimationData);
    GlobalFreePtr(pWndData->pSoundData);
    GlobalFreePtr(pWndData->pTextureData);
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(SPRITE_DATA) * (DWORD)MAX_SPRITES;

  pWndData->pSpriteData = (SPRITE_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pSpriteData == NULL)
  {
    GlobalFreePtr(pWndData->pItemData);
    GlobalFreePtr(pWndData->pLifeFormData);
    GlobalFreePtr(pWndData->pAnimationData);
    GlobalFreePtr(pWndData->pSoundData);
    GlobalFreePtr(pWndData->pTextureData);
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(NOISE_DATA) * (DWORD)MAX_NOISES;

  pWndData->pNoiseData = (NOISE_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pNoiseData == NULL)
  {
    GlobalFreePtr(pWndData->pSpriteData);
    GlobalFreePtr(pWndData->pItemData);
    GlobalFreePtr(pWndData->pLifeFormData);
    GlobalFreePtr(pWndData->pAnimationData);
    GlobalFreePtr(pWndData->pSoundData);
    GlobalFreePtr(pWndData->pTextureData);
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(LEVEL_DATA) * (DWORD)MAX_LEVELS;

  pWndData->pLevelData = (LEVEL_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pLevelData == NULL)
  {
    GlobalFreePtr(pWndData->pNoiseData);
    GlobalFreePtr(pWndData->pSpriteData);
    GlobalFreePtr(pWndData->pItemData);
    GlobalFreePtr(pWndData->pLifeFormData);
    GlobalFreePtr(pWndData->pAnimationData);
    GlobalFreePtr(pWndData->pSoundData);
    GlobalFreePtr(pWndData->pTextureData);
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  dwSize = (DWORD)sizeof(ZONE_DATA) * (DWORD)MAX_LEVELS;

  pWndData->pZoneData = (ZONE_DATA *)GlobalAllocPtr(GPTR,dwSize);

  if (pWndData->pZoneData == NULL)
  {
    GlobalFreePtr(pWndData->pLevelData);
    GlobalFreePtr(pWndData->pNoiseData);
    GlobalFreePtr(pWndData->pSpriteData);
    GlobalFreePtr(pWndData->pItemData);
    GlobalFreePtr(pWndData->pLifeFormData);
    GlobalFreePtr(pWndData->pAnimationData);
    GlobalFreePtr(pWndData->pSoundData);
    GlobalFreePtr(pWndData->pTextureData);
    GlobalFreePtr(pWndData->pBitmapData);
    GlobalFreePtr(pWndData->pPalette);
    GlobalFreePtr(pWndData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return NULL;
  }

  //
  //  Initialze various structure members.
  //
  
  if (cid)
  {
    pWndData->cid = cid;
  }
  else
  {
    pWndData->cid = ++wNextCid;
  }

  _fstrcpy(pWndData->file_version, szVersion);

  _dos_getdate(&dos_date);

  wsprintf( pWndData->file_date,
            "%02d/%02d/%02d",
            dos_date.month,
            dos_date.day,
            dos_date.year % 100 );

  pWndData->nLevels = 0;
  pWndData->level_index = 0;

  //
  //  Success!
  //

  return pWndData;

} // Frame_CreateWindowData


/*******************************************************************

    NAME:       Frame_NewLevel

    SYNOPSIS:   Allocates memory for a new level.

    ENTRY:      pWndData - The window data pointer.

                index - The index of the level to be initialized.

    RETURN:     TRUE on success, FALSE on error.

********************************************************************/

BOOL Frame_NewLevel( WND_DATA *pWndData, INT index )
{
  DWORD dwSize;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + index;

  dwSize = (DWORD)sizeof(GROUP) * (DWORD)MAX_GROUPS;

  pLevel->pGroupData = (GROUP *)GlobalAllocPtr(GPTR,dwSize);

  if (pLevel->pGroupData == NULL)
  {
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return FALSE;
  }

  dwSize = (DWORD)sizeof(TRIGGER) * (DWORD)MAX_TRIGGERS;

  pLevel->pTriggerData = (TRIGGER *)GlobalAllocPtr(GPTR,dwSize);

  if (pLevel->pTriggerData == NULL)
  {
    GlobalFreePtr(pLevel->pGroupData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return FALSE;
  }

  dwSize = (DWORD)sizeof(MOTION) * (DWORD)MAX_MOTIONS;

  pLevel->pMotionData = (MOTION *)GlobalAllocPtr(GPTR,dwSize);

  if (pLevel->pMotionData == NULL)
  {
    GlobalFreePtr(pLevel->pTriggerData);
    GlobalFreePtr(pLevel->pGroupData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return FALSE;
  }

  dwSize = (DWORD)sizeof(MARK) * (DWORD)MAX_MARKS;

  pLevel->pMarkData = (MARK *)GlobalAllocPtr(GPTR,dwSize);

  if (pLevel->pMarkData == NULL)
  {
    GlobalFreePtr(pLevel->pMotionData);
    GlobalFreePtr(pLevel->pTriggerData);
    GlobalFreePtr(pLevel->pGroupData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return FALSE;
  }
  
  dwSize = (DWORD)sizeof(VERTEX) * (DWORD)MAX_NUM_VERTICES;

  pLevel->pVertexData = (VERTEX huge *)GlobalAllocPtr(GPTR,dwSize);

  if (pLevel->pVertexData == NULL)
  {
    GlobalFreePtr(pLevel->pMarkData);
    GlobalFreePtr(pLevel->pMotionData);
    GlobalFreePtr(pLevel->pTriggerData);
    GlobalFreePtr(pLevel->pGroupData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return FALSE;
  }
  
  dwSize = (DWORD)sizeof(POLYGON) * (DWORD)MAX_NUM_POLYGONS;

  pLevel->pPolygonData = (POLYGON huge *)GlobalAllocPtr(GPTR,dwSize);

  if (pLevel->pPolygonData == NULL)
  {
    GlobalFreePtr(pLevel->pVertexData);
    GlobalFreePtr(pLevel->pMarkData);
    GlobalFreePtr(pLevel->pMotionData);
    GlobalFreePtr(pLevel->pTriggerData);
    GlobalFreePtr(pLevel->pGroupData);
    
    MsgBox( NULL,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return FALSE;
  }

  _fstrcpy(pLevel->level_name,"Untitled");

  _fmemset(pWndData->pZoneData + index, 0, sizeof(ZONE_DATA));

  pLevel->view_depth = 200L;
  pLevel->view_width = 200L;
  pLevel->view_height = 125L;

#if 1
  pLevel->view_home_rotation = PI / 2.0; // looking north (toward +y)
  pLevel->view_home_elevation = PI / 2.0; // looking straight down
  pLevel->view_home_x = 0L;             // +x is east, -x is west
  pLevel->view_home_y = 0L;             // +y is north, -y is south
  pLevel->view_home_z = 50000L;         // 50 meters up
#else
  pLevel->view_home_rotation = 0.0;     // looking east (toward +x)
  pLevel->view_home_elevation = 0.0;    // looking east
  pLevel->view_home_x = 0L;             // +x is east, -x is west
  pLevel->view_home_y = 0L;             // +y is north, -y is south
  pLevel->view_home_z = 10000L;         // 10 meters up
#endif

  pLevel->view_rotation = pLevel->view_home_rotation;
  pLevel->view_elevation = pLevel->view_home_elevation;
  pLevel->view_x = pLevel->view_home_x;
  pLevel->view_y = pLevel->view_home_y;
  pLevel->view_z = pLevel->view_home_z;
  pLevel->view_delta = 500L;

  pLevel->grid_x = 0L;
  pLevel->grid_y = 0L;
  pLevel->grid_z = 0L;
  pLevel->grid_rotation = 0.0;
  pLevel->grid_elevation = 0.0;
  pLevel->grid_delta = 500L;

  pLevel->grid_spacing = 1000L;         // 1 meter spacing
  pLevel->grid_size = 100000L;          // 100 meter x 100 meter grid
  pLevel->bMaintainGridDist = FALSE;
  pLevel->bLockViewToGrid = FALSE;
  pLevel->bSnapToGrid = TRUE;

  return TRUE;

} // Frame_NewLevel


/*******************************************************************

    NAME:       Frame_FreeLevel

    SYNOPSIS:   Frees all the memory assoicated with the level
                index given.

    ENTRY:      pWndData - The window data pointer.

                index - The index of the level to be deallocated.

********************************************************************/

VOID Frame_FreeLevel( WND_DATA *pWndData, INT index )
{
  if (pWndData->pZoneData[index].data)
  {
    GlobalFreePtr(pWndData->pZoneData[index].data);
    pWndData->pZoneData[index].data = NULL;
    pWndData->pZoneData[index].size = 0;
  }

  GlobalFreePtr(pWndData->pLevelData[index].pPolygonData);
  GlobalFreePtr(pWndData->pLevelData[index].pVertexData);
  GlobalFreePtr(pWndData->pLevelData[index].pMarkData);
  GlobalFreePtr(pWndData->pLevelData[index].pMotionData);
  GlobalFreePtr(pWndData->pLevelData[index].pTriggerData);
  GlobalFreePtr(pWndData->pLevelData[index].pGroupData);

  return;

} // Frame_FreeLevel


/*******************************************************************

    NAME:       Frame_FreeWindowData

    SYNOPSIS:   Frees all the memory assoicated with the window data
                pointer given.

    ENTRY:      pWndData - The window data pointer.

********************************************************************/

VOID Frame_FreeWindowData( WND_DATA *pWndData )
{
  int i;

  if (pWndData->pUndoData)
  {
    GlobalFreePtr(pWndData->pUndoData);
  }

  if (pWndData->pDragData)
  {
    GlobalFreePtr(pWndData->pDragData);
  }

  if (pWndData->hbmView)
  {
    DeleteObject(pWndData->hbmView);
  }

  for(i = pWndData->nBitmaps - 1; i >= 0; --i)
  {
    if (pWndData->pBitmapData[i].data)
    {
      GlobalFreePtr(pWndData->pBitmapData[i].data);
    }
  }

  for(i = pWndData->nSounds - 1; i >= 0; --i)
  {
    if (pWndData->pSoundData[i].data)
    {
      GlobalFreePtr(pWndData->pSoundData[i].data);
    }
  }

  for(i = pWndData->nLevels - 1; i >= 0; --i)
  {
    Frame_FreeLevel(pWndData, i);
  }
    
  GlobalFreePtr(pWndData->pZoneData);
  GlobalFreePtr(pWndData->pLevelData);
  GlobalFreePtr(pWndData->pNoiseData);
  GlobalFreePtr(pWndData->pSpriteData);
  GlobalFreePtr(pWndData->pItemData);
  GlobalFreePtr(pWndData->pLifeFormData);
  GlobalFreePtr(pWndData->pAnimationData);
  GlobalFreePtr(pWndData->pSoundData);
  GlobalFreePtr(pWndData->pTextureData);
  GlobalFreePtr(pWndData->pBitmapData);
  GlobalFreePtr(pWndData->pPalette);
  GlobalFreePtr(pWndData);

  return;

} // Frame_FreeWindowData
