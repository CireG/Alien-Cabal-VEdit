/*******************************************************************

    vrtxdlg.c

    Routines for displaying and managing the edit vertices dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export EditVertexProperties_DlgProc( HWND   hwnd,
                                                     UINT   nMessage,
                                                     WPARAM wParam,
                                                     LPARAM lParam );

VOID EditVertexProperties_InitListBox( HWND hwnd );

VOID EditVertexProperties_OnCommand( HWND hwnd,
                                     INT  id,
                                     HWND hwndCtl,
                                     UINT codeNotify );

BOOL EditVertexProperties_OnInitDialog( HWND   hwndDlg,
                                        HWND   hwndFocus,
                                        LPARAM lParam );

BOOL CALLBACK __export EditVertexPosition_DlgProc( HWND   hwnd,
                                                   UINT   nMessage,
                                                   WPARAM wParam,
                                                   LPARAM lParam );

VOID EditVertexPosition_OnCommand( HWND hwnd,
                                   INT  id,
                                   HWND hwndCtl,
                                   UINT codeNotify );

BOOL EditVertexPosition_OnInitDialog( HWND   hwndDlg,
                                      HWND   hwndFocus,
                                      LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static LEVEL_DATA *pLevel;
static INT nNewSpriteIndex;
static INT nNewNoiseIndex;
static INT nNewLifeFormIndex;
static INT nNewOrientation;
static INT nNewItemIndex;
static INT nNewTriggerIndex;
static WORD wNewFlags;
static char szAbsX[12];
static char szAbsY[12];
static char szAbsZ[12];
static long delta_x;
static long delta_y;
static long delta_z;


//
//  Public functions.
//

/*******************************************************************

    NAME:       EditVertexPropertiesDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int EditVertexPropertiesDialog(HWND hwndParent, WND_DATA *pWndData)
{
  INT i, nResult;
  WORD wMask;
  VERTEX huge *p;
  FARPROC pfnProc;

  po = pWndData;
  pLevel = pWndData->pLevelData + pWndData->level_index;

  wMask = VF_SPRITE | VF_NOISE | VF_LIFEFORM | VF_ITEM | VF_TRIGGER | 
          VF_PLAYER_START | VF_DIFFICULTY_LEVEL_1 | VF_DIFFICULTY_LEVEL_2 | 
          VF_DIFFICULTY_LEVEL_3;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    p = pLevel->pVertexData + i;

    if (p->flags & VF_SELECTED)
    {
      wNewFlags = p->flags & wMask;
      break;
    }
  }

  nNewSpriteIndex = p->sprite_index;
  nNewNoiseIndex = p->noise_index;
  nNewLifeFormIndex = p->lifeform_index;
  nNewOrientation = p->orientation;
  nNewItemIndex = p->item_index;
  nNewTriggerIndex = p->trigger_index;
  
  pfnProc = MakeProcInstance((FARPROC)EditVertexProperties_DlgProc, hInst);

  nResult = DialogBox( hInst,
                       IDD_VERTEX_PROPERTIES,
                       hwndParent,
                       (DLGPROC)pfnProc );

  if (nResult == IDOK)
  {
    Object_CreateUndoData(hwndParent, pWndData);

    for(i = 0; i < pLevel->nVertices; ++i)
    {
      p = pLevel->pVertexData + i;

      if (p->flags & VF_SELECTED)
      {
        p->flags &= ~wMask;
        p->flags |= wNewFlags;

        p->sprite_index = nNewSpriteIndex;
        p->noise_index = nNewNoiseIndex;
        p->lifeform_index = nNewLifeFormIndex;
        p->orientation = nNewOrientation;
        p->item_index = nNewItemIndex;
        p->trigger_index = nNewTriggerIndex;
      }
    }
  }

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditVertexPropertiesDialog


/*******************************************************************

    NAME:       EditVertexPositionDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int EditVertexPositionDialog(HWND hwndParent, WND_DATA *pWndData)
{
  INT nResult;
  FARPROC pfnProc;

  po = pWndData;
  pLevel = pWndData->pLevelData + pWndData->level_index;
  
  pfnProc = MakeProcInstance((FARPROC)EditVertexPosition_DlgProc, hInst);

  nResult = DialogBox( hInst,
                       IDD_VERTEX_POSITION,
                       hwndParent,
                       (DLGPROC)pfnProc );

  if (nResult == IDOK)
  {
    Object_CreateUndoData(hwndParent, pWndData);
    SetSelectedVertices(pLevel, szAbsX, szAbsY, szAbsZ);
    MoveSelectedVertices(pLevel, delta_x, delta_y, delta_z);
    DeleteDuplicateVertices(pLevel, TRUE);
  }

  FreeProcInstance(pfnProc);
    
  return nResult;
    
} // EditVertexPositionDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       EditVertexProperties_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditVertexProperties_DlgProc( HWND   hwnd,
                                                     UINT   nMessage,
                                                     WPARAM wParam,
                                                     LPARAM lParam )
{
  switch(nMessage)
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditVertexProperties_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditVertexProperties_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor             );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy              );
  }

  return FALSE;

} // EditVertexProperties_DlgProc


/*******************************************************************

    NAME:       EditVertexProperties_InitListBox

    SYNOPSIS:   Initialize the list box and label depending on the
                vertex flags.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditVertexProperties_InitListBox(HWND hwnd)
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_LIST);

  ListBox_ResetContent(hwndCtrl);

  if (wNewFlags & VF_SPRITE)
  {
    for(i = 0; i < po->nSprites; ++i)
    {
      ListBox_AddString(hwndCtrl, po->pSpriteData[i].sprite_name);
    }

    if (nNewSpriteIndex != -1)
    {
      ListBox_SetCurSel(hwndCtrl, nNewSpriteIndex);
    }

    hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_LIST_STATIC);
    Static_SetText(hwndCtrl, (LPSTR)"Sprites:");
  }
  else if (wNewFlags & VF_LIFEFORM)
  {
    for(i = 0; i < po->nLifeForms; ++i)
    {
      ListBox_AddString(hwndCtrl, po->pLifeFormData[i].lifeform_name);
    }

    if (nNewLifeFormIndex != -1)
    {
      ListBox_SetCurSel(hwndCtrl, nNewLifeFormIndex);
    }

    hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_LIST_STATIC);
    Static_SetText(hwndCtrl, (LPSTR)"Life Forms:");
  }
  else if (wNewFlags & VF_ITEM)
  {
    for(i = 0; i < po->nItems; ++i)
    {
      ListBox_AddString(hwndCtrl, po->pItemData[i].item_name);
    }

    if (nNewItemIndex != -1)
    {
      ListBox_SetCurSel(hwndCtrl, nNewItemIndex);
    }

    hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_LIST_STATIC);
    Static_SetText(hwndCtrl, (LPSTR)"Items:");
  }
  else if (wNewFlags & VF_NOISE)
  {
    for(i = 0; i < po->nNoises; ++i)
    {
      ListBox_AddString(hwndCtrl, po->pNoiseData[i].noise_name);
    }

    if (nNewNoiseIndex != -1)
    {
      ListBox_SetCurSel(hwndCtrl, nNewNoiseIndex);
    }

    hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_LIST_STATIC);
    Static_SetText(hwndCtrl, (LPSTR)"Noises:");
  }
  else if (wNewFlags & VF_TRIGGER)
  {
    for(i = 0; i < pLevel->nTriggers; ++i)
    {
      ListBox_AddString(hwndCtrl, pLevel->pTriggerData[i].trigger_name);
    }

    if (nNewTriggerIndex != -1)
    {
      ListBox_SetCurSel(hwndCtrl, nNewTriggerIndex);
    }

    hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_LIST_STATIC);
    Static_SetText(hwndCtrl, (LPSTR)"Triggers:");
  }
  else
  {
    hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_LIST_STATIC);
    Static_SetText(hwndCtrl, (LPSTR)"");
  }

  return;

} // EditVertexProperties_InitListBox


/*******************************************************************

    NAME:       EditVertexProperties_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/

VOID EditVertexProperties_OnCommand( HWND hwnd,
                                     INT  id,
                                     HWND hwndCtl,
                                     UINT codeNotify )
{
  INT i;
  LRESULT lResult;

  switch(id)
  {
    case IDC_VERTEX_LIST:

      if (codeNotify == LBN_SELCHANGE)
      {
        lResult = ListBox_GetCurSel(hwndCtl);

        if (lResult >= 0)
        {
          if (wNewFlags & VF_SPRITE)
          {
            if (lResult < po->nSprites)
            {
              nNewSpriteIndex = (INT)lResult;
            }
          }
          else if (wNewFlags & VF_LIFEFORM)
          {
            if (lResult < po->nLifeForms)
            {
              nNewLifeFormIndex = (INT)lResult;
            }
          }
          else if (wNewFlags & VF_ITEM)
          {
            if (lResult < po->nItems)
            {
              nNewItemIndex = (INT)lResult;
            }
          }
          else if (wNewFlags & VF_NOISE)
          {
            if (lResult < po->nNoises)
            {
              nNewNoiseIndex = (INT)lResult;
            }
          }
          else if (wNewFlags & VF_TRIGGER)
          {
            if (lResult < pLevel->nTriggers)
            {
              nNewTriggerIndex = (INT)lResult;
            }
          }
        }
      }
      break;
      
    case IDC_VERTEX_SPRITE:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~VF_SPRITE;
        }
        else
        {
          wNewFlags |= VF_SPRITE;
          wNewFlags &= ~(VF_LIFEFORM | VF_ITEM | VF_NOISE | VF_TRIGGER | VF_PLAYER_START);
          CheckDlgButton(hwnd, IDC_VERTEX_LIFEFORM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_ITEM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_NOISE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_TRIGGER, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_PLAYER_START, 0);
        }

        EditVertexProperties_InitListBox(hwnd);
      }
      break;
      
    case IDC_VERTEX_LIFEFORM:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~VF_LIFEFORM;
        }
        else
        {
          wNewFlags |= (VF_LIFEFORM | VF_DIFFICULTY_LEVEL_1 | VF_DIFFICULTY_LEVEL_2 | VF_DIFFICULTY_LEVEL_3);
          wNewFlags &= ~(VF_SPRITE | VF_ITEM | VF_NOISE | VF_TRIGGER | VF_PLAYER_START);
          CheckDlgButton(hwnd, IDC_VERTEX_SPRITE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_ITEM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_NOISE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_TRIGGER, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_PLAYER_START, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_1, 1);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_2, 1);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_3, 1);
        }

        EditVertexProperties_InitListBox(hwnd);
      }
      break;
      
    case IDC_VERTEX_ITEM:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~VF_ITEM;
        }
        else
        {
          wNewFlags |= (VF_ITEM | VF_DIFFICULTY_LEVEL_1 | VF_DIFFICULTY_LEVEL_2 | VF_DIFFICULTY_LEVEL_3);
          wNewFlags &= ~(VF_SPRITE | VF_LIFEFORM | VF_NOISE | VF_TRIGGER | VF_PLAYER_START);
          CheckDlgButton(hwnd, IDC_VERTEX_SPRITE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_LIFEFORM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_NOISE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_TRIGGER, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_PLAYER_START, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_1, 1);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_2, 1);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_3, 1);
        }

        EditVertexProperties_InitListBox(hwnd);
      }
      break;
      
    case IDC_VERTEX_NOISE:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~VF_NOISE;
        }
        else
        {
          wNewFlags |= VF_NOISE;
          wNewFlags &= ~(VF_SPRITE | VF_LIFEFORM | VF_ITEM | VF_TRIGGER | VF_PLAYER_START);
          CheckDlgButton(hwnd, IDC_VERTEX_SPRITE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_LIFEFORM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_ITEM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_TRIGGER, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_PLAYER_START, 0);
        }

        EditVertexProperties_InitListBox(hwnd);
      }
      break;
      
    case IDC_VERTEX_TRIGGER:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~VF_TRIGGER;
        }
        else
        {
          wNewFlags |= VF_TRIGGER;
          wNewFlags &= ~(VF_SPRITE | VF_LIFEFORM | VF_ITEM | VF_NOISE | VF_PLAYER_START);
          CheckDlgButton(hwnd, IDC_VERTEX_SPRITE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_LIFEFORM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_ITEM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_NOISE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_PLAYER_START, 0);
        }

        EditVertexProperties_InitListBox(hwnd);
      }
      break;
      
    case IDC_VERTEX_PLAYER_START:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~VF_PLAYER_START;
        }
        else
        {
          wNewFlags |= (VF_PLAYER_START | VF_DIFFICULTY_LEVEL_1 | VF_DIFFICULTY_LEVEL_2 | VF_DIFFICULTY_LEVEL_3);
          wNewFlags &= ~(VF_SPRITE | VF_LIFEFORM | VF_ITEM | VF_NOISE | VF_TRIGGER);
          CheckDlgButton(hwnd, IDC_VERTEX_SPRITE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_LIFEFORM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_ITEM, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_NOISE, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_TRIGGER, 0);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_1, 1);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_2, 1);
          CheckDlgButton(hwnd, IDC_VERTEX_DIFFICULTY_LEVEL_3, 1);
        }

        EditVertexProperties_InitListBox(hwnd);
      }
      break;
      
    case IDC_VERTEX_45:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          nNewOrientation = (nNewOrientation & ~1);
        }
        else
        {
          nNewOrientation = (nNewOrientation | 1);
        }
      }
      break;
      
    case IDC_VERTEX_90:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          nNewOrientation = (nNewOrientation & ~2);
        }
        else
        {
          nNewOrientation = (nNewOrientation | 2);
        }
      }
      break;
      
    case IDC_VERTEX_180:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          nNewOrientation = (nNewOrientation & ~4);
        }
        else
        {
          nNewOrientation = (nNewOrientation | 4);
        }
      }
      break;
      
    case IDC_VERTEX_DIFFICULTY_LEVEL_1:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags = (wNewFlags & ~VF_DIFFICULTY_LEVEL_1);
        }
        else
        {
          wNewFlags = (wNewFlags | VF_DIFFICULTY_LEVEL_1);
        }
      }
      break;
      
    case IDC_VERTEX_DIFFICULTY_LEVEL_2:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags = (wNewFlags & ~VF_DIFFICULTY_LEVEL_2);
        }
        else
        {
          wNewFlags = (wNewFlags | VF_DIFFICULTY_LEVEL_2);
        }
      }
      break;
      
    case IDC_VERTEX_DIFFICULTY_LEVEL_3:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags = (wNewFlags & ~VF_DIFFICULTY_LEVEL_3);
        }
        else
        {
          wNewFlags = (wNewFlags | VF_DIFFICULTY_LEVEL_3);
        }
      }
      break;
      
    case IDOK:

      if (wNewFlags & VF_SPRITE)
      {
        if (nNewSpriteIndex == -1)
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No sprite assigned" );

          break;
        }
      }
      else if (wNewFlags & VF_LIFEFORM)
      {
        if (nNewLifeFormIndex == -1)
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No life form assigned" );

          break;
        }
      }
      else if (wNewFlags & VF_ITEM)
      {
        if (nNewItemIndex == -1)
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No item assigned" );

          break;
        }
      }
      else if (wNewFlags & VF_TRIGGER)
      {
        if (nNewTriggerIndex == -1)
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "No trigger assigned" );

          break;
        }
      }

      EndDialog(hwnd, id);
      break;
      
    case IDCANCEL:

      EndDialog(hwnd, id);
      break;
  }
  
} // EditVertexProperties_OnCommand


/*******************************************************************

    NAME:       EditVertexProperties_OnInitDialog

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

BOOL EditVertexProperties_OnInitDialog( HWND   hwnd,
                                        HWND   hwndFocus,
                                        LPARAM lParam )
{
  CheckDlgButton( hwnd,
                  IDC_VERTEX_SPRITE,
                  (wNewFlags & VF_SPRITE) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_LIFEFORM,
                  (wNewFlags & VF_LIFEFORM) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_ITEM,
                  (wNewFlags & VF_ITEM) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_NOISE,
                  (wNewFlags & VF_NOISE) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_TRIGGER,
                  (wNewFlags & VF_TRIGGER) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_PLAYER_START,
                  (wNewFlags & VF_PLAYER_START) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_45,
                  (nNewOrientation & 1) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_90,
                  (nNewOrientation & 2) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_180,
                  (nNewOrientation & 4) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_DIFFICULTY_LEVEL_1,
                  (wNewFlags & VF_DIFFICULTY_LEVEL_1) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_DIFFICULTY_LEVEL_2,
                  (wNewFlags & VF_DIFFICULTY_LEVEL_2) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_VERTEX_DIFFICULTY_LEVEL_3,
                  (wNewFlags & VF_DIFFICULTY_LEVEL_3) ? 1 : 0 );

  EditVertexProperties_InitListBox(hwnd);

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // EditVertexProperties_OnInitDialog


/*******************************************************************

    NAME:       EditVertexPosition_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditVertexPosition_DlgProc( HWND   hwnd,
                                                   UINT   nMessage,
                                                   WPARAM wParam,
                                                   LPARAM lParam )
{
  switch(nMessage)
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditVertexPosition_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditVertexPosition_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor           );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy            );
  }

  return FALSE;

} // EditVertexPosition_DlgProc


/*******************************************************************

    NAME:       EditVertexPosition_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/

VOID EditVertexPosition_OnCommand( HWND hwnd,
                                   INT  id,
                                   HWND hwndCtl,
                                   UINT codeNotify )
{
  CHAR szBuf[128];

  switch(id)
  {
    case IDC_VERTEX_X:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szAbsX),
                     (LPARAM)((LPSTR)szAbsX) );
      }
      break;
      
    case IDC_VERTEX_Y:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szAbsY),
                     (LPARAM)((LPSTR)szAbsY) );
      }
      break;
      
    case IDC_VERTEX_Z:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szAbsZ),
                     (LPARAM)((LPSTR)szAbsZ) );
      }
      break;
      
    case IDC_VERTEX_DX:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        delta_x = atol(szBuf);
      }
      break;
      
    case IDC_VERTEX_DY:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        delta_y = atol(szBuf);
      }
      break;
      
    case IDC_VERTEX_DZ:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        delta_z = atol(szBuf);
      }
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog(hwnd, id);
      break;
  }
  
} // EditVertexPosition_OnCommand


/*******************************************************************

    NAME:       EditVertexPosition_OnInitDialog

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

BOOL EditVertexPosition_OnInitDialog( HWND   hwnd,
                                      HWND   hwndFocus,
                                      LPARAM lParam )
{
  INT i;
  LONG abs_x, abs_y, abs_z;
  BOOL bSet;
  CHAR szBuf[128];
  HWND hwndCtrl;
  VERTEX huge *p;

  p = pLevel->pVertexData;

  delta_x = 0L;
  delta_y = 0L;
  delta_z = 0L;

  bSet = FALSE;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (p->flags & VF_SELECTED)
    {
      if (bSet)
      {
        if ((LONG)Round(p->x) != abs_x)
        {
          szAbsX[0] = '\0';
        }

        if ((LONG)Round(p->y) != abs_y)
        {
          szAbsY[0] = '\0';
        }

        if ((LONG)Round(p->z) != abs_z)
        {
          szAbsZ[0] = '\0';
        }
      }
      else
      {
        abs_x = (LONG)Round(p->x);
        abs_y = (LONG)Round(p->y);
        abs_z = (LONG)Round(p->z);
        wsprintf(szAbsX, "%ld", abs_x);
        wsprintf(szAbsY, "%ld", abs_y);
        wsprintf(szAbsZ, "%ld", abs_z);
        bSet = TRUE;
      }
    }

    ++p;
  }


  hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_X);
  Edit_SetText(hwndCtrl, szAbsX);
  Edit_LimitText(hwndCtrl, sizeof(szAbsX) - 1);

  hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_Y);
  Edit_SetText(hwndCtrl, szAbsY);
  Edit_LimitText(hwndCtrl, sizeof(szAbsY) - 1);

  hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_Z);
  Edit_SetText(hwndCtrl, szAbsZ);
  Edit_LimitText(hwndCtrl, sizeof(szAbsZ) - 1);

  hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_DX);
  wsprintf(szBuf, "%ld", delta_x);
  Edit_SetText(hwndCtrl, szBuf);
  Edit_LimitText(hwndCtrl, 11);

  hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_DY);
  wsprintf(szBuf, "%ld", delta_y);
  Edit_SetText(hwndCtrl, szBuf);
  Edit_LimitText(hwndCtrl, 11);

  hwndCtrl = GetDlgItem(hwnd, IDC_VERTEX_DZ);
  wsprintf(szBuf, "%ld", delta_z);
  Edit_SetText(hwndCtrl, szBuf);
  Edit_LimitText(hwndCtrl, 11);

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // EditVertexPosition_OnInitDialog
