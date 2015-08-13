/*******************************************************************

    statsdlg.c

    Routines for displaying and managing the statistics dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewStatistics_DlgProc( HWND   hwnd,
                                               UINT   nMessage,
                                               WPARAM wParam,
                                               LPARAM lParam );

VOID ViewStatistics_OnCommand( HWND hwnd,
                               INT  id,
                               HWND hwndCtrl,
                               UINT codeNotify );

BOOL ViewStatistics_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam );


//
//  Private data.
//

static WND_DATA *po;
static LEVEL_DATA *pLevel;


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewStatisticsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewStatisticsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  po = pWndData;
  pLevel = pWndData->pLevelData + pWndData->level_index;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewStatistics_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_STATISTICS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );
    
  return nResult;
    
} // ViewStatisticsDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewStatistics_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewStatistics_DlgProc( HWND   hwnd,
                                               UINT   nMessage,
                                               WPARAM wParam,
                                               LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,     ViewStatistics_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG,  ViewStatistics_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,    AllDialogs_OnCtlColor       );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,     AllDialogs_OnDestroy        );
  }

  return FALSE;

} // ViewStatistics_DlgProc


/*******************************************************************

    NAME:       ViewStatistics_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewStatistics_OnCommand( HWND hwnd,
                               INT  id,
                               HWND hwndCtrl,
                               UINT codeNotify )
{
  switch(id)
  {
    case IDOK: case IDCANCEL:

      if ( hPalCommon )
      {
        DeleteObject( hPalCommon );
        hPalCommon = NULL;
      }

      EndDialog( hwnd, id );
      break;
  }
  
} // ViewStatistics_OnCommand


/*******************************************************************

    NAME:       ViewStatistics_OnInitDialog

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

BOOL ViewStatistics_OnInitDialog( HWND hwnd, HWND hwndFocus, LPARAM lParam )
{
  INT i, tabs[3], nSelectedVertices;
  HWND hwndCtrl;
  VERTEX huge *pVertex;
  INT nLifeForms[MAX_LIFEFORMS*3];
  INT nItems[MAX_ITEMS*3];
  char szBuf[128];

  _fmemset(nLifeForms, 0, sizeof(nLifeForms));
  _fmemset(nItems, 0, sizeof(nItems));

  nSelectedVertices = CountSelectedVertices(pLevel);

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    pVertex = pLevel->pVertexData + i;

    if (nSelectedVertices > 0 && (pVertex->flags & VF_SELECTED) == 0)
    {
      continue;
    }

    if (pVertex->flags & VF_LIFEFORM)
    {
      if (pVertex->lifeform_index >= 0 && pVertex->lifeform_index < po->nLifeForms)
      {
        if (pVertex->flags & VF_DIFFICULTY_LEVEL_1)
        {
          ++nLifeForms[pVertex->lifeform_index];
        }
        if (pVertex->flags & VF_DIFFICULTY_LEVEL_2)
        {
          ++nLifeForms[pVertex->lifeform_index + MAX_LIFEFORMS];
        }
        if (pVertex->flags & VF_DIFFICULTY_LEVEL_3)
        {
          ++nLifeForms[pVertex->lifeform_index + MAX_LIFEFORMS * 2];
        }
      }
    }
    else if (pVertex->flags & VF_ITEM)
    {
      if (pVertex->item_index >= 0 && pVertex->item_index < po->nItems)
      {
        if (pVertex->flags & VF_DIFFICULTY_LEVEL_1)
        {
          ++nItems[pVertex->item_index];
        }
        if (pVertex->flags & VF_DIFFICULTY_LEVEL_2)
        {
          ++nItems[pVertex->item_index + MAX_ITEMS];
        }
        if (pVertex->flags & VF_DIFFICULTY_LEVEL_3)
        {
          ++nItems[pVertex->item_index + MAX_ITEMS * 2];
        }
      }
    }
  }

  hwndCtrl = GetDlgItem( hwnd, IDC_STATISTICS_LIST );

  tabs[0] = 96;
  tabs[1] = tabs[0] + 32;
  tabs[2] = tabs[1] + 32;

  ListBox_SetTabStops(hwndCtrl, 3, &tabs);

  ListBox_AddString( hwndCtrl, "Difficulty Level\t1\t2\t3" );
  ListBox_AddString( hwndCtrl, "" );

  for(i = 0; i < po->nLifeForms; ++i)
  {
    if (nLifeForms[i] > 0 ||
        nLifeForms[i + MAX_LIFEFORMS] > 0 ||
        nLifeForms[i + MAX_LIFEFORMS * 2] > 0)
    {
      wsprintf(szBuf,
        "%s\t%d\t%d\t%d",
        po->pLifeFormData[i].lifeform_name,
        nLifeForms[i],
        nLifeForms[i + MAX_LIFEFORMS],
        nLifeForms[i + MAX_LIFEFORMS * 2]);

      ListBox_AddString( hwndCtrl, szBuf );
    }
  }

  for(i = 0; i < po->nItems; ++i)
  {
    if (nItems[i] > 0 ||
        nItems[i + MAX_ITEMS] > 0 ||
        nItems[i + MAX_ITEMS * 2] > 0)
    {
      wsprintf(szBuf,
        "%s\t%d\t%d\t%d",
        po->pItemData[i].item_name,
        nItems[i],
        nItems[i + MAX_ITEMS],
        nItems[i + MAX_ITEMS * 2]);

      ListBox_AddString( hwndCtrl, szBuf );
    }
  }

  ListBox_SetCurSel( hwndCtrl, 2 );

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewStatistics_OnInitDialog
