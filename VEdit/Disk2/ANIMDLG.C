/*******************************************************************

    animdlg.c

    Routines for displaying and managing the animations dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewAnimations_DlgProc( HWND   hwnd,
                                               UINT   nMessage,
                                               WPARAM wParam,
                                               LPARAM lParam );

VOID ViewAnimations_AnimationChanged( HWND hwnd );

VOID ViewAnimations_OnCommand( HWND hwnd,
                               INT  id,
                               HWND hwndCtrl,
                               UINT codeNotify );

BOOL ViewAnimations_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );

INT EditAnimationDialog( HWND hwndParent );

BOOL CALLBACK __export EditAnimation_DlgProc( HWND   hwnd,
                                              UINT   nMessage,
                                              WPARAM wParam,
                                              LPARAM lParam );

VOID EditAnimation_ElementChanged( HWND hwnd );

VOID EditAnimation_OnCommand( HWND hwnd,
                              INT  id,
                              HWND hwndCtrl,
                              UINT codeNotify );

BOOL EditAnimation_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static INT nListContent;
static CHAR szNewAnimationName[32];
static INT nNewNumElements;
static INT nNewDelay;
static INT nNewFirstElement;
static INT nNewIndex[MAX_ELEMENTS];
static double dNewPixelSize;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewAnimationsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewAnimationsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewAnimations_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_ANIMATIONS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewAnimationsDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewAnimations_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewAnimations_DlgProc( HWND   hwnd,
                                               UINT   nMessage,
                                               WPARAM wParam,
                                               LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewAnimations_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewAnimations_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor       );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy        );
  }

  return FALSE;

} // ViewAnimations_DlgProc


/*******************************************************************

    NAME:       ViewAnimations_AnimationChanged

    SYNOPSIS:   Handles a change in currently selected animation.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID ViewAnimations_AnimationChanged( HWND hwnd )
{
  INT i, j, k;
  CHAR szBuf[128];
  HWND hwndList;
  LRESULT lResult;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_ANIMATION_LIST ) );

  if ( lResult >= 0 && lResult < po->nAnimations )
  {
    i = (INT)lResult;

    hwndList = GetDlgItem( hwnd, IDC_ANIMATION_RESOURCE_LIST );

    ListBox_ResetContent( hwndList );

    for( j = 0; j < po->pAnimationData[i].num_elements; ++j )
    {
      k = po->pAnimationData[i].index[j];

      if (k != -1)
      {
        if (k & AF_SOUND_RESOURCE)
        {
          k &= ~AF_SOUND_RESOURCE;

          if (k >= 0 && k < po->nSounds)
          {
            ListBox_AddString( hwndList, po->pSoundData[k].sound_name );
          }
        }
        else if (k >= 0 && k < po->nBitmaps)
        {
          ListBox_AddString( hwndList, po->pBitmapData[k].bitmap_name );
        }
      }
    }

    k = po->pAnimationData[i].first_element;

    ListBox_SetCurSel( hwndList, k );

    j = po->pAnimationData[i].index[k];

    if ( (j & AF_SOUND_RESOURCE) == 0 && j >= 0 && j < po->nBitmaps )
    {
      PreviewBitmap( hwnd,
                     IDC_ANIMATION_BITMAP_PREVIEW,
                     IDC_ANIMATION_BITMAP_SIZE,
                     &po->pBitmapData[j],
                     po->pPalette );
    }
    else
    {
      PreviewBitmap( hwnd,
                     IDC_ANIMATION_BITMAP_PREVIEW,
                     IDC_ANIMATION_BITMAP_SIZE,
                     NULL,
                     NULL );
    }

    wsprintf( szBuf, "%d", po->pAnimationData[i].num_elements );
    Static_SetText( GetDlgItem( hwnd, IDC_ANIMATION_NUM_ELEMENTS ), szBuf );

    wsprintf( szBuf, "%d", po->pAnimationData[i].delay );
    Static_SetText( GetDlgItem( hwnd, IDC_ANIMATION_DELAY ), szBuf );

    DoubleToString( szBuf, po->pAnimationData[i].pixel_size, 2 );
    Edit_SetText( GetDlgItem( hwnd, IDC_ANIMATION_PIXEL_SIZE ), szBuf );
  }

  return;
  
} // ViewAnimations_AnimationChanged


/*******************************************************************

    NAME:       ViewAnimations_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewAnimations_OnCommand( HWND hwnd,
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
    case IDC_ANIMATION_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        ViewAnimations_AnimationChanged( hwnd );
      }
      break;

    case IDC_ANIMATION_RESOURCE_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        if ( lResult != LB_ERR )
        {
          i = (INT)lResult;

          lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_ANIMATION_LIST ) );

          if ( lResult != LB_ERR )
          {
            po->pAnimationData[lResult].first_element = i;
            i = po->pAnimationData[lResult].index[i];

            if (i != -1)
            {
              if (i & AF_SOUND_RESOURCE)
              {
                i = -1;
              }
            }

            bChange = TRUE;
          }
          else
          {
            i = -1;
          }

          if ( i >= 0 && i < po->nBitmaps )
          {
            PreviewBitmap( hwnd,
                           IDC_ANIMATION_BITMAP_PREVIEW,
                           IDC_ANIMATION_BITMAP_SIZE,
                           &po->pBitmapData[i],
                           po->pPalette );
          }
          else
          {
            PreviewBitmap( hwnd,
                           IDC_ANIMATION_BITMAP_PREVIEW,
                           IDC_ANIMATION_BITMAP_SIZE,
                           NULL,
                           NULL );
          }
        }
      }
      break;

    case IDC_ANIMATION_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_ANIMATION_LIST ) );

        if ( lResult >= 0 && lResult < po->nAnimations )
        {
          i = (INT)lResult;

          SendMessage( hwndCtrl,
                       WM_GETTEXT,
                       (WPARAM)sizeof(szBuf),
                       (LPARAM)((LPSTR)szBuf) );

          po->pAnimationData[i].pixel_size = (float)atof( szBuf );

          bChange = TRUE;
        }
      }
      break;

    case IDC_ANIMATION_NEW:

      i = po->nAnimations;

      if (i == MAX_ANIMATIONS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d animations has been reached.",
                MAX_ANIMATIONS );

        break;
      }

      _fmemset(szNewAnimationName, 0, sizeof(szNewAnimationName));

      nNewNumElements = 0;
      nNewDelay = 1;
      nNewFirstElement = 0;
      dNewPixelSize = 15.62;

      for(j = 0; j < MAX_ELEMENTS; ++j)
      {
        nNewIndex[j] = -1;
      }

      if (EditAnimationDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_ANIMATION_LIST);

      ListBox_AddString(hwndList, szNewAnimationName);

      _fmemset(&po->pAnimationData[i], 0, sizeof(ANIMATION_DATA));
      _fstrcpy(po->pAnimationData[i].animation_name, szNewAnimationName);
      po->pAnimationData[i].pixel_size = (float)dNewPixelSize;
      po->pAnimationData[i].num_elements = nNewNumElements;
      po->pAnimationData[i].delay = nNewDelay;
      po->pAnimationData[i].first_element = nNewFirstElement;

      for(j = 0; j < nNewNumElements; ++j)
      {
        po->pAnimationData[i].index[j] = nNewIndex[j];
      }

      ++po->nAnimations;

      ListBox_SetCurSel(hwndList, i);
      ViewAnimations_AnimationChanged(hwnd);

      bChange = TRUE;

      break;

    case IDC_ANIMATION_DELETE:

      hwndList = GetDlgItem( hwnd, IDC_ANIMATION_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nAnimations )
      {
        i = (INT)lResult;
        Object_DeleteAnimation( po, i );
        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        ViewAnimations_AnimationChanged( hwnd );

        bChange = TRUE;
      }

      break;

    case IDC_ANIMATION_EDIT:

      hwndList = GetDlgItem( hwnd, IDC_ANIMATION_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nAnimations )
      {
        i = (INT)lResult;

        _fstrcpy( szNewAnimationName, po->pAnimationData[i].animation_name );
        dNewPixelSize = (double)po->pAnimationData[i].pixel_size;
        nNewNumElements = po->pAnimationData[i].num_elements;
        nNewDelay = po->pAnimationData[i].delay;
        nNewFirstElement = po->pAnimationData[i].first_element;

        for(j = 0; j < MAX_ELEMENTS; ++j)
        {
          if (j < nNewNumElements)
          {
            nNewIndex[j] = po->pAnimationData[i].index[j];
          }
          else
          {
            nNewIndex[j] = -1;
          }
        }

        if ( EditAnimationDialog( hwnd ) == IDCANCEL )
        {
          break;
        }

        _fmemset( &po->pAnimationData[i], 0, sizeof(ANIMATION_DATA) );
        _fstrcpy( po->pAnimationData[i].animation_name, szNewAnimationName );
        po->pAnimationData[i].pixel_size = (float)dNewPixelSize;
        po->pAnimationData[i].num_elements = nNewNumElements;
        po->pAnimationData[i].delay = nNewDelay;
        po->pAnimationData[i].first_element = nNewFirstElement;

        for( j = 0; j < nNewNumElements; ++j )
        {
          po->pAnimationData[i].index[j] = nNewIndex[j];
        }

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < po->nAnimations; ++i )
        {
          ListBox_AddString( hwndList, po->pAnimationData[i].animation_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

        ViewAnimations_AnimationChanged( hwnd );

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
  
} // ViewAnimations_OnCommand


/*******************************************************************

    NAME:       ViewAnimations_OnInitDialog

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

BOOL ViewAnimations_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  CHAR szBuf[128];
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_ANIMATION_LIST );

  for( i = 0; i < po->nAnimations; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pAnimationData[i].animation_name );
  }

  ListBox_SetCurSel( hwndCtrl, 0 );

  hwndCtrl = GetDlgItem( hwnd, IDC_ANIMATION_NUM_ELEMENTS );
  wsprintf( szBuf, "%d", po->pAnimationData[0].num_elements );
  Edit_SetText( hwndCtrl, szBuf );

  hwndCtrl = GetDlgItem( hwnd, IDC_ANIMATION_DELAY );
  wsprintf( szBuf, "%d", po->pAnimationData[0].delay );
  Edit_SetText( hwndCtrl, szBuf );

  hwndCtrl = GetDlgItem( hwnd, IDC_ANIMATION_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );

  ViewAnimations_AnimationChanged( hwnd );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewAnimations_OnInitDialog


/*******************************************************************

    NAME:       EditAnimationDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

INT EditAnimationDialog( HWND hwndParent )
{
  INT nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance( (FARPROC)EditAnimation_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_EDIT_ANIMATION,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // EditAnimationDialog


/*******************************************************************

    NAME:       EditAnimation_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditAnimation_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditAnimation_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditAnimation_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor      );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy       );
  }

  return FALSE;

} // EditAnimation_DlgProc


/*******************************************************************

    NAME:       EditAnimation_ElementChanged

    SYNOPSIS:   Handles element selection changes.

    ENTRY:      hwnd - Dialog box window handle.

********************************************************************/

VOID EditAnimation_ElementChanged( HWND hwnd )
{
  INT i, j, nPreview;
  HWND hwndList;
  LRESULT lResult;

  nPreview = -1;

  lResult = ListBox_GetCurSel( GetDlgItem( hwnd, IDC_EDIT_ANIMATION_ELEMENTS ) );

  if ( lResult >= 0 && lResult < nNewNumElements )
  {
    nNewFirstElement = (INT)lResult;

    hwndList = GetDlgItem( hwnd, IDC_EDIT_ANIMATION_RESOURCES );

    i = nNewIndex[nNewFirstElement];

    if (i != -1 && (i & AF_SOUND_RESOURCE))
    {
      if (nListContent != 1)
      {
        ListBox_ResetContent(hwndList);

        for(j = 0; j < po->nSounds; ++j)
        {
          ListBox_AddString(hwndList, po->pSoundData[j].sound_name);
        }

        nListContent = 1;
      }

      j = i & ~AF_SOUND_RESOURCE;

      if (j >= 0 && j < po->nSounds)
      {
        ListBox_SetCurSel( hwndList, j );
      }
      else
      {
        ListBox_SetCurSel( hwndList, -1 );
      }
    }
    else
    {
      if (nListContent != 0)
      {
        ListBox_ResetContent(hwndList);

        for(j = 0; j < po->nBitmaps; ++j)
        {
          ListBox_AddString(hwndList, po->pBitmapData[j].bitmap_name);
        }

        nListContent = 0;
      }

      if (i >= 0 && i < po->nBitmaps)
      {
        ListBox_SetCurSel( hwndList, i );
        nPreview = i;
      }
      else
      {
        ListBox_SetCurSel( hwndList, -1 );
      }
    }

    if (nPreview != -1)
    {
      PreviewBitmap( hwnd,
                     IDC_EDIT_ANIMATION_PREVIEW,
                     IDC_EDIT_ANIMATION_BITMAP_SIZE,
                     &po->pBitmapData[nPreview],
                     po->pPalette );
    }
    else
    {
      PreviewBitmap( hwnd,
                     IDC_EDIT_ANIMATION_PREVIEW,
                     IDC_EDIT_ANIMATION_BITMAP_SIZE,
                     NULL,
                     NULL );
    }
  }

  return;
  
} // EditAnimation_ElementChanged


/*******************************************************************

    NAME:       EditAnimation_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditAnimation_OnCommand( HWND hwnd,
                              INT  id,
                              HWND hwndCtrl,
                              UINT codeNotify )
{
  INT i;
  CHAR szBuf[128];
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_ANIMATION_ELEMENTS:

      if ( codeNotify == LBN_SELCHANGE )
      {
        EditAnimation_ElementChanged( hwnd );
      }
      break;

    case IDC_EDIT_ANIMATION_RESOURCES:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtrl );

        hwndList = GetDlgItem( hwnd, IDC_EDIT_ANIMATION_ELEMENTS );

        if (nListContent == 0)
        {
          if ( lResult >= 0 && lResult < po->nBitmaps )
          {
            i = (INT)lResult;

            lResult = ListBox_GetCurSel(hwndList);

            if ( lResult >= 0 && lResult < nNewNumElements )
            {
              nNewIndex[lResult] = i;
              ListBox_DeleteString(hwndList, (int)lResult);
              ListBox_InsertString(hwndList, (int)lResult, po->pBitmapData[i].bitmap_name);
              ListBox_SetCurSel(hwndList, (int)lResult);
            }

            PreviewBitmap( hwnd,
                           IDC_EDIT_ANIMATION_PREVIEW,
                           IDC_EDIT_ANIMATION_BITMAP_SIZE,
                           &po->pBitmapData[i],
                           po->pPalette );
          }
          else
          {
            PreviewBitmap( hwnd,
                           IDC_EDIT_ANIMATION_PREVIEW,
                           IDC_EDIT_ANIMATION_BITMAP_SIZE,
                           NULL,
                           NULL );
          }
        }
        else
        {
          if ( lResult >= 0 && lResult < po->nSounds )
          {
            i = (INT)lResult;

            lResult = ListBox_GetCurSel(hwndList);

            if ( lResult >= 0 && lResult < nNewNumElements )
            {
              nNewIndex[lResult] = i | AF_SOUND_RESOURCE;
              ListBox_DeleteString(hwndList, (int)lResult);
              ListBox_InsertString(hwndList, (int)lResult, po->pSoundData[i].sound_name);
              ListBox_SetCurSel(hwndList, (int)lResult);
            }
          }

          PreviewBitmap( hwnd,
                         IDC_EDIT_ANIMATION_PREVIEW,
                         IDC_EDIT_ANIMATION_BITMAP_SIZE,
                         NULL,
                         NULL );
        }
      }
      break;

    case IDC_EDIT_ANIMATION_DELAY:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        nNewDelay = atoi( szBuf );
      }
      break;

    case IDC_EDIT_ANIMATION_PIXEL_SIZE:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szBuf),
                     (LPARAM)((LPSTR)szBuf) );

        dNewPixelSize = atof( szBuf );
      }
      break;

    case IDC_EDIT_ANIMATION_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewAnimationName),
                     (LPARAM)((LPSTR)szNewAnimationName) );
      }
      break;

    case IDC_EDIT_ANIMATION_SHOW_BITMAPS:

      hwndList = GetDlgItem(hwnd, IDC_EDIT_ANIMATION_RESOURCES);

      ListBox_ResetContent(hwndList);

      for(i = 0; i < po->nBitmaps; ++i)
      {
        ListBox_AddString(hwndList, po->pBitmapData[i].bitmap_name);
      }

      nListContent = 0;

      lResult = ListBox_GetCurSel(GetDlgItem(hwnd, IDC_EDIT_ANIMATION_ELEMENTS));

      if (lResult >= 0 && lResult < nNewNumElements)
      {
        if (nNewIndex[lResult] != -1)
        {
          if ((nNewIndex[lResult] & AF_SOUND_RESOURCE) == 0)
          {
            ListBox_SetCurSel(hwndList, nNewIndex[lResult]);
          }
        }
      }

      break;

    case IDC_EDIT_ANIMATION_SHOW_SOUNDS:

      hwndList = GetDlgItem(hwnd, IDC_EDIT_ANIMATION_RESOURCES);

      ListBox_ResetContent(hwndList);

      for(i = 0; i < po->nSounds; ++i)
      {
        ListBox_AddString(hwndList, po->pSoundData[i].sound_name);
      }

      nListContent = 1;

      lResult = ListBox_GetCurSel(GetDlgItem(hwnd, IDC_EDIT_ANIMATION_ELEMENTS));

      if (lResult >= 0 && lResult < nNewNumElements)
      {
        if (nNewIndex[lResult] != -1)
        {
          if (nNewIndex[lResult] & AF_SOUND_RESOURCE)
          {
            ListBox_SetCurSel(hwndList, nNewIndex[lResult] & ~AF_SOUND_RESOURCE);
          }
        }
      }

      break;

    case IDC_EDIT_ANIMATION_APPEND:

      if (nNewNumElements == MAX_ELEMENTS)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_EDIT_ANIMATION_ELEMENTS);

      nNewIndex[nNewNumElements++] = -1;

      wsprintf(szBuf, "%d", nNewNumElements);
      Static_SetText(GetDlgItem(hwnd, IDC_EDIT_ANIMATION_NUM_ELEMENTS), szBuf);

//    wsprintf(szBuf, "Element %d", nNewNumElements);
      ListBox_AddString(hwndList, "<unassigned>");

      ListBox_SetCurSel(hwndList, nNewNumElements - 1);

      EditAnimation_ElementChanged(hwnd);

      break;

    case IDC_EDIT_ANIMATION_INSERT:

      if (nNewNumElements == MAX_ELEMENTS)
      {
        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_EDIT_ANIMATION_ELEMENTS);

      lResult = ListBox_GetCurSel(hwndList);

      if ((lResult >= 0 && lResult < nNewNumElements) || nNewNumElements == 0)
      {
        if (nNewNumElements == 0)
        {
          lResult = 0;
        }

        for(i = nNewNumElements; i > (int)lResult; --i)
        {
          nNewIndex[i] = nNewIndex[i - 1];
        }

        nNewIndex[i] = -1;
        ++nNewNumElements;
        wsprintf(szBuf, "%d", nNewNumElements);
        Static_SetText(GetDlgItem(hwnd, IDC_EDIT_ANIMATION_NUM_ELEMENTS), szBuf);

        ListBox_ResetContent(hwndList);

        for(i = 0; i < nNewNumElements; ++i)
        {
          if (nNewIndex[i] == -1)
          {
            ListBox_AddString(hwndList, "<unassigned>");
          }
          else if (nNewIndex[i] & AF_SOUND_RESOURCE)
          {
            ListBox_AddString(hwndList, po->pSoundData[nNewIndex[i] & ~AF_SOUND_RESOURCE].sound_name);
          }
          else
          {
            ListBox_AddString(hwndList, po->pBitmapData[nNewIndex[i]].bitmap_name);
          }
        }

        ListBox_SetCurSel(hwndList, (int)lResult + 1);

        EditAnimation_ElementChanged(hwnd);
      }

      break;

    case IDC_EDIT_ANIMATION_DELETE:

      hwndList = GetDlgItem(hwnd, IDC_EDIT_ANIMATION_ELEMENTS);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < nNewNumElements)
      {
        for(i = (int)lResult; i < nNewNumElements - 1; ++i)
        {
          nNewIndex[i] = nNewIndex[i + 1];
        }

        --nNewNumElements;
        wsprintf( szBuf, "%d", nNewNumElements );
        Static_SetText( GetDlgItem( hwnd, IDC_EDIT_ANIMATION_NUM_ELEMENTS ), szBuf );

        ListBox_ResetContent(hwndList);

        for(i = 0; i < nNewNumElements; ++i)
        {
          if (nNewIndex[i] == -1)
          {
            ListBox_AddString(hwndList, "<unassigned>");
          }
          else if (nNewIndex[i] & AF_SOUND_RESOURCE)
          {
            ListBox_AddString(hwndList, po->pSoundData[nNewIndex[i] & ~AF_SOUND_RESOURCE].sound_name);
          }
          else
          {
            ListBox_AddString(hwndList, po->pBitmapData[nNewIndex[i]].bitmap_name);
          }
        }

        if ((int)lResult == nNewNumElements)
        {
          --lResult;
        }

        ListBox_SetCurSel(hwndList, (int)lResult);
        EditAnimation_ElementChanged(hwnd);
      }

      break;
      
    case IDOK:

      if ( !szNewAnimationName[0] )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The animations must be given a name." );

        break;
      }

      if ( nNewNumElements <= 0 )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "There must be at least 1 frame in the animation." );

        break;
      }

      if ( nNewDelay <= 0 )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The delay must be >= 1" );

        break;
      }

      EndDialog( hwnd, id );
      break;
      
    case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // EditAnimation_OnCommand


/*******************************************************************

    NAME:       EditAnimation_OnInitDialog

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

BOOL EditAnimation_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i;
  CHAR szBuf[128];
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_ANIMATION_PIXEL_SIZE );
  Edit_LimitText( hwndCtrl, 8 );

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_ANIMATION_NAME );
  Edit_LimitText( hwndCtrl, 30 );

  nListContent = 0;

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_ANIMATION_RESOURCES );

  for( i = 0; i < po->nBitmaps; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pBitmapData[i].bitmap_name );
  }

  hwndCtrl = GetDlgItem( hwnd, IDC_EDIT_ANIMATION_ELEMENTS );

  for(i = 0; i < nNewNumElements; ++i)
  {
    if (nNewIndex[i] == -1)
    {
      ListBox_AddString(hwndCtrl, "<unassigned>");
    }
    else if (nNewIndex[i] & AF_SOUND_RESOURCE)
    {
      ListBox_AddString(hwndCtrl, po->pSoundData[nNewIndex[i] & ~AF_SOUND_RESOURCE].sound_name);
    }
    else
    {
      ListBox_AddString(hwndCtrl, po->pBitmapData[nNewIndex[i]].bitmap_name);
    }
  }

  ListBox_SetCurSel(hwndCtrl, nNewFirstElement);

  wsprintf( szBuf, "%d", nNewNumElements );
  Static_SetText( GetDlgItem( hwnd, IDC_EDIT_ANIMATION_NUM_ELEMENTS ), szBuf );

  wsprintf( szBuf, "%d", nNewDelay );
  Edit_SetText( GetDlgItem( hwnd, IDC_EDIT_ANIMATION_DELAY ), szBuf );

  DoubleToString( szBuf, dNewPixelSize, 2 );
  Edit_SetText( GetDlgItem( hwnd, IDC_EDIT_ANIMATION_PIXEL_SIZE ), szBuf );

  Edit_SetText( GetDlgItem( hwnd, IDC_EDIT_ANIMATION_NAME ), szNewAnimationName );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // EditAnimation_OnInitDialog
