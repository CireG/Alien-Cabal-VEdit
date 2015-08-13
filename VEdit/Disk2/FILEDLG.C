/*******************************************************************

    filedlg.c

    Routines for displaying and managing the file properties dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export FileInfo_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam );

VOID FileInfo_OnCommand( HWND hwnd,
                         INT  id,
                         HWND hwndCtl,
                         UINT codeNotify );

BOOL FileInfo_OnInitDialog( HWND   hwndDlg,
                            HWND   hwndFocus,
                            LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static char new_name[32];
static char new_author[32];


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewFileInfoDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewFileInfoDialog( HWND hwndParent, WND_DATA *pWndData )
{
  INT nResult;
  FARPROC pfnProc;

  po = pWndData;

  _fstrcpy( new_name, po->file_name );
  _fstrcpy( new_author, po->file_author );
  
  pfnProc = MakeProcInstance( (FARPROC)FileInfo_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_FILE_INFO,
                       hwndParent,
                       (DLGPROC)pfnProc );

  if ( nResult == IDOK )
  {
    Object_DeleteUndoData( hwndParent, pWndData );

    _fstrcpy( po->file_name, new_name );
    _fstrcpy( po->file_author, new_author );
  }

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // ViewFileInfoDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       FileInfo_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export FileInfo_DlgProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    FileInfo_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, FileInfo_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy  );
  }

  return FALSE;

} // FileInfo_DlgProc


/*******************************************************************

    NAME:       FileInfo_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID FileInfo_OnCommand( HWND hwnd,
                         INT  id,
                         HWND hwndCtl,
                         UINT codeNotify )
{
  switch(id)
  {
    case IDC_FILE_NAME:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(new_name),
                     (LPARAM)((LPSTR)new_name) );

        bChange = TRUE;
      }
      break;
      
    case IDC_FILE_AUTHOR:

      if ( codeNotify == EN_CHANGE )
      {
        SendMessage( hwndCtl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(new_author),
                     (LPARAM)((LPSTR)new_author) );

        bChange = TRUE;
      }
      break;
      
    case IDOK: case IDCANCEL:

      EndDialog( hwnd, id );
      break;
  }
  
} // FileInfo_OnCommand


/*******************************************************************

    NAME:       FileInfo_OnInitDialog

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

BOOL FileInfo_OnInitDialog( HWND   hwnd,
                            HWND   hwndFocus,
                            LPARAM lParam )
{
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_FILE_NAME );
  Edit_SetText( hwndCtrl, new_name );
  Edit_LimitText( hwndCtrl, sizeof(po->file_name) - 1 );

  hwndCtrl = GetDlgItem( hwnd, IDC_FILE_AUTHOR );
  Edit_SetText( hwndCtrl, new_author );
  Edit_LimitText( hwndCtrl, sizeof(po->file_author) - 1 );

  hwndCtrl = GetDlgItem( hwnd, IDC_FILE_DATE );
  Static_SetText( hwndCtrl, po->file_date );

  hwndCtrl = GetDlgItem( hwnd, IDC_FILE_VERSION );
  Static_SetText( hwndCtrl, po->file_version );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // FileInfo_OnInitDialog
