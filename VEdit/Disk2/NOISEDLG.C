/*******************************************************************

    noisedlg.c

    Routines for displaying and managing the noise dialogs.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewNoises_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam );

VOID ViewNoises_NoiseChanged(HWND hwnd);

VOID ViewNoises_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify );

BOOL ViewNoises_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

INT EditNoiseDialog(HWND hwndParent);

BOOL CALLBACK __export EditNoise_DlgProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam );

VOID EditNoise_InitList(HWND hwnd);

VOID EditNoise_OnCommand( HWND hwnd,
                          INT  id,
                          HWND hwndCtrl,
                          UINT codeNotify );

BOOL EditNoise_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static CHAR szNewNoiseName[32];
static WORD wNewFlags;
static INT nNewSoundIndex;
static double dNewRadius;
static double dNewHeight;
static INT nNewDelay;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewNoisesDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewNoisesDialog(HWND hwndParent, WND_DATA *pWndData)
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;

  pfnProc = MakeProcInstance((FARPROC)ViewNoises_DlgProc, hInst);

  nResult = DialogBox( hInst,
                       IDD_VIEW_NOISES,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance(pfnProc);

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewNoisesDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewNoises_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewNoises_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewNoises_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewNoises_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor   );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy    );
  }

  return FALSE;

} // ViewNoises_DlgProc


/*******************************************************************

    NAME:       ViewNoises_NoiseChanged

    SYNOPSIS:   Handles a change in currently selected noise.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID ViewNoises_NoiseChanged( HWND hwnd )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  lResult = ListBox_GetCurSel(GetDlgItem(hwnd, IDC_NOISE_LIST));

  if (lResult >= 0 && lResult < po->nNoises)
  {
    i = (INT)lResult;

    DoubleToString(szBuf, po->pNoiseData[i].radius, 0);
    Edit_SetText(GetDlgItem(hwnd, IDC_NOISE_RADIUS), szBuf);

    DoubleToString(szBuf, po->pNoiseData[i].height, 0);
    Edit_SetText(GetDlgItem(hwnd, IDC_NOISE_HEIGHT), szBuf);

    wsprintf(szBuf, "%d", po->pNoiseData[i].delay);
    Edit_SetText(GetDlgItem(hwnd, IDC_NOISE_DELAY), szBuf);
  }

  return;
  
} // ViewNoises_NoiseChanged


/*******************************************************************

    NAME:       ViewNoises_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewNoises_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify )
{
  INT i, j;
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_NOISE_LIST:

      if (codeNotify == LBN_SELCHANGE)
      {
        ViewNoises_NoiseChanged(hwnd);
      }
      break;

    case IDC_NOISE_NEW:

      i = po->nNoises;

      if (i == MAX_NOISES)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d noises has been reached.",
                MAX_NOISES );

        break;
      }

      _fmemset(szNewNoiseName, 0, sizeof(szNewNoiseName));

      wNewFlags = 0;
      nNewSoundIndex = -1;
      dNewRadius = 1000.0;
      dNewHeight = 3000.0;
      nNewDelay = 0;

      if (EditNoiseDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_NOISE_LIST);

      ListBox_AddString(hwndList, szNewNoiseName);

      _fmemset(&po->pNoiseData[i], 0, sizeof(NOISE_DATA));

      _fstrcpy(po->pNoiseData[i].noise_name, szNewNoiseName);

      po->pNoiseData[i].flags = wNewFlags;
      po->pNoiseData[i].sound_index = nNewSoundIndex;
      po->pNoiseData[i].radius = (float)dNewRadius;
      po->pNoiseData[i].height = (float)dNewHeight;
      po->pNoiseData[i].delay = nNewDelay;

      ++po->nNoises;

      ListBox_SetCurSel(hwndList, i);
      ViewNoises_NoiseChanged(hwnd);

      bChange = TRUE;

      break;

    case IDC_NOISE_DELETE:

      hwndList = GetDlgItem(hwnd, IDC_NOISE_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < po->nNoises)
      {
        i = (INT)lResult;
        Object_DeleteNoise(po, i);
        ListBox_DeleteString(hwndList, i);

        if (i == 0)
          ListBox_SetCurSel(hwndList, 0);
        else
          ListBox_SetCurSel(hwndList, i - 1);

        ViewNoises_NoiseChanged(hwnd);

        bChange = TRUE;
      }

      break;

    case IDC_NOISE_EDIT:

      hwndList = GetDlgItem(hwnd, IDC_NOISE_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < po->nNoises)
      {
        i = (INT)lResult;

        _fstrcpy(szNewNoiseName, po->pNoiseData[i].noise_name);

        wNewFlags = po->pNoiseData[i].flags;
        nNewSoundIndex = po->pNoiseData[i].sound_index;
        dNewRadius = (double)po->pNoiseData[i].radius;
        dNewHeight = (double)po->pNoiseData[i].height;
        nNewDelay = po->pNoiseData[i].delay;

        if (EditNoiseDialog(hwnd) == IDCANCEL)
        {
          break;
        }

        if (!szNewNoiseName[0])
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "The noise must be given a name." );

          break;
        }

        _fmemset(&po->pNoiseData[i], 0, sizeof(NOISE_DATA));

        _fstrcpy(po->pNoiseData[i].noise_name, szNewNoiseName);

        po->pNoiseData[i].flags = wNewFlags;
        po->pNoiseData[i].sound_index = nNewSoundIndex;
        po->pNoiseData[i].radius = (float)dNewRadius;
        po->pNoiseData[i].height = (float)dNewHeight;
        po->pNoiseData[i].delay = nNewDelay;

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for(i = 0; i < po->nNoises; ++i)
        {
          ListBox_AddString(hwndList, po->pNoiseData[i].noise_name);
        }

        ListBox_SetTopIndex(hwndList, j);
        ListBox_SetCurSel(hwndList, (INT)lResult);

        ViewNoises_NoiseChanged(hwnd);

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
  
} // ViewNoises_OnCommand


/*******************************************************************

    NAME:       ViewNoises_OnInitDialog

    SYNOPSIS:   Handles the WM_INITDIALOG message.

    ENTRY:      hwnd - The dialog box window handle. 
    
                hwndFocus - The control which will receive focus by
                  default (if we return TRUE).
                
                lParam - The dwData sent when creating the dialog box
                  if created with one of the ...Param() functions.

    RETURNS:    BOOL - Return TRUE to let the hwndFocus control have
                  focus. Return FALSE if we are setting it ourselves.

********************************************************************/

BOOL ViewNoises_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_NOISE_LIST);

  for(i = 0; i < po->nNoises; ++i)
  {
    ListBox_AddString(hwndCtrl, po->pNoiseData[i].noise_name);
  }

  ListBox_SetCurSel(hwndCtrl, 0);

  ViewNoises_NoiseChanged(hwnd);

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // ViewNoises_OnInitDialog


/*******************************************************************

    NAME:       EditNoiseDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

INT EditNoiseDialog(HWND hwndParent)
{
  INT nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance((FARPROC)EditNoise_DlgProc, hInst);

  nResult = DialogBox( hInst,
                       IDD_EDIT_NOISE,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance(pfnProc);
    
  return nResult;
    
} // EditNoiseDialog


/*******************************************************************

    NAME:       EditNoise_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditNoise_DlgProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditNoise_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditNoise_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor  );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy   );
  }

  return FALSE;

} // EditNoise_DlgProc


/*******************************************************************

    NAME:       EditNoise_InitList

    SYNOPSIS:   Initialize the list box with sounds.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditNoise_InitList(HWND hwnd)
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_NOISE_LIST);

  ListBox_ResetContent(hwndCtrl);

  for(i = 0; i < po->nSounds; ++i)
  {
    ListBox_AddString(hwndCtrl, po->pSoundData[i].sound_name);
  }

  ListBox_SetCurSel(hwndCtrl, nNewSoundIndex);

  return;

} // EditNoise_InitList


/*******************************************************************

    NAME:       EditNoise_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/

VOID EditNoise_OnCommand( HWND hwnd,
                          INT  id,
                          HWND hwndCtrl,
                          UINT codeNotify )
{
  CHAR szBuf[128];
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_NOISE_LIST:

      if (codeNotify == LBN_SELCHANGE)
      {
        lResult = ListBox_GetCurSel(hwndCtrl);

        if ( lResult >= 0 && lResult < po->nSounds )
        {
          nNewSoundIndex = (INT)lResult;
        }
      }
      break;

    case IDC_EDIT_NOISE_NAME:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewNoiseName),
                     (LPARAM)((LPSTR)szNewNoiseName) );
      }
      break;

    case IDC_EDIT_NOISE_RADIUS:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewRadius = atof( szBuf );
      }
      break;

    case IDC_EDIT_NOISE_HEIGHT:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewHeight = atof(szBuf);
      }
      break;

    case IDC_EDIT_NOISE_DELAY:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        nNewDelay = atoi(szBuf);
      }
      break;
      
    case IDOK:

      if (!szNewNoiseName[0])
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The noise must be given a name." );

        break;
      }

      if (nNewSoundIndex == -1)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "No sound assigned" );

        break;
      }

      EndDialog(hwnd, id);
      break;
      
    case IDCANCEL:

      EndDialog(hwnd, id);
      break;
  }
  
} // EditNoise_OnCommand


/*******************************************************************

    NAME:       EditNoise_OnInitDialog

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

BOOL EditNoise_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
  CHAR szBuf[128];
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_NOISE_NAME);
  Edit_LimitText(hwndCtrl, 30);
  Edit_SetText(hwndCtrl, szNewNoiseName);

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_NOISE_RADIUS);
  Edit_LimitText(hwndCtrl, 8);
  DoubleToString(szBuf, dNewRadius, 0);
  Edit_SetText(hwndCtrl, szBuf);

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_NOISE_HEIGHT);
  Edit_LimitText(hwndCtrl, 8);
  DoubleToString(szBuf, dNewHeight, 0);
  Edit_SetText(hwndCtrl, szBuf);

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_NOISE_DELAY);
  Edit_LimitText(hwndCtrl, 8);
  wsprintf(szBuf, "%d", nNewDelay);
  Edit_SetText(hwndCtrl, szBuf);

  EditNoise_InitList(hwnd);

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // EditNoise_OnInitDialog
