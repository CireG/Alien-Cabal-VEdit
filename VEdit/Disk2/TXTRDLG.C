/*******************************************************************

    txtrdlg.c

    Routines for displaying and managing the textures dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewTextures_DlgProc( HWND   hwnd,
                                             UINT   nMessage,
                                             WPARAM wParam,
                                             LPARAM lParam );

VOID ViewTextures_TextureChanged( HWND hwnd );

VOID ViewTextures_OnCommand( HWND hwnd,
                             INT  id,
                             HWND hwndCtrl,
                             UINT codeNotify );

BOOL ViewTextures_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

BOOL CALLBACK __export NewTexture_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam );

VOID NewTexture_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify );

BOOL NewTexture_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

BOOL CALLBACK __export RenameTexture_DlgProc( HWND   hwnd,
                                              UINT   nMessage,
                                              WPARAM wParam,
                                              LPARAM lParam );

VOID RenameTexture_OnCommand( HWND hwnd,
                              INT  id,
                              HWND hwndCtrl,
                              UINT codeNotify );

BOOL RenameTexture_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static CHAR szNewTextureName[32];


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewTexturesDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewTexturesDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewTextures_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_TEXTURES,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewTexturesDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       NewTextureDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int NewTextureDialog( HWND hwndParent )
{
  int nResult;
  FARPROC pfnProc;

  szNewTextureName[0] = '\0';
  
  pfnProc = MakeProcInstance( (FARPROC)NewTexture_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_NEW_TEXTURE,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // NewTextureDialog


/*******************************************************************

    NAME:       NewTexture_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export NewTexture_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    NewTexture_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, NewTexture_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor   );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy    );
  }

  return FALSE;

} // NewTexture_DlgProc


/*******************************************************************

    NAME:       NewTexture_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID NewTexture_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify )
{
  switch(id)
  {
    case IDC_NEW_TEXTURE_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewTextureName),
                     (LPARAM)((LPSTR)szNewTextureName) );
      }
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // NewTexture_OnCommand


/*******************************************************************

    NAME:       NewTexture_OnInitDialog

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

BOOL NewTexture_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_NEW_TEXTURE_NAME );
  Edit_LimitText( hwndCtrl, 30 );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // NewTexture_OnInitDialog


/*******************************************************************

    NAME:       RenameTextureDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int RenameTextureDialog( HWND hwndParent )
{
  int nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance( (FARPROC)RenameTexture_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_NEW_TEXTURE,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // RenameTextureDialog


/*******************************************************************

    NAME:       RenameTexture_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export RenameTexture_DlgProc( HWND   hwnd,
                                              UINT   nMessage,
                                              WPARAM wParam,
                                              LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    RenameTexture_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, RenameTexture_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor      );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy       );
  }

  return FALSE;

} // RenameTexture_DlgProc


/*******************************************************************

    NAME:       RenameTexture_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID RenameTexture_OnCommand( HWND hwnd,
                              INT  id,
                              HWND hwndCtrl,
                              UINT codeNotify )
{
  switch(id)
  {
    case IDC_NEW_TEXTURE_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewTextureName),
                     (LPARAM)((LPSTR)szNewTextureName) );
      }
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // RenameTexture_OnCommand


/*******************************************************************

    NAME:       RenameTexture_OnInitDialog

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

BOOL RenameTexture_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  HWND hwndCtrl;

  SetWindowText( hwnd, "Rename Texture" );

  hwndCtrl = GetDlgItem( hwnd, IDC_NEW_TEXTURE_NAME );
  Edit_SetText( hwndCtrl, szNewTextureName );
  Edit_LimitText( hwndCtrl, 30 );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // RenameTexture_OnInitDialog


/*******************************************************************

    NAME:       ViewTextures_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewTextures_DlgProc( HWND   hwnd,
                                             UINT   nMessage,
                                             WPARAM wParam,
                                             LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewTextures_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewTextures_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor     );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy      );
  }

  return FALSE;

} // ViewTextures_DlgProc


/*******************************************************************

    NAME:       ViewTextures_TextureChanged

    SYNOPSIS:   Handles new texture selections.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID ViewTextures_TextureChanged( HWND hwnd )
{
  INT i, j;
  CHAR szBuf[128];
  HWND hwndList;
  LRESULT lResult;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_TEXTURE_LIST ) );

  if ( lResult >= 0 && lResult < po->nTextures )
  {
    i = (INT)lResult;
    j = po->pTextureData[i].bitmap_index;

    hwndList = GetDlgItem( hwnd, IDC_TEXTURE_BITMAP_LIST );

    if ( j >= 0 && j < po->nBitmaps )
    {
      ListBox_SetCurSel( hwndList, j );

      PreviewBitmap( hwnd,
                     IDC_TEXTURE_BITMAP_PREVIEW,
                     IDC_TEXTURE_BITMAP_SIZE,
                     &po->pBitmapData[j],
                     po->pPalette );
    }
    else
    {
      ListBox_SetCurSel( hwndList, -1 );

      PreviewBitmap( hwnd,
                     IDC_TEXTURE_BITMAP_PREVIEW,
                     IDC_TEXTURE_BITMAP_SIZE,
                     NULL,
                     NULL );
    }

    DoubleToString( szBuf, po->pTextureData[i].pixel_size, 2 );
    Edit_SetText( GetDlgItem( hwnd, IDC_TEXTURE_PIXEL_SIZE ), szBuf );
  }

  return;

} // ViewTextures_TextureChanged


/*******************************************************************

    NAME:       ViewTextures_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewTextures_OnCommand( HWND hwnd,
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
    case IDC_TEXTURE_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        ViewTextures_TextureChanged( hwnd );
      }
      break;

    case IDC_TEXTURE_BITMAP_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_TEXTURE_LIST ) );

        if ( lResult >= 0 && lResult < po->nTextures )
        {
          i = (INT)lResult;

          lResult = ListBox_GetCurSel( hwndCtrl );

          if ( lResult >= 0 && lResult < po->nBitmaps )
          {
            PreviewBitmap( hwnd,
                           IDC_TEXTURE_BITMAP_PREVIEW,
                           IDC_TEXTURE_BITMAP_SIZE,
                           &po->pBitmapData[lResult],
                           po->pPalette );

            po->pTextureData[i].bitmap_index = (INT)lResult;
          }
          else
          {
            PreviewBitmap( hwnd,
                           IDC_TEXTURE_BITMAP_PREVIEW,
                           IDC_TEXTURE_BITMAP_SIZE,
                           NULL,
                           NULL );
          }
        }
      }
      break;

    case IDC_TEXTURE_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_TEXTURE_LIST ) );

        if ( lResult >= 0 && lResult < po->nTextures )
        {
          i = (INT)lResult;

          SendMessage( hwndCtrl,
                       WM_GETTEXT,
                       (WPARAM)sizeof(szBuf),
                       (LPARAM)((LPSTR)szBuf) );

          po->pTextureData[i].pixel_size = (float)atof( szBuf );

          bChange = TRUE;
        }
      }
      break;

    case IDC_NEW_TEXTURE:

      i = po->nTextures;

      if (i == MAX_TEXTURES)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d textures has been reached.",
                MAX_TEXTURES );

        break;
      }

      if (NewTextureDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      if (!szNewTextureName[0])
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_TEXTURE_LIST);

      ListBox_AddString(hwndList, szNewTextureName);

      po->pTextureData[i].bitmap_index = -1;
      po->pTextureData[i].pixel_size = (float)15.62;
      po->pTextureData[i].color_index = 0;

      _fstrcpy(po->pTextureData[i].texture_name, szNewTextureName);

      ++po->nTextures;

      ListBox_SetCurSel(hwndList, i);
      ViewTextures_TextureChanged(hwnd);

      bChange = TRUE;

      break;

    case IDC_DELETE_TEXTURE:

      hwndList = GetDlgItem( hwnd, IDC_TEXTURE_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nTextures )
      {
        i = (INT)lResult;
        Object_DeleteTexture( po, i );
        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        ViewTextures_TextureChanged( hwnd );

        bChange = TRUE;
      }

      break;

    case IDC_RENAME_TEXTURE:

      hwndList = GetDlgItem( hwnd, IDC_TEXTURE_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nTextures )
      {
        i = (INT)lResult;

        _fstrcpy( szNewTextureName, po->pTextureData[i].texture_name );

        if ( RenameTextureDialog( hwnd ) == IDCANCEL )
        {
          break;
        }

        if ( !szNewTextureName[0] )
        {
          break;
        }

        _fstrcpy( po->pTextureData[i].texture_name, szNewTextureName );

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < po->nTextures; ++i )
        {
          ListBox_AddString( hwndList, po->pTextureData[i].texture_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

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
  
} // ViewTextures_OnCommand


/*******************************************************************

    NAME:       ViewTextures_OnInitDialog

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

BOOL ViewTextures_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_TEXTURE_LIST );

  for( i = 0; i < po->nTextures; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pTextureData[i].texture_name );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  hwndCtrl = GetDlgItem( hwnd, IDC_TEXTURE_BITMAP_LIST );

  for( i = 0; i < po->nBitmaps; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pBitmapData[i].bitmap_name );
  }

  hwndCtrl = GetDlgItem( hwnd, IDC_TEXTURE_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );

  ViewTextures_TextureChanged( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewTextures_OnInitDialog
