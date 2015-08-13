/*******************************************************************

    trigdlg.c

    Routines for displaying and managing the trigger dialogs.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewTriggers_DlgProc( HWND   hwnd,
                                             UINT   nMessage,
                                             WPARAM wParam,
                                             LPARAM lParam );

VOID ViewTriggers_TriggerChanged( HWND hwnd );

VOID ViewTriggers_OnCommand( HWND hwnd,
                             INT  id,
                             HWND hwndCtrl,
                             UINT codeNotify );

BOOL ViewTriggers_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

INT EditTriggerDialog( HWND hwndParent );

BOOL CALLBACK __export EditTrigger_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam );

VOID EditTrigger_InitList( HWND hwnd );

VOID EditTrigger_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify );

BOOL EditTrigger_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static LEVEL_DATA *pLevel;
static CHAR szNewTriggerName[32];
static double dNewRadius;
static double dNewHeight;
static INT nListContent;
static INT nNewItemType;
static INT nNewSoundIndex;
static WORD wNewFlags;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewTriggersDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewTriggersDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pfnProc = MakeProcInstance( (FARPROC)ViewTriggers_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_TRIGGERS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewTriggersDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewTriggers_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewTriggers_DlgProc( HWND   hwnd,
                                             UINT   nMessage,
                                             WPARAM wParam,
                                             LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewTriggers_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewTriggers_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor     );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy      );
  }

  return FALSE;

} // ViewTriggers_DlgProc


/*******************************************************************

    NAME:       ViewTriggers_TriggerChanged

    SYNOPSIS:   Handles a change in currently selected trigger.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID ViewTriggers_TriggerChanged( HWND hwnd )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_TRIGGER_LIST ) );

  if ( lResult >= 0 && lResult < pLevel->nTriggers )
  {
    i = (INT)lResult;

    DoubleToString( szBuf, pLevel->pTriggerData[i].radius, 0 );
    Edit_SetText( GetDlgItem( hwnd, IDC_TRIGGER_RADIUS ), szBuf );

    DoubleToString( szBuf, pLevel->pTriggerData[i].height, 0 );
    Edit_SetText( GetDlgItem( hwnd, IDC_TRIGGER_HEIGHT ), szBuf );

    CheckDlgButton( hwnd,
                    IDC_TRIGGER_PROXIMITY,
                    (pLevel->pTriggerData[i].flags & TF_PROXIMITY_TRIGGER) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_TRIGGER_USER,
                    (pLevel->pTriggerData[i].flags & TF_USER_TRIGGER) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_TRIGGER_SOUND,
                    (pLevel->pTriggerData[i].flags & TF_SOUND) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_TRIGGER_END_LEVEL,
                    (pLevel->pTriggerData[i].flags & TF_END_LEVEL) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_TRIGGER_END_GAME,
                    (pLevel->pTriggerData[i].flags & TF_END_GAME) ? 1 : 0 );

    if (pLevel->pTriggerData[i].flags & TF_ITEM_REQUIRED)
    {
      Static_SetText( GetDlgItem( hwnd, IDC_TRIGGER_ITEM_REQUIRED ),
                      GetItemTypeString( pLevel->pTriggerData[i].item_type ) );
    }
    else
    {
      Static_SetText( GetDlgItem( hwnd, IDC_TRIGGER_ITEM_REQUIRED ),
                      (LPSTR)"" );
    }
  }

  return;
  
} // ViewTriggers_TriggerChanged


/*******************************************************************

    NAME:       ViewTriggers_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewTriggers_OnCommand( HWND hwnd,
                             INT  id,
                             HWND hwndCtrl,
                             UINT codeNotify )
{
  INT i, j;
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_TRIGGER_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        ViewTriggers_TriggerChanged( hwnd );
      }
      break;

    case IDC_TRIGGER_NEW:

      i = pLevel->nTriggers;

      if (i == MAX_TRIGGERS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d triggers has been reached.",
                MAX_TRIGGERS );

        break;
      }

      _fmemset(szNewTriggerName, 0, sizeof(szNewTriggerName));

      dNewRadius = 1000.0;
      dNewHeight = 3000.0;
      nNewItemType = -1;
      nNewSoundIndex = -1;
      wNewFlags = TF_USER_TRIGGER;

      if (EditTriggerDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_TRIGGER_LIST);

      ListBox_AddString(hwndList, szNewTriggerName);

      _fmemset(&pLevel->pTriggerData[i], 0, sizeof(TRIGGER));

      _fstrcpy(pLevel->pTriggerData[i].trigger_name, szNewTriggerName);

      pLevel->pTriggerData[i].radius = (float)dNewRadius;
      pLevel->pTriggerData[i].height = (float)dNewHeight;
      pLevel->pTriggerData[i].flags = wNewFlags;
      pLevel->pTriggerData[i].item_type = nNewItemType;
      pLevel->pTriggerData[i].sound_index = nNewSoundIndex;

      ++pLevel->nTriggers;

      ListBox_SetCurSel(hwndList, i);
      ViewTriggers_TriggerChanged(hwnd);

      bChange = TRUE;

      break;

    case IDC_TRIGGER_DELETE:

      hwndList = GetDlgItem( hwnd, IDC_TRIGGER_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nTriggers )
      {
        i = (INT)lResult;
        DeleteTrigger( pLevel, i );
        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        ViewTriggers_TriggerChanged( hwnd );

        bChange = TRUE;
      }

      break;

    case IDC_TRIGGER_EDIT:

      hwndList = GetDlgItem( hwnd, IDC_TRIGGER_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nTriggers )
      {
        i = (INT)lResult;

        _fstrcpy( szNewTriggerName, pLevel->pTriggerData[i].trigger_name );
        dNewRadius = (double)pLevel->pTriggerData[i].radius;
        dNewHeight = (double)pLevel->pTriggerData[i].height;
        wNewFlags = pLevel->pTriggerData[i].flags;
        nNewItemType = pLevel->pTriggerData[i].item_type;
        nNewSoundIndex = pLevel->pTriggerData[i].sound_index;

        if ( EditTriggerDialog( hwnd ) == IDCANCEL )
        {
          break;
        }

        if ( !szNewTriggerName[0] )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "The trigger must be given a name." );

          break;
        }

        _fmemset( &pLevel->pTriggerData[i], 0, sizeof(TRIGGER) );
        _fstrcpy( pLevel->pTriggerData[i].trigger_name, szNewTriggerName );
        pLevel->pTriggerData[i].radius = (float)dNewRadius;
        pLevel->pTriggerData[i].height = (float)dNewHeight;
        pLevel->pTriggerData[i].flags = wNewFlags;
        pLevel->pTriggerData[i].item_type = nNewItemType;
        pLevel->pTriggerData[i].sound_index = nNewSoundIndex;

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < pLevel->nTriggers; ++i )
        {
          ListBox_AddString( hwndList, pLevel->pTriggerData[i].trigger_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

        ViewTriggers_TriggerChanged( hwnd );

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
  
} // ViewTriggers_OnCommand


/*******************************************************************

    NAME:       ViewTriggers_OnInitDialog

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

BOOL ViewTriggers_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_TRIGGER_LIST );

  for( i = 0; i < pLevel->nTriggers; ++i )
  {
    ListBox_AddString( hwndCtrl, pLevel->pTriggerData[i].trigger_name );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  ViewTriggers_TriggerChanged( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewTriggers_OnInitDialog


/*******************************************************************

    NAME:       EditTriggerDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

INT EditTriggerDialog( HWND hwndParent )
{
  INT nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance( (FARPROC)EditTrigger_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_EDIT_TRIGGER,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditTriggerDialog


/*******************************************************************

    NAME:       EditTrigger_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditTrigger_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditTrigger_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditTrigger_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor    );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy     );
  }

  return FALSE;

} // EditTrigger_DlgProc


/*******************************************************************

    NAME:       EditTrigger_InitList

    SYNOPSIS:   Initialize the list box according to current list
                content (nListContent).

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditTrigger_InitList( HWND hwnd )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_TRIGGER_LIST );

  ListBox_ResetContent( hwndCtrl );

  if ( nListContent == 0 )
  {
    for( i = 0; i < NUM_ITEM_TYPES; ++i )
    {
      ListBox_AddString( hwndCtrl, GetItemTypeString( i ) );
    }

    ListBox_SetCurSel( hwndCtrl, nNewItemType );

    hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_TRIGGER_LIST_STATIC);
    Static_SetText(hwndCtrl, (LPSTR)"Items:");
  }
  else
  {
    for( i = 0; i < po->nSounds; ++i )
    {
      ListBox_AddString( hwndCtrl, po->pSoundData[i].sound_name );
    }

    ListBox_SetCurSel( hwndCtrl, nNewSoundIndex );

    hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_TRIGGER_LIST_STATIC);
    Static_SetText(hwndCtrl, (LPSTR)"Sounds:");
  }

  return;

} // EditTrigger_InitList


/*******************************************************************

    NAME:       EditTrigger_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/

VOID EditTrigger_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_TRIGGER_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        if ( nListContent == 0 )
        {
          if ( lResult >= 0 && lResult < po->nItems )
          {
            nNewItemType = (INT)lResult;
          }
        }
        else
        {
          if ( lResult >= 0 && lResult < po->nSounds )
          {
            nNewSoundIndex = (INT)lResult;
          }
        }
      }
      break;

    case IDC_EDIT_TRIGGER_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewTriggerName),
                     (LPARAM)((LPSTR)szNewTriggerName) );
      }
      break;

    case IDC_EDIT_TRIGGER_RADIUS:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewRadius = atof( szBuf );
      }
      break;

    case IDC_EDIT_TRIGGER_HEIGHT:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewHeight = atof( szBuf );
      }
      break;
      
    case IDC_EDIT_TRIGGER_PROXIMITY:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = ((wNewFlags | TF_USER_TRIGGER ) & ~(TF_PROXIMITY_TRIGGER));
          CheckDlgButton(hwnd, IDC_EDIT_TRIGGER_USER, 1);
        }
        else
        {
          wNewFlags = ((wNewFlags | TF_PROXIMITY_TRIGGER ) & ~(TF_USER_TRIGGER));
          CheckDlgButton(hwnd, IDC_EDIT_TRIGGER_USER, 0);
        }
      }
      break;
      
    case IDC_EDIT_TRIGGER_USER:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = ((wNewFlags | TF_PROXIMITY_TRIGGER ) & ~(TF_USER_TRIGGER));
          CheckDlgButton(hwnd, IDC_EDIT_TRIGGER_PROXIMITY, 1);
        }
        else
        {
          wNewFlags = ((wNewFlags | TF_USER_TRIGGER ) & ~(TF_PROXIMITY_TRIGGER));
          CheckDlgButton(hwnd, IDC_EDIT_TRIGGER_PROXIMITY, 0);
        }
      }
      break;
      
    case IDC_EDIT_TRIGGER_ITEM_REQUIRED:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = (wNewFlags & ~TF_ITEM_REQUIRED);
        }
        else
        {
          wNewFlags = (wNewFlags | TF_ITEM_REQUIRED);

          if ( nListContent != 0 )
          {
            nListContent = 0;
            EditTrigger_InitList( hwnd );
          }
        }
      }
      break;
      
    case IDC_EDIT_TRIGGER_SOUND:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~TF_SOUND;
        }
        else
        {
          wNewFlags |= TF_SOUND;

          if ( nListContent != 1 )
          {
            nListContent = 1;
            EditTrigger_InitList( hwnd );
          }
        }
      }
      break;
      
    case IDC_EDIT_TRIGGER_END_LEVEL:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~TF_END_LEVEL;
        }
        else
        {
          wNewFlags = ((wNewFlags | TF_END_LEVEL) & ~(TF_END_GAME));
          CheckDlgButton(hwnd, IDC_EDIT_TRIGGER_END_GAME, 0);
        }
      }
      break;
      
    case IDC_EDIT_TRIGGER_END_GAME:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~TF_END_GAME;
        }
        else
        {
          wNewFlags = ((wNewFlags | TF_END_GAME) & ~(TF_END_LEVEL));
          CheckDlgButton(hwnd, IDC_EDIT_TRIGGER_END_LEVEL, 0);
        }
      }
      break;

    case IDC_EDIT_TRIGGER_ITEMS:

      nListContent = 0;
      EditTrigger_InitList( hwnd );
      break;
      
    case IDC_EDIT_TRIGGER_SOUNDS:

      nListContent = 1;
      EditTrigger_InitList( hwnd );
      break;
      
    case IDOK:

      if ( !szNewTriggerName[0] )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The trigger must be given a name." );

        break;
      }

      if ( wNewFlags & TF_ITEM_REQUIRED )
      {
        if ( nNewItemType == -1 )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No item assigned" );

          if ( nListContent != 0 )
          {
            nListContent = 0;
            EditTrigger_InitList( hwnd );
          }

          break;
        }
      }

      if ( wNewFlags & TF_SOUND )
      {
        if ( nNewSoundIndex == -1 )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No sound assigned" );

          if ( nListContent != 1 )
          {
            nListContent = 1;
            EditTrigger_InitList( hwnd );
          }

          break;
        }
      }

      EndDialog( hwnd, id );
      break;
      
    case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // EditTrigger_OnCommand


/*******************************************************************

    NAME:       EditTrigger_OnInitDialog

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

BOOL EditTrigger_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  CHAR szBuf[128];
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_TRIGGER_NAME );
  Edit_LimitText( hwndCtrl, 30 );
  Edit_SetText( hwndCtrl, szNewTriggerName );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_TRIGGER_RADIUS );
  Edit_LimitText( hwndCtrl, 8 );
  DoubleToString( szBuf, dNewRadius, 0 );
  Edit_SetText( hwndCtrl, szBuf );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_TRIGGER_HEIGHT );
  Edit_LimitText( hwndCtrl, 8 );
  DoubleToString( szBuf, dNewHeight, 0 );
  Edit_SetText( hwndCtrl, szBuf );

  CheckDlgButton( hwnd,
                  IDC_EDIT_TRIGGER_PROXIMITY,
                  (wNewFlags & TF_PROXIMITY_TRIGGER) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_TRIGGER_USER,
                  (wNewFlags & TF_USER_TRIGGER) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_TRIGGER_ITEM_REQUIRED,
                  (wNewFlags & TF_ITEM_REQUIRED) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_TRIGGER_SOUND,
                  (wNewFlags & TF_SOUND) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_TRIGGER_END_LEVEL,
                  (wNewFlags & TF_END_LEVEL) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_TRIGGER_END_GAME,
                  (wNewFlags & TF_END_GAME) ? 1 : 0 );

  nListContent = 0;

  EditTrigger_InitList( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // EditTrigger_OnInitDialog
