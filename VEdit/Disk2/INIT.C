/*******************************************************************

    init.c

    This file contains application & instance initialization routines.

********************************************************************/


#include "vedit.h"


//
//  Public functions.
//

/*******************************************************************

    NAME:       InitApplication

    SYNOPSIS:   Performs application-wide initialization.

    ENTRY:      hInstance - The current instance handle.

    RETURNS:    BOOL - TRUE if everything initialized OK, FALSE
                    if something tragic happened.

********************************************************************/

BOOL InitApplication( HINSTANCE hInstance )
{
  INT      n, nResult;
  WORD     wReturn;
  WNDCLASS WndClass;

  //
  //  Save the instance handle.  We'll need it often.
  //

  hInst = hInstance;

  //
  //  Initialize some of the global strings.
  //

  pszAppName         = "VEDIT";
  pszAppLongName     = "Virtual Editor";
  pszFrameClass      = "VEditFrameClass";
  pszMDIClientClass  = "MDICLIENT";
  pszObjectClass     = "VEditObjectClass";
  pszValidDosChars   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-~!@#$%^&(){}";
  
  nResult = LoadString( hInst, IDS_VERSION, szVersion, sizeof( szVersion ) );
  
  if ( nResult == 0)
  {
    MsgBox( NULL, MB_ICONSTOP, "Unable to load version string" );
    return FALSE;
  }

  //
  //  Get the path and name for the program INI file.
  //
  
  wReturn = GetWindowsDirectory( (LPSTR)szWinDir, sizeof(szWinDir) - 12 );

  if ( wReturn == 0 )
  {
    MsgBox( NULL, MB_ICONSTOP, "Unable to find windows directory." );
    return FALSE;
  }

  if ( wReturn > sizeof(szWinDir) - 12 )
  {
    MsgBox( NULL, MB_ICONSTOP, "Windows directory name too long." );
    return FALSE;
  }

  n = _fstrlen( szWinDir );

  if ( n > 0 )
  {
    if ( szWinDir[n-1] != '\\' )
    {
      _fstrcat( szWinDir, "\\" );
    }
  }

  _fstrcpy( szIniFile, szWinDir );
  _fstrcat( szIniFile, "VEDIT.INI" );

  //
  //  Get the current path from the INI file.
  //
  
  GetPrivateProfileString( pszAppName,
                           "Path",
                           "",
                           szPath,
                           sizeof(szPath),
                           szIniFile );

  n = _fstrlen(szPath);

  if ( n > 0 )
  {
    if ( szPath[n-1] != '\\' )
    {
      _fstrcat( szPath, "\\" );
    }
  }

  _fstrupr( szPath );

  bGridOn = TRUE;
  lGridSpacing = 1000L;

  //
  //  Load the bitmaps for the various buttons.
  //
  
  hbmSelectPointerNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_PTR1) );
  hbmSelectPointerPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_PTR2) );
  hbmSelectVertexNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SELVERT1) );
  hbmSelectVertexPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SELVERT2) );
  hbmSelectPolygonNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SELPOLY1) );
  hbmSelectPolygonPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SELPOLY2) );
  hbmSelectGroupNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SELGRP1) );
  hbmSelectGroupPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SELGRP2) );
  hbmNewVertexNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_NEWVERT1) );
  hbmNewVertexPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_NEWVERT2) );
  hbmNewPolygonNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_NEWPOLY1) );
  hbmNewPolygonPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_NEWPOLY2) );
  hbmRotatePolygonNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_ROTPOLY1) );
  hbmRotatePolygonPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_ROTPOLY2) );
  hbmFlipPolygonNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_FLPPOLY1) );
  hbmFlipPolygonPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_FLPPOLY2) );
  hbmLinkPolygonNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_LNKPOLY1) );
  hbmLinkPolygonPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_LNKPOLY2) );
  hbmUpNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_UP1) );
  hbmUpPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_UP2) );
  hbmDownNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_DOWN1) );
  hbmDownPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_DOWN2) );
  hbmLeftNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_LEFT1) );
  hbmLeftPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_LEFT2) );
  hbmRightNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_RIGHT1) );
  hbmRightPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_RIGHT2) );
  hbmTiltUpNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_TILTUP1) );
  hbmTiltUpPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_TILTUP2) );
  hbmTiltDownNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_TILTDN1) );
  hbmTiltDownPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_TILTDN2) );
  hbmRotateLeftNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SPINLF1) );
  hbmRotateLeftPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SPINLF2) );
  hbmRotateRightNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SPINRT1) );
  hbmRotateRightPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_SPINRT2) );
  hbmZoomInNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_ZIN1) );
  hbmZoomInPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_ZIN2) );
  hbmZoomOutNormal = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_ZOUT1) );
  hbmZoomOutPressed = LoadBitmap( hInst, MAKEINTRESOURCE(IDB_ZOUT2) );

  //
  //  Register our clipboard data type.
  //

  uFormatCode = RegisterClipboardFormat("VEditObject");

  if ( uFormatCode == 0 )
  {
    return FALSE;
  }
  
  //
  //  Initialize & register the various window classes.
  //

  WndClass.style         = CS_HREDRAW | CS_VREDRAW;
  WndClass.lpfnWndProc   = (WNDPROC)Frame_WndProc;
  WndClass.cbClsExtra    = 0;
  WndClass.cbWndExtra    = 0;
  WndClass.hInstance     = hInstance;
  WndClass.hIcon         = LoadIcon( hInstance, IDI_FRAME );
  WndClass.hCursor       = LoadCursor( NULL, IDC_ARROW );
  WndClass.hbrBackground = GetStockObject( WHITE_BRUSH );
  WndClass.lpszMenuName  = IDM_FRAME;
  WndClass.lpszClassName = pszFrameClass;

  if ( !RegisterClass( &WndClass ) )
  {
    return FALSE;
  }

  WndClass.lpfnWndProc   = (WNDPROC)Object_WndProc;
  WndClass.cbWndExtra    = sizeof(LPVOID);
  WndClass.hIcon         = LoadIcon( hInstance, IDI_OBJECT );
  WndClass.lpszClassName = pszObjectClass;

  if ( !RegisterClass( &WndClass ) )
  {
    return FALSE;
  }

  //
  //  Initialize the PRINTDLG struct.
  //

  printDialog.lStructSize = sizeof(PRINTDLG);
  printDialog.hDevMode    = NULL;
  printDialog.hDevNames   = NULL;
  printDialog.Flags       = PD_RETURNDC;  
//printDialog.Flags       = PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS;
  printDialog.nCopies     = 1;

  //
  //  Success!
  //

  return TRUE;

} // InitApplication


/*******************************************************************

    NAME:       InitInstance

    SYNOPSIS:   Performs instance-wide initialization.

    ENTRY:      hInstance - The current instance handle.

                pszCmdLine - Command line arguments.

                nCmdShow - ShowWindow parameter.

    RETURNS:    BOOL - TRUE if everything initialized OK, FALSE
                    if something tragic happened.

********************************************************************/

BOOL InitInstance( HINSTANCE hInstance,
                   LPSTR     pszCmdLine,
                   INT       nCmdShow )
{
  //
  //  Create the frame window.
  //

  hwndMDIFrame = CreateWindow( pszFrameClass,
                               pszAppName,
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               NULL,
                               NULL,
                               hInstance,
                               NULL );

  if ( hwndMDIFrame == NULL )
  {
    return FALSE;
  }

  //
  //  Load our accelerators.
  //

  hAccel = LoadAccelerators( hInst, IDA_FRAME );

  if ( hAccel == NULL )
  {
    return FALSE;
  }

  //
  //  Display the main window.
  //

  nCmdShow = SW_SHOWMAXIMIZED;

  ShowWindow( hwndMDIFrame, nCmdShow );
  UpdateWindow( hwndMDIFrame );

  //
  //  Success!
  //

  return TRUE;

} // InitInstance
