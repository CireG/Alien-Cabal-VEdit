/*******************************************************************

    sprtdlg.c

    Routines for displaying and managing the sprite dialogs.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewSprites_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam );

VOID ViewSprites_SpriteChanged(HWND hwnd);

VOID ViewSprites_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify );

BOOL ViewSprites_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

INT EditSpriteDialog(HWND hwndParent);

BOOL CALLBACK __export EditSprite_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam );

VOID EditSprite_InitList(HWND hwnd);

VOID EditSprite_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify );

BOOL EditSprite_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static CHAR szNewSpriteName[32];
static WORD wNewFlags;
static INT nNewBitmapIndex;
static INT nNewAnimationIndex;
static INT nNewDamageAnimationIndex;
static INT nNewDamageAnimationIndex2;
static INT nListContent;
static double dNewPixelSize;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewSpritesDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewSpritesDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewSprites_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_SPRITES,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewSpritesDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewSprites_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewSprites_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewSprites_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewSprites_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor    );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy     );
  }

  return FALSE;

} // ViewSprites_DlgProc


/*******************************************************************

    NAME:       ViewSprites_SpriteChanged

    SYNOPSIS:   Handles a change in currently selected sprite.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID ViewSprites_SpriteChanged( HWND hwnd )
{
  INT i, index, bmp_index;
  CHAR szBuf[64];
  LPSTR lpszName;
  LPSTR lpszLabel;
  LRESULT lResult;

  lpszLabel = (LPSTR)"";
  lpszName  = lpszLabel;

  lResult = ListBox_GetCurSel(GetDlgItem(hwnd, IDC_SPRITE_LIST));

  if (lResult >= 0 && lResult < po->nSprites)
  {
    i = (INT)lResult;

    bmp_index = -1;

    if (po->pSpriteData[i].flags & SF_ANIMATED)
    {
      ANIMATION_DATA *pAnimation;

      lpszLabel = (LPSTR)"Animation:";
      index = po->pSpriteData[i].animation_index;

      if (index >= 0 && index < po->nAnimations)
      {
        pAnimation = po->pAnimationData + index;

        lpszName  = (LPSTR)pAnimation->animation_name;
        bmp_index = GetFirstBitmapIndex(pAnimation);
      }
    }
    else
    {
      BITMAP_DATA *pBitmap;

      lpszLabel = (LPSTR)"Bitmap:";
      index = po->pSpriteData[i].bitmap_index;

      if (index >= 0 && index < po->nBitmaps)
      {
        pBitmap = po->pBitmapData + index;

        lpszName  = (LPSTR)pBitmap->bitmap_name;
        bmp_index = po->pSpriteData[i].bitmap_index;
      }
    }

    if (bmp_index >= 0 && bmp_index < po->nBitmaps)
    {
      PreviewBitmap( hwnd,
                     IDC_SPRITE_BITMAP_PREVIEW,
                     IDC_SPRITE_BITMAP_SIZE,
                     &po->pBitmapData[bmp_index],
                     po->pPalette );
    }
    else
    {
      PreviewBitmap( hwnd,
                     IDC_SPRITE_BITMAP_PREVIEW,
                     IDC_SPRITE_BITMAP_SIZE,
                     NULL,
                     NULL );
    }

    CheckDlgButton( hwnd,
                    IDC_SPRITE_HANGING,
                    (po->pSpriteData[i].flags & SF_HANGING) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_SPRITE_WALK_THROUGH,
                    (po->pSpriteData[i].flags & SF_WALK_THROUGH) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_SPRITE_ANIMATED,
                    (po->pSpriteData[i].flags & SF_ANIMATED) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_SPRITE_DAMAGE_ANIMATED,
                    (po->pSpriteData[i].flags & SF_DAMAGE_ANIMATED) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_SPRITE_DAMAGE_ANIMATED_2,
                    (po->pSpriteData[i].flags & SF_DAMAGE_ANIMATED_2) ? 1 : 0 );

    CheckDlgButton( hwnd,
                    IDC_SPRITE_DESTRUCTABLE,
                    (po->pSpriteData[i].flags & SF_DESTRUCTABLE) ? 1 : 0 );

    DoubleToString( szBuf, po->pSpriteData[i].pixel_size, 2 );
    Edit_SetText( GetDlgItem( hwnd, IDC_SPRITE_PIXEL_SIZE ), szBuf );
  }

  Static_SetText(GetDlgItem(hwnd, IDC_SPRITE_TYPE_STATIC), lpszLabel);
  Static_SetText(GetDlgItem(hwnd, IDC_SPRITE_TYPE_NAME), lpszName);

  return;
  
} // ViewSprites_SpriteChanged


/*******************************************************************

    NAME:       ViewSprites_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewSprites_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify )
{
  INT i, j;
  CHAR szBuf[128];
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_SPRITE_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        ViewSprites_SpriteChanged( hwnd );
      }
      break;

    case IDC_SPRITE_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_SPRITE_LIST ) );

        if ( lResult >= 0 && lResult < po->nSprites )
        {
          i = (INT)lResult;

          SendMessage( hwndCtrl,
                       WM_GETTEXT,
                       (WPARAM)sizeof(szBuf),
                       (LPARAM)((LPSTR)szBuf) );

          po->pSpriteData[i].pixel_size = (float)atof( szBuf );

          bChange = TRUE;
        }
      }
      break;

    case IDC_SPRITE_NEW:

      i = po->nSprites;

      if (i == MAX_SPRITES)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d sprites has been reached.",
                MAX_SPRITES );

        break;
      }

      _fmemset(szNewSpriteName, 0, sizeof(szNewSpriteName));

      wNewFlags = 0;
      nNewBitmapIndex = -1;
      nNewAnimationIndex = -1;
      nNewDamageAnimationIndex = -1;
      nNewDamageAnimationIndex2 = -1;
      dNewPixelSize = 15.62;

      if (EditSpriteDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_SPRITE_LIST);

      ListBox_AddString(hwndList, szNewSpriteName);

      _fmemset(&po->pSpriteData[i], 0, sizeof(SPRITE_DATA));

      _fstrcpy(po->pSpriteData[i].sprite_name, szNewSpriteName);

      po->pSpriteData[i].flags = wNewFlags;
      po->pSpriteData[i].bitmap_index = nNewBitmapIndex;
      po->pSpriteData[i].animation_index = nNewAnimationIndex;
      po->pSpriteData[i].damage_animation_index = nNewDamageAnimationIndex;
      po->pSpriteData[i].damage_animation_index_2 = nNewDamageAnimationIndex2;
      po->pSpriteData[i].pixel_size = (float)dNewPixelSize;

      ++po->nSprites;

      ListBox_SetCurSel(hwndList, i);
      ViewSprites_SpriteChanged(hwnd);

      bChange = TRUE;

      break;

    case IDC_SPRITE_DELETE:

      hwndList = GetDlgItem(hwnd, IDC_SPRITE_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < po->nSprites)
      {
        i = (INT)lResult;
        Object_DeleteSprite(po, i);
        ListBox_DeleteString(hwndList, i);

        if ( i == 0 )
          ListBox_SetCurSel(hwndList, 0);
        else
          ListBox_SetCurSel(hwndList, i - 1);

        ViewSprites_SpriteChanged(hwnd);

        bChange = TRUE;
      }

      break;

    case IDC_SPRITE_EDIT:

      hwndList = GetDlgItem(hwnd, IDC_SPRITE_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < po->nSprites)
      {
        i = (INT)lResult;

        _fstrcpy(szNewSpriteName, po->pSpriteData[i].sprite_name);

        wNewFlags = po->pSpriteData[i].flags;
        nNewBitmapIndex = po->pSpriteData[i].bitmap_index;
        nNewAnimationIndex = po->pSpriteData[i].animation_index;
        nNewDamageAnimationIndex = po->pSpriteData[i].damage_animation_index;
        nNewDamageAnimationIndex2 = po->pSpriteData[i].damage_animation_index_2;
        dNewPixelSize = (double)po->pSpriteData[i].pixel_size;

        if (EditSpriteDialog(hwnd) == IDCANCEL)
        {
          break;
        }

        if (!szNewSpriteName[0])
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "The sprite must be given a name." );

          break;
        }

        _fmemset(&po->pSpriteData[i], 0, sizeof(SPRITE_DATA));

        _fstrcpy(po->pSpriteData[i].sprite_name, szNewSpriteName);

        po->pSpriteData[i].flags = wNewFlags;
        po->pSpriteData[i].bitmap_index = nNewBitmapIndex;
        po->pSpriteData[i].animation_index = nNewAnimationIndex;
        po->pSpriteData[i].damage_animation_index = nNewDamageAnimationIndex;
        po->pSpriteData[i].damage_animation_index_2 = nNewDamageAnimationIndex2;
        po->pSpriteData[i].pixel_size = (float)dNewPixelSize;

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for(i = 0; i < po->nSprites; ++i)
        {
          ListBox_AddString(hwndList, po->pSpriteData[i].sprite_name);
        }

        ListBox_SetTopIndex(hwndList, j);
        ListBox_SetCurSel(hwndList, (INT)lResult);

        ViewSprites_SpriteChanged(hwnd);

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
  
} // ViewSprites_OnCommand


/*******************************************************************

    NAME:       ViewSprites_OnInitDialog

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

BOOL ViewSprites_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_SPRITE_LIST);

  for(i = 0; i < po->nSprites; ++i)
  {
    ListBox_AddString(hwndCtrl, po->pSpriteData[i].sprite_name);
  }

  ListBox_SetCurSel(hwndCtrl, 0);

  hwndCtrl = GetDlgItem( hwnd, IDC_SPRITE_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );

  ViewSprites_SpriteChanged(hwnd);

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // ViewSprites_OnInitDialog


/*******************************************************************

    NAME:       EditSpriteDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

INT EditSpriteDialog(HWND hwndParent)
{
  INT nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance((FARPROC)EditSprite_DlgProc, hInst);

  nResult = DialogBox(hInst, IDD_EDIT_SPRITE, hwndParent, (DLGPROC)pfnProc);

  FreeProcInstance(pfnProc);
    
  return nResult;
    
} // EditSpriteDialog


/*******************************************************************

    NAME:       EditSprite_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditSprite_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditSprite_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditSprite_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor   );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy    );
  }

  return FALSE;

} // EditSprite_DlgProc


/*******************************************************************

    NAME:       EditSprite_InitList

    SYNOPSIS:   Initialize the list box and label depending on the
                current list content.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditSprite_InitList(HWND hwnd)
{
  INT i, nSelIndex, nBmpIndex;
  LPSTR lpszLabel;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_SPRITE_LIST);

  ListBox_ResetContent(hwndCtrl);

  if (nListContent == 0)
  {
    for(i = 0; i < po->nBitmaps; ++i)
    {
      ListBox_AddString(hwndCtrl, po->pBitmapData[i].bitmap_name);
    }

    lpszLabel = "Bitmaps:";
    nSelIndex = nNewBitmapIndex;
    nBmpIndex = nNewBitmapIndex;
  }
  else if (nListContent == 1)
  {
    for(i = 0; i < po->nAnimations; ++i)
    {
      ListBox_AddString(hwndCtrl, po->pAnimationData[i].animation_name);
    }

    lpszLabel = "Animations:";
    nSelIndex = nNewAnimationIndex;

    if (nNewAnimationIndex == -1)
    {
      nBmpIndex = -1;
    }
    else
    {
      nBmpIndex = GetFirstBitmapIndex(po->pAnimationData + nNewAnimationIndex);
    }
  }
  else if (nListContent == 2)
  {
    for(i = 0; i < po->nAnimations; ++i)
    {
      ListBox_AddString(hwndCtrl, po->pAnimationData[i].animation_name);
    }

    lpszLabel = "Damage Animation 1:";
    nSelIndex = nNewDamageAnimationIndex;

    if (nNewDamageAnimationIndex == -1)
    {
      nBmpIndex = -1;
    }
    else
    {
      nBmpIndex = GetFirstBitmapIndex(po->pAnimationData + nNewDamageAnimationIndex);
    }
  }
  else
  {
    for(i = 0; i < po->nAnimations; ++i)
    {
      ListBox_AddString(hwndCtrl, po->pAnimationData[i].animation_name);
    }

    lpszLabel = "Damage Animation 2:";
    nSelIndex = nNewDamageAnimationIndex2;

    if (nNewDamageAnimationIndex2 == -1)
    {
      nBmpIndex = -1;
    }
    else
    {
      nBmpIndex = GetFirstBitmapIndex(po->pAnimationData + nNewDamageAnimationIndex2);
    }
  }

  if (nSelIndex != -1)
  {
    ListBox_SetCurSel(hwndCtrl, nSelIndex);
  }

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_SPRITE_LIST_STATIC);
  Static_SetText(hwndCtrl, lpszLabel);

  if (nBmpIndex != -1)
  {
    PreviewBitmap( hwnd,
                   IDC_EDIT_SPRITE_BITMAP_PREVIEW,
                   IDC_EDIT_SPRITE_BITMAP_SIZE,
                   &po->pBitmapData[nBmpIndex],
                   po->pPalette );
  }
  else
  {
    PreviewBitmap( hwnd,
                   IDC_EDIT_SPRITE_BITMAP_PREVIEW,
                   IDC_EDIT_SPRITE_BITMAP_SIZE,
                   NULL,
                   NULL );
  }

  return;

} // EditSprite_InitList


/*******************************************************************

    NAME:       EditSprite_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditSprite_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_SPRITE_LIST:

      if (codeNotify == LBN_SELCHANGE)
      {
        lResult = ListBox_GetCurSel(hwndCtrl);

        i = (INT)lResult;

        if (nListContent == 0)
        {
          nNewBitmapIndex = i;
        }
        else if (nListContent == 1)
        {
          dNewPixelSize = (double)po->pAnimationData[i].pixel_size;
          DoubleToString( szBuf, dNewPixelSize, 2 );
          Edit_SetText( GetDlgItem( hwnd, IDC_EDIT_SPRITE_PIXEL_SIZE ), szBuf );

          nNewAnimationIndex = i;
          i = GetFirstBitmapIndex(po->pAnimationData + i);
        }
        else if (nListContent == 2)
        {
          nNewDamageAnimationIndex = i;
          i = GetFirstBitmapIndex(po->pAnimationData + i);
        }
        else
        {
          nNewDamageAnimationIndex2 = i;
          i = GetFirstBitmapIndex(po->pAnimationData + i);
        }

        if (i >= 0 && i < po->nBitmaps)
        {
          PreviewBitmap( hwnd,
                         IDC_EDIT_SPRITE_BITMAP_PREVIEW,
                         IDC_EDIT_SPRITE_BITMAP_SIZE,
                         &po->pBitmapData[i],
                         po->pPalette );
        }
        else
        {
          PreviewBitmap( hwnd,
                         IDC_EDIT_SPRITE_BITMAP_PREVIEW,
                         IDC_EDIT_SPRITE_BITMAP_SIZE,
                         NULL,
                         NULL );
        }
      }
      break;

    case IDC_EDIT_SPRITE_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewSpriteName),
                     (LPARAM)((LPSTR)szNewSpriteName) );
      }
      break;

    case IDC_EDIT_SPRITE_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewPixelSize = atof( szBuf );
      }
      break;
      
    case IDC_EDIT_SPRITE_HANGING:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~SF_HANGING;
        }
        else
        {
          wNewFlags |= SF_HANGING;
        }
      }
      break;
      
    case IDC_EDIT_SPRITE_WALK_THROUGH:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~SF_WALK_THROUGH;
        }
        else
        {
          wNewFlags |= SF_WALK_THROUGH;
        }
      }
      break;
      
    case IDC_EDIT_SPRITE_ANIMATED:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if ( i == 0 )
        {
          wNewFlags &= ~SF_ANIMATED;
          nListContent = 0;
          EditSprite_InitList(hwnd);
        }
        else
        {
          wNewFlags |= SF_ANIMATED;
          nListContent = 1;
          EditSprite_InitList(hwnd);
        }
      }
      break;
      
    case IDC_EDIT_SPRITE_DAMAGE_ANIMATED:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~SF_DAMAGE_ANIMATED;

          if (wNewFlags & SF_ANIMATED)
          {
            nListContent = 1;
          }
          else
          {
            nListContent = 0;
          }

          EditSprite_InitList(hwnd);
        }
        else
        {
          wNewFlags |= SF_DAMAGE_ANIMATED;
          nListContent = 2;
          EditSprite_InitList(hwnd);
        }
      }
      break;
      
    case IDC_EDIT_SPRITE_DAMAGE_ANIMATED_2:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~SF_DAMAGE_ANIMATED_2;

          if (wNewFlags & SF_ANIMATED)
          {
            nListContent = 1;
          }
          else
          {
            nListContent = 0;
          }

          EditSprite_InitList(hwnd);
        }
        else
        {
          wNewFlags |= SF_DAMAGE_ANIMATED_2;
          nListContent = 3;
          EditSprite_InitList(hwnd);
        }
      }
      break;
      
    case IDC_EDIT_SPRITE_DESTRUCTABLE:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~SF_DESTRUCTABLE;
        }
        else
        {
          wNewFlags |= SF_DESTRUCTABLE;
        }
      }
      break;
      
    case IDC_EDIT_SPRITE_SHOW_BITMAPS:

      nListContent = 0;
      EditSprite_InitList(hwnd);
      break;
      
    case IDC_EDIT_SPRITE_SHOW_ANIMATIONS:

      if (nListContent == 1)
      {
        nListContent = 2;
      }
      else if (nListContent == 2)
      {
        nListContent = 3;
      }
      else
      {
        nListContent = 1;
      }
      EditSprite_InitList(hwnd);
      break;
      
    case IDOK:

      if (!szNewSpriteName[0])
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The sprite must be given a name." );

        break;
      }

      EndDialog(hwnd, id);
      break;
      
    case IDCANCEL:

      EndDialog(hwnd, id);
      break;
  }
  
} // EditSprite_OnCommand


/*******************************************************************

    NAME:       EditSprite_OnInitDialog

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

BOOL EditSprite_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
  HWND hwndCtrl;
  CHAR szBuf[128];

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_SPRITE_NAME);
  Edit_LimitText(hwndCtrl, 30);
  Edit_SetText(hwndCtrl, szNewSpriteName);

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_SPRITE_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );
  DoubleToString( szBuf, dNewPixelSize, 2 );
  Edit_SetText( hwndCtrl, szBuf );

  nListContent = 0;
  EditSprite_InitList(hwnd);

  CheckDlgButton( hwnd,
                  IDC_EDIT_SPRITE_HANGING,
                  (wNewFlags & SF_HANGING) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_SPRITE_WALK_THROUGH,
                  (wNewFlags & SF_WALK_THROUGH) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_SPRITE_ANIMATED,
                  (wNewFlags & SF_ANIMATED) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_SPRITE_DAMAGE_ANIMATED,
                  (wNewFlags & SF_DAMAGE_ANIMATED) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_SPRITE_DAMAGE_ANIMATED_2,
                  (wNewFlags & SF_DAMAGE_ANIMATED_2) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_SPRITE_DESTRUCTABLE,
                  (wNewFlags & SF_DESTRUCTABLE) ? 1 : 0 );

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // EditSprite_OnInitDialog
