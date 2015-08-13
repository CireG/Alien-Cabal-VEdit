/*******************************************************************

    wnd.c

    Generic routines for managing various windows.

********************************************************************/


#include "vedit.h"


//
//  Public functions.
//

/*******************************************************************

    NAME:       Wnd_CreateNew

    SYNOPSIS:   Creates a new MDI child window.

    ENTRY:      pWndData - Pointer to the WND_DATA structure
                  for this new window.

                pszWndClass - Pointer to the windows class string.
                
    RETURNS:    HWND - Handle to the new child window.

********************************************************************/

HWND Wnd_CreateNew( WND_DATA *pWndData, LPSTR pszWndClass )
{
  HWND hwnd;
  CHAR szBuf[128];
  MDICREATESTRUCT mcs;

  GenWindowTitle( pWndData, szBuf, sizeof(szBuf) );
  
  mcs.szClass = pszWndClass;
  mcs.szTitle = (LPSTR)szBuf;
  mcs.hOwner  = hInst;
  mcs.x       = CW_USEDEFAULT;
  mcs.y       = CW_USEDEFAULT;
  mcs.cx      = CW_USEDEFAULT;
  mcs.cy      = CW_USEDEFAULT;
  mcs.style   = WS_MAXIMIZE;
//mcs.style   = WS_VSCROLL | WS_HSCROLL;

  hwnd = FORWARD_WM_MDICREATE( hwndMDIClient,
                               (LPMDICREATESTRUCT)&mcs,
                               SendMessage );

  if ( hwnd == NULL )
  {
      MsgBox( hwndMDIClient,
              MB_ICONSTOP | MB_OK,
              "Unable to create MDI child window" );

      return NULL;
  }

  SetWindowLong( hwnd, GWL_DATA_PTR, (LONG)pWndData );

  //
  //  Success!
  //

  ShowWindow( hwnd, SW_SHOW );

  return hwnd;

} // Wnd_CreateNew


/*******************************************************************

    NAME:       Wnd_OnClose

    SYNOPSIS:   Handles WM_CLOSE message.

    ENTRY:      hwnd - Window handle.

********************************************************************/

VOID Wnd_OnClose( HWND hwnd )
{
  if ( Wnd_OnQueryEndSession( hwnd ) )
  {
    FORWARD_WM_CLOSE( hwnd, DefMDIChildProc );
  }

} // Wnd_OnClose


/*******************************************************************

    NAME:       Wnd_OnDestroy

    SYNOPSIS:   Handles WM_DESTROY message.

    ENTRY:      hwnd - Window handle.

********************************************************************/

VOID Wnd_OnDestroy( HWND hwnd )
{
  WND_DATA *pWndData;

  pWndData = WNDPTR( hwnd );

  if ( pWndData != NULL )
  {
    //
    //  Free the windows attached data.
    //

    GlobalFreePtr( pWndData );
    SetWindowLong( hwnd, GWL_DATA_PTR, 0L );
  }

} // Wnd_OnDestroy


/*******************************************************************

    NAME:       Wnd_OnQueryEndSession

    SYNOPSIS:   Handles WM_QUERYENDSESSION message.

    ENTRY:      hwnd - Window handle.

    RETURN:     TRUE if ok to close the window.
                FALSE if not.
                
********************************************************************/

BOOL Wnd_OnQueryEndSession( HWND hwnd )
{
  INT nResult;
  WND_DATA *pWndData;

  pWndData = WNDPTR( hwnd );

  if ( pWndData != NULL )
  {
    //
    //  If the data has changed then ask the user to abort.
    //

    if ( pWndData->loaded )
    {
      if ( pWndData->need_to_save )
      {
        nResult = MsgBox( hwndMDIClient,
                          MB_YESNOCANCEL | MB_ICONQUESTION,
                          "Save changes to %s ?",
                          pWndData->name );
      
        if ( nResult == IDCANCEL )
        {
          return FALSE;         // Don't close
        }
      
        if ( nResult == IDYES )
        {
          Frame_FileSave( pWndData );
        }
      }
    }
  }

  return TRUE;

} // Wnd_OnQueryEndSession
