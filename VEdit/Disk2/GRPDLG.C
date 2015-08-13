/*******************************************************************

    grpdlg.c

    Routines for displaying and managing the groups dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewGroups_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam );

VOID ViewGroups_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify );

BOOL ViewGroups_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

BOOL CALLBACK __export NewGroup_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam );

VOID NewGroup_OnCommand( HWND hwnd,
                         INT  id,
                         HWND hwndCtrl,
                         UINT codeNotify );

BOOL NewGroup_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

BOOL CALLBACK __export RenameGroup_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam );

VOID RenameGroup_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify );

BOOL RenameGroup_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

BOOL CALLBACK __export EditGroupAssignments_DlgProc( HWND   hwnd,
                                                     UINT   nMessage,
                                                     WPARAM wParam,
                                                     LPARAM lParam );

VOID EditGroupAssignments_OnCommand( HWND hwnd,
                                     INT  id,
                                     HWND hwndCtrl,
                                     UINT codeNotify );

BOOL EditGroupAssignments_OnInitDialog( HWND hwnd,
                                        HWND hwndFocus,
                                        LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static LEVEL_DATA *pLevel;
static HWND hParent;
static CHAR szNewGroupName[32];
static INT nNewGroupIndex;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewGroupsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewGroupsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;
  po = pWndData;
  pLevel = pWndData->pLevelData + pWndData->level_index;
  hParent = hwndParent;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewGroups_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_GROUPS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewGroupsDialog


/*******************************************************************

    NAME:       EditGroupAssignmentsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int EditGroupAssignmentsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  INT i, nResult;
  POLYGON huge *pPolygon;
  VERTEX huge *pVertex;
  FARPROC pfnProc;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  bChange = FALSE;

  nResult = 0;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    pPolygon = pLevel->pPolygonData + i;

    if (pPolygon->flags & PF_SELECTED)
    {
      if (nResult == 0)
      {
        ++nResult;
        nNewGroupIndex = pPolygon->group_index;
      }
      else if (pPolygon->group_index != nNewGroupIndex)
      {
        ++nResult;
      }
    }
  }

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    pVertex = pLevel->pVertexData + i;

    if (pVertex->flags & VF_SELECTED)
    {
      if (nResult == 0)
      {
        ++nResult;
        nNewGroupIndex = pVertex->group_index;
      }
      else if (pVertex->group_index != nNewGroupIndex)
      {
        ++nResult;
      }
    }
  }

  if (nResult != 1)
  {
    nNewGroupIndex = -1;
  }

  pfnProc = MakeProcInstance( (FARPROC)EditGroupAssignments_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_EDIT_GROUP_ASSIGNMENTS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  if (nResult == IDOK && nNewGroupIndex >= 0 && nNewGroupIndex < pLevel->nGroups)
  {
    Object_DeleteUndoData( hwndParent, pWndData );

    for(i = 0; i < pLevel->nPolygons; ++i)
    {
      pPolygon = pLevel->pPolygonData + i;

      if (pPolygon->flags & PF_SELECTED)
      {
        pPolygon->group_index = nNewGroupIndex;
      }
    }

    for(i = 0; i < pLevel->nVertices; ++i)
    {
      pVertex = pLevel->pVertexData + i;

      if (pVertex->flags & VF_SELECTED)
      {
        pVertex->group_index = nNewGroupIndex;
      }
    }
  }

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditGroupAssignmentsDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       NewGroupDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int NewGroupDialog( HWND hwndParent )
{
  int nResult;
  FARPROC pfnProc;

  szNewGroupName[0] = '\0';
  
  pfnProc = MakeProcInstance( (FARPROC)NewGroup_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_NEW_GROUP,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // NewGroupDialog


/*******************************************************************

    NAME:       NewGroup_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export NewGroup_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    NewGroup_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, NewGroup_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy  );
  }

  return FALSE;

} // NewGroup_DlgProc


/*******************************************************************

    NAME:       NewGroup_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID NewGroup_OnCommand( HWND hwnd,
                         INT  id,
                         HWND hwndCtrl,
                         UINT codeNotify )
{
  switch(id)
  {
    case IDC_NEW_GROUP_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewGroupName),
                     (LPARAM)((LPSTR)szNewGroupName) );
      }
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // NewGroup_OnCommand


/*******************************************************************

    NAME:       NewGroup_OnInitDialog

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

BOOL NewGroup_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_NEW_GROUP_NAME );
  Edit_LimitText( hwndCtrl, 30 );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // NewGroup_OnInitDialog


/*******************************************************************

    NAME:       RenameGroupDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int RenameGroupDialog( HWND hwndParent )
{
  int nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance( (FARPROC)RenameGroup_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_NEW_GROUP,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // RenameGroupDialog


/*******************************************************************

    NAME:       RenameGroup_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export RenameGroup_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    RenameGroup_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, RenameGroup_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor    );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy     );
  }

  return FALSE;

} // RenameGroup_DlgProc


/*******************************************************************

    NAME:       RenameGroup_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID RenameGroup_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify )
{
  switch(id)
  {
    case IDC_NEW_GROUP_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewGroupName),
                     (LPARAM)((LPSTR)szNewGroupName) );
      }
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // RenameGroup_OnCommand


/*******************************************************************

    NAME:       RenameGroup_OnInitDialog

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

BOOL RenameGroup_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  HWND hwndCtrl;

  SetWindowText( hwnd, "Rename Group" );

  hwndCtrl = GetDlgItem( hwnd, IDC_NEW_GROUP_NAME );
  Edit_SetText( hwndCtrl, szNewGroupName );
  Edit_LimitText( hwndCtrl, 30 );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // RenameGroup_OnInitDialog


/*******************************************************************

    NAME:       ViewGroups_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewGroups_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,     ViewGroups_OnCommand     );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG,  ViewGroups_OnInitDialog  );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,    AllDialogs_OnCtlColor    );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,     AllDialogs_OnDestroy     );
  }

  return FALSE;

} // ViewGroups_DlgProc


/*******************************************************************

    NAME:       ViewGroups_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewGroups_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify )
{
  INT i, j;
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_NEW_GROUP:

      i = pLevel->nGroups;

      if (i == MAX_GROUPS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d groups has been reached.",
                MAX_GROUPS );

        break;
      }

      if (NewGroupDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      if (!szNewGroupName[0])
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_GROUP_LIST);

      ListBox_AddString(hwndList, szNewGroupName);

      _fstrcpy(pLevel->pGroupData[i].group_name, szNewGroupName);

      ++pLevel->nGroups;

      ListBox_SetCurSel(hwndList, i);

      bChange = TRUE;

      break;

    case IDC_DELETE_GROUP:

      hwndList = GetDlgItem( hwnd, IDC_GROUP_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nGroups )
      {
        i = (INT)lResult;
        DeleteGroup( pLevel, i );
        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        bChange = TRUE;
      }

      break;

    case IDC_RENAME_GROUP:

      hwndList = GetDlgItem( hwnd, IDC_GROUP_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nGroups )
      {
        i = (INT)lResult;

        _fstrcpy( szNewGroupName, pLevel->pGroupData[i].group_name );

        if ( RenameGroupDialog( hwnd ) == IDCANCEL )
        {
          break;
        }

        if ( !szNewGroupName[0] )
        {
          break;
        }

        _fstrcpy( pLevel->pGroupData[i].group_name, szNewGroupName );

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < pLevel->nGroups; ++i )
        {
          ListBox_AddString( hwndList, pLevel->pGroupData[i].group_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

        bChange = TRUE;
      }

      break;

    case IDC_HIDE_GROUP:

      hwndList = GetDlgItem( hwnd, IDC_GROUP_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nGroups )
      {
        i = (INT)lResult;
        HideGroup( pLevel, i );
        Object_ViewChanged(hParent, po);
        bChange = TRUE;
      }

      break;

    case IDC_SHOW_GROUP:

      hwndList = GetDlgItem( hwnd, IDC_GROUP_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nGroups )
      {
        i = (INT)lResult;
        ShowGroup( pLevel, i );
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
  
} // ViewGroups_OnCommand


/*******************************************************************

    NAME:       ViewGroups_OnInitDialog

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

BOOL ViewGroups_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_GROUP_LIST );

  for( i = 0; i < pLevel->nGroups; ++i )
  {
    ListBox_AddString( hwndCtrl, pLevel->pGroupData[i].group_name );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewGroups_OnInitDialog


/*******************************************************************

    NAME:       EditGroupAssignments_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditGroupAssignments_DlgProc( HWND   hwnd,
                                                     UINT   nMessage,
                                                     WPARAM wParam,
                                                     LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditGroupAssignments_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditGroupAssignments_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor             );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy              );
  }

  return FALSE;

} // EditGroupAssignments_DlgProc


/*******************************************************************

    NAME:       EditGroupAssignments_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditGroupAssignments_OnCommand( HWND hwnd,
                                     INT  id,
                                     HWND hwndCtrl,
                                     UINT codeNotify )
{
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_GROUP_ASSIGNMENTS_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        if ( lResult >= 0 && lResult < pLevel->nGroups )
        {
          nNewGroupIndex = (INT)lResult;
        }

        bChange = TRUE;
      }
      break;

    case IDOK: case IDCANCEL:

      if ( hPalCommon )
      {
        DeleteObject( hPalCommon );
        hPalCommon = NULL;
      }

      EndDialog( hwnd, IDOK );
      break;
  }
  
} // EditGroupAssignments_OnCommand


/*******************************************************************

    NAME:       EditGroupAssignments_OnInitDialog

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

BOOL EditGroupAssignments_OnInitDialog( HWND   hwnd,
                                        HWND   hwndFocus,
                                        LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_GROUP_ASSIGNMENTS_LIST );

  for( i = 0; i < pLevel->nGroups; ++i )
  {
    ListBox_AddString( hwndCtrl, pLevel->pGroupData[i].group_name );
  }

  ListBox_SetCurSel( hwndCtrl, nNewGroupIndex );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // EditGroupAssignments_OnInitDialog
