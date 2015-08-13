/*******************************************************************

    itemdlg.c

    Routines for displaying and managing the item dialogs.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewItems_DlgProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam );

VOID ViewItems_ItemChanged( HWND hwnd );

VOID ViewItems_OnCommand( HWND hwnd,
                          INT  id,
                          HWND hwndCtrl,
                          UINT codeNotify );

BOOL ViewItems_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

INT EditItemDialog( HWND hwndParent );

BOOL CALLBACK __export EditItem_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam );

VOID EditItem_ElementChanged( HWND hwnd );

VOID EditItem_TypeChanged( HWND hwnd );

VOID EditItem_OnCommand( HWND hwnd,
                         INT  id,
                         HWND hwndCtrl,
                         UINT codeNotify );

BOOL EditItem_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static CHAR szNewItemName[32];
static INT nNewItemType;
static INT nNewIndex[MAX_ELEMENTS];
static INT nListContent;
static double dNewPixelSize;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewItemsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewItemsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewItems_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_ITEMS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewItemsDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewItems_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewItems_DlgProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewItems_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewItems_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor  );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy   );
  }

  return FALSE;

} // ViewItems_DlgProc


/*******************************************************************

    NAME:       ViewItems_ItemChanged

    SYNOPSIS:   Handles a change in currently selected item.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID ViewItems_ItemChanged( HWND hwnd )
{
  INT i, j, k, num_elements, num_bitmaps;
  CHAR szBuf[128];
  HWND hwndList;
  LRESULT lResult;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_ITEM_LIST ) );

  if ( lResult >= 0 && lResult < po->nItems )
  {
    i = (INT)lResult;

    hwndList = GetDlgItem( hwnd, IDC_ITEM_RESOURCES );

    ListBox_ResetContent( hwndList );

    num_elements = GetNumItemElements( po->pItemData[i].item_type );
    num_bitmaps = GetNumItemBitmaps( po->pItemData[i].item_type );

    for( j = 0; j < num_elements; ++j )
    {
      k = po->pItemData[i].index[j];

      if ( j < num_bitmaps )
      {
        if ( k >= 0 && k < po->nBitmaps )
        {
          ListBox_AddString( hwndList, po->pBitmapData[k].bitmap_name );
        }
      }
      else
      {
        if ( k >= 0 && k < po->nSounds )
        {
          ListBox_AddString( hwndList, po->pSoundData[k].sound_name );
        }
      }
    }

    ListBox_SetCurSel( hwndList, 0 );

    j = po->pItemData[i].index[0];

    if ( j >= 0 && j < po->nBitmaps && num_bitmaps > 0 )
    {
      PreviewBitmap( hwnd,
                     IDC_ITEM_BITMAP_PREVIEW,
                     IDC_ITEM_BITMAP_SIZE,
                     &po->pBitmapData[j],
                     po->pPalette );
    }
    else
    {
      PreviewBitmap( hwnd,
                     IDC_ITEM_BITMAP_PREVIEW,
                     IDC_ITEM_BITMAP_SIZE,
                     NULL,
                     NULL );
    }

    Static_SetText( GetDlgItem( hwnd, IDC_ITEM_TYPE ),
                    GetItemTypeString( po->pItemData[i].item_type ) );

    DoubleToString( szBuf, po->pItemData[i].pixel_size, 2 );
    Edit_SetText( GetDlgItem( hwnd, IDC_ITEM_PIXEL_SIZE ), szBuf );
  }

  return;
  
} // ViewItems_ItemChanged


/*******************************************************************

    NAME:       ViewItems_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewItems_OnCommand( HWND hwnd,
                          INT  id,
                          HWND hwndCtrl,
                          UINT codeNotify )
{
  INT i, j, n;
  CHAR szBuf[128];
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_ITEM_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        ViewItems_ItemChanged( hwnd );
      }
      break;

    case IDC_ITEM_RESOURCES:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        if ( lResult != LB_ERR )
        {
          ListBox_GetText( hwndCtrl, lResult, szBuf );

          for( j = 0; j < po->nBitmaps; ++j )
          {
            if ( !_fstricmp( szBuf, po->pBitmapData[j].bitmap_name ) )
            {
              break;
            }
          }

          if ( j < po->nBitmaps )
          {
            PreviewBitmap( hwnd,
                           IDC_ITEM_BITMAP_PREVIEW,
                           IDC_ITEM_BITMAP_SIZE,
                           &po->pBitmapData[j],
                           po->pPalette );
          }
          else
          {
            PreviewBitmap( hwnd,
                           IDC_ITEM_BITMAP_PREVIEW,
                           IDC_ITEM_BITMAP_SIZE,
                           NULL,
                           NULL );
          }
        }
      }
      break;

    case IDC_ITEM_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_ITEM_LIST ) );

        if ( lResult >= 0 && lResult < po->nItems )
        {
          i = (INT)lResult;

          SendMessage( hwndCtrl,
                       WM_GETTEXT,
                       (WPARAM)sizeof(szBuf),
                       (LPARAM)((LPSTR)szBuf) );

          po->pItemData[i].pixel_size = (float)atof( szBuf );

          bChange = TRUE;
        }
      }
      break;

    case IDC_ITEM_NEW:

      i = po->nItems;

      if (i == MAX_ITEMS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d items has been reached.",
                MAX_ITEMS );

        break;
      }

      _fmemset( szNewItemName, 0, sizeof(szNewItemName) );
      _fmemset( nNewIndex, 0, sizeof(nNewIndex) );
      nNewItemType = -1;
      dNewPixelSize = 15.62;

      for(j = 0; j < MAX_ELEMENTS; ++j)
      {
        nNewIndex[j] = -1;
      }

      if (EditItemDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_ITEM_LIST);

      ListBox_AddString(hwndList, szNewItemName);

      _fmemset(&po->pItemData[i], 0, sizeof(ITEM_DATA));
      _fstrcpy(po->pItemData[i].item_name, szNewItemName);
      po->pItemData[i].pixel_size = (float)dNewPixelSize;
      po->pItemData[i].item_type = nNewItemType;

      n = GetNumItemElements(nNewItemType);

      for(j = 0; j < n; ++j)
      {
        po->pItemData[i].index[j] = nNewIndex[j];
      }

      ++po->nItems;

      ListBox_SetCurSel(hwndList, i);
      ViewItems_ItemChanged(hwnd);

      bChange = TRUE;

      break;

    case IDC_ITEM_DELETE:

      hwndList = GetDlgItem( hwnd, IDC_ITEM_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nItems )
      {
        i = (INT)lResult;
        Object_DeleteItem( po, i );
        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        ViewItems_ItemChanged( hwnd );

        bChange = TRUE;
      }

      break;

    case IDC_ITEM_EDIT:

      hwndList = GetDlgItem( hwnd, IDC_ITEM_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nItems )
      {
        i = (INT)lResult;

        _fstrcpy( szNewItemName, po->pItemData[i].item_name );
        dNewPixelSize = (double)po->pItemData[i].pixel_size;
        nNewItemType = po->pItemData[i].item_type;

        n = GetNumItemElements( nNewItemType );

        for( j = 0; j < n; ++j )
        {
          nNewIndex[j] = po->pItemData[i].index[j];
        }

        if ( EditItemDialog( hwnd ) == IDCANCEL )
        {
          break;
        }

        if ( !szNewItemName[0] )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "The item must be given a name." );

          break;
        }

        _fmemset( &po->pItemData[i], 0, sizeof(ITEM_DATA) );
        _fstrcpy( po->pItemData[i].item_name, szNewItemName );
        po->pItemData[i].pixel_size = (float)dNewPixelSize;
        po->pItemData[i].item_type = nNewItemType;

        n = GetNumItemElements( nNewItemType );

        for( j = 0; j < n; ++j )
        {
          po->pItemData[i].index[j] = nNewIndex[j];
        }

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < po->nItems; ++i )
        {
          ListBox_AddString( hwndList, po->pItemData[i].item_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

        ViewItems_ItemChanged( hwnd );

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
  
} // ViewItems_OnCommand


/*******************************************************************

    NAME:       ViewItems_OnInitDialog

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

BOOL ViewItems_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_ITEM_LIST );

  for( i = 0; i < po->nItems; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pItemData[i].item_name );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  hwndCtrl = GetDlgItem( hwnd, IDC_ITEM_TYPE );
  Static_SetText( hwndCtrl, GetItemTypeString( po->pItemData[0].item_type ) );

  hwndCtrl = GetDlgItem( hwnd, IDC_ITEM_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );

  ViewItems_ItemChanged( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewItems_OnInitDialog


/*******************************************************************

    NAME:       EditItemDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

INT EditItemDialog( HWND hwndParent )
{
  INT nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance( (FARPROC)EditItem_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_EDIT_ITEM,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditItemDialog


/*******************************************************************

    NAME:       EditItem_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditItem_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditItem_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditItem_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy  );
  }

  return FALSE;

} // EditItem_DlgProc


/*******************************************************************

    NAME:       EditItem_ElementChanged

    SYNOPSIS:   Handles element selection changes.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID EditItem_ElementChanged( HWND hwnd )
{
  INT i, j, n;
  HWND hwndList;
  LRESULT lResult;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_EDIT_ITEM_ELEMENTS ) );

  if ( lResult != LB_ERR )
  {
    hwndList = GetDlgItem( hwnd, IDC_EDIT_ITEM_RESOURCES );

    i = nNewIndex[lResult];

    n = GetNumItemBitmaps( nNewItemType );

    if ( lResult < n )
    {
      if ( nListContent != 0 )
      {
        ListBox_ResetContent( hwndList );

        for( j = 0; j < po->nBitmaps; ++j )
        {
          ListBox_AddString( hwndList, po->pBitmapData[j].bitmap_name );
        }

        nListContent = 0;
      }

      if ( i >= 0 && i < po->nBitmaps )
      {
        ListBox_SetCurSel( hwndList, i );

        PreviewBitmap( hwnd,
                       IDC_EDIT_ITEM_PREVIEW,
                       IDC_EDIT_ITEM_BITMAP_SIZE,
                       &po->pBitmapData[i],
                       po->pPalette );
      }
      else
      {
        ListBox_SetCurSel( hwndList, -1 );

        PreviewBitmap( hwnd,
                       IDC_EDIT_ITEM_PREVIEW,
                       IDC_EDIT_ITEM_BITMAP_SIZE,
                       NULL,
                       NULL );
      }
    }
    else
    {
      if ( nListContent != 1 )
      {
        ListBox_ResetContent( hwndList );

        for( j = 0; j < po->nSounds; ++j )
        {
          ListBox_AddString( hwndList, po->pSoundData[j].sound_name );
        }

        nListContent = 1;
      }

      if ( i >= 0 && i < po->nSounds )
      {
        ListBox_SetCurSel( hwndList, i );
      }
      else
      {
        ListBox_SetCurSel( hwndList, -1 );
      }

      PreviewBitmap( hwnd,
                     IDC_EDIT_ITEM_PREVIEW,
                     IDC_EDIT_ITEM_BITMAP_SIZE,
                     NULL,
                     NULL );
    }
  }

  return;
  
} // EditItem_ElementChanged


/*******************************************************************

    NAME:       EditItem_TypeChanged

    SYNOPSIS:   Handles changes of the item type.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditItem_TypeChanged( HWND hwnd )
{
  INT i, n;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_ITEM_ELEMENTS );

  ListBox_ResetContent( hwndCtrl );

  n = GetNumItemElements( nNewItemType );

  for( i = 0; i < n; ++i )
  {
    ListBox_AddString( hwndCtrl, GetItemElementString( nNewItemType, i ) );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  EditItem_ElementChanged( hwnd );

  return;

} // EditItem_TypeChanged


/*******************************************************************

    NAME:       EditItem_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditItem_OnCommand( HWND hwnd,
                         INT  id,
                         HWND hwndCtrl,
                         UINT codeNotify )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_ITEM_ELEMENTS:

      if ( codeNotify == LBN_SELCHANGE )
      {
        EditItem_ElementChanged( hwnd );
      }
      break;

    case IDC_EDIT_ITEM_RESOURCES:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        i = (INT)lResult;

        lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_EDIT_ITEM_ELEMENTS ) );

        if ( lResult != LB_ERR )
        {
          nNewIndex[lResult] = i;
        }

        if ( nListContent == 0 && i >= 0 && i < po->nBitmaps )
        {
          PreviewBitmap( hwnd,
                         IDC_EDIT_ITEM_PREVIEW,
                         IDC_EDIT_ITEM_BITMAP_SIZE,
                         &po->pBitmapData[i],
                         po->pPalette );
        }
        else
        {
          PreviewBitmap( hwnd,
                         IDC_EDIT_ITEM_PREVIEW,
                         IDC_EDIT_ITEM_BITMAP_SIZE,
                         NULL,
                         NULL );
        }
      }
      break;

    case IDC_EDIT_ITEM_TYPE:

      if ( codeNotify == CBN_SELCHANGE )
      {
        lResult = ComboBox_GetCurSel( hwndCtrl );

        if ( lResult >= 0 && lResult < NUM_ITEM_TYPES )
        {
          nNewItemType = (INT)lResult;

          EditItem_TypeChanged( hwnd );
        }
      }
      break;

    case IDC_EDIT_ITEM_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewPixelSize = atof( szBuf );
      }
      break;

    case IDC_EDIT_ITEM_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewItemName),
                     (LPARAM)((LPSTR)szNewItemName) );
      }
      break;
      
    case IDOK:

      if ( nNewItemType == -1 )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The item must be assigned a type." );

        break;
      }

      if ( !szNewItemName[0] )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The item must be given a name." );

        break;
      }

      EndDialog( hwnd, id );
      break;
      
    case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // EditItem_OnCommand


/*******************************************************************

    NAME:       EditItem_OnInitDialog

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

BOOL EditItem_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  CHAR szBuf[128];
  HWND hwndCtrl;

  nListContent = -1;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_ITEM_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_ITEM_NAME );
  Edit_LimitText( hwndCtrl, 30 );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_ITEM_TYPE );

  for( i = 0; i < NUM_ITEM_TYPES; ++i )
  {
    ComboBox_AddString( hwndCtrl, GetItemTypeString( i ) );
  }

  ComboBox_SetCurSel( hwndCtrl, nNewItemType );

  DoubleToString( szBuf, dNewPixelSize, 2 );
  Edit_SetText( GetDlgItem( hwnd, IDC_EDIT_ITEM_PIXEL_SIZE ), szBuf );

  Edit_SetText( GetDlgItem( hwnd, IDC_EDIT_ITEM_NAME ), szNewItemName );

  EditItem_TypeChanged( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // EditItem_OnInitDialog
