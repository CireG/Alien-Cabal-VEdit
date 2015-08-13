/*******************************************************************

    bmpdlg.c

    Routines for displaying and managing the bitmap dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL ImportBitmap( HWND hwndParent, WND_DATA *pWndData );

BOOL ReloadBitmap( HWND hwndParent, WND_DATA *pWndData, INT index );

BOOL ExportBitmap( HWND hwndParent, WND_DATA *pWndData, INT index );

BOOL CALLBACK __export ViewBitmaps_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam );

VOID ViewBitmaps_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtl,
                            UINT codeNotify );

BOOL ViewBitmaps_OnInitDialog( HWND   hwndDlg,
                               HWND   hwndFocus,
                               LPARAM lParam );


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static CHAR *szFilter = "BMP files (*.bmp)\000*.bmp\000"
                        "All Files (*.*)\000*.*\000";

//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewBitmapsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewBitmapsDialog( HWND hwndParent, WND_DATA *pWndData )
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;
  
  pfnProc = MakeProcInstance( (FARPROC)ViewBitmaps_DlgProc, hInst );

  nResult = DialogBox( hInst,
                       IDD_VIEW_BITMAPS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance( pfnProc );

  if ( bChange )
  {
    Object_DeleteUndoData( hwndParent, pWndData );
  }
    
  return nResult;
    
} // ViewBitmapsDialog


/*******************************************************************

    NAME:       PreviewBitmap

    SYNOPSIS:   Display the bitmap in the control given.

    ENTRY:      hwnd - The window for this dialog.

                idPreview - The preview control id.

                idBmpSize - The bitmap size control id.

                pBitmapData - Pointer to the bitmap data.

                pPalette - Pointre to the palette data.

********************************************************************/

VOID PreviewBitmap( HWND          hwnd,
                    INT           idPreview,
                    INT           idBmpSize,
                    BITMAP_DATA  *pBitmapData,
                    PALETTE_DATA *pPalette )
{
  INT i, rxlen, rylen, xlen, ylen;
  CHAR szBuf[128];
  HDC hdc;
  RECT rc;
  POINT pt;
  HPEN hPenOld;
  HBRUSH hBrushOld;
  BITMAPINFO *bi;
  WORD wVersion, wSize;
  LOGPALETTE *pLogPal;
  HPALETTE hPalOld;

  hdc = GetDC( hwnd );

  GetWindowRect( GetDlgItem( hwnd, idPreview ), &rc );

//InflateRect( &rc, -60, -60 );

  rxlen = rc.right - rc.left - 4;
  rylen = rc.bottom - rc.top - 4;

  pt.x = rc.left;
  pt.y = rc.top;

  ScreenToClient( hwnd, &pt );

  hPenOld = SelectObject( hdc, GetStockObject(BLACK_PEN) );
  hBrushOld = SelectObject( hdc, GetStockObject(GRAY_BRUSH) );

  Rectangle( hdc,
             pt.x,
             pt.y,
             pt.x + rc.right - rc.left,
             pt.y + rc.bottom - rc.top );

  SelectObject( hdc, hBrushOld );
  SelectObject( hdc, hPenOld );

  if ( pBitmapData != NULL && pPalette != NULL )
  {
    wSize = sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);

    bi = (BITMAPINFO *)GlobalAllocPtr( GPTR, (DWORD)wSize );

    bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi->bmiHeader.biWidth = pBitmapData->x_len;
    bi->bmiHeader.biHeight = pBitmapData->y_len;
    bi->bmiHeader.biPlanes = 1;
    bi->bmiHeader.biBitCount = 8;
    bi->bmiHeader.biSizeImage = bi->bmiHeader.biHeight * 4 * ( ( bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount + 31 ) / 32 );

    for( i = 0; i < 256; ++i )
    {
      bi->bmiColors[i].rgbRed = pPalette->red[i];
      bi->bmiColors[i].rgbGreen = pPalette->green[i];
      bi->bmiColors[i].rgbBlue = pPalette->blue[i];
    }

    wVersion = LOWORD(GetVersion());

    xlen = (int)bi->bmiHeader.biWidth;
    ylen = (int)bi->bmiHeader.biHeight;

    if ( xlen > rxlen )
    {
      ylen = (int)((long)ylen * (long)(rxlen) / (long)xlen);
      xlen = rxlen;
    }

    if ( ylen > rylen )
    {
      xlen = (int)((long)xlen * (long)(rylen) / (long)ylen);
      ylen = rylen;
    }
 
    pt.x += ( rc.right - rc.left - xlen ) / 2;

/*
    if ( HIBYTE(wVersion) <= 11 )
    {
      pt.y -= ( rc.bottom - rc.top - ylen ) / 2;
    }
    else
*/
    {
      pt.y += ( rc.bottom - rc.top - ylen ) / 2;
    }

    if ( hPalCommon == NULL )
    {
      wSize = sizeof(LOGPALETTE) + 256 * sizeof(PALETTEENTRY);

      pLogPal = (LOGPALETTE *)GlobalAllocPtr( GPTR, (DWORD)wSize );

      pLogPal->palNumEntries = 256;
      pLogPal->palVersion = 0x0300;

      for( i = 0; i < 256; ++i )
      {
        pLogPal->palPalEntry[i].peRed = LOBYTE(pPalette->red[i]);
        pLogPal->palPalEntry[i].peGreen = LOBYTE(pPalette->green[i]);
        pLogPal->palPalEntry[i].peBlue = LOBYTE(pPalette->blue[i]);
//      pLogPal->palPalEntry[i].peFlags = PC_RESERVED;
        pLogPal->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
      }

      hPalCommon = CreatePalette( pLogPal );

      GlobalFreePtr( pLogPal );
    }

    if ( hPalCommon )
    {
      hPalOld = SelectPalette( hdc, hPalCommon, FALSE );
      RealizePalette( hdc );
    }

    SetStretchBltMode(hdc, STRETCH_DELETESCANS);

    //
    //  In order to avoid a bug in the StretchDIBits function, pad
    //  the bitmap to a width evenly divisble by 4.
    //

    if (pBitmapData->x_len & 3)
    {
      int new_x_len;
      DWORD dwTmpSize;
      BYTE *pTmpData;

      new_x_len = (((pBitmapData->x_len >> 2) + 1) << 2);

      dwTmpSize = (DWORD)new_x_len * (DWORD)pBitmapData->y_len;

      pTmpData = (BYTE *)GlobalAllocPtr(GPTR,dwTmpSize);

      if (pTmpData)
      {
        int clr_index, tmp_index;

        //
        //  Try to find a color in the palette matching the background
        //  color for the bitmap preview area so that the extra pixels
        //  won't be seen.
        //

        clr_index = 0;

        if (pPalette)
        {
          for(tmp_index = 0; tmp_index < 256; ++tmp_index)
          {
            if (pPalette->red[tmp_index] == 128 &&
                pPalette->green[tmp_index] == 128 &&
                pPalette->blue[tmp_index] == 128)
            {
              clr_index = tmp_index;
              break;
            }
          }
        }

        memset(pTmpData, clr_index, (WORD)dwTmpSize);

        for(tmp_index = 0; tmp_index < pBitmapData->y_len; ++tmp_index)
        {
          memcpy(pTmpData + tmp_index * new_x_len,
                 pBitmapData->data + tmp_index * pBitmapData->x_len,
                 pBitmapData->x_len);
        }

        StretchDIBits( hdc,
                       pt.x,
                       pt.y,
                       xlen,
                       ylen,
                       0,
                       0,
                       new_x_len,
                       pBitmapData->y_len,
                       pTmpData,
                       bi,
                       DIB_RGB_COLORS,
                       SRCCOPY );

        GlobalFreePtr(pTmpData);
      }
    }
    else
    {
      StretchDIBits( hdc,
                     pt.x,
                     pt.y,
                     xlen,
                     ylen,
                     0,
                     0,
                     pBitmapData->x_len,
                     pBitmapData->y_len,
                     pBitmapData->data,
                     bi,
                     DIB_RGB_COLORS,
                     SRCCOPY );
    }

    if ( hPalCommon )
    {
      SelectPalette( hdc, hPalOld, FALSE );
    }

    GlobalFreePtr( bi );
  }

  ReleaseDC( hwnd, hdc );

  if ( pBitmapData != NULL )
  {
    wsprintf( szBuf, "%dh x %dv", pBitmapData->x_len, pBitmapData->y_len );
  }
  else
  {
    _fstrcpy( szBuf, "0h x 0v" );
  }

  Edit_SetText( GetDlgItem( hwnd, idBmpSize ), szBuf );

  return;

} // PreviewBitmap


//
//  Private functions.
//

/*******************************************************************

    NAME:       ReloadBitmap

    SYNOPSIS:   Reload a bitmap from disk.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

BOOL ReloadBitmap( HWND hwndParent, WND_DATA *pWndData, INT index )
{
  INT i, j, k, n;
  LONG image_size;
  HFILE fh;
  BYTE *p, *p1;
  WORD wBytes;
  BOOL bResult;
  BITMAPFILEHEADER bf;
  BITMAPINFOHEADER bi;
  RGBQUAD rgb;
  OPENFILENAME ofn;
  CHAR szDirName[256], szFile[256], szFileTitle[256];

  _fstrcpy( szDirName, szPath );

  n = _fstrlen( szDirName );

  if (n > 0)
  {
    if (szDirName[n-1] == '\\' )
    {
      szDirName[n-1] = '\0';
    }
  }

  _fstrcpy( szFile, pWndData->pBitmapData[index].bitmap_name );

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

  fh = _lopen( szFile, READ );

  if ( fh == HFILE_ERROR )
  {
    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Unable to open %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( _lread( fh, &bf, sizeof(bf) ) != sizeof(bf) )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error reading bitmap file header for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( _lread( fh, &bi, sizeof(bi) ) != sizeof(bi) )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error reading bitmap file info for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( bi.biBitCount != 8 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "%s is not a 256 color bitmap",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( bi.biWidth > 32767 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Bitmap width for %s is invalid",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( bi.biHeight > 32767 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Bitmap height for %s is invalid",
            (LPSTR)szFile );

    return FALSE;
  }

  image_size = bi.biWidth * bi.biHeight;

  if (image_size > 65536L)
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Bitmap %s is >64k",
            (LPSTR)szFile );

    return FALSE;
  }

  k = 0;

  for( i = 0; i < 256; ++i )
  {
    if ( _lread( fh, &rgb, sizeof(rgb) ) != sizeof(rgb) )
    {
      _lclose(fh);

      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Error reading palette of %s",
              (LPSTR)szFile );

      return FALSE;
    }

    if ( pWndData->nBitmaps > 0 )
    {
      if ( pWndData->pPalette->red[i] != rgb.rgbRed ||
           pWndData->pPalette->green[i] != rgb.rgbGreen ||
           pWndData->pPalette->blue[i] != rgb.rgbBlue )
      {
        ++k;
      }
    }

    pWndData->pPalette->red[i] = rgb.rgbRed;
    pWndData->pPalette->green[i] = rgb.rgbGreen;
    pWndData->pPalette->blue[i] = rgb.rgbBlue;
  }

  if ( k > 0 )
  {
    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "%d palette entries were changed in the current palette.",
            k );
  }

  p = (BYTE *)GlobalAllocPtr( GPTR, (DWORD)image_size );

  if ( p == NULL )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Out of memory reading bitmap file %s",
            (LPSTR)szFile );

    return FALSE;
  }

  _llseek( fh, bf.bfOffBits, SEEK_SET );

  wBytes = (WORD)(((bi.biWidth * bi.biBitCount + 31) / 32) * 4);

  p1 = (BYTE *)GlobalAllocPtr( GPTR, (DWORD)wBytes );

  for( j = 0; j < bi.biHeight; ++j )
  {
    if ( _lread( fh, p1, wBytes ) != wBytes )
    {
      GlobalFreePtr( p1 );
      GlobalFreePtr( p );
      _lclose(fh);

      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Error reading bitmap file for %s",
              (LPSTR)szFile );

      return FALSE;
    }

    _fmemcpy( p + (long)j * (long)bi.biWidth, p1, (int)bi.biWidth );
  }

  GlobalFreePtr( p1 );

  _lclose(fh);

  GlobalFreePtr( pWndData->pBitmapData[index].data );

  _fstrcpy( pWndData->pBitmapData[index].bitmap_name, szFileTitle );

  pWndData->pBitmapData[index].x_len = (int)bi.biWidth;
  pWndData->pBitmapData[index].y_len = (int)bi.biHeight;
  pWndData->pBitmapData[index].data = p;

  return TRUE;
  
} // ReloadBitmap


/*******************************************************************

    NAME:       ImportBitmap

    SYNOPSIS:   Show the open dialog box and allow the user to
                select a bitmap to import.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

BOOL ImportBitmap( HWND hwndParent, WND_DATA *pWndData )
{
  INT i, j, k, n;
  LONG image_size;
  HFILE fh;
  BYTE *p, *p1;
  WORD wBytes;
  BOOL bResult;
  OPENFILENAME ofn;
  BITMAPFILEHEADER bf;
  BITMAPINFOHEADER bi;
  RGBQUAD rgb;
  CHAR szDirName[256], szFile[256], szFileTitle[256];

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

  fh = _lopen( szFile, READ );

  if ( fh == HFILE_ERROR )
  {
    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Unable to open %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( _lread( fh, &bf, sizeof(bf) ) != sizeof(bf) )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error reading bitmap file header for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( _lread( fh, &bi, sizeof(bi) ) != sizeof(bi) )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error reading bitmap file info for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( bi.biBitCount != 8 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "%s is not a 256 color bitmap",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( bi.biWidth > 32767 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Bitmap width for %s is invalid",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( bi.biHeight > 32767 )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Bitmap height for %s is invalid",
            (LPSTR)szFile );

    return FALSE;
  }

  image_size = bi.biWidth * bi.biHeight;

  if (image_size > 65536L)
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Bitmap %s is >64k",
            (LPSTR)szFile );

    return FALSE;
  }

  k = 0;

  for( i = 0; i < 256; ++i )
  {
    if ( _lread( fh, &rgb, sizeof(rgb) ) != sizeof(rgb) )
    {
      _lclose(fh);

      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Error reading palette of %s",
              (LPSTR)szFile );

      return FALSE;
    }

    if ( pWndData->nBitmaps > 0 )
    {
      if ( pWndData->pPalette->red[i] != rgb.rgbRed ||
           pWndData->pPalette->green[i] != rgb.rgbGreen ||
           pWndData->pPalette->blue[i] != rgb.rgbBlue )
      {
        ++k;
      }
    }
    else
    {
      pWndData->pPalette->red[i] = rgb.rgbRed;
      pWndData->pPalette->green[i] = rgb.rgbGreen;
      pWndData->pPalette->blue[i] = rgb.rgbBlue;
    }
  }

  if ( k > 0 )
  {
    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "%d palette entries of %s do not match the current palette.",
            k,
            (LPSTR)szFile );
  }

  p = (BYTE *)GlobalAllocPtr( GPTR, (DWORD)image_size );

  if ( p == NULL )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Out of memory reading bitmap file %s",
            (LPSTR)szFile );

    return FALSE;
  }

  _llseek( fh, bf.bfOffBits, SEEK_SET );

  wBytes = (WORD)(((bi.biWidth * bi.biBitCount + 31) / 32) * 4);

  p1 = (BYTE *)GlobalAllocPtr( GPTR, (DWORD)wBytes );

  for( j = 0; j < bi.biHeight; ++j )
  {
    if ( _lread( fh, p1, wBytes ) != wBytes )
    {
      GlobalFreePtr( p1 );
      GlobalFreePtr( p );
      _lclose(fh);

      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Error reading bitmap file for %s",
              (LPSTR)szFile );

      return FALSE;
    }

    _fmemcpy( p + (long)j * (long)bi.biWidth, p1, (int)bi.biWidth );
  }

  GlobalFreePtr( p1 );

  _lclose(fh);

  k = pWndData->nBitmaps;

  _fstrcpy( pWndData->pBitmapData[k].bitmap_name, szFileTitle );

  pWndData->pBitmapData[k].x_len = (int)bi.biWidth;
  pWndData->pBitmapData[k].y_len = (int)bi.biHeight;
  pWndData->pBitmapData[k].data = p;
  pWndData->pBitmapData[k].flags = 0;

  ++pWndData->nBitmaps;

  return TRUE;
  
} // ImportBitmap


/*******************************************************************

    NAME:       ExportBitmap

    SYNOPSIS:   Show the save as dialog box and allow the user to
                select a name for bitmap to be exported.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

BOOL ExportBitmap( HWND hwndParent, WND_DATA *pWndData, INT index )
{
  INT i, j, n;
  HFILE fh;
  BYTE *p;
  WORD wBytes;
  BOOL bResult;
  OPENFILENAME ofn;
  BITMAPFILEHEADER bf;
  BITMAPINFOHEADER bi;
  RGBQUAD rgb;
  CHAR szDirName[256], szFile[256], szFileTitle[256];

  static CHAR *szFilter = "BMP files (*.bmp)\000*.bmp\000";

  _fstrcpy( szDirName, szPath );

  n = _fstrlen( szDirName );

  if (n > 0)
  {
    if (szDirName[n-1] == '\\' )
    {
      szDirName[n-1] = '\0';
    }
  }
  
  _fstrcpy( szFile, pWndData->pBitmapData[index].bitmap_name );

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

  _fmemset( &bi, 0, sizeof(bi) );

  bi.biSize = sizeof(BITMAPINFOHEADER);
  bi.biWidth = pWndData->pBitmapData[index].x_len;
  bi.biHeight = pWndData->pBitmapData[index].y_len;
  bi.biPlanes = 1;
  bi.biBitCount = 8;
  bi.biSizeImage = bi.biHeight * 4 * ( ( bi.biWidth * bi.biBitCount + 31 ) / 32 );

  _fmemset( &bf, 0, sizeof(bf) );

  bf.bfType = 0x4d42;
  bf.bfOffBits = sizeof(BITMAPFILEHEADER) + bi.biSize + 256 * sizeof(RGBQUAD);
  bf.bfSize = bf.bfOffBits + bi.biSizeImage;

  if ( _lwrite( fh, &bf, sizeof(bf) ) != sizeof(bf) )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error writing bitmap file header for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  if ( _lwrite( fh, &bi, sizeof(bi) ) != sizeof(bi) )
  {
    _lclose(fh);

    MsgBox( hwndParent,
            MB_ICONEXCLAMATION,
            "Error writing bitmap file info for %s",
            (LPSTR)szFile );

    return FALSE;
  }

  for( i = 0; i < 256; ++i )
  {
    rgb.rgbRed = pWndData->pPalette->red[i];
    rgb.rgbGreen = pWndData->pPalette->green[i];
    rgb.rgbBlue = pWndData->pPalette->blue[i];

    if ( _lwrite( fh, &rgb, sizeof(rgb) ) != sizeof(rgb) )
    {
      _lclose(fh);

      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Error writing palette of %s",
              (LPSTR)szFile );

      return FALSE;
    }
  }

  wBytes = (WORD)(((bi.biWidth * bi.biBitCount + 31) / 32) * 4);

  p = (BYTE *)GlobalAllocPtr( GPTR, (DWORD)wBytes );

  for( j = 0; j < bi.biHeight; ++j )
  {
    _fmemcpy( p,
              pWndData->pBitmapData[index].data + (long)j * (long)bi.biWidth,
              (int)bi.biWidth );

    if ( _lwrite( fh, p, wBytes ) != wBytes )
    {
      GlobalFreePtr( p );

      _lclose(fh);

      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Error writing bitmap file for %s",
              (LPSTR)szFile );

      return FALSE;
    }
  }

  GlobalFreePtr( p );

  _lclose(fh);

  return TRUE;
  
} // ExportBitmap


/*******************************************************************

    NAME:       ViewBitmaps_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewBitmaps_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
  switch( nMessage )
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewBitmaps_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewBitmaps_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor    );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy     );
  }

  return FALSE;

} // ViewBitmaps_DlgProc


/*******************************************************************

    NAME:       ViewBitmaps_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID ViewBitmaps_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtl,
                            UINT codeNotify )
{
  INT i, j;
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_BITMAP_LIST:

      if ( codeNotify == LBN_SELCHANGE )
      {
        lResult = ListBox_GetCurSel( hwndCtl );

        if ( lResult >= 0 && lResult < po->nBitmaps )
        {
          PreviewBitmap( hwnd,
                         IDC_BITMAP_PREVIEW,
                         IDC_BITMAP_SIZE,
                         &po->pBitmapData[lResult],
                         po->pPalette );

          i = ( po->pBitmapData[lResult].flags & BF_TRANSPARENT ) ? 1 : 0;
        }
        else
        {
          PreviewBitmap( hwnd,
                         IDC_BITMAP_PREVIEW,
                         IDC_BITMAP_SIZE,
                         NULL,
                         NULL );

          i = 0;
        }

        CheckDlgButton( hwnd, IDC_BITMAP_TRANSPARENT, i );
      }
      break;
      
    case IDC_BITMAP_TRANSPARENT:

      if ( codeNotify == BN_CLICKED )
      {
        hwndList = GetDlgItem( hwnd, IDC_BITMAP_LIST );

        lResult = ListBox_GetCurSel( hwndList );

        if ( lResult >= 0 && lResult < po->nBitmaps )
        {
          i = (int)lResult;

          if ( SendMessage( hwndCtl, BM_GETCHECK, (WPARAM)0, (LPARAM)0 ) )
          {
            po->pBitmapData[i].flags = (po->pBitmapData[i].flags | BF_TRANSPARENT);
          }
          else
          {
            po->pBitmapData[i].flags = (po->pBitmapData[i].flags & ~BF_TRANSPARENT);
          }
        }

        bChange = TRUE;
      }
      break;

    case IDC_RELOAD_BITMAP:

      hwndList = GetDlgItem( hwnd, IDC_BITMAP_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nBitmaps )
      {
        if (ReloadBitmap( hwnd, po, (INT)lResult ) )
        {
          j = ListBox_GetTopIndex(hwndList);

          ListBox_ResetContent(hwndList);

          for( i = 0; i < po->nBitmaps; ++i )
          {
            ListBox_AddString( hwndList, po->pBitmapData[i].bitmap_name );
          }

          ListBox_SetTopIndex( hwndList, j );
          ListBox_SetCurSel( hwndList, (INT)lResult );
          SendMessage(hwnd, WM_COMMAND, IDC_BITMAP_LIST, MAKELONG(hwndList, LBN_SELCHANGE) );

          bChange = TRUE;
        }
      }

      break;

    case IDC_IMPORT_BITMAP:

      if ( po->nBitmaps == MAX_BITMAPS )
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d bitmaps has been reached.",
                MAX_BITMAPS );
      }
      else
      {
        if ( ImportBitmap( hwnd, po ) )
        {
          i = po->nBitmaps - 1;

          hwndList = GetDlgItem( hwnd, IDC_BITMAP_LIST );

          ListBox_AddString( hwndList, (LPSTR)po->pBitmapData[i].bitmap_name );

          ListBox_SetCurSel( hwndList, i );

          SendMessage( hwnd,
                       WM_COMMAND,
                       (WPARAM)IDC_BITMAP_LIST,
                       (LPARAM)(MAKELONG(hwndList,LBN_SELCHANGE)) );

          bChange = TRUE;
        }
      }
      break;

    case IDC_EXPORT_BITMAP:

      hwndList = GetDlgItem( hwnd, IDC_BITMAP_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nBitmaps )
      {
        ExportBitmap( hwnd, po, (INT)lResult );
      }

      break;

    case IDC_DELETE_BITMAP:

      hwndList = GetDlgItem( hwnd, IDC_BITMAP_LIST );

      lResult = ListBox_GetCurSel( hwndList );

      if ( lResult >= 0 && lResult < po->nBitmaps )
      {
        i = (INT)lResult;

        Object_DeleteBitmap(po, i);

        ListBox_DeleteString( hwndList, i );

        if ( i == 0 )
          ListBox_SetCurSel( hwndList, 0 );
        else
          ListBox_SetCurSel( hwndList, i - 1 );

        SendMessage( hwnd,
                     WM_COMMAND,
                     (WPARAM)IDC_BITMAP_LIST,
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
  
} // ViewBitmaps_OnCommand


/*******************************************************************

    NAME:       ViewBitmaps_OnInitDialog

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

BOOL ViewBitmaps_OnInitDialog( HWND   hwnd,
                               HWND   hwndFocus,
                               LPARAM lParam )
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem( hwnd, IDC_BITMAP_LIST );

  for( i = 0; i < po->nBitmaps; ++i )
  {
    ListBox_AddString( hwndCtrl, po->pBitmapData[i].bitmap_name );
  }

  AllDialogs_OnInitDialog( hwnd, hwndFocus, lParam );

  return TRUE;

} // ViewBitmaps_OnInitDialog
