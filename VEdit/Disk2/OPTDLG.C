/*******************************************************************

    optdlg.c

    Routines for displaying and managing the options dialogs.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewOptions_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam );

VOID ViewOptions_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtl,
                            UINT codeNotify );

BOOL ViewOptions_OnInitDialog( HWND   hwndDlg,
                               HWND   hwndFocus,
                               LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static LEVEL_DATA *pl;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewOptionsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewOptionsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  LEVEL_DATA *pLevel;
  FARPROC pfnProc;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pl = (LEVEL_DATA *)GlobalAllocPtr( GPTR, sizeof(LEVEL_DATA) );
  
  if ( pl )
  {
    pl->view_home_rotation = pLevel->view_home_rotation;
    pl->view_home_elevation = pLevel->view_home_elevation;
    pl->view_home_x = pLevel->view_home_x;
    pl->view_home_y = pLevel->view_home_y;
    pl->view_home_z = pLevel->view_home_z;
    pl->view_rotation = pLevel->view_rotation;
    pl->view_elevation = pLevel->view_elevation;
    pl->view_x = pLevel->view_x;
    pl->view_y = pLevel->view_y;
    pl->view_z = pLevel->view_z;
    pl->view_delta = pLevel->view_delta;
    pl->grid_x = pLevel->grid_x;
    pl->grid_y = pLevel->grid_y;
    pl->grid_z = pLevel->grid_z;
    pl->grid_rotation = pLevel->grid_rotation;
    pl->grid_elevation = pLevel->grid_elevation;
    pl->grid_delta = pLevel->grid_delta;
    pl->grid_spacing = pLevel->grid_spacing;
    pl->grid_size = pLevel->grid_size;
    pl->bMaintainGridDist = pLevel->bMaintainGridDist;
    pl->bLockViewToGrid = pLevel->bLockViewToGrid;
    pl->bSnapToGrid = pLevel->bSnapToGrid;

    pfnProc = MakeProcInstance( (FARPROC)ViewOptions_DlgProc, hInst );

    nResult = DialogBox( hInst,
                         IDD_VIEW_OPTIONS,
                         hwndParent,
                         (DLGPROC)pfnProc );

    FreeProcInstance( pfnProc );
    
    if ( nResult == IDOK )
    {
      pLevel->view_home_rotation = pl->view_home_rotation;
      pLevel->view_home_elevation = pl->view_home_elevation;
      pLevel->view_home_x = pl->view_home_x;
      pLevel->view_home_y = pl->view_home_y;
      pLevel->view_home_z = pl->view_home_z;
      pLevel->view_rotation = pl->view_rotation;
      pLevel->view_elevation = pl->view_elevation;
      pLevel->view_x = pl->view_x;
      pLevel->view_y = pl->view_y;
      pLevel->view_z = pl->view_z;
      pLevel->view_delta = pl->view_delta;
      pLevel->grid_x = pl->grid_x;
      pLevel->grid_y = pl->grid_y;
      pLevel->grid_z = pl->grid_z;
      pLevel->grid_rotation = pl->grid_rotation;
      pLevel->grid_elevation = pl->grid_elevation;
      pLevel->grid_delta = pl->grid_delta;
      pLevel->bMaintainGridDist = pl->bMaintainGridDist;
      pLevel->bLockViewToGrid = pl->bLockViewToGrid;
      pLevel->bSnapToGrid = pl->bSnapToGrid;
      pLevel->grid_spacing = pl->grid_spacing;
      pLevel->grid_size = pl->grid_size;
    }

    GlobalFreePtr( pl );
  }
  else
  {
    nResult = IDCANCEL;
  }
    
  return nResult;
    
} // ViewOptionsDialog


/*******************************************************************

    NAME:       ViewOptions_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewOptions_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewOptions_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewOptions_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor    );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy     );
  }

  return FALSE;

} // ViewOptions_DlgProc


/*******************************************************************

    NAME:       ViewOptions_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewOptions_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtl,
                            UINT codeNotify )
{
  int i;
  char szBuf[128];
  
  switch(id)
  {
    case IDC_VIEW_X:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_x = atol( szBuf );
      }
      break;

    case IDC_VIEW_Y:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_y = atol( szBuf );
      }
      break;

    case IDC_VIEW_Z:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_z = atol( szBuf );
      }
      break;

    case IDC_VIEW_ROTATION:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_rotation = atof( szBuf ) * PI / 180.0;
      }
      break;

    case IDC_VIEW_ELEVATION:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_elevation = atof( szBuf ) * PI / 180.0;
      }
      break;

    case IDC_VIEW_HOME_X:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_home_x = atol( szBuf );
      }
      break;

    case IDC_VIEW_HOME_Y:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_home_y = atol( szBuf );
      }
      break;

    case IDC_VIEW_HOME_Z:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_home_z = atol( szBuf );
      }
      break;

    case IDC_VIEW_HOME_ROTATION:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_home_rotation = atof( szBuf ) * PI / 180.0;
      }
      break;

    case IDC_VIEW_HOME_ELEVATION:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_home_elevation = atof( szBuf ) * PI / 180.0;
      }
      break;
      
    case IDC_VIEW_DELTA:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->view_delta = atol( szBuf );
      }
      break;

    case IDC_GRID_X:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->grid_x = atol( szBuf );
      }
      break;

    case IDC_GRID_Y:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->grid_y = atol( szBuf );
      }
      break;

    case IDC_GRID_Z:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->grid_z = atol( szBuf );
      }
      break;

    case IDC_GRID_ROTATION:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->grid_rotation = atof( szBuf ) * PI / 180.0;
      }
      break;

    case IDC_GRID_ELEVATION:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->grid_elevation = atof( szBuf ) * PI / 180.0;
      }
      break;
      
    case IDC_GRID_DELTA:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->grid_delta = atol( szBuf );
      }
      break;
      
    case IDC_GRID_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->grid_size = atol( szBuf );
      }
      break;
      
    case IDC_GRID_SPACING:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        pl->grid_spacing = atol( szBuf );
      }
      break;
      
    case IDC_MAINTAIN_GRID_DIST:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        pl->bMaintainGridDist = i & 1;
      }
      break;
      
    case IDC_LOCK_VIEW_TO_GRID:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        pl->bLockViewToGrid = i & 1;
      }
      break;
      
    case IDC_SNAP_TO_GRID:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        pl->bSnapToGrid = i & 1;
      }
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // ViewOptions_OnCommand


/*******************************************************************

    NAME:       ViewOptions_OnInitDialog

    SYNOPSIS:   Handles the WM_INITDIALOG message. When the dialog
                box is created we need to fill the list box with
                choices.

    ENTRY:      hwnd - The dialog box window handle. 
    
                hwndFocus - The control which will receive focus by
                  default (if we return TRUE).
                
                lParam - The dwData sent when creating the dialog box
                  if created with one of the ...Param() functions.

    RETURNS:    BOOL - Return TRUE to let the hwndFocus control have
                  focus. Return FALSE if we are setting it ourselves.

********************************************************************/

BOOL ViewOptions_OnInitDialog( HWND   hwnd,
                               HWND   hwndFocus,
                               LPARAM lParam )
{
  HWND hwndCtrl;
  char szBuf[128];

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_X );
  wsprintf( szBuf, "%ld", pl->view_x );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_Y );
  wsprintf( szBuf, "%ld", pl->view_y );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_Z );
  wsprintf( szBuf, "%ld", pl->view_z );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_ROTATION );
  DoubleToString( szBuf, pl->view_rotation * 180.0 / PI, 1 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_ELEVATION );
  DoubleToString( szBuf, pl->view_elevation * 180.0 / PI, 1 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_HOME_X );
  wsprintf( szBuf, "%ld", pl->view_home_x );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_HOME_Y );
  wsprintf( szBuf, "%ld", pl->view_home_y );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_HOME_Z );
  wsprintf( szBuf, "%ld", pl->view_home_z );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_HOME_ROTATION );
  DoubleToString( szBuf, pl->view_home_rotation * 180.0 / PI, 1 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_HOME_ELEVATION );
  DoubleToString( szBuf, pl->view_home_elevation * 180.0 / PI, 1 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_VIEW_DELTA );
  wsprintf( szBuf, "%ld", pl->view_delta );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_GRID_X );
  wsprintf( szBuf, "%ld", pl->grid_x );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_GRID_Y );
  wsprintf( szBuf, "%ld", pl->grid_y );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_GRID_Z );
  wsprintf( szBuf, "%ld", pl->grid_z );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_GRID_ROTATION );
  DoubleToString( szBuf, pl->grid_rotation * 180.0 / PI, 1 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_GRID_ELEVATION );
  DoubleToString( szBuf, pl->grid_elevation * 180.0 / PI, 1 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_GRID_DELTA );
  wsprintf( szBuf, "%ld", pl->grid_delta );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_GRID_SIZE );
  wsprintf( szBuf, "%ld", pl->grid_size );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_GRID_SPACING );
  wsprintf( szBuf, "%ld", pl->grid_spacing );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  CheckDlgButton( hwnd, IDC_MAINTAIN_GRID_DIST, pl->bMaintainGridDist );

  CheckDlgButton( hwnd, IDC_LOCK_VIEW_TO_GRID, pl->bLockViewToGrid );

  CheckDlgButton( hwnd, IDC_SNAP_TO_GRID, pl->bSnapToGrid );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewOptions_OnInitDialog
