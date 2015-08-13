/*******************************************************************

    snddlg.c

    Routines for displaying and managing the sound dialog.

********************************************************************/


#include "vedit.h"
#include <mmsystem.h>


//
//  Private prototypes.
//

BOOL LoadSoundData( HWND hwndParent, SOUND_DATA *psd, LPSTR szFile );

BOOL ReloadSound( HWND hwndParent, WND_DATA *pWndData, INT index );

BOOL ImportSound( HWND hwndParent, WND_DATA *pWndData );

BOOL ExportSound( HWND hwndParent, WND_DATA *pWndData, INT index );

BOOL CALLBACK __export ViewSounds_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam );

VOID ViewSounds_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtl,
                           UINT codeNotify );

BOOL ViewSounds_OnInitDialog( HWND   hwndDlg,
                              HWND   hwndFocus,
                              LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static CHAR *szFilter = "WAV files (*.wav)\000*.wav\000"
                        "All Files (*.*)\000*.*\000";

//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewSoundsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewSoundsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewSounds_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_SOUNDS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewSoundsDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       LoadSoundData

    SYNOPSIS:   Load the given sound file into the index.

    ENTRY:      hwndParent - The parent window for this dialog.

                psd - The sound data structure pointer.

                szFile - The file name (full file spec).

    EXIT:       TRUE on success, FALSE on error.

********************************************************************/

BOOL LoadSoundData( HWND hwndParent, SOUND_DATA *psd, LPSTR szFile )
{
  int i, n, f_tag, f_channels, f_bits_per_sample;
  long f_len, f_sample_rate, data_offset, data_size;
  HFILE fh;
  BYTE huge *pData;
  CHAR szBuf[256];

  fh = _lopen( szFile, READ );

  if ( fh == HFILE_ERROR )
  {
    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Unable to open %s",
            (LPSTR)szFile );

    return FALSE;
  }

  _fmemset( szBuf, 0, sizeof(szBuf) );

  if ( _lread( fh, szBuf, sizeof(szBuf) ) < 4 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error reading sound file header for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  i = 0;
  n = 0;

  while( i < sizeof(szBuf) - 4 )
  {
    if ( _fstrnicmp( &szBuf[i], "RIFF", 4 ) == 0 )
    {
      n = 1;
      i += 8;
    }
    else if ( _fstrnicmp( &szBuf[i], "WAVE", 4 ) == 0 )
    {
      if ( n < 1 )
      {
        break;
      }

      n = 2;
      i += 4;
    }
    else if ( _fstrnicmp( &szBuf[i], "fmt", 3 ) == 0 )
    {
      if ( n < 2 )
      {
        break;
      }

      f_len = *((long *)(&szBuf[i+4]));
      f_tag = *((int *)(&szBuf[i+8]));
      f_channels = *((int *)(&szBuf[i+10]));
      f_sample_rate = *((long *)(&szBuf[i+12]));
      f_bits_per_sample = *((int *)(&szBuf[i+22]));

      n = 3;
      i += (int)f_len + 8;
    }
    else if ( _fstrnicmp( &szBuf[i], "data", 4 ) == 0 )
    {
      data_offset = (long)(i + 8);
      data_size = *((long *)(&szBuf[i+4]));
      n = 4;
      break;
    }
    else
    {
      ++i;
    }
  }

  if ( n != 4 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Unable to find sound data in %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( f_tag != 1 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Data in %s is not in PCM format.",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( f_channels != 1 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Data in %s is not single channel (mono).",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( f_sample_rate < 22000 || f_sample_rate > 22100 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Data in %s was recorded at %ldHz (22050Hz required).",
            (LPSTR)szFile, f_sample_rate );

    return FALSE;
  }

  if ( f_bits_per_sample != 8 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Data in %s is not 8 bit data.",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( data_size == 0L )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Data in %s has zero length.",
            (LPSTR)szFile );

    return FALSE;
  }

  pData = (BYTE huge *)GlobalAllocPtr( GPTR, (DWORD)data_size );

  if ( pData == NULL )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Out of memory reading sound file %s",
            (LPSTR)szFile );

    return FALSE;
  }

  _llseek( fh, data_offset, SEEK_SET );

  if ( _hread( fh, pData, data_size ) != data_size )
  {
    GlobalFreePtr( pData );
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error reading sound file for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  _lclose(fh);

  psd->length = data_size;
  psd->data = pData;

#if 0

  //
  // Convert from unsigned 8 bit data to signed 8 bit data.
  //

  while( data_size > 0L )
  {
    *pData++ -= 128;
    --data_size;
  }

#endif

  return TRUE;
  
} // LoadSoundData


/*******************************************************************

    NAME:       ReloadSound

    SYNOPSIS:   Reload a sound from disk.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

BOOL ReloadSound( HWND hwndParent, WND_DATA *pWndData, INT index )
{
  int n;
  BOOL bResult;
  OPENFILENAME ofn;
  CHAR szDirName[256], szFile[256], szFileTitle[256];
  SOUND_DATA *psd;

  psd = &pWndData->pSoundData[index];

  _fstrcpy( szDirName, szPath );

  n = _fstrlen( szDirName );

  if (n > 0)
  {
    if (szDirName[n-1] == '\\' )
    {
      szDirName[n-1] = '\0';
    }
  }

  _fstrcpy( szFile, psd->sound_name );

  _fmemset( &ofn, 0, sizeof(OPENFILENAME) );

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwndParent;
  ofn.hInstance = hInst;
  ofn.lpstrFilter = szFilter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile= szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof(szFileTitle);
  ofn.lpstrInitialDir = szDirName;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  bResult = GetOpenFileName( &ofn );
    
  if ( !bResult )
  {
    DWORD dwError;
    
    dwError = CommDlgExtendedError();
    
    if ( dwError )
    {
      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Common dialog error %lu",
              dwError );
    }
              
    return FALSE;
  }

  _fstrcpy(psd->sound_name, szFileTitle);

  GlobalFreePtr(psd->data);

  psd->length = 0;
  psd->data = NULL;

  if ( LoadSoundData( hwndParent, psd, szFile ) == FALSE )
  {
    return FALSE;
  }

  return TRUE;
  
} // ReloadSound


/*******************************************************************

    NAME:       ImportSound

    SYNOPSIS:   Show the open dialog box and allow the user to
                select a sound to import.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

BOOL ImportSound( HWND hwndParent, WND_DATA *pWndData )
{
  int n;
  BOOL bResult;
  OPENFILENAME ofn;
  CHAR szDirName[256], szFile[256], szFileTitle[256];
  SOUND_DATA *psd;

  _fstrcpy( szDirName, szPath );

  n = _fstrlen( szDirName );

  if (n > 0)
  {
    if (szDirName[n-1] == '\\' )
    {
      szDirName[n-1] = '\0';
    }
  }
  
  szFile[0] = '\0';

  _fmemset( &ofn, 0, sizeof(OPENFILENAME) );

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwndParent;
  ofn.hInstance = hInst;
  ofn.lpstrFilter = szFilter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile= szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof(szFileTitle);
  ofn.lpstrInitialDir = szDirName;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  bResult = GetOpenFileName( &ofn );
    
  if ( !bResult )
  {
    DWORD dwError;
    
    dwError = CommDlgExtendedError();
    
    if ( dwError )
    {
      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Common dialog error %lu",
              dwError );
    }
              
    return FALSE;
  }

  psd = &pWndData->pSoundData[pWndData->nSounds];

  if ( LoadSoundData( hwndParent, psd, szFile ) == FALSE )
  {
    return FALSE;
  }

  _fstrcpy( psd->sound_name, szFileTitle );

  psd->flags = 0;

  ++pWndData->nSounds;

  return TRUE;
  
} // ImportSound


/*******************************************************************

    NAME:       ExportSound

    SYNOPSIS:   Show the save as dialog box and allow the user to
                select a name for sound to be exported.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

BOOL ExportSound( HWND hwndParent, WND_DATA *pWndData, INT index )
{
  INT n;
  LONG lsize;
  HFILE fh;
  BOOL bResult;
  OPENFILENAME ofn;
  CHAR szDirName[256], szFile[256], szFileTitle[256];

  struct
  {
    char    r_id[4];
    long    r_len;
    char    w_id[4];
    char    f_id[4];
    long    f_len;
    int     f_tag;
    int     f_channels;
    long    f_sample_rate;
    long    f_avg_bytes_per_sec;
    int     f_block_align;
    int     f_bits_per_sample;
    char    d_id[4];
    long    d_len;

  } hdr;

  static CHAR *szFilter = "WAV files (*.wav)\000*.wav\000";

  _fstrcpy( szDirName, szPath );

  n = _fstrlen( szDirName );

  if (n > 0)
  {
    if (szDirName[n-1] == '\\' )
    {
      szDirName[n-1] = '\0';
    }
  }
  
  _fstrcpy( szFile, pWndData->pSoundData[index].sound_name );

  _fmemset( &ofn, 0, sizeof(OPENFILENAME) );

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwndParent;
  ofn.hInstance = hInst;
  ofn.lpstrFilter = szFilter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile= szFile;
  ofn.nMaxFile = sizeof(szFile);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof(szFileTitle);
  ofn.lpstrInitialDir = szDirName;
  ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
  
  bResult = GetSaveFileName( &ofn );
    
  if ( !bResult )
  {
    DWORD dwError;
    
    dwError = CommDlgExtendedError();
    
    if ( dwError )
    {
      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Common dialog error %lu",
              dwError );
    }
              
    return FALSE;
  }

  fh = _lcreat( szFile, READ );

  if ( fh == HFILE_ERROR )
  {
    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Unable to create %s",
            (LPSTR)szFile );

    return FALSE;
  }

  lsize = pWndData->pSoundData[index].length;

  hdr.r_id[0] = 'R';
  hdr.r_id[1] = 'I';
  hdr.r_id[2] = 'F';
  hdr.r_id[3] = 'F';
  hdr.r_len = sizeof(hdr) + lsize - 8L;
  hdr.w_id[0] = 'W';
  hdr.w_id[1] = 'A';
  hdr.w_id[2] = 'V';
  hdr.w_id[3] = 'E';
  hdr.f_id[0] = 'f';
  hdr.f_id[1] = 'm';
  hdr.f_id[2] = 't';
  hdr.f_id[3] = ' ';
  hdr.f_len = 16L;
  hdr.f_tag = 1;
  hdr.f_channels = 1;
  hdr.f_sample_rate = 22050L;
  hdr.f_avg_bytes_per_sec = 22050L;
  hdr.f_block_align = 1;
  hdr.f_bits_per_sample = 8;
  hdr.d_id[0] = 'd';
  hdr.d_id[1] = 'a';
  hdr.d_id[2] = 't';
  hdr.d_id[3] = 'a';
  hdr.d_len = lsize;

  if ( _lwrite( fh, &hdr, sizeof(hdr) ) != sizeof(hdr) )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error writing sound file header for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( _hwrite( fh, pWndData->pSoundData[index].data, lsize) != lsize )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error writing sound file data for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  _lclose(fh);

  return TRUE;
  
} // ExportSound


/*******************************************************************

    NAME:       PlaySound

    SYNOPSIS:   Play the sound indicated.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

BOOL PlaySound( HWND hwndParent, WND_DATA *pWndData, INT index )
{
  LONG lsize;
  DWORD dwSize;
  BOOL bRet;

  struct
  {
    char    r_id[4];
    long    r_len;
    char    w_id[4];
    char    f_id[4];
    long    f_len;
    int     f_tag;
    int     f_channels;
    long    f_sample_rate;
    long    f_avg_bytes_per_sec;
    int     f_block_align;
    int     f_bits_per_sample;
    char    d_id[4];
    long    d_len;

  } hdr;

  bRet = FALSE;

  if (pSoundPlayData)
  {
    sndPlaySound(NULL, 0);
    GlobalFreePtr(pSoundPlayData);
    pSoundPlayData = NULL;
  }

  lsize = pWndData->pSoundData[index].length;

  hdr.r_id[0] = 'R';
  hdr.r_id[1] = 'I';
  hdr.r_id[2] = 'F';
  hdr.r_id[3] = 'F';
  hdr.r_len = sizeof(hdr) + lsize - 8L;
  hdr.w_id[0] = 'W';
  hdr.w_id[1] = 'A';
  hdr.w_id[2] = 'V';
  hdr.w_id[3] = 'E';
  hdr.f_id[0] = 'f';
  hdr.f_id[1] = 'm';
  hdr.f_id[2] = 't';
  hdr.f_id[3] = ' ';
  hdr.f_len = 16L;
  hdr.f_tag = 1;
  hdr.f_channels = 1;
  hdr.f_sample_rate = 22050L;
  hdr.f_avg_bytes_per_sec = 22050L;
  hdr.f_block_align = 1;
  hdr.f_bits_per_sample = 8;
  hdr.d_id[0] = 'd';
  hdr.d_id[1] = 'a';
  hdr.d_id[2] = 't';
  hdr.d_id[3] = 'a';
  hdr.d_len = lsize;

  dwSize = sizeof(hdr) + lsize + 16; /* add an extra 16 bytes just in case */

  pSoundPlayData = (BYTE huge *)GlobalAllocPtr(GPTR, dwSize);

  if (pSoundPlayData)
  {
    UINT uFlags;

    hmemcpy(pSoundPlayData, &hdr, sizeof(hdr));
    hmemcpy(pSoundPlayData + sizeof(hdr), pWndData->pSoundData[index].data, lsize);

    uFlags = SND_ASYNC | SND_MEMORY;
    bRet = sndPlaySound((LPCSTR)pSoundPlayData, uFlags);
  }

  return bRet;
  
} // PlaySound


/*******************************************************************

    NAME:       ViewSounds_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewSounds_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewSounds_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewSounds_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor   );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy    );
  }

  return FALSE;

} // ViewSounds_DlgProc


/*******************************************************************

    NAME:       ViewSounds_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewSounds_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtl,
                           UINT codeNotify )
{
  INT i, j;
  HWND hwndList;
  CHAR szBuf[80];
  LRESULT lResult;

  switch(id)
  {
    case IDC_SOUND_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtl );

        if ( lResult >= 0 && lResult < po->nSounds )
        {
          wsprintf( szBuf, "%ld", po->pSoundData[lResult].length );
        }
        else
        {
          _fstrcpy( szBuf, "0" );
        }

        Edit_SetText( GetDlgItem( hwnd, IDC_SOUND_LENGTH ), szBuf );
      }
      else if ( codeNotify == LBN_DBLCLK )
      {
        lResult = ListBox_GetCurSel( hwndCtl );

        if ( lResult >= 0 && lResult < po->nSounds )
        {
          PlaySound( hwnd, po, (INT)lResult );
        }
      }

      break;

    case IDC_PLAY_SOUND:

      hwndList = GetDlgItem( hwnd, IDC_SOUND_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nSounds )
      {
        PlaySound( hwnd, po, (INT)lResult );
      }

      break;

    case IDC_RELOAD_SOUND:

      hwndList = GetDlgItem( hwnd, IDC_SOUND_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nSounds )
      {
        ReloadSound( hwnd, po, (INT)lResult );

        j = ListBox_GetTopIndex(hwndList);

        ListBox_ResetContent(hwndList);

        for( i = 0; i < po->nSounds; ++i )
        {
          ListBox_AddString( hwndList, po->pSoundData[i].sound_name );
        }

        ListBox_SetTopIndex( hwndList, j );
        ListBox_SetCurSel( hwndList, (INT)lResult );

        SendMessage( hwnd,
                     WM_COMMAND,
                     (WPARAM)IDC_SOUND_LIST,
                     (LPARAM)(MAKELONG(hwndList,LBN_SELCHANGE)) );

        bChange = TRUE;
      }

      break;

    case IDC_IMPORT_SOUND:

      if ( po->nSounds == MAX_SOUNDS )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d sounds has been reached.",
                MAX_SOUNDS );
      }
      else
      {
        if ( ImportSound( hwnd, po ) )
        {
          i = po->nSounds - 1;

          hwndList = GetDlgItem( hwnd, IDC_SOUND_LIST );

          ListBox_AddString( hwndList, (LPSTR)po->pSoundData[i].sound_name );

          ListBox_SetCurSel( hwndList, i );

          SendMessage( hwnd,
                       WM_COMMAND,
                       (WPARAM)IDC_SOUND_LIST,
                       (LPARAM)(MAKELONG(hwndList,LBN_SELCHANGE)) );

          bChange = TRUE;
        }
      }
      break;

    case IDC_EXPORT_SOUND:

      hwndList = GetDlgItem( hwnd, IDC_SOUND_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nSounds )
      {
        ExportSound( hwnd, po, (INT)lResult );
      }

      break;

    case IDC_DELETE_SOUND:

      hwndList = GetDlgItem( hwnd, IDC_SOUND_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nSounds )
      {
        i = (INT)lResult;

        Object_DeleteSound(po, i);

        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        SendMessage( hwnd,
                     WM_COMMAND,
                     (WPARAM)IDC_SOUND_LIST,
                     (LPARAM)(MAKELONG(hwndList,LBN_SELCHANGE)) );

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
  
} // ViewSounds_OnCommand


/*******************************************************************

    NAME:       ViewSounds_OnInitDialog

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

BOOL ViewSounds_OnInitDialog( HWND   hwnd,
                              HWND   hwndFocus,
                              LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_SOUND_LIST );

  for( i = 0; i < po->nSounds; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pSoundData[i].sound_name );
  }

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewSounds_OnInitDialog
