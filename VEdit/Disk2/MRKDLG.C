/*******************************************************************

    mrkdlg.c

    Routines for displaying and managing the marks dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewMarks_DlgProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam );

VOID ViewMarks_OnCommand( HWND hwnd,
                          INT  id,
                          HWND hwndCtrl,
                          UINT codeNotify );

BOOL ViewMarks_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

BOOL CALLBACK __export EditMark_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam );

VOID EditMark_OnCommand( HWND hwnd,
                         INT  id,
                         HWND hwndCtrl,
                         UINT codeNotify );

BOOL EditMark_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static LEVEL_DATA *pLevel;
static HWND hParent;
static char szMarkName[32];
static char szMarkX[12];
static char szMarkY[12];
static char szMarkZ[12];
static char szMarkRotation[12];
static char szMarkElevation[12];


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewMarksDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewMarksDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;
  po = pWndData;
  pLevel = pWndData->pLevelData + pWndData->level_index;
  hParent = hwndParent;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewMarks_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_MARKS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewMarksDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       EditMarkDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int EditMarkDialog( HWND hwndParent )
{
  int nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance( (FARPROC)EditMark_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_EDIT_MARK,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditMarkDialog


/*******************************************************************

    NAME:       EditMark_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditMark_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditMark_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditMark_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy  );
  }

  return FALSE;

} // EditMark_DlgProc


/*******************************************************************

    NAME:       EditMark_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditMark_OnCommand( HWND hwnd,
                         INT  id,
                         HWND hwndCtrl,
                         UINT codeNotify )
{
  switch(id)
  {
    case IDC_MARK_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szMarkName),
                     (LPARAM)((LPSTR)szMarkName) );
      }
      break;
      
    case IDC_MARK_X:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szMarkX),
                     (LPARAM)((LPSTR)szMarkX) );
      }
      break;
      
    case IDC_MARK_Y:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szMarkY),
                     (LPARAM)((LPSTR)szMarkY) );
      }
      break;
      
    case IDC_MARK_Z:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szMarkZ),
                     (LPARAM)((LPSTR)szMarkZ) );
      }
      break;
      
    case IDC_MARK_ROTATION:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szMarkRotation),
                     (LPARAM)((LPSTR)szMarkRotation) );
      }
      break;
      
    case IDC_MARK_ELEVATION:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szMarkElevation),
                     (LPARAM)((LPSTR)szMarkElevation) );
      }
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // EditMark_OnCommand


/*******************************************************************

    NAME:       EditMark_OnInitDialog

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

BOOL EditMark_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_MARK_NAME );
  Edit_SetText( hwndCtrl, szMarkName );
  Edit_LimitText( hwndCtrl, 30 );

  hwndCtrl = GetDlgItem( hwnd, IDC_MARK_X );
  Edit_SetText( hwndCtrl, szMarkX );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_MARK_Y );
  Edit_SetText( hwndCtrl, szMarkY );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_MARK_Z );
  Edit_SetText( hwndCtrl, szMarkZ );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_MARK_ROTATION );
  Edit_SetText( hwndCtrl, szMarkRotation );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_MARK_ELEVATION );
  Edit_SetText( hwndCtrl, szMarkElevation );
  Edit_LimitText( hwndCtrl, 8 );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // EditMark_OnInitDialog


/*******************************************************************

    NAME:       ViewMarks_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewMarks_DlgProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,     ViewMarks_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG,  ViewMarks_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,    AllDialogs_OnCtlColor  );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,     AllDialogs_OnDestroy   );
  }

  return FALSE;

} // ViewMarks_DlgProc


/*******************************************************************

    NAME:       ViewMarks_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewMarks_OnCommand( HWND hwnd,
                          INT  id,
                          HWND hwndCtrl,
                          UINT codeNotify )
{
  INT i, j;
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_MARK_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        if ( lResult >= 0 && lResult < pLevel->nMarks )
        {
          pLevel->nLastMark = (INT)lResult;
        }
      }
      break;

    case IDC_NEW_MARK:

      i = pLevel->nMarks;

      if (i == MAX_MARKS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d marks has been reached.",
                MAX_MARKS );

        break;
      }

      szMarkName[0] = '\0';
      wsprintf(szMarkX, "%ld", pLevel->view_x);
      wsprintf(szMarkY, "%ld", pLevel->view_y);
      wsprintf(szMarkZ, "%ld", pLevel->view_z);
      DoubleToString(szMarkRotation, pLevel->view_rotation * 180.0 / PI, 2);
      DoubleToString(szMarkElevation, pLevel->view_elevation * 180.0 / PI, 2);

      if (EditMarkDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      if (!szMarkName[0])
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_MARK_LIST);

      ListBox_AddString(hwndList, szMarkName);

      _fstrcpy(pLevel->pMarkData[i].mark_name, szMarkName);
      pLevel->pMarkData[i].x = atol(szMarkX);
      pLevel->pMarkData[i].y = atol(szMarkY);
      pLevel->pMarkData[i].z = atol(szMarkZ);
      pLevel->pMarkData[i].rotation = atof(szMarkRotation) * PI / 180.0;
      pLevel->pMarkData[i].elevation = atof(szMarkElevation) * PI / 180.0;

      ++pLevel->nMarks;

      ListBox_SetCurSel(hwndList, i);
      pLevel->nLastMark = i;

      bChange = TRUE;

      break;

    case IDC_DELETE_MARK:

      hwndList = GetDlgItem( hwnd, IDC_MARK_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nMarks )
      {
        i = (INT)lResult;
        DeleteMark( pLevel, i );
        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        bChange = TRUE;
      }

      break;

    case IDC_EDIT_MARK:

      hwndList = GetDlgItem( hwnd, IDC_MARK_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nMarks )
      {
        i = (INT)lResult;

        _fstrcpy(szMarkName, pLevel->pMarkData[i].mark_name);
        wsprintf(szMarkX, "%ld", pLevel->pMarkData[i].x);
        wsprintf(szMarkY, "%ld", pLevel->pMarkData[i].y);
        wsprintf(szMarkZ, "%ld", pLevel->pMarkData[i].z);
        DoubleToString(szMarkRotation, pLevel->pMarkData[i].rotation * 180.0 / PI, 2);
        DoubleToString(szMarkElevation, pLevel->pMarkData[i].elevation * 180.0 / PI, 2);

        if ( EditMarkDialog( hwnd ) == IDCANCEL )
        {
          break;
        }

        if ( !szMarkName[0] )
        {
          break;
        }

        _fstrcpy( pLevel->pMarkData[i].mark_name, szMarkName );
        pLevel->pMarkData[i].x = atol(szMarkX);
        pLevel->pMarkData[i].y = atol(szMarkY);
        pLevel->pMarkData[i].z = atol(szMarkZ);
        pLevel->pMarkData[i].rotation = atof(szMarkRotation) * PI / 180.0;
        pLevel->pMarkData[i].elevation = atof(szMarkElevation) * PI / 180.0;

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < pLevel->nMarks; ++i )
        {
          ListBox_AddString( hwndList, pLevel->pMarkData[i].mark_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

        bChange = TRUE;
      }

      break;

    case IDC_GO_TO_MARK:

      hwndList = GetDlgItem( hwnd, IDC_MARK_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nMarks )
      {
        i = (INT)lResult;
        pLevel->view_x = pLevel->pMarkData[i].x;
        pLevel->view_y = pLevel->pMarkData[i].y;
        pLevel->view_z = pLevel->pMarkData[i].z;
        pLevel->view_rotation = pLevel->pMarkData[i].rotation;
        pLevel->view_elevation = pLevel->pMarkData[i].elevation;
        Object_ViewStatusUpdate(hParent, po);
        Object_ViewChanged(hParent, po);
        bChange = TRUE;
      }

      break;
      
    case IDOK: case IDCANCEL:

      if ( hPalCommon )
      {
        DeleteObject( hPalCommon );
        hPalCommon = NULL;
      }

      EndDialog( hwnd, id );
      break;
  }
  
} // ViewMarks_OnCommand


/*******************************************************************

    NAME:       ViewMarks_OnInitDialog

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

BOOL ViewMarks_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_MARK_LIST );

  for( i = 0; i < pLevel->nMarks; ++i )
  {
    ListBox_AddString( hwndCtrl, pLevel->pMarkData[i].mark_name );
  }

  if (pLevel->nLastMark < 0 || pLevel->nLastMark >= pLevel->nMarks)
  {
    pLevel->nLastMark = -1;
  }

  if (pLevel->nMarks > 0)
  {
    ListBox_SetCurSel( hwndCtrl, pLevel->nLastMark );
  }

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewMarks_OnInitDialog
