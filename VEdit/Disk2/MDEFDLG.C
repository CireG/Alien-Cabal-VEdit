/*******************************************************************

    mdefdlg.c

    Routines for displaying and managing the motion definition dialogs.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewMotions_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam );

VOID ViewMotions_MotionChanged( HWND hwnd );

VOID ViewMotions_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify );

BOOL ViewMotions_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

INT EditMotionDialog( HWND hwndParent );

BOOL CALLBACK __export EditMotion_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam );

VOID EditMotion_InitList( HWND hwnd );

VOID EditMotion_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify );

BOOL EditMotion_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static INT nListContent;
static WND_DATA *po;
static LEVEL_DATA *pLevel;
static INT nNewTriggerIndex;
static INT nNewStartSoundIndex;
static INT nNewRunSoundIndex;
static INT nNewStopSoundIndex;
static CHAR szNewMotionName[32];
static double dNewDx;
static double dNewDy;
static double dNewDz;
static INT nNewNumSteps;
static INT nNewDelay;
static WORD wNewFlags;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewMotionsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewMotionsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;

  pLevel = pWndData->pLevelData + pWndData->level_index;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewMotions_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_MOTIONS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewMotionsDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewMotions_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewMotions_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewMotions_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewMotions_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor    );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy     );
  }

  return FALSE;

} // ViewMotions_DlgProc


/*******************************************************************

    NAME:       ViewMotions_MotionChanged

    SYNOPSIS:   Handles a change in currently selected motion.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID ViewMotions_MotionChanged( HWND hwnd )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_MOTION_LIST ) );

  if ( lResult >= 0 && lResult < pLevel->nMotions )
  {
    i = (INT)lResult;

    DoubleToString( szBuf, pLevel->pMotionData[i].dx, 0 );
    Edit_SetText( GetDlgItem( hwnd, IDC_MOTION_DX ), szBuf );

    DoubleToString( szBuf, pLevel->pMotionData[i].dy, 0 );
    Edit_SetText( GetDlgItem( hwnd, IDC_MOTION_DY ), szBuf );

    DoubleToString( szBuf, pLevel->pMotionData[i].dz, 0 );
    Edit_SetText( GetDlgItem( hwnd, IDC_MOTION_DZ ), szBuf );

    wsprintf( szBuf, "%d", pLevel->pMotionData[i].num_steps );
    Edit_SetText( GetDlgItem( hwnd, IDC_MOTION_NUM_STEPS ), szBuf );

    wsprintf( szBuf, "%d", pLevel->pMotionData[i].return_delay );
    Edit_SetText( GetDlgItem( hwnd, IDC_MOTION_RETURN_DELAY ), szBuf );

    CheckDlgButton( hwnd,
                    IDC_MOTION_AUTO_RUN,
                    (pLevel->pMotionData[i].flags & MF_AUTO_RUN) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_MOTION_AUTO_RETURN,
                    (pLevel->pMotionData[i].flags & MF_AUTO_RETURN) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_MOTION_TRIGGER_RETURN,
                    (pLevel->pMotionData[i].flags & MF_TRIGGER_RETURN) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_MOTION_START_SOUND,
                    (pLevel->pMotionData[i].flags & MF_START_SOUND) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_MOTION_RUN_SOUND,
                    (pLevel->pMotionData[i].flags & MF_RUN_SOUND) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_MOTION_STOP_SOUND,
                    (pLevel->pMotionData[i].flags & MF_STOP_SOUND) ? 1 : 0 );
  }

  return;
  
} // ViewMotions_MotionChanged


/*******************************************************************

    NAME:       ViewMotions_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewMotions_OnCommand( HWND hwnd,
                             INT  id,
                             HWND hwndCtrl,
                             UINT codeNotify )
{
  INT i, j;
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_MOTION_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        ViewMotions_MotionChanged( hwnd );
      }
      break;

    case IDC_MOTION_NEW:

      i = pLevel->nMotions;

      if (i == MAX_MOTIONS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d motions has been reached.",
                MAX_MOTIONS );

        break;
      }

      _fmemset(szNewMotionName, 0, sizeof(szNewMotionName));

      nNewTriggerIndex = -1;
      nNewStartSoundIndex = -1;
      nNewRunSoundIndex = -1;
      nNewStopSoundIndex = -1;
      dNewDx = 0.0;
      dNewDy = 0.0;
      dNewDz = 0.0;
      nNewNumSteps = 0;
      nNewDelay = 55;
      wNewFlags = MF_AUTO_RETURN;

      if (EditMotionDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_MOTION_LIST);

      ListBox_AddString(hwndList, szNewMotionName);

      _fmemset(&pLevel->pMotionData[i], 0, sizeof(MOTION));

      _fstrcpy(pLevel->pMotionData[i].motion_name, szNewMotionName);

      pLevel->pMotionData[i].trigger_index = nNewTriggerIndex;
      pLevel->pMotionData[i].sound_index_start = nNewStartSoundIndex;
      pLevel->pMotionData[i].sound_index_run = nNewRunSoundIndex;
      pLevel->pMotionData[i].sound_index_stop = nNewStopSoundIndex;
      pLevel->pMotionData[i].dx = (float)dNewDx;
      pLevel->pMotionData[i].dy = (float)dNewDy;
      pLevel->pMotionData[i].dz = (float)dNewDz;
      pLevel->pMotionData[i].num_steps = nNewNumSteps;
      pLevel->pMotionData[i].return_delay = nNewDelay;
      pLevel->pMotionData[i].flags = wNewFlags;

      ++pLevel->nMotions;

      ListBox_SetCurSel(hwndList, i);
      ViewMotions_MotionChanged(hwnd);

      bChange = TRUE;

      break;

    case IDC_MOTION_DELETE:

      hwndList = GetDlgItem( hwnd, IDC_MOTION_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nMotions )
      {
        i = (INT)lResult;
        DeleteMotion( pLevel, i );
        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        ViewMotions_MotionChanged( hwnd );

        bChange = TRUE;
      }

      break;

    case IDC_MOTION_EDIT:

      hwndList = GetDlgItem( hwnd, IDC_MOTION_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < pLevel->nMotions )
      {
        i = (INT)lResult;

        _fstrcpy( szNewMotionName, pLevel->pMotionData[i].motion_name );
        nNewTriggerIndex = pLevel->pMotionData[i].trigger_index;
        nNewStartSoundIndex = pLevel->pMotionData[i].sound_index_start;
        nNewRunSoundIndex = pLevel->pMotionData[i].sound_index_run;
        nNewStopSoundIndex = pLevel->pMotionData[i].sound_index_stop;
        dNewDx = (double)pLevel->pMotionData[i].dx;
        dNewDy = (double)pLevel->pMotionData[i].dy;
        dNewDz = (double)pLevel->pMotionData[i].dz;
        nNewNumSteps = pLevel->pMotionData[i].num_steps;
        nNewDelay = pLevel->pMotionData[i].return_delay;
        wNewFlags = pLevel->pMotionData[i].flags;

        if ( EditMotionDialog( hwnd ) == IDCANCEL )
        {
          break;
        }

        if ( !szNewMotionName[0] )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "The motion must be given a name." );

          break;
        }

        _fmemset( &pLevel->pMotionData[i], 0, sizeof(MOTION) );

        _fstrcpy( pLevel->pMotionData[i].motion_name, szNewMotionName );

        pLevel->pMotionData[i].trigger_index = nNewTriggerIndex;
        pLevel->pMotionData[i].sound_index_start = nNewStartSoundIndex;
        pLevel->pMotionData[i].sound_index_run = nNewRunSoundIndex;
        pLevel->pMotionData[i].sound_index_stop = nNewStopSoundIndex;
        pLevel->pMotionData[i].dx = (float)dNewDx;
        pLevel->pMotionData[i].dy = (float)dNewDy;
        pLevel->pMotionData[i].dz = (float)dNewDz;
        pLevel->pMotionData[i].num_steps = nNewNumSteps;
        pLevel->pMotionData[i].return_delay = nNewDelay;
        pLevel->pMotionData[i].flags = wNewFlags;

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < pLevel->nMotions; ++i )
        {
          ListBox_AddString( hwndList, pLevel->pMotionData[i].motion_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

        ViewMotions_MotionChanged( hwnd );

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
  
} // ViewMotions_OnCommand


/*******************************************************************

    NAME:       ViewMotions_OnInitDialog

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

BOOL ViewMotions_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_MOTION_LIST );

  for( i = 0; i < pLevel->nMotions; ++i )
  {
    ListBox_AddString( hwndCtrl, pLevel->pMotionData[i].motion_name );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  ViewMotions_MotionChanged( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewMotions_OnInitDialog


/*******************************************************************

    NAME:       EditMotionDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

INT EditMotionDialog( HWND hwndParent )
{
  INT nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance( (FARPROC)EditMotion_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_EDIT_MOTION,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditMotionDialog


/*******************************************************************

    NAME:       EditMotion_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditMotion_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditMotion_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditMotion_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor   );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy    );
  }

  return FALSE;

} // EditMotion_DlgProc


/*******************************************************************

    NAME:       EditMotion_InitList

    SYNOPSIS:   Initialize the list box according to current list
                content (nListContent).

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditMotion_InitList( HWND hwnd )
{
  INT i;
  HWND hwndCtrl, hwndStatic;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_MOTION_LIST );
  hwndStatic = GetDlgItem(hwnd, IDC_EDIT_MOTION_LIST_STATIC);

  ListBox_ResetContent( hwndCtrl );

  if ( nListContent == 0 )
  {
    for( i = 0; i < pLevel->nTriggers; ++i )
    {
      ListBox_AddString( hwndCtrl, pLevel->pTriggerData[i].trigger_name );
    }

    ListBox_SetCurSel( hwndCtrl, nNewTriggerIndex );

    Static_SetText(hwndStatic, (LPSTR)"Triggers:");
  }
  else
  {
    for( i = 0; i < po->nSounds; ++i )
    {
      ListBox_AddString( hwndCtrl, po->pSoundData[i].sound_name );
    }

    if ( nListContent == 1 )
    {
      ListBox_SetCurSel( hwndCtrl, nNewStartSoundIndex );
      Static_SetText(hwndStatic, (LPSTR)"Start sound:");
    }
    else if ( nListContent == 2 )
    {
      ListBox_SetCurSel( hwndCtrl, nNewRunSoundIndex );
      Static_SetText(hwndStatic, (LPSTR)"Run sound:");
    }
    else
    {
      ListBox_SetCurSel( hwndCtrl, nNewStopSoundIndex );
      Static_SetText(hwndStatic, (LPSTR)"Stop sound:");
    }
  }

  return;

} // EditTrigger_InitList


/*******************************************************************

    NAME:       EditMotion_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditMotion_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_MOTION_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        if ( nListContent == 0 )
        {
          if ( lResult >= 0 && lResult < pLevel->nTriggers )
          {
            nNewTriggerIndex = (INT)lResult;
          }
        }
        else
        {
          if ( lResult >= 0 && lResult < po->nSounds )
          {
            if ( nListContent == 1 )
            {
              nNewStartSoundIndex = (INT)lResult;
            }
            else if ( nListContent == 2 )
            {
              nNewRunSoundIndex = (INT)lResult;
            }
            else
            {
              nNewStopSoundIndex = (INT)lResult;
            }
          }
        }
      }
      break;

    case IDC_EDIT_MOTION_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewMotionName),
                     (LPARAM)((LPSTR)szNewMotionName) );
      }
      break;

    case IDC_EDIT_MOTION_DX:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewDx = atof( szBuf );
      }
      break;

    case IDC_EDIT_MOTION_DY:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewDy = atof( szBuf );
      }
      break;

    case IDC_EDIT_MOTION_DZ:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewDz = atof( szBuf );
      }
      break;

    case IDC_EDIT_MOTION_NUM_STEPS:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        nNewNumSteps = atoi( szBuf );
      }
      break;

    case IDC_EDIT_MOTION_RETURN_DELAY:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        nNewDelay = atoi( szBuf );
      }
      break;
      
    case IDC_EDIT_MOTION_AUTO_RUN:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = (wNewFlags & ~(MF_AUTO_RUN));
        }
        else
        {
          wNewFlags = (wNewFlags | MF_AUTO_RUN);
        }
      }
      break;
      
    case IDC_EDIT_MOTION_AUTO_RETURN:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = (wNewFlags & ~(MF_AUTO_RETURN));
        }
        else
        {
          wNewFlags = (wNewFlags | MF_AUTO_RETURN);
        }
      }
      break;
      
    case IDC_EDIT_MOTION_TRIGGER_RETURN:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = (wNewFlags & ~(MF_TRIGGER_RETURN));
        }
        else
        {
          wNewFlags = (wNewFlags | MF_TRIGGER_RETURN);

          if ( nListContent != 0 )
          {
            nListContent = 0;
            EditMotion_InitList( hwnd );
          }
        }
      }
      break;
      
    case IDC_EDIT_MOTION_START_SOUND:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = (wNewFlags & ~(MF_START_SOUND));
        }
        else
        {
          wNewFlags = (wNewFlags | MF_START_SOUND);

          if ( nListContent != 1 )
          {
            nListContent = 1;
            EditMotion_InitList( hwnd );
          }
        }
      }
      break;
      
    case IDC_EDIT_MOTION_RUN_SOUND:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = (wNewFlags & ~(MF_RUN_SOUND));
        }
        else
        {
          wNewFlags = (wNewFlags | MF_RUN_SOUND);

          if ( nListContent != 2 )
          {
            nListContent = 2;
            EditMotion_InitList( hwnd );
          }
        }
      }
      break;
      
    case IDC_EDIT_MOTION_STOP_SOUND:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags = (wNewFlags & ~(MF_STOP_SOUND));
        }
        else
        {
          wNewFlags = (wNewFlags | MF_STOP_SOUND);

          if ( nListContent != 3 )
          {
            nListContent = 3;
            EditMotion_InitList( hwnd );
          }
        }
      }
      break;
      
    case IDC_EDIT_MOTION_TRIGGERS:

      nListContent = 0;
      EditMotion_InitList( hwnd );
      break;
      
    case IDC_EDIT_MOTION_SOUNDS:

      if ( ++nListContent == 4 )
      {
        nListContent = 1;
      }

      EditMotion_InitList( hwnd );
      break;
      
    case IDOK:

      if ( !szNewMotionName[0] )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The motion must be given a name." );

        break;
      }

      if ( nNewNumSteps <= 0 )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "Steps must be > 0 and < 32767." );

        break;
      }

      if ( wNewFlags & MF_TRIGGER_RETURN )
      {
        if ( nNewTriggerIndex == -1 )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No trigger assigned" );

          if ( nListContent != 0 )
          {
            nListContent = 0;
            EditMotion_InitList( hwnd );
          }

          break;
        }
      }

      if ( wNewFlags & MF_START_SOUND )
      {
        if ( nNewStartSoundIndex == -1 )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No start sound assigned" );

          if ( nListContent != 1 )
          {
            nListContent = 1;
            EditMotion_InitList( hwnd );
          }

          break;
        }
      }

      if ( wNewFlags & MF_RUN_SOUND )
      {
        if ( nNewRunSoundIndex == -1 )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No run sound assigned" );

          if ( nListContent != 2 )
          {
            nListContent = 2;
            EditMotion_InitList( hwnd );
          }

          break;
        }
      }

      if ( wNewFlags & MF_STOP_SOUND )
      {
        if ( nNewStopSoundIndex == -1 )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No stop sound assigned" );

          if ( nListContent != 3 )
          {
            nListContent = 3;
            EditMotion_InitList( hwnd );
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
  
} // EditMotion_OnCommand


/*******************************************************************

    NAME:       EditMotion_OnInitDialog

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

BOOL EditMotion_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  CHAR szBuf[128];
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_MOTION_NAME );
  Edit_LimitText( hwndCtrl, 30 );
  Edit_SetText( hwndCtrl, szNewMotionName );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_MOTION_DX );
  Edit_LimitText( hwndCtrl, 8 );
  DoubleToString( szBuf, dNewDx, 0 );
  Edit_SetText( hwndCtrl, szBuf );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_MOTION_DY );
  Edit_LimitText( hwndCtrl, 8 );
  DoubleToString( szBuf, dNewDy, 0 );
  Edit_SetText( hwndCtrl, szBuf );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_MOTION_DZ );
  Edit_LimitText( hwndCtrl, 8 );
  DoubleToString( szBuf, dNewDz, 0 );
  Edit_SetText( hwndCtrl, szBuf );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_MOTION_NUM_STEPS );
  Edit_LimitText( hwndCtrl, 5 );
  wsprintf( szBuf, "%d", nNewNumSteps );
  Edit_SetText( hwndCtrl, szBuf );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_MOTION_RETURN_DELAY );
  Edit_LimitText( hwndCtrl, 5 );
  wsprintf( szBuf, "%d", nNewDelay );
  Edit_SetText( hwndCtrl, szBuf );

  CheckDlgButton( hwnd,
                  IDC_EDIT_MOTION_AUTO_RUN,
                  (wNewFlags & MF_AUTO_RUN) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_MOTION_AUTO_RETURN,
                  (wNewFlags & MF_AUTO_RETURN) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_MOTION_TRIGGER_RETURN,
                  (wNewFlags & MF_TRIGGER_RETURN) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_MOTION_START_SOUND,
                  (wNewFlags & MF_START_SOUND) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_MOTION_RUN_SOUND,
                  (wNewFlags & MF_RUN_SOUND) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_MOTION_STOP_SOUND,
                  (wNewFlags & MF_STOP_SOUND) ? 1 : 0 );

  nListContent = 0;

  EditMotion_InitList( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // EditMotion_OnInitDialog
