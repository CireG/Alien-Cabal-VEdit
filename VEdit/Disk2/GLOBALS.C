/*******************************************************************

    globals.c

    Global variable declarations for the VEDIT project.

********************************************************************/


#include "vedit.h"


//
//  Various handles.
//

HINSTANCE       hInst;                          // The current instance handle
HACCEL          hAccel;                         // Accelerator table handle
HWND            hwndMDIFrame;                   // MDI frame  window handle
HWND            hwndMDIClient;                  // MDI client window handle
HPALETTE        hPalCommon;                     // Common bitmap palette


//
//  Window class names.
//

LPSTR           pszFrameClass;                  // MDI frame  window class
LPSTR           pszMDIClientClass;              // MDI client window class
LPSTR           pszObjectClass;                 // MDI child  window class


//
//  Miscellaneous data.
//

LPSTR           pszAppName;                     // Application name
LPSTR           pszAppLongName;                 // Application long name
CHAR            szIniFile[128];                 // Application INI file name
CHAR            szWinDir[128];                  // Windows directory
PRINTDLG        printDialog;                    // Print dialog structure
LPSTR           pszValidDosChars;               // Chars usable in DOS names
BYTE huge      *pSoundPlayData;                 // Current sound playing

//
//  Application specific globals.
//

UINT            uFormatCode;                    // clipboard format code
CHAR            szVersion[16];                  // Software version
CHAR            szPath[128];                    // Current path
BOOL            bGridOn;                        // Drawing plane grid
LONG            lGridSpacing;                   // Spacing in mm
HBITMAP         hbmSelectPointerNormal;         // button normal
HBITMAP         hbmSelectPointerPressed;        // button pressed
HBITMAP         hbmSelectVertexNormal;          // button normal
HBITMAP         hbmSelectVertexPressed;         // button pressed
HBITMAP         hbmSelectPolygonNormal;         // button normal
HBITMAP         hbmSelectPolygonPressed;        // button pressed
HBITMAP         hbmSelectReferenceNormal;       // button normal
HBITMAP         hbmSelectReferencePressed;      // button pressed
HBITMAP         hbmSelectGroupNormal;           // button normal
HBITMAP         hbmSelectGroupPressed;          // button pressed
HBITMAP         hbmSetGridNormal;               // button normal
HBITMAP         hbmSetGridPressed;              // button pressed
HBITMAP         hbmNewVertexNormal;             // button normal
HBITMAP         hbmNewVertexPressed;            // button pressed
HBITMAP         hbmNewPolygonNormal;            // button normal
HBITMAP         hbmNewPolygonPressed;           // button pressed
HBITMAP         hbmRotatePolygonNormal;         // button normal
HBITMAP         hbmRotatePolygonPressed;        // button pressed
HBITMAP         hbmFlipPolygonNormal;           // button normal
HBITMAP         hbmFlipPolygonPressed;          // button pressed
HBITMAP         hbmLinkPolygonNormal;           // button normal
HBITMAP         hbmLinkPolygonPressed;          // button pressed
HBITMAP         hbmUpNormal;                    // button normal
HBITMAP         hbmUpPressed;                   // button pressed
HBITMAP         hbmDownNormal;                  // button normal
HBITMAP         hbmDownPressed;                 // button pressed
HBITMAP         hbmLeftNormal;                  // button normal
HBITMAP         hbmLeftPressed;                 // button pressed
HBITMAP         hbmRightNormal;                 // button normal
HBITMAP         hbmRightPressed;                // button pressed
HBITMAP         hbmTiltUpNormal;                // button normal
HBITMAP         hbmTiltUpPressed;               // button pressed
HBITMAP         hbmTiltDownNormal;              // button normal
HBITMAP         hbmTiltDownPressed;             // button pressed
HBITMAP         hbmRotateLeftNormal;            // button normal
HBITMAP         hbmRotateLeftPressed;           // button pressed
HBITMAP         hbmRotateRightNormal;           // button normal
HBITMAP         hbmRotateRightPressed;          // button pressed
HBITMAP         hbmZoomInNormal;                // button normal
HBITMAP         hbmZoomInPressed;               // button pressed
HBITMAP         hbmZoomOutNormal;               // button normal
HBITMAP         hbmZoomOutPressed;              // button pressed
