/*******************************************************************

    plydlg.c

    Routines for displaying and managing the polygon properties dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export EditPolygonProperties_DlgProc( HWND   hwnd,
                                                      UINT   nMessage,
                                                      WPARAM wParam,
                                                      LPARAM lParam );

VOID EditPolygonProperties_ShowTextures( HWND hwnd );

VOID EditPolygonProperties_ShowAnimations( HWND hwnd );

VOID EditPolygonProperties_ShowDamageAnimations( HWND hwnd );

VOID EditPolygonProperties_ShowMotions( HWND hwnd );

VOID EditPolygonProperties_OnCommand( HWND hwnd,
                                      INT  id,
                                      HWND hwndCtl,
                                      UINT codeNotify );

BOOL EditPolygonProperties_OnInitDialog( HWND   hwndDlg,
                                         HWND   hwndFocus,
                                         LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static LEVEL_DATA *pLevel;
static INT nListContent;
static INT nNewTextureIndex;
static INT nNewAnimationIndex;
static INT nNewMotionIndex;
static INT nNewDamageAnimationIndex;
static WORD wNewFlags;
static double dNewTextureXOffset;
static double dNewTextureYOffset;
static double dNewTextureDX;
static double dNewTextureDY;

//
//  Public functions.
//

/*******************************************************************

    NAME:       EditPolygonPropertiesDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int EditPolygonPropertiesDialog( HWND hwndParent, WND_DATA *pWndData )
{
  INT i, nResult;
  WORD wMask;
  POLYGON huge *p;
  FARPROC pfnProc;

  po = pWndData;
  pLevel = pWndData->pLevelData + pWndData->level_index;

  wMask = PF_DOUBLE_SIDED | PF_INVISIBLE | PF_WALK_THROUGH | PF_ANIMATED | 
          PF_MOTION_ANIMATED | PF_DAMAGE_ANIMATED | PF_MOVING_POLYGON | 
          PF_SECRET_PLACE;

  for( i = 0; i < pLevel->nPolygons; ++i)
  {
    p = pLevel->pPolygonData + i;

    if ( p->flags & PF_SELECTED )
    {
      wNewFlags = p->flags & wMask;

      nNewMotionIndex = p->motion_index;

      while( p->flags & PF_LINKED )
      {
        p = pLevel->pPolygonData + p->polygon_link;
      }

      break;
    }
  }

  nNewTextureIndex = p->texture_index;
  nNewAnimationIndex = p->animation_index;
  nNewDamageAnimationIndex = p->damage_animation_index;
  dNewTextureXOffset = (double)p->texture_x_offset;
  dNewTextureYOffset = (double)p->texture_y_offset;
  dNewTextureDX = (double)p->texture_dx;
  dNewTextureDY = (double)p->texture_dy;
  
  pfnProc = MakeProcInstance( (FARPROC)EditPolygonProperties_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_POLYGON_PROPERTIES,
                       hwndParent,
                       (DLGPROC)pfnProc );

  if ( nResult == IDOK )
  {
    Object_CreateUndoData(hwndParent, pWndData);

    for( i = 0; i < pLevel->nPolygons; ++i)
    {
      p = pLevel->pPolygonData + i;

      if ( p->flags & PF_SELECTED )
      {
        p->flags &= ~wMask;
        p->flags |= wNewFlags;

        p->motion_index = nNewMotionIndex;

        while( p->flags & PF_LINKED )
        {
          p = pLevel->pPolygonData + p->polygon_link;
        }

        p->texture_index = nNewTextureIndex;
        p->animation_index = nNewAnimationIndex;
        p->damage_animation_index = nNewDamageAnimationIndex;
        p->texture_x_offset = (float)dNewTextureXOffset;
        p->texture_y_offset = (float)dNewTextureYOffset;
        p->texture_dx = (float)dNewTextureDX;
        p->texture_dy = (float)dNewTextureDY;
      }
    }
  }

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditPolygonPropertiesDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       EditPolygonProperties_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditPolygonProperties_DlgProc( HWND   hwnd,
                                                      UINT   nMessage,
                                                      WPARAM wParam,
                                                      LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditPolygonProperties_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditPolygonProperties_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor              );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy               );
  }

  return FALSE;

} // EditPolygonProperties_DlgProc


/*******************************************************************

    NAME:       EditPolygonProperties_ShowTextures

    SYNOPSIS:   Initialize the list box with textures.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditPolygonProperties_ShowTextures( HWND hwnd )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_LIST );

  ListBox_ResetContent( hwndCtrl );

  for( i = 0; i < po->nTextures; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pTextureData[i].texture_name );
  }

  if ( nNewTextureIndex != -1 )
  {
    ListBox_SetCurSel( hwndCtrl, nNewTextureIndex );
  }

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_LIST_STATIC );
  Static_SetText( hwndCtrl, (LPSTR)"Textures:" );

  nListContent = 0;

  return;

} // EditPolygonProperties_ShowTextures


/*******************************************************************

    NAME:       EditPolygonProperties_ShowAnimations

    SYNOPSIS:   Initialize the list box with animations.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditPolygonProperties_ShowAnimations( HWND hwnd )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_LIST );

  ListBox_ResetContent( hwndCtrl );

  for( i = 0; i < po->nAnimations; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pAnimationData[i].animation_name );
  }

  if ( nNewAnimationIndex != -1 )
  {
    ListBox_SetCurSel( hwndCtrl, nNewAnimationIndex );
  }

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_LIST_STATIC );
  Static_SetText( hwndCtrl, (LPSTR)"Animations:" );

  nListContent = 1;

  return;

} // EditPolygonProperties_ShowAnimations


/*******************************************************************

    NAME:       EditPolygonProperties_ShowDamageAnimations

    SYNOPSIS:   Initialize the list box with damage animations.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditPolygonProperties_ShowDamageAnimations( HWND hwnd )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_LIST );

  ListBox_ResetContent( hwndCtrl );

  for( i = 0; i < po->nAnimations; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pAnimationData[i].animation_name );
  }

  if ( nNewDamageAnimationIndex != -1 )
  {
    ListBox_SetCurSel( hwndCtrl, nNewDamageAnimationIndex );
  }

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_LIST_STATIC );
  Static_SetText( hwndCtrl, (LPSTR)"Damage Animations:" );

  nListContent = 2;

  return;

} // EditPolygonProperties_ShowDamageAnimations


/*******************************************************************

    NAME:       EditPolygonProperties_ShowMotions

    SYNOPSIS:   Initialize the list box with motions.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditPolygonProperties_ShowMotions( HWND hwnd )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_LIST );

  ListBox_ResetContent( hwndCtrl );

  for( i = 0; i < pLevel->nMotions; ++i )
  {
    ListBox_AddString( hwndCtrl, pLevel->pMotionData[i].motion_name );
  }

  if ( nNewMotionIndex != -1 )
  {
    ListBox_SetCurSel( hwndCtrl, nNewMotionIndex );
  }

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_LIST_STATIC );
  Static_SetText( hwndCtrl, (LPSTR)"Motions:" );

  nListContent = 3;

  return;

} // EditPolygonProperties_ShowMotions


/*******************************************************************

    NAME:       EditPolygonProperties_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditPolygonProperties_OnCommand( HWND hwnd,
                                      INT  id,
                                      HWND hwndCtl,
                                      UINT codeNotify )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  switch(id)
  {
    case IDC_POLYGON_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtl );

        if ( lResult >= 0 )
        {
          if ( nListContent == 0 )
          {
            if ( lResult < po->nTextures )
            {
              nNewTextureIndex = (INT)lResult;
            }
          }
          else if ( nListContent == 1 )
          {
            if ( lResult < po->nAnimations )
            {
              nNewAnimationIndex = (INT)lResult;
            }
          }
          else if ( nListContent == 2 )
          {
            if ( lResult < po->nAnimations )
            {
              nNewDamageAnimationIndex = (INT)lResult;
            }
          }
          else
          {
            if ( lResult < pLevel->nMotions )
            {
              nNewMotionIndex = (INT)lResult;
            }
          }
        }
      }
      break;
      
    case IDC_POLYGON_DOUBLE_SIDED:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        if ( i == 0 )
        {
          wNewFlags &= ~PF_DOUBLE_SIDED;
        }
        else
        {
          wNewFlags |= PF_DOUBLE_SIDED;
          wNewFlags &= ~PF_INVISIBLE;
          CheckDlgButton(hwnd, IDC_POLYGON_INVISIBLE, 0);
        }
      }
      break;
      
    case IDC_POLYGON_INVISIBLE:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        if ( i == 0 )
        {
          wNewFlags &= ~PF_INVISIBLE;
        }
        else
        {
          wNewFlags |= PF_INVISIBLE;
          wNewFlags &= ~(PF_ANIMATED | PF_MOTION_ANIMATED);
          CheckDlgButton(hwnd, IDC_POLYGON_ANIMATED, 0);
          CheckDlgButton(hwnd, IDC_POLYGON_MOTION_ANIMATED, 0);
        }
      }
      break;
      
    case IDC_POLYGON_WALK_THROUGH:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        if ( i == 0 )
        {
          wNewFlags &= ~PF_WALK_THROUGH;
        }
        else
        {
          wNewFlags |= PF_WALK_THROUGH;
        }
      }
      break;
      
    case IDC_POLYGON_ANIMATED:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        if ( i == 0 )
        {
          wNewFlags &= ~PF_ANIMATED;
          EditPolygonProperties_ShowTextures(hwnd);
        }
        else
        {
          wNewFlags |= PF_ANIMATED;
          wNewFlags &= ~(PF_INVISIBLE | PF_MOTION_ANIMATED);
          CheckDlgButton(hwnd, IDC_POLYGON_INVISIBLE, 0);
          CheckDlgButton(hwnd, IDC_POLYGON_MOTION_ANIMATED, 0);
          EditPolygonProperties_ShowAnimations(hwnd);
        }
      }
      break;
      
    case IDC_POLYGON_MOTION_ANIMATED:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        if ( i == 0 )
        {
          wNewFlags &= ~PF_MOTION_ANIMATED;
          EditPolygonProperties_ShowTextures(hwnd);
        }
        else
        {
          wNewFlags |= PF_MOTION_ANIMATED;
          wNewFlags &= ~(PF_INVISIBLE | PF_ANIMATED);
          CheckDlgButton(hwnd, IDC_POLYGON_INVISIBLE, 0);
          CheckDlgButton(hwnd, IDC_POLYGON_ANIMATED, 0);
          EditPolygonProperties_ShowAnimations(hwnd);
        }
      }
      break;
      
    case IDC_POLYGON_DAMAGE_ANIMATED:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        if ( i == 0 )
        {
          wNewFlags &= ~PF_DAMAGE_ANIMATED;

          if (wNewFlags & (PF_ANIMATED | PF_MOTION_ANIMATED))
          {
            EditPolygonProperties_ShowAnimations(hwnd);
          }
          else
          {
            EditPolygonProperties_ShowTextures(hwnd);
          }
        }
        else
        {
          wNewFlags |= PF_DAMAGE_ANIMATED;
          EditPolygonProperties_ShowDamageAnimations(hwnd);
        }
      }
      break;
      
    case IDC_POLYGON_MOVING_POLYGON:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        if ( i == 0 )
        {
          wNewFlags = ( wNewFlags & ~PF_MOVING_POLYGON );

          if (wNewFlags & (PF_ANIMATED | PF_MOTION_ANIMATED))
          {
            EditPolygonProperties_ShowAnimations(hwnd);
          }
          else
          {
            EditPolygonProperties_ShowTextures(hwnd);
          }
        }
        else
        {
          wNewFlags = ( wNewFlags | PF_MOVING_POLYGON );
          EditPolygonProperties_ShowMotions(hwnd);
        }
      }
      break;
      
    case IDC_POLYGON_SECRET_PLACE:

      if ( codeNotify == BN_CLICKED )
      {
        i = (int)SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 );

        if ( i == 0 )
        {
          wNewFlags &= ~PF_SECRET_PLACE;
        }
        else
        {
          wNewFlags |= PF_SECRET_PLACE;
        }
      }
      break;
      
    case IDC_POLYGON_TEXTURE_X_OFFSET:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewTextureXOffset = atof( szBuf );
      }
      break;
      
    case IDC_POLYGON_TEXTURE_Y_OFFSET:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewTextureYOffset = atof( szBuf );
      }
      break;
      
    case IDC_POLYGON_TEXTURE_DX:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewTextureDX = atof( szBuf );
      }
      break;
      
    case IDC_POLYGON_TEXTURE_DY:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewTextureDY = atof( szBuf );
      }
      break;
      
    case IDC_POLYGON_SHOW_TEXTURES:

      EditPolygonProperties_ShowTextures( hwnd );
      break;
      
    case IDC_POLYGON_SHOW_ANIMATIONS:

      if (nListContent == 1)
      {
        EditPolygonProperties_ShowDamageAnimations( hwnd );
      }
      else
      {
        EditPolygonProperties_ShowAnimations( hwnd );
      }
      break;
      
    case IDC_POLYGON_SHOW_MOTIONS:

      EditPolygonProperties_ShowMotions( hwnd );
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // EditPolygonProperties_OnCommand


/*******************************************************************

    NAME:       EditPolygonProperties_OnInitDialog

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

BOOL EditPolygonProperties_OnInitDialog( HWND   hwnd,
                                         HWND   hwndFocus,
                                         LPARAM lParam )
{
  CHAR szBuf[128];
  HWND hwndCtrl;

  CheckDlgButton( hwnd,
                  IDC_POLYGON_DOUBLE_SIDED,
                  (wNewFlags & PF_DOUBLE_SIDED) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_POLYGON_INVISIBLE,
                  ( wNewFlags & PF_INVISIBLE ) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_POLYGON_WALK_THROUGH,
                  ( wNewFlags & PF_WALK_THROUGH ) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_POLYGON_ANIMATED,
                  ( wNewFlags & PF_ANIMATED ) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_POLYGON_MOTION_ANIMATED,
                  ( wNewFlags & PF_MOTION_ANIMATED ) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_POLYGON_DAMAGE_ANIMATED,
                  ( wNewFlags & PF_DAMAGE_ANIMATED ) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_POLYGON_MOVING_POLYGON,
                  ( wNewFlags & PF_MOVING_POLYGON ) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_POLYGON_SECRET_PLACE,
                  ( wNewFlags & PF_SECRET_PLACE ) ? 1 : 0 );

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_TEXTURE_X_OFFSET );
  DoubleToString( szBuf, dNewTextureXOffset, 2 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_TEXTURE_Y_OFFSET );
  DoubleToString( szBuf, dNewTextureYOffset, 2 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_TEXTURE_DX );
  DoubleToString( szBuf, dNewTextureDX, 2 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_POLYGON_TEXTURE_DY );
  DoubleToString( szBuf, dNewTextureDY, 2 );
  Edit_SetText( hwndCtrl, szBuf );
  Edit_LimitText( hwndCtrl, 8 );

  if (wNewFlags & (PF_ANIMATED | PF_MOTION_ANIMATED))
  {
    EditPolygonProperties_ShowAnimations( hwnd );
  }
  else
  {
    EditPolygonProperties_ShowTextures( hwnd );
  }

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // EditPolygonProperties_OnInitDialog
