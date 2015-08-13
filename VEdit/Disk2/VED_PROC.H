/*******************************************************************

    ved_proc.h

    Global procedure declarations for the VEDIT project.

********************************************************************/


#ifndef _VED_PROC_H_
#define _VED_PROC_H_


//
//  Generic window functions.
//

HWND Wnd_CreateNew( WND_DATA *pWndData, LPSTR pszWndClass );

VOID Wnd_OnClose( HWND hwnd );

VOID Wnd_OnDestroy( HWND hwnd );

BOOL Wnd_OnQueryEndSession( HWND hwnd );


//
//  Dialog functions.
//

BOOL AllDialogs_OnInitDialog( HWND   hwnd,
                              HWND   hwndFocus,
                              LPARAM lParam );

HBRUSH AllDialogs_OnCtlColor( HWND hwnd,
                              HDC  hdc,
                              HWND hwndChild,
                              INT  type );

VOID AllDialogs_OnDestroy( HWND hwnd );

VOID AboutDialog( HWND hwndParent );

VOID PreviewBitmap( HWND          hwnd,
                    INT           idPreview,
                    INT           idBmpSize,
                    BITMAP_DATA  *pBitmapData,
                    PALETTE_DATA *pPalette );

int EditVertexPositionDialog( HWND hwndParent, WND_DATA *pWndData );

int EditVertexPropertiesDialog( HWND hwndParent, WND_DATA *pWndData );

int EditPolygonPropertiesDialog( HWND hwndParent, WND_DATA *pWndData );

int EditGroupAssignmentsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewOptionsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewStatisticsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewFileInfoDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewLevelsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewBitmapsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewTexturesDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewSoundsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewGroupsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewAnimationsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewLifeFormsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewItemsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewTriggersDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewMotionsDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewSpritesDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewNoisesDialog( HWND hwndParent, WND_DATA *pWndData );

int ViewMarksDialog( HWND hwndParent, WND_DATA *pWndData );


//
//  Frame window functions.
//

LRESULT CALLBACK __export Frame_WndProc( HWND   hwnd,
                                         UINT   nMessage,
                                         WPARAM wParam,
                                         LPARAM lParam );

VOID Frame_FileSave( WND_DATA *pWndData );

WND_DATA *Frame_CreateWindowData( WORD cid );

BOOL Frame_NewLevel( WND_DATA *pWndData, INT index );

VOID Frame_FreeLevel( WND_DATA *pWndData, INT index );

VOID Frame_FreeWindowData( WND_DATA *pWndData );


//
//  File read functions.
//

BOOL FilterObject( WND_DATA *pWndData );

BOOL LoadObjectFile( LPSTR lpszFile, WND_DATA *pWndData );


//
//  File write functions.
//

BOOL SaveObjectFile( LPSTR lpszFile, WND_DATA *pWndData );

BOOL RebuildZoneData(WND_DATA *pWndData);


//
//  Window data functions.
//

INT CountSelectedVertices( LEVEL_DATA *pLevel );

INT CountSelectedPolygons( LEVEL_DATA *pLevel );

INT CountAllSelections( LEVEL_DATA *pLevel );

INT CountAllHidden( LEVEL_DATA *pLevel );

VOID ClearSelections( LEVEL_DATA *pLevel );

VOID HideSelections( LEVEL_DATA *pLevel );

VOID HideGroup( LEVEL_DATA *pLevel, INT nGroup );

VOID HideAll( LEVEL_DATA *pLevel );

VOID ShowGroup( LEVEL_DATA *pLevel, INT nGroup );

VOID ShowAll( LEVEL_DATA *pLevel );

INT FindVertex( LEVEL_DATA *pLevel, INT nStart, VERTEX *pVertex );

INT FindPolygon( LEVEL_DATA *pLevel, INT nStart, POLYGON *pPolygon );

INT ReplaceVertex( LEVEL_DATA *pLevel, INT nOld, INT nNew );

VOID ReplacePolygon( LEVEL_DATA *pLevel, INT nOld, INT nNew );

VOID RotatePolygon(POLYGON huge *pPolygon, INT nRotation);

VOID FlipPolygon(POLYGON huge *pPolygon);

VOID ComputeBitmapCoordinates( TEXEL *p1, TEXEL *p2, TEXEL *p );

VOID ComputeBitmapCoordinates2( TEXEL *p1, TEXEL *p2, TEXEL *p );

VOID DeleteVertex( LEVEL_DATA *pLevel, INT nIndex );

VOID DeletePolygon( LEVEL_DATA *pLevel, INT nIndex );

VOID DeleteGroup( LEVEL_DATA *pLevel, INT index );

VOID DeleteTrigger( LEVEL_DATA *pLevel, INT index );

VOID DeleteMotion( LEVEL_DATA *pLevel, INT index );

VOID DeleteMark(LEVEL_DATA *pLevel, INT index);

VOID DeleteSelections( LEVEL_DATA *pLevel );

BOOL NewVertex( LEVEL_DATA *pLevel, LONG x, LONG y, LONG z );

BOOL NewPolygon( LEVEL_DATA *pLevel, INT nVertices, INT *pVertices );

VOID UnlinkPolygon( WND_DATA *pWndData, LEVEL_DATA *pLevel, INT nIndex );

VOID ConvertToUnlinked( LEVEL_DATA *pLevel, INT nLinked, POLYGON *pUnlinked );

VOID SetSelectedVertices( LEVEL_DATA *pLevel, LPSTR sx, LPSTR sy, LPSTR sz );

VOID DeleteDuplicateVertices( LEVEL_DATA *pLevel, BOOL bSelOnly );

VOID DeleteDuplicatePolygons( LEVEL_DATA *pLevel );

VOID MoveSelectedVertices( LEVEL_DATA *pLevel, LONG ldx, LONG ldy, LONG ldz );

VOID ExtendPolygonSelections( LEVEL_DATA *pLevel );


//
//  Window functions.
//

LRESULT CALLBACK __export Object_WndProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam );

HWND Object_CreateNew( WND_DATA *pWndData );

VOID Object_DrawPage( HDC hdc, RECT *rc, INT nMapMode, WND_DATA *pWndData );

VOID Object_ViewChanged( HWND hwnd, WND_DATA *pWndData );

VOID Object_CreateUndoData( HWND hwnd, WND_DATA *pWndData );

VOID Object_DeleteUndoData( HWND hwnd, WND_DATA *pWndData );

VOID Object_EditUndo( HWND hwnd, WND_DATA *pWndData );

VOID Object_EditCopy( HWND hwnd, WND_DATA *pWndData );

VOID Object_EditPaste( HWND hwnd, WND_DATA *pWndData );

VOID Object_ViewStatusUpdate( HWND hwnd, WND_DATA *pWndData );

VOID Object_GridStatusUpdate( HWND hwnd, WND_DATA *pWndData );

VOID Object_ViewHome( HWND hwnd, WND_DATA *pWndData );

VOID Object_DeleteBitmap(WND_DATA *pWndData, INT index);

VOID Object_DeleteTexture( WND_DATA *pWndData, INT index );

VOID Object_DeleteAnimation( WND_DATA *pWndData, INT index );

VOID Object_DeleteLifeForm( WND_DATA *pWndData, INT index );

VOID Object_DeleteItem( WND_DATA *pWndData, INT index );

VOID Object_DeleteSprite( WND_DATA *pWndData, INT index );

VOID Object_DeleteNoise( WND_DATA *pWndData, INT index );

VOID Object_DeleteSound( WND_DATA *pWndData, INT index );

VOID Object_DeleteLevel(WND_DATA *pWndData, INT index);

BOOL Object_InsertLevel(WND_DATA *pWndData, INT index);

VOID Object_ResetPixelSize( HWND hwnd, WND_DATA *pWndData );


//
//  Initialization functions.
//

BOOL InitApplication( HINSTANCE hInstance );

BOOL InitInstance( HINSTANCE hInstance,
                   LPSTR     pszCmdLine,
                   INT       nCmdShow );


//
//  Utility functions.
//

LPSTR GenWindowTitle( WND_DATA *pWndData, LPSTR szTitle, INT cbTitle );
INT LoadStr( UINT idResource, LPSTR lpszBuffer, INT cbBuffer );
double Round( double num );
LPSTR DoubleToString( LPSTR szBuf, double num, INT precision );
INT MsgBox( HWND hwndParent, UINT fuType, LPSTR pszFormat, ... );
VOID ExtPrintf( HDC hdc, INT x, INT y, LPSTR pszFormat, ... );
BOOL isalnum_str( char *str );
VOID StripTrailingSpaces( char *str );
VOID StripExtension( char *dos_name );
BOOL CreatePath( char *path );
BOOL CopyFile( LPSTR szReadFile, LPSTR szWriteFile );
INT FilePrintf( HFILE hf, LPSTR pszFormat, ... );
INT GetFirstBitmapIndex(ANIMATION_DATA *pAnimation);
INT GetNumLifeFormElements( INT nLifeFormType );
INT GetNumLifeFormBitmaps( INT nLifeFormType );
INT GetNumLifeFormSounds( INT nLifeFormType );
LPSTR GetLifeFormTypeString( INT nLifeFormType );
LPSTR GetLifeFormElementString( INT nLifeFormType, INT nElement );
INT GetNumItemElements( INT nItemType );
INT GetNumItemBitmaps( INT nItemType );
INT GetNumItemSounds( INT nItemType );
LPSTR GetItemTypeString( INT nItemType );
LPSTR GetItemElementString( INT nItemType, INT nElement );
BOOL PointInPolygon( POLYGON *p, VERTEX huge *v, double x, double y, double z );
BOOL IsPolygonFlat(POLYGON *p, VERTEX huge *v);
BOOL IsPolygonConvex(POLYGON *p, VERTEX huge *v);

#endif  // _VED_PROC_H_
