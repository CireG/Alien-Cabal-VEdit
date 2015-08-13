/*******************************************************************

    lfrmdlg.c

    Routines for displaying and managing the life form dialogs.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewLifeForms_DlgProc( HWND   hwnd,
                                              UINT   nMessage,
                                              WPARAM wParam,
                                              LPARAM lParam );

VOID ViewLifeForms_LifeFormChanged( HWND hwnd );

VOID ViewLifeForms_OnCommand( HWND hwnd,
                              INT  id,
                              HWND hwndCtrl,
                              UINT codeNotify );

BOOL ViewLifeForms_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

INT EditLifeFormDialog( HWND hwndParent );

BOOL CALLBACK __export EditLifeForm_DlgProc( HWND   hwnd,
                                             UINT   nMessage,
                                             WPARAM wParam,
                                             LPARAM lParam );

VOID EditLifeForm_ElementChanged( HWND hwnd );

VOID EditLifeForm_TypeChanged( HWND hwnd );

VOID EditLifeForm_OnCommand( HWND hwnd,
                             INT  id,
                             HWND hwndCtrl,
                             UINT codeNotify );

BOOL EditLifeForm_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static CHAR szNewLifeFormName[32];
static INT nNewLifeFormType;
static INT nNewIndex[MAX_ELEMENTS];
static INT nListContent;
static double dNewPixelSize;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewLifeFormsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewLifeFormsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewLifeForms_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_LIFEFORMS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewLifeFormsDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewLifeForms_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewLifeForms_DlgProc( HWND   hwnd,
                                              UINT   nMessage,
                                              WPARAM wParam,
                                              LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewLifeForms_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewLifeForms_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor      );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy       );
  }

  return FALSE;

} // ViewLifeForms_DlgProc


/*******************************************************************

    NAME:       ViewLifeForms_LifeFormChanged

    SYNOPSIS:   Handles a change in currently selected life form.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID ViewLifeForms_LifeFormChanged( HWND hwnd )
{
  INT i, j, k, num_elements, num_bitmaps;
  CHAR szBuf[128];
  HWND hwndList;
  LRESULT lResult;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_LIFEFORM_LIST ) );

  if ( lResult >= 0 && lResult < po->nLifeForms )
  {
    i = (INT)lResult;

    hwndList = GetDlgItem( hwnd, IDC_LIFEFORM_RESOURCES );

    ListBox_ResetContent( hwndList );

    num_elements = GetNumLifeFormElements( po->pLifeFormData[i].lifeform_type );
    num_bitmaps = GetNumLifeFormBitmaps( po->pLifeFormData[i].lifeform_type );

    for( j = 0; j < num_elements; ++j )
    {
      k = po->pLifeFormData[i].index[j];

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

    j = po->pLifeFormData[i].index[0];

    if ( j >= 0 && j < po->nBitmaps && num_bitmaps > 0 )
    {
      PreviewBitmap( hwnd,
                     IDC_LIFEFORM_BITMAP_PREVIEW,
                     IDC_LIFEFORM_BITMAP_SIZE,
                     &po->pBitmapData[j],
                     po->pPalette );
    }
    else
    {
      PreviewBitmap( hwnd,
                     IDC_LIFEFORM_BITMAP_PREVIEW,
                     IDC_LIFEFORM_BITMAP_SIZE,
                     NULL,
                     NULL );
    }

    Static_SetText( GetDlgItem( hwnd, IDC_LIFEFORM_TYPE ),
                    GetLifeFormTypeString( po->pLifeFormData[i].lifeform_type ) );

    DoubleToString( szBuf, po->pLifeFormData[i].pixel_size, 2 );
    Edit_SetText( GetDlgItem( hwnd, IDC_LIFEFORM_PIXEL_SIZE ), szBuf );
  }

  return;
  
} // ViewLifeForms_LifeFormChanged


/*******************************************************************

    NAME:       ViewLifeForms_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewLifeForms_OnCommand( HWND hwnd,
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
    case IDC_LIFEFORM_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        ViewLifeForms_LifeFormChanged( hwnd );
      }
      break;

    case IDC_LIFEFORM_RESOURCES:

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
                           IDC_LIFEFORM_BITMAP_PREVIEW,
                           IDC_LIFEFORM_BITMAP_SIZE,
                           &po->pBitmapData[j],
                           po->pPalette );
          }
          else
          {
            PreviewBitmap( hwnd,
                           IDC_LIFEFORM_BITMAP_PREVIEW,
                           IDC_LIFEFORM_BITMAP_SIZE,
                           NULL,
                           NULL );
          }
        }
      }
      break;

    case IDC_LIFEFORM_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_LIFEFORM_LIST ) );

        if ( lResult >= 0 && lResult < po->nLifeForms )
        {
          i = (INT)lResult;

          SendMessage( hwndCtrl,
                       WM_GETTEXT,
                       (WPARAM)sizeof(szBuf),
                       (LPARAM)((LPSTR)szBuf) );

          po->pLifeFormData[i].pixel_size = (float)atof( szBuf );

          bChange = TRUE;
        }
      }
      break;

    case IDC_LIFEFORM_NEW:

      i = po->nLifeForms;

      if (i == MAX_LIFEFORMS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d life forms has been reached.",
                MAX_LIFEFORMS );

        break;
      }

      _fmemset(szNewLifeFormName, 0, sizeof(szNewLifeFormName));

      nNewLifeFormType = -1;
      dNewPixelSize = 15.62;

      for(j = 0; j < MAX_ELEMENTS; ++j)
      {
        nNewIndex[j] = -1;
      }

      if (EditLifeFormDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_LIFEFORM_LIST);

      ListBox_AddString(hwndList, szNewLifeFormName);

      _fmemset(&po->pLifeFormData[i], 0, sizeof(LIFEFORM_DATA));
      _fstrcpy(po->pLifeFormData[i].lifeform_name, szNewLifeFormName);
      po->pLifeFormData[i].pixel_size = (float)dNewPixelSize;
      po->pLifeFormData[i].lifeform_type = nNewLifeFormType;

      n = GetNumLifeFormElements(nNewLifeFormType);

      for(j = 0; j < n; ++j)
      {
        po->pLifeFormData[i].index[j] = nNewIndex[j];
      }

      ++po->nLifeForms;

      ListBox_SetCurSel(hwndList, i);
      ViewLifeForms_LifeFormChanged(hwnd);

      bChange = TRUE;

      break;

    case IDC_LIFEFORM_DELETE:

      hwndList = GetDlgItem( hwnd, IDC_LIFEFORM_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nLifeForms )
      {
        i = (INT)lResult;
        Object_DeleteLifeForm( po, i );
        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        ViewLifeForms_LifeFormChanged( hwnd );

        bChange = TRUE;
      }

      break;

    case IDC_LIFEFORM_EDIT:

      hwndList = GetDlgItem( hwnd, IDC_LIFEFORM_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nLifeForms )
      {
        i = (INT)lResult;

        _fstrcpy( szNewLifeFormName, po->pLifeFormData[i].lifeform_name );
        dNewPixelSize = (double)po->pLifeFormData[i].pixel_size;
        nNewLifeFormType = po->pLifeFormData[i].lifeform_type;

        n = GetNumLifeFormElements( nNewLifeFormType );

        for( j = 0; j < n; ++j )
        {
          nNewIndex[j] = po->pLifeFormData[i].index[j];
        }

        if ( EditLifeFormDialog( hwnd ) == IDCANCEL )
        {
          break;
        }

        if ( !szNewLifeFormName[0] )
        {
          MsgBox( hwnd,
                  MB_ICONEXCLAMATION,
                  "The life forms must be given a name." );

          break;
        }

        _fmemset( &po->pLifeFormData[i], 0, sizeof(LIFEFORM_DATA) );
        _fstrcpy( po->pLifeFormData[i].lifeform_name, szNewLifeFormName );
        po->pLifeFormData[i].pixel_size = (float)dNewPixelSize;
        po->pLifeFormData[i].lifeform_type = nNewLifeFormType;

        n = GetNumLifeFormElements( nNewLifeFormType );

        for( j = 0; j < n; ++j )
        {
          po->pLifeFormData[i].index[j] = nNewIndex[j];
        }

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < po->nLifeForms; ++i )
        {
          ListBox_AddString( hwndList, po->pLifeFormData[i].lifeform_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

        ViewLifeForms_LifeFormChanged( hwnd );

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
  
} // ViewLifeForms_OnCommand


/*******************************************************************

    NAME:       ViewLifeForms_OnInitDialog

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

BOOL ViewLifeForms_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_LIFEFORM_LIST );

  for( i = 0; i < po->nLifeForms; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pLifeFormData[i].lifeform_name );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  hwndCtrl = GetDlgItem( hwnd, IDC_LIFEFORM_TYPE );
  Static_SetText( hwndCtrl, GetLifeFormTypeString( po->pLifeFormData[0].lifeform_type ) );

  hwndCtrl = GetDlgItem( hwnd, IDC_LIFEFORM_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );

  ViewLifeForms_LifeFormChanged( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewLifeForms_OnInitDialog


/*******************************************************************

    NAME:       EditLifeFormDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

INT EditLifeFormDialog( HWND hwndParent )
{
  INT nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance( (FARPROC)EditLifeForm_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_EDIT_LIFEFORM,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditLifeFormDialog


/*******************************************************************

    NAME:       EditLifeForm_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditLifeForm_DlgProc( HWND   hwnd,
                                             UINT   nMessage,
                                             WPARAM wParam,
                                             LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditLifeForm_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditLifeForm_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor     );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy      );
  }

  return FALSE;

} // EditLifeForm_DlgProc


/*******************************************************************

    NAME:       EditLifeForm_ElementChanged

    SYNOPSIS:   Handles element selection changes.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID EditLifeForm_ElementChanged( HWND hwnd )
{
  INT i, j, n;
  HWND hwndList;
  LRESULT lResult;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_ELEMENTS ) );

  if ( lResult != LB_ERR )
  {
    hwndList = GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_RESOURCES );

    i = nNewIndex[lResult];

    n = GetNumLifeFormBitmaps( nNewLifeFormType );

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
                       IDC_EDIT_LIFEFORM_PREVIEW,
                       IDC_EDIT_LIFEFORM_BITMAP_SIZE,
                       &po->pBitmapData[i],
                       po->pPalette );
      }
      else
      {
        ListBox_SetCurSel( hwndList, -1 );

        PreviewBitmap( hwnd,
                       IDC_EDIT_LIFEFORM_PREVIEW,
                       IDC_EDIT_LIFEFORM_BITMAP_SIZE,
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
                     IDC_EDIT_LIFEFORM_PREVIEW,
                     IDC_EDIT_LIFEFORM_BITMAP_SIZE,
                     NULL,
                     NULL );
    }
  }

  return;
  
} // EditLifeForm_ElementChanged


/*******************************************************************

    NAME:       EditLifeForm_TypeChanged

    SYNOPSIS:   Handles changes of the life form type.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditLifeForm_TypeChanged( HWND hwnd )
{
  INT i, n;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_ELEMENTS );

  ListBox_ResetContent( hwndCtrl );

  n = GetNumLifeFormElements( nNewLifeFormType );

  for( i = 0; i < n; ++i )
  {
    ListBox_AddString( hwndCtrl, GetLifeFormElementString( nNewLifeFormType, i ) );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  EditLifeForm_ElementChanged( hwnd );

  return;

} // EditLifeForm_TypeChanged


/*******************************************************************

    NAME:       EditLifeForm_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditLifeForm_OnCommand( HWND hwnd,
                             INT  id,
                             HWND hwndCtrl,
                             UINT codeNotify )
{
  INT i;
  CHAR szBuf[128];
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_LIFEFORM_ELEMENTS:

      if ( codeNotify == LBN_SELCHANGE )
      {
        EditLifeForm_ElementChanged( hwnd );
      }
      break;

    case IDC_EDIT_LIFEFORM_RESOURCES:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        i = (INT)lResult;

        lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_ELEMENTS ) );

        if ( lResult != LB_ERR )
        {
          nNewIndex[lResult] = i;
        }

        if ( nListContent == 0 && i >= 0 && i < po->nBitmaps )
        {
          PreviewBitmap( hwnd,
                         IDC_EDIT_LIFEFORM_PREVIEW,
                         IDC_EDIT_LIFEFORM_BITMAP_SIZE,
                         &po->pBitmapData[i],
                         po->pPalette );
        }
        else
        {
          PreviewBitmap( hwnd,
                         IDC_EDIT_LIFEFORM_PREVIEW,
                         IDC_EDIT_LIFEFORM_BITMAP_SIZE,
                         NULL,
                         NULL );
        }
      }
      break;

    case IDC_EDIT_LIFEFORM_TYPE:

      if ( codeNotify == CBN_SELCHANGE )
      {
        lResult = ComboBox_GetCurSel( hwndCtrl );

        if ( lResult >= 0 && lResult < NUM_LIFEFORM_TYPES )
        {
          nNewLifeFormType = (INT)lResult;

          EditLifeForm_TypeChanged( hwnd );
        }
      }
      break;

    case IDC_EDIT_LIFEFORM_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewPixelSize = atof( szBuf );
      }
      break;

    case IDC_EDIT_LIFEFORM_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewLifeFormName),
                     (LPARAM)((LPSTR)szNewLifeFormName) );
      }
      break;
      
    case IDOK:

      if ( nNewLifeFormType == -1 )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The life form must be assigned a type." );

        break;
      }

      if ( !szNewLifeFormName[0] )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The life form must be given a name." );

        break;
      }

      EndDialog( hwnd, id );
      break;
      
    case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // EditLifeForm_OnCommand


/*******************************************************************

    NAME:       EditLifeForm_OnInitDialog

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

BOOL EditLifeForm_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  CHAR szBuf[128];
  HWND hwndCtrl;

  nListContent = -1;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_NAME );
  Edit_LimitText( hwndCtrl, 30 );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_TYPE );

  for( i = 0; i < NUM_LIFEFORM_TYPES; ++i )
  {
    ComboBox_AddString( hwndCtrl, GetLifeFormTypeString( i ) );
  }

  ComboBox_SetCurSel( hwndCtrl, nNewLifeFormType );

  DoubleToString( szBuf, dNewPixelSize, 2 );
  Edit_SetText( GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_PIXEL_SIZE ), szBuf );

  Edit_SetText( GetDlgItem( hwnd, IDC_EDIT_LIFEFORM_NAME ), szNewLifeFormName );

  EditLifeForm_TypeChanged( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // EditLifeForm_OnInitDialog
