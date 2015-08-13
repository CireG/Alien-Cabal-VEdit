/*******************************************************************

    ved_data.h

    Global variable declarations for the VEDIT project.

********************************************************************/


#ifndef _VED_DATA_H_
#define _VED_DATA_H_


//
//  Various handles.
//

extern HINSTANCE        hInst;                  // The current instance handle
extern HACCEL           hAccel;                 // Accelerator table handle
extern HWND             hwndMDIFrame;           // MDI frame  window handle
extern HWND             hwndMDIClient;          // MDI client window handle
extern HPALETTE         hPalCommon;             // Common bitmap palette


//
//  Window class names.
//

extern LPSTR            pszFrameClass;          // MDI frame  window class
extern LPSTR            pszMDIClientClass;      // MDI client window class
extern LPSTR            pszObjectClass;         // MDI child  window class


//
//  Miscellaneous data.
//

extern  LPSTR           pszAppName;             // Application name
extern  LPSTR           pszAppLongName;         // Application long name
extern  CHAR            szIniFile[128];         // Application INI file name
extern  CHAR            szWinDir[128];          // Windows directory
extern  PRINTDLG        printDialog;            // Print dialog structure
extern  LPSTR           pszValidDosChars;       // Chars usable in DOS names
extern  BYTE huge      *pSoundPlayData;         // Current sound playing


//
//  Application specific globals.
//

extern UINT             uFormatCode;            // clipboard format code
extern CHAR             szVersion[16];          // Software version
extern CHAR             szPath[128];            // Current path
extern BOOL             bGridOn;                // Drawing plane grid
extern LONG             lGridSpacing;           // Spacing in mm
extern HBITMAP          hbmSelectPointerNormal; // button normal
extern HBITMAP          hbmSelectPointerPressed;// button pressed
extern HBITMAP          hbmSelectVertexNormal;  // button normal
extern HBITMAP          hbmSelectVertexPressed; // button pressed
extern HBITMAP          hbmSelectPolygonNormal; // button normal
extern HBITMAP          hbmSelectPolygonPressed;// button pressed
extern HBITMAP          hbmSelectReferenceNormal;// button normal
extern HBITMAP          hbmSelectReferencePressed;// button pressed
extern HBITMAP          hbmSelectGroupNormal;   // button normal
extern HBITMAP          hbmSelectGroupPressed;  // button pressed
extern HBITMAP          hbmSetGridNormal;       // button normal
extern HBITMAP          hbmSetGridPressed;      // button pressed
extern HBITMAP          hbmNewVertexNormal;     // button normal
extern HBITMAP          hbmNewVertexPressed;    // button pressed
extern HBITMAP          hbmNewPolygonNormal;    // button normal
extern HBITMAP          hbmNewPolygonPressed;   // button pressed
extern HBITMAP          hbmRotatePolygonNormal; // button normal
extern HBITMAP          hbmRotatePolygonPressed;// button pressed
extern HBITMAP          hbmFlipPolygonNormal;   // button normal
extern HBITMAP          hbmFlipPolygonPressed;  // button pressed
extern HBITMAP          hbmLinkPolygonNormal;   // button normal
extern HBITMAP          hbmLinkPolygonPressed;  // button pressed
extern HBITMAP          hbmUpNormal;            // button normal
extern HBITMAP          hbmUpPressed;           // button pressed
extern HBITMAP          hbmDownNormal;          // button normal
extern HBITMAP          hbmDownPressed;         // button pressed
extern HBITMAP          hbmLeftNormal;          // button normal
extern HBITMAP          hbmLeftPressed;         // button pressed
extern HBITMAP          hbmRightNormal;         // button normal
extern HBITMAP          hbmRightPressed;        // button pressed
extern HBITMAP          hbmTiltUpNormal;        // button normal
extern HBITMAP          hbmTiltUpPressed;       // button pressed
extern HBITMAP          hbmTiltDownNormal;      // button normal
extern HBITMAP          hbmTiltDownPressed;     // button pressed
extern HBITMAP          hbmRotateLeftNormal;    // button normal
extern HBITMAP          hbmRotateLeftPressed;   // button pressed
extern HBITMAP          hbmRotateRightNormal;   // button normal
extern HBITMAP          hbmRotateRightPressed;  // button pressed
extern HBITMAP          hbmZoomInNormal;        // button normal
extern HBITMAP          hbmZoomInPressed;       // button pressed
extern HBITMAP          hbmZoomOutNormal;       // button normal
extern HBITMAP          hbmZoomOutPressed;      // button pressed

#endif  // _VED_DATA_H_
