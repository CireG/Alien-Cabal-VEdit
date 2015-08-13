/*******************************************************************

    vedit.c

    Main startup file for the VEDIT project.

********************************************************************/


#include "vedit.h"


//
//  Public functions.
//

/*******************************************************************

    NAME:       WinMain

    SYNOPSIS:   Normal Windows application entry point.

    ENTRY:      hInstance - The current application instance.

                hPrev - The previous instance.  Don't depend on this
                    value, as under Win32 it is always NULL.

                pszCmdLine - Points to command line arguments.

                nCmdShow - Specifies how the initial window should
                    be displayed.  Should just be passed onto
                    ShowWindow.

********************************************************************/

INT PASCAL WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrev,
                    LPSTR     pszCmdLine,
                    INT       nCmdShow )
{
  MSG msg;

  //
  //  Application-level initialization.
  //

  if ( !InitApplication( hInstance ) )
  {
    return FALSE;
  }

  //
  //  Instance-level initialization.
  //

  if ( !InitInstance( hInstance, pszCmdLine, nCmdShow ) )
  {
    return FALSE;
  }

  //
  //  Usual MDI message loop.
  //

  while ( GetMessage( &msg, 0, 0, 0 ) )
  {
    if ( !TranslateMDISysAccel( hwndMDIClient, &msg ) &&
         !TranslateAccelerator( hwndMDIFrame, hAccel, &msg ) )
    {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
  }

  return msg.wParam;

} // WinMain
