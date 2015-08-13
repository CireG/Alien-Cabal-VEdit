/*******************************************************************

    dialogs.c

    Routines for displaying and managing the various dialogs used
    by the VEDIT project.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export About_DlgProc( HWND   hwnd,
                                      UINT   nMessage,
                                      WPARAM wParam,
                                      LPARAM lParam );

VOID About_OnCommand( HWND hwnd,
                      INT  id,
                      HWND hwndCtl,
                      UINT codeNotify );


//
//  Private data.
//

static INT nBrushDlg;
static HBRUSH hBrushDlg;


//
//  Public functions.
//

/*******************************************************************

    NAME:       AboutDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

VOID AboutDialog( HWND hwndParent )
{
  FARPROC pfnProc;

  pfnProc = MakeProcInstance( (FARPROC)About_DlgProc, hInst );

  DialogBox( hInst,
             IDD_ABOUT,
             hwndParent,
             (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

} // AboutDialog


/*******************************************************************

    NAME:       AllDialogs_OnInitDialog

    SYNOPSIS:   Handles the portion of the WM_INITDIALOG message
                processing common to all dialogs.

    ENTRY:      hwnd - The dialog box window handle. 
    
                hwndFocus - The control which will receive focus by
                  default (if we return TRUE).
                
                lParam - The dwData sent when creating the dialog box
                  if created with one of the ...Param() functions.

    RETURNS:    BOOL - Return TRUE to let the hwndFocus control have
                  focus. Return FALSE if we are setting it ourselves.

********************************************************************/

BOOL AllDialogs_OnInitDialog( HWND hwnd,
                              HWND hwndFocus,
                              LPARAM lParam )
{
  ++nBrushDlg;

  if ( hBrushDlg )
  {
    return TRUE;
  }
  
  hBrushDlg = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
   
  return TRUE;

} // AllDialogs_OnInitDialog


/*******************************************************************

    NAME:       AllDialogs_OnCtlColor

    SYNOPSIS:   Handles the portion of the WM_CTLCOLOR message
                processing common to all dialogs.

    ENTRY:      hwnd - The dialog box window handle. 
    
                hdc - The child-window display context.
                
                hwndChild - The handle of child window.
                
                type - The type of control.

    RETURNS:    HBRUSH - a handle to the brush that is to be used
                for painting the control background or NULL.

********************************************************************/

HBRUSH AllDialogs_OnCtlColor( HWND hwnd,
                              HDC  hdc,
                              HWND hwndChild,
                              INT  type)
{
  if ( hBrushDlg )
  {
    if ( type == CTLCOLOR_DLG )         // dialog background
    {
      UnrealizeObject( hBrushDlg );
      SetBrushOrg( hdc, 0 , 0 );
      SelectObject( hdc, hBrushDlg );
      return hBrushDlg;
    }
    else if ( type == CTLCOLOR_STATIC ) // static text controls
    {
      SetBkMode( hdc, TRANSPARENT );
      SelectObject( hdc, hBrushDlg );
      return hBrushDlg;
    }
    else if ( type == CTLCOLOR_BTN )    // group boxes (not buttons?)
    {
      SetBkMode( hdc, TRANSPARENT );
      SelectObject( hdc, hBrushDlg );
      return hBrushDlg;
    }
  }
  
  return NULL;
  
} // AllDialogs_OnCtlColor


/*******************************************************************

    NAME:       AllDialogs_OnDestroy

    SYNOPSIS:   Handles the portion of the WM_DESTROY message
                processing common to all dialogs.

    ENTRY:      hwnd - Window handle.

********************************************************************/

VOID AllDialogs_OnDestroy( HWND hwnd )
{
  if ( nBrushDlg > 0 )
  {
    --nBrushDlg;
  }

  if ( hBrushDlg && nBrushDlg == 0 )
  {
    DeleteObject( hBrushDlg );
    hBrushDlg = NULL;
  }
  
} // AllDialogs_OnDestroy


//
//  Private functions.
//

/*******************************************************************

    NAME:       About_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export About_DlgProc( HWND   hwnd,
                                      UINT   nMessage,
                                      WPARAM wParam,
                                      LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    About_OnCommand         );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, AllDialogs_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor   );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy    );
  }

  return FALSE;

} // About_DlgProc


/*******************************************************************

    NAME:       About_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/

VOID About_OnCommand( HWND hwnd,
                      INT  id,
                      HWND hwndCtl,
                      UINT codeNotify )
{
  if ( ( id == IDOK ) || ( id == IDCANCEL ) )
  {
    EndDialog( hwnd, TRUE );
  }

} // About_OnCommand
