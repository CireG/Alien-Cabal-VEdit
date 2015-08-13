/*******************************************************************

    objwnd.c

    Routines for managing the windows.

********************************************************************/


#include "vedit.h"


//
//  Private definitions.
//

#define OBJWND_TIMER_ID         1
#define AUTO_REPEAT_DELAY       5

// Control positioning definitions.

#define CONTROL_AREA_HEIGHT1    24      // top control area
#define CONTROL_AREA_HEIGHT2    24      // bottom control area
#define CONTROL_AREA_WIDTH1     80      // left side control area
#define CONTROL_AREA_WIDTH2     150     // right side control area
#define CONTROL_AREA_MARGIN     25      // left and right side upper margin
#define GRID_CONTROLS_OFFSET    230     // right side draw controls y offset
#define BTN_WIDTH1              20      // left side controls
#define BTN_HEIGHT1             20      // left side controls
#define BTN_WIDTH2              16      // right side controls
#define BTN_HEIGHT2             16      // right side controls
#define BTN_MARGIN              2       // right side button spacing
#define BTN_MARGIN2             4       // right side button spacing
#define BTN_MARGIN3             4       // left side button spacing
#define BTN_MARGIN4             16      // left side button spacing

// Control indecies.

#define MODE_POINTER            0
#define MODE_SELECT_VERTEX      1
#define MODE_SELECT_POLYGON     2
#define MODE_SELECT_GROUP       3
#define MODE_NEW_VERTEX         4
#define MODE_NEW_POLYGON        5
#define MODE_ROTATE_POLYGON     6
#define MODE_FLIP_POLYGON       7
#define MODE_LINK_POLYGON       8
#define VIEW_PAN_UP             9
#define VIEW_PAN_DOWN           10
#define VIEW_PAN_LEFT           11
#define VIEW_PAN_RIGHT          12
#define VIEW_TILT_UP            13
#define VIEW_TILT_DOWN          14
#define VIEW_ROTATE_LEFT        15
#define VIEW_ROTATE_RIGHT       16
#define VIEW_ZOOM_IN            17
#define VIEW_ZOOM_OUT           18
#define GRID_Y_INCREASE         19
#define GRID_Y_DECREASE         20
#define GRID_X_INCREASE         21
#define GRID_X_DECREASE         22
#define GRID_TILT_UP            23
#define GRID_TILT_DOWN          24
#define GRID_ROTATE_LEFT        25
#define GRID_ROTATE_RIGHT       26
#define GRID_Z_INCREASE         27
#define GRID_Z_DECREASE         28
#define NUM_MODE_CONTROLS       9
#define NUM_VIEW_CONTROLS       10
#define NUM_GRID_CONTROLS       10
#define NUM_CONTROLS            29

// Status window defines

#define STATUS_HEIGHT           14
#define STATUS_VERT_SPACING     18
#define STATUS_COLUMN1_POS      16
#define STATUS_COLUMN2_POS      82
#define STATUS_COLUMN1_WIDTH    46
#define STATUS_COLUMN2_WIDTH    54
#define VIEW_STATUS_OFFSET      140
#define GRID_STATUS_OFFSET      230
#define CURSOR_STATUS_OFFSET    250
#define CURSOR_STATUS_COL_POS   16

// Status indicies

#define VIEW_STATUS_THETA       0
#define VIEW_STATUS_PHI         1
#define VIEW_STATUS_X           2
#define VIEW_STATUS_Y           3
#define VIEW_STATUS_Z           4
#define GRID_STATUS_THETA       5
#define GRID_STATUS_PHI         6
#define GRID_STATUS_X           7
#define GRID_STATUS_Y           8
#define GRID_STATUS_Z           9
#define CURSOR_STATUS_ABS_X     10
#define CURSOR_STATUS_ABS_Y     11
#define CURSOR_STATUS_ABS_Z     12
#define CURSOR_STATUS_REL_X     13
#define CURSOR_STATUS_REL_Y     14
#define CURSOR_STATUS_REL_Z     15
#define NUM_STATUS_WINDOWS      16

//
//  Private prototypes.
//

VOID Object_ClearWindow( HDC hdc, RECT *rc, INT nMapMode );

VOID Object_ShowTitle( HDC hdc, RECT *rc, LPSTR pszFormat, ... );

VOID Object_ShowMessage( HDC hdc, RECT *rc, LPSTR pszFormat, ... );

VOID Object_GetButtonRects( RECT *rc, RECT *btnRect );

VOID Object_DrawButtons( HDC hdc, RECT *rc );

VOID Object_GetStatusRects( RECT *rc, RECT *stsRect );

VOID Object_DrawStatusWindows( HDC hdc, RECT *rc );

VOID Object_ClearStatusWindow( HDC hdc, RECT *rc, WND_DATA *pWndData, INT nIndex );

VOID Object_UpdateStatusWindow( HDC hdc, RECT *rc, WND_DATA *pWndData, INT nIndex );

VOID Object_ControlPressed( HDC hdc, RECT *rc, WND_DATA *pWndData, INT nIndex );

VOID Object_DrawLine( HDC hdc, WND_DATA *pWndData, double *pLine, INT x_off, INT y_off );

VOID Object_DrawGrid( HDC hdc, WND_DATA *pWndData );

VOID Object_DrawVertex( HDC hdc, WND_DATA *pWndData, LONG x, LONG y, LONG z, INT nOrder, INT nDir );

VOID Object_DrawVertices( HDC hdc, WND_DATA *pWndData );

VOID Object_DrawPolygon( HDC hdc, WND_DATA *pWndData, INT nVertices, WORD *pVertices );

VOID Object_DrawPolygons( HDC hdc, WND_DATA *pWndData );

VOID Object_CalcCursorPosition( WND_DATA *pWndData, INT x, INT y );

VOID Object_DrawCursorLines( HDC hdc, WND_DATA *pWndData, INT x, INT y );

VOID Object_DrawSelectedVertex( HDC hdc, WND_DATA *pWndData, INT x, INT y );

VOID Object_DrawSelectedPolygon( HDC hdc, WND_DATA *pWndData, INT x, INT y );

VOID Object_DrawLinkArrow( HDC hdc, WND_DATA *pWndData, POLYGON *pLinked, POLYGON *pLink );

VOID Object_RegenerateView( INT cx, INT cy, WND_DATA *pWndData );

VOID Object_DrawView( HDC hdc, HBITMAP hbm, INT width, INT height );

BOOL Object_OnCreate( HWND hwnd, CREATESTRUCT FAR* lpCreateStruct );

VOID Object_OnDestroy( HWND hwnd );

VOID Object_OnTimer( HWND hwnd, UINT id );

VOID Object_OnSize( HWND hwnd, UINT state, int cx, int cy );

VOID Object_OnPaint( HWND hwnd );

VOID Object_OnLButtonDown( HWND hwnd,
                           BOOL fDoubleClick,
                           INT  x,
                           INT  y,
                           UINT keyFlags );

VOID Object_OnMouseMove( HWND hwnd,
                         INT  x,
                         INT  y,
                         UINT keyFlags);

VOID Object_OnLButtonUp( HWND hwnd,
                         INT  x,
                         INT  y,
                         UINT keyFlags );

VOID Object_OnKey( HWND hwnd, UINT vk, BOOL fDown, INT cRepeat, UINT flags );

VOID Object_OnChar( HWND hwnd, UINT vk, INT cRepeat );


//
//  Private data.
//

BOOL bGotCursor;
BOOL bControlActive;
INT nControlIndex;
INT nAutoRepeatDelay;
INT nMode;


//
//  Public functions.
//

/*******************************************************************

    NAME:       Object_CreateNew

    SYNOPSIS:   Creates a new window.

    ENTRY:      pWndData - Window data pointer.

    RETURNS:    HWND - Handle to the new child window.

********************************************************************/

HWND Object_CreateNew(WND_DATA *pWndData)
{
  HWND hwnd;

  hwnd = Wnd_CreateNew(pWndData, pszObjectClass);

  return hwnd;

} // Object_CreateNew


/*******************************************************************

    NAME:       Object_DrawPage

    SYNOPSIS:   Display (or print) the page.

    ENTRY:      hdc - Device context.
                
                nMapMode - MM_TEXT (screen) or MM_LOENGLISH (printer)

                pWndData - Window data pointer.

********************************************************************/

VOID Object_DrawPage( HDC       hdc,
                      RECT     *rc,
                      INT       nMapMode,
                      WND_DATA *pWndData )
{
  //
  //  Clear the screen.
  //

  Object_ClearWindow(hdc, rc, nMapMode);

  //
  //  Draw the controls.
  //

  Object_DrawButtons(hdc, rc);

  //
  //  Draw the status windows.
  //

  Object_DrawStatusWindows(hdc, rc);

  return;

} // Object_DrawPage


/*******************************************************************

    NAME:       Object_ViewChanged

    SYNOPSIS:   Regenerate and draw the view.

********************************************************************/

VOID Object_ViewChanged(HWND hwnd, WND_DATA *pWndData)
{
  HDC hdc;
  RECT rc;

  hdc = GetDC(hwnd);

  GetClientRect(hwnd, &rc);

  Object_RegenerateView( rc.right - rc.left,
                         rc.bottom - rc.top,
                         pWndData );

  Object_DrawView( hdc,
                   pWndData->hbmView,
                   pWndData->bm_width,
                   pWndData->bm_height );

  ReleaseDC(hwnd, hdc);
      
  return;

} // Object_ViewChanged


/*******************************************************************

    NAME:       Object_CreateUndoData

    SYNOPSIS:   Create undo data for current level.

    ENTRY:      hwnd - Parent window handle.

                pWndData - Window data pointer.

********************************************************************/

VOID Object_CreateUndoData( HWND hwnd, WND_DATA *pWndData )
{
  INT num_vertices, num_polygons;
  BYTE huge *pData;
  LEVEL_DATA *pLevel;
  DWORD dwSize;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  if (pWndData->pUndoData)
  {
    GlobalFreePtr(pWndData->pUndoData);
    pWndData->pUndoData = NULL;
  }

  pLevel = pWndData->pLevelData + pWndData->level_index;

  num_vertices = pLevel->nVertices;
  num_polygons = pLevel->nPolygons;

  //
  //  Allocate memory for the data.
  //

  dwSize = (DWORD)sizeof(num_vertices) +
           (DWORD)sizeof(num_polygons) +
           (DWORD)num_vertices * (DWORD)sizeof(VERTEX) +
           (DWORD)num_polygons * (DWORD)sizeof(POLYGON);

  pData = GlobalAllocPtr(GPTR,dwSize);

  if (pData == NULL)
  {
    SetCursor(hcurSave);
    return;
  }

  //
  //  Copy the data into the memory block.
  //

  pWndData->pUndoData = pData;

  _fmemcpy(pData, &num_vertices, sizeof(num_vertices));
  pData += sizeof(num_vertices);

  _fmemcpy(pData, &num_polygons, sizeof(num_polygons));
  pData += sizeof(num_polygons);

  dwSize = (DWORD)num_vertices * (DWORD)sizeof(VERTEX);
  hmemcpy(pData, pLevel->pVertexData, dwSize);
  pData += dwSize;

  dwSize = (DWORD)num_polygons * (DWORD)sizeof(POLYGON);
  hmemcpy(pData, pLevel->pPolygonData, dwSize);

  SetCursor(hcurSave);

  return;

} // Object_CreateUndoData


/*******************************************************************

    NAME:       Object_DeleteUndoData

    SYNOPSIS:   Delete undo data for current level.

    ENTRY:      hwnd - Parent window handle.

                pWndData - Window data pointer.

********************************************************************/

VOID Object_DeleteUndoData( HWND hwnd, WND_DATA *pWndData )
{
  if (pWndData->pUndoData)
  {
    GlobalFreePtr(pWndData->pUndoData);
    pWndData->pUndoData = NULL;
  }

  return;

} // Object_DeleteUndoData


/*******************************************************************

    NAME:       Object_EditUndo

    SYNOPSIS:   Undo the last operation.

    ENTRY:      hwnd - Parent window handle.

                pWndData - Window data pointer.

********************************************************************/

VOID Object_EditUndo( HWND hwnd, WND_DATA *pWndData )
{
  INT num_vertices, num_polygons;
  BYTE huge *pData;
  DWORD dwSize;
  LEVEL_DATA *pLevel;
  HCURSOR hcurSave;

  if (pWndData->pUndoData == NULL)
  {
    return;
  }

  pLevel = pWndData->pLevelData + pWndData->level_index;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  pData = pWndData->pUndoData;

  _fmemcpy(&num_vertices, pData, sizeof(num_vertices));
  pData += sizeof(num_vertices);

  _fmemcpy(&num_polygons, pData, sizeof(num_polygons));
  pData += sizeof(num_polygons);

  //
  //  Copy the undo data to the current level.
  //

  pLevel->nVertices = num_vertices;
  dwSize = (DWORD)sizeof(VERTEX) * (DWORD)num_vertices;
  hmemcpy(pLevel->pVertexData, pData, dwSize);
  pData += dwSize;

  pLevel->nPolygons = num_polygons;
  dwSize = (DWORD)sizeof(POLYGON) * (DWORD)num_polygons;
  hmemcpy(pLevel->pPolygonData, pData, dwSize);

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwnd, pWndData);

  SetCursor(hcurSave);

  return;

} // Object_EditUndo


/*******************************************************************

    NAME:       Object_EditCopy

    SYNOPSIS:   Copy the selected vertices to the clipboard.

********************************************************************/

VOID Object_EditCopy(HWND hwnd, WND_DATA *pWndData)
{
  INT i, j, n;
  INT huge *pVertexSelected, huge *pVertexIndex, huge *pPolygonIndex;
  HANDLE hData;
  BYTE huge *pData;
  LEVEL_DATA *pLevel;
  POLYGON huge *pPolygon, cb_polygon;
  DWORD dwSize;
  CB_INFO cb;
  HCURSOR hcurSave;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  //
  //  Create the vertex selection and new index arrays.
  //

  dwSize = (DWORD)MAX_NUM_VERTICES * (DWORD)sizeof(INT) +
           (DWORD)MAX_NUM_VERTICES * (DWORD)sizeof(INT) +
           (DWORD)MAX_NUM_POLYGONS * (DWORD)sizeof(INT);

  pVertexSelected = (INT huge *)GlobalAllocPtr(GPTR,dwSize);

  if (pVertexSelected == NULL)
  {
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return;
  }

  pVertexIndex  = pVertexSelected + MAX_NUM_VERTICES;
  pPolygonIndex = pVertexIndex + MAX_NUM_VERTICES;

  _fmemset(&cb, 0, sizeof(cb));

  _fstrcpy(cb.level_name, pLevel->level_name);
  _fstrcpy(cb.level_author, pLevel->level_author);
  _fstrcpy(cb.sol_text, pLevel->sol_text);
  _fstrcpy(cb.eol_text, pLevel->eol_text);

  cb.flags = pLevel->flags;
  cb.bkg_bitmap_index = pLevel->bkg_bitmap_index;
  cb.sol_bitmap_index = pLevel->sol_bitmap_index;
  cb.eol_bitmap_index = pLevel->eol_bitmap_index;

  cb.num_polygons = 0;

  pPolygon = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (pPolygon->flags & PF_SELECTED)
    {
      pPolygonIndex[i] = cb.num_polygons++;

      n = (pPolygon->flags & 7) + 3;

      for(j = 0; j < n; ++j)
      {
        pVertexSelected[pPolygon->vertices[j]] = TRUE;
      }
    }

    ++pPolygon;
  }

  cb.num_vertices = 0;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if ((pLevel->pVertexData + i)->flags & VF_SELECTED)
    {
      pVertexSelected[i] = TRUE;
    }

    if (pVertexSelected[i])
    {
      pVertexIndex[i] = cb.num_vertices++;
    }
  }

  cb.num_triggers = pLevel->nTriggers;
  cb.num_motions = pLevel->nMotions;
  cb.num_groups = pLevel->nGroups;
  cb.num_marks = pLevel->nMarks;

  //
  //  Allocate memory for the data.
  //

  dwSize = (DWORD)sizeof(cb) +
           (DWORD)cb.num_vertices * (DWORD)sizeof(VERTEX) +
           (DWORD)cb.num_polygons * (DWORD)sizeof(POLYGON) +
           (DWORD)cb.num_triggers * (DWORD)sizeof(TRIGGER) +
           (DWORD)cb.num_motions  * (DWORD)sizeof(MOTION) +
           (DWORD)cb.num_groups   * (DWORD)sizeof(GROUP) +
           (DWORD)cb.num_marks    * (DWORD)sizeof(MARK);

  hData = GlobalAlloc(GHND, dwSize);

  if (hData == NULL)
  {
    GlobalFreePtr(pVertexSelected);
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return;
  }

  pData = GlobalLock(hData);

  //
  //  Copy the data into the memory block.
  //

  cb.grid_x = pLevel->grid_x;
  cb.grid_y = pLevel->grid_y;
  cb.grid_z = pLevel->grid_z;
  cb.grid_rotation = pLevel->grid_rotation;
  cb.grid_elevation = pLevel->grid_elevation;

  _fmemcpy(pData, &cb, sizeof(cb));

  pData += sizeof(cb);

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (pVertexSelected[i])
    {
      _fmemcpy(pData, pLevel->pVertexData + i, sizeof(VERTEX));
      ((VERTEX *)pData)->flags |= VF_SELECTED;
      pData += sizeof(VERTEX);
    }
  }

  pPolygon = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (pPolygon->flags & PF_SELECTED)
    {
      _fmemcpy(&cb_polygon, pPolygon, sizeof(POLYGON));

      if (pPolygon->flags & PF_LINKED)
      {
        if ((pLevel->pPolygonData + pPolygon->polygon_link)->flags & PF_SELECTED)
        {
          cb_polygon.polygon_link = pPolygonIndex[pPolygon->polygon_link];
        }
        else
        {
          ConvertToUnlinked(pLevel, i, &cb_polygon);
        }
      }

      n = (pPolygon->flags & 7) + 3;

      for(j = 0; j < n; ++j)
      {
        cb_polygon.vertices[j] = pVertexIndex[pPolygon->vertices[j]];
      }

      cb_polygon.flags |= PF_SELECTED;
      _fmemcpy( pData, &cb_polygon, sizeof(POLYGON) );
      pData += sizeof(POLYGON);
    }

    ++pPolygon;
  }

  for(i = 0; i < cb.num_triggers; ++i)
  {
    _fmemcpy( pData, &pLevel->pTriggerData[i], sizeof(TRIGGER) );
    pData += sizeof(TRIGGER);
  }

  for(i = 0; i < cb.num_motions; ++i)
  {
    _fmemcpy( pData, &pLevel->pMotionData[i], sizeof(MOTION) );
    pData += sizeof(MOTION);
  }

  for(i = 0; i < cb.num_groups; ++i)
  {
    _fmemcpy( pData, &pLevel->pGroupData[i], sizeof(GROUP) );
    pData += sizeof(GROUP);
  }

  for(i = 0; i < cb.num_marks; ++i)
  {
    _fmemcpy( pData, &pLevel->pMarkData[i], sizeof(MARK) );
    pData += sizeof(MARK);
  }

  GlobalUnlock(hData);
  GlobalFreePtr(pVertexSelected);

  //
  //  Send the data to the clipboard.
  //

  if (!OpenClipboard(hwnd))
  {
    GlobalFree(hData);
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to open the clipboard" );

    return;
  }

  if (!EmptyClipboard())
  {
    CloseClipboard();
    GlobalFree(hData);
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to empty the clipboard" );

    return;
  }

  if (!SetClipboardData(uFormatCode, hData))
  {
    CloseClipboard();
    GlobalFree(hData);
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable set the clipboard data" );

    return;
  }

  CloseClipboard();
  SetCursor(hcurSave);

  return;

} // Object_EditCopy


/*******************************************************************

    NAME:       Object_EditPaste

    SYNOPSIS:   Paste the polygons and vertices from the clipboard.

********************************************************************/

VOID Object_EditPaste(HWND hwnd, WND_DATA *pWndData)
{
  HANDLE hData;
  BYTE huge *pData;
  INT i, j, n;
  LEVEL_DATA *pLevel;
  POLYGON huge *pPolygonData;
  VERTEX huge *pVertexData;
  DWORD dwSize;
  CB_INFO cb;
  double sin_grid_theta, cos_grid_theta, sin_grid_phi, cos_grid_phi;
  double sin_cb_theta, cos_cb_theta, sin_cb_phi, cos_cb_phi;
  double rx, ry, rz, x, y, z;
  HCURSOR hcurSave;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  //
  //  Open the clipboard.
  //

  if (!OpenClipboard(hwnd))
  {
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to open the clipboard" );

    return;
  }

  //
  //  Get a handle to the clipboard data.
  //

  hData = GetClipboardData(uFormatCode);

  if (hData == NULL)
  {
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Nothing to paste" );

    return;
  }

  pData = GlobalLock(hData);

  _fmemcpy(&cb, pData, sizeof(cb));

  pData += sizeof(cb);

  //
  //  Make sure there is enough space in memory.
  //

  if (pLevel->nVertices > MAX_NUM_VERTICES - cb.num_vertices)
  {
    GlobalUnlock(hData);
    CloseClipboard();
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Not enough vertex space" );

    return;
  }

  if (pLevel->nPolygons > MAX_NUM_POLYGONS - cb.num_polygons)
  {
    GlobalUnlock(hData);
    CloseClipboard();
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Not enough polygon space" );

    return;
  }

  //
  //  Deselect all current selections.
  //

  ClearSelections(pLevel);
  Object_ViewChanged(hwnd, pWndData);

  //
  //  Append the clipboard data.
  //

  pVertexData = pLevel->pVertexData + pLevel->nVertices;
  pPolygonData = pLevel->pPolygonData + pLevel->nPolygons;

  dwSize = (DWORD)sizeof(VERTEX) * (DWORD)cb.num_vertices;
  hmemcpy(pVertexData, pData, dwSize);
  pData += dwSize;

  dwSize = (DWORD)sizeof(POLYGON) * (DWORD)cb.num_polygons;
  hmemcpy(pPolygonData, pData, dwSize);
  pData += dwSize;

  GlobalUnlock(hData);
  CloseClipboard();

  //
  //  Adjust all vertices for the change in grid position.
  //

  sin_cb_theta = sin(cb.grid_rotation);
  cos_cb_theta = cos(cb.grid_rotation);
  sin_cb_phi   = sin(cb.grid_elevation);
  cos_cb_phi   = cos(cb.grid_elevation);

  sin_grid_theta = sin(pLevel->grid_rotation);
  cos_grid_theta = cos(pLevel->grid_rotation);
  sin_grid_phi   = sin(pLevel->grid_elevation);
  cos_grid_phi   = cos(pLevel->grid_elevation);

  for(i = 0; i < cb.num_vertices; ++i)
  {
    //
    //  Translate coordinates from real space to grid space.
    //

    rx = (double)((pVertexData + i)->x - cb.grid_x);
    ry = (double)((pVertexData + i)->y - cb.grid_y);
    rz = (double)((pVertexData + i)->z - cb.grid_z);

    x = (rx * cos_cb_theta + ry * sin_cb_theta) * cos_cb_phi - rz * sin_cb_phi;
    y = (ry * cos_cb_theta - rx * sin_cb_theta);
    z = (ry * sin_cb_theta + rx * cos_cb_theta) * sin_cb_phi + rz * cos_cb_phi;

    //
    //  Translate coordinates from grid space to real space.
    //

    rx = (x * cos_grid_phi + z * sin_grid_phi) * cos_grid_theta - y * sin_grid_theta;
    ry = (x * cos_grid_phi + z * sin_grid_phi) * sin_grid_theta + y * cos_grid_theta;
    rz = (z * cos_grid_phi - x * sin_grid_phi);

    rx += (double)pLevel->grid_x;
    ry += (double)pLevel->grid_y;
    rz += (double)pLevel->grid_z;

    (pVertexData + i)->x = (float)rx;
    (pVertexData + i)->y = (float)ry;
    (pVertexData + i)->z = (float)rz;

    if ((pVertexData + i)->group_index >= pLevel->nGroups)
    {
      (pVertexData + i)->group_index = -1;
    }

    if ((pVertexData + i)->lifeform_index >= pWndData->nLifeForms)
    {
      (pVertexData + i)->lifeform_index = -1;
    }

    if ((pVertexData + i)->item_index >= pWndData->nItems)
    {
      (pVertexData + i)->item_index = -1;
    }

    if ((pVertexData + i)->sprite_index >= pWndData->nSprites)
    {
      (pVertexData + i)->sprite_index = -1;
    }

    if ((pVertexData + i)->noise_index >= pWndData->nNoises)
    {
      (pVertexData + i)->noise_index = -1;
    }

    if ((pVertexData + i)->trigger_index >= pLevel->nTriggers)
    {
      (pVertexData + i)->trigger_index = -1;
    }
  }

  //
  //  Check for invalid indecies and adjust all vertex and polygon
  //  references.
  //

  for(i = 0; i < cb.num_polygons; ++i)
  {
    if ((pPolygonData + i)->texture_index >= pWndData->nTextures)
    {
      (pPolygonData + i)->texture_index = -1;
    }

    if ((pPolygonData + i)->group_index >= pLevel->nGroups)
    {
      (pPolygonData + i)->group_index = -1;
    }

    if ((pPolygonData + i)->animation_index >= pWndData->nAnimations)
    {
      (pPolygonData + i)->animation_index = -1;
    }

    if ((pPolygonData + i)->motion_index >= pLevel->nMotions)
    {
      (pPolygonData + i)->motion_index = -1;
    }

    if ((pPolygonData + i)->flags & PF_LINKED)
    {
      (pPolygonData + i)->polygon_link += pLevel->nPolygons;
    }

    if ((pPolygonData + i)->damage_animation_index >= pWndData->nAnimations)
    {
      (pPolygonData + i)->damage_animation_index = -1;
    }

    n = ((pPolygonData + i)->flags & 7) + 3;

    for(j = 0; j < n; ++j)
    {
      (pPolygonData + i)->vertices[j] += pLevel->nVertices;
    }
  }

  //
  //  Adjust the array sizes to include the new data.
  //

  pLevel->nVertices += cb.num_vertices;
  pLevel->nPolygons += cb.num_polygons;
  pVertexData = pLevel->pVertexData;
  pPolygonData = pLevel->pPolygonData;

  //
  //  Remove duplicate vertices.
  //

  DeleteDuplicateVertices(pLevel, FALSE);

  //
  //  Remove duplicate polygons.
  //

  DeleteDuplicatePolygons(pLevel);

#if 0

  //
  //  Select duplicate polygons.
  //

  n = 0;

  for(i = pLevel->nPolygons - cb.num_polygons; i < pLevel->nPolygons; ++i)
  {
    nDupe = FindPolygon(pLevel, i + 1, pPolygonData + i);

    if ( nDupe != -1 )
    {
      if (n == 0)
      {
        ClearSelections(pLevel);
      }
      ++n;
      (pPolygonData + i)->flags |= PF_SELECTED;
//    ReplacePolygon(pLevel, nDupe, i);
//    DeletePolygon(pLevel, nDupe);
    }
  }

#endif

  pLevel->bRebuildZone = TRUE;

  Object_ViewChanged(hwnd, pWndData);

  SetCursor(hcurSave);

#if 0

  if (n > 0)
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Selected polygons have duplicates" );
  }

#endif

  return;

} // Object_EditPaste


/*******************************************************************

    NAME:       Object_ViewStatusUpdate

    SYNOPSIS:   Update the view status windows.

********************************************************************/

VOID Object_ViewStatusUpdate(HWND hwnd, WND_DATA *pWndData)
{
  HDC hdc;
  RECT rc;

  hdc = GetDC(hwnd);
  GetClientRect(hwnd, &rc);

  Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_THETA);
  Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_PHI);
  Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_X);
  Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_Y);
  Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_Z);

  ReleaseDC(hwnd, hdc);
      
  return;

} // Object_ViewStatusUpdate


/*******************************************************************

    NAME:       Object_GridStatusUpdate

    SYNOPSIS:   Update the view status windows.

********************************************************************/

VOID Object_GridStatusUpdate(HWND hwnd, WND_DATA *pWndData)
{
  HDC hdc;
  RECT rc;

  hdc = GetDC(hwnd);
  GetClientRect(hwnd, &rc);

  Object_UpdateStatusWindow(hdc, &rc, pWndData, GRID_STATUS_THETA);
  Object_UpdateStatusWindow(hdc, &rc, pWndData, GRID_STATUS_PHI);
  Object_UpdateStatusWindow(hdc, &rc, pWndData, GRID_STATUS_X);
  Object_UpdateStatusWindow(hdc, &rc, pWndData, GRID_STATUS_Y);
  Object_UpdateStatusWindow(hdc, &rc, pWndData, GRID_STATUS_Z);

  ReleaseDC(hwnd, hdc);
      
  return;

} // Object_GridStatusUpdate


/*******************************************************************

    NAME:       Object_ViewHome

    SYNOPSIS:   Change the view back to the home position.

********************************************************************/

VOID Object_ViewHome(HWND hwnd, WND_DATA *pWndData)
{
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->view_rotation = pLevel->view_home_rotation;
  pLevel->view_elevation = pLevel->view_home_elevation;
  pLevel->view_x = pLevel->view_home_x;
  pLevel->view_y = pLevel->view_home_y;
  pLevel->view_z = pLevel->view_home_z;

  Object_ViewStatusUpdate(hwnd, pWndData);

  Object_ViewChanged(hwnd, pWndData);
      
  return;

} // Object_ViewHome


/*******************************************************************

    NAME:       Object_DeleteBitmap

    SYNOPSIS:   Delete the bitmap at the indicated index.

********************************************************************/

VOID Object_DeleteBitmap(WND_DATA *pWndData, INT index)
{
  INT i, j, n;
  BYTE *p;
  WORD wSize;
  LEVEL_DATA *pLevel;

  if (index < 0 || index > pWndData->nBitmaps - 1)
  {
    return;
  }

  //
  //  Update the bitmap index of affected textures.
  //

  for(i = 0; i < pWndData->nTextures; ++i)
  {
    if (pWndData->pTextureData[i].bitmap_index == index)
    {
      pWndData->pTextureData[i].bitmap_index = -1;
    }
    else if (pWndData->pTextureData[i].bitmap_index > index)
    {
      --pWndData->pTextureData[i].bitmap_index;
    }
  }

  //
  //  Update the bitmap index of affected animations.
  //

  for(i = 0; i < pWndData->nAnimations; ++i)
  {
    n = pWndData->pAnimationData[i].num_elements;

    for(j = 0; j < n; ++j)
    {
      if (pWndData->pAnimationData[i].index[j] != -1)
      {
        if ((pWndData->pAnimationData[i].index[j] & AF_SOUND_RESOURCE) == 0)
        {
          if (pWndData->pAnimationData[i].index[j] == index)
          {
            pWndData->pAnimationData[i].index[j] = -1;
          }
          else if (pWndData->pAnimationData[i].index[j] > index)
          {
            --pWndData->pAnimationData[i].index[j];
          }
        }
      }
    }
  }

  //
  //  Update the bitmap index of affected life forms.
  //

  for(i = 0; i < pWndData->nLifeForms; ++i)
  {
    n = GetNumLifeFormBitmaps(pWndData->pLifeFormData[i].lifeform_type);

    for(j = 0; j < n; ++j)
    {
      if (pWndData->pLifeFormData[i].index[j] == index)
      {
        pWndData->pLifeFormData[i].index[j] = -1;
      }
      else if (pWndData->pLifeFormData[i].index[j] > index)
      {
        --pWndData->pLifeFormData[i].index[j];
      }
    }
  }

  //
  //  Update the bitmap index of affected items.
  //

  for(i = 0; i < pWndData->nItems; ++i)
  {
    n = GetNumItemBitmaps(pWndData->pItemData[i].item_type);

    for(j = 0; j < n; ++j)
    {
      if (pWndData->pItemData[i].index[j] == index)
      {
        pWndData->pItemData[i].index[j] = -1;
      }
      else if (pWndData->pItemData[i].index[j] > index)
      {
        --pWndData->pItemData[i].index[j];
      }
    }
  }

  //
  //  Update the bitmap index of affected sprites.
  //

  for(i = 0; i < pWndData->nSprites; ++i)
  {
    if (pWndData->pSpriteData[i].bitmap_index == index)
    {
      pWndData->pSpriteData[i].bitmap_index = -1;
    }
    else if (pWndData->pSpriteData[i].bitmap_index > index)
    {
      --pWndData->pSpriteData[i].bitmap_index;
    }
  }

  //
  //  Update other affected bitmap indecies.
  //

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    if (pLevel->bkg_bitmap_index == index)
    {
      pLevel->bkg_bitmap_index = -1;
    }
    else if (pLevel->bkg_bitmap_index > index)
    {
      --pLevel->bkg_bitmap_index;
    }

    if (pLevel->sol_bitmap_index == index)
    {
      pLevel->sol_bitmap_index = -1;
    }
    else if (pLevel->sol_bitmap_index > index)
    {
      --pLevel->sol_bitmap_index;
    }

    if (pLevel->eol_bitmap_index == index)
    {
      pLevel->eol_bitmap_index = -1;
    }
    else if (pLevel->eol_bitmap_index > index)
    {
      --pLevel->eol_bitmap_index;
    }

    ++pLevel;
  }

  //
  //  Now delete the bitmap.
  //

  GlobalFreePtr(pWndData->pBitmapData[index].data);

  p = (BYTE *)(&pWndData->pBitmapData[index]);

  wSize = (pWndData->nBitmaps - index - 1) * sizeof(BITMAP_DATA);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(BITMAP_DATA), wSize);
  }

  --pWndData->nBitmaps;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteBitmap


/*******************************************************************

    NAME:       Object_DeleteTexture

    SYNOPSIS:   Delete the texture at the indicated index.

********************************************************************/

VOID Object_DeleteTexture(WND_DATA *pWndData, INT index)
{
  INT i, j;
  BYTE *p;
  WORD wSize;
  POLYGON huge *pPolygon;
  LEVEL_DATA *pLevel;

  if (index < 0 || index > pWndData->nTextures - 1)
  {
    return;
  }

  //
  //  Update the texture index of affected polygons.
  //

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pPolygon = pLevel->pPolygonData;

    for(j = 0; j < pLevel->nPolygons; ++j)
    {
      if (pPolygon->texture_index == index)
      {
        pPolygon->texture_index = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pPolygon->texture_index > index)
      {
        --pPolygon->texture_index;
      }

      ++pPolygon;
    }

    ++pLevel;
  }

  //
  //  Now delete the texture.
  //

  p = (BYTE *)(&pWndData->pTextureData[index]);

  wSize = (pWndData->nTextures - index - 1) * sizeof(TEXTURE_DATA);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(TEXTURE_DATA), wSize);
  }

  --pWndData->nTextures;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteTexture


/*******************************************************************

    NAME:       Object_DeleteAnimation

    SYNOPSIS:   Delete the animation at the indicated index.

********************************************************************/

VOID Object_DeleteAnimation(WND_DATA *pWndData, INT index)
{
  INT i, j;
  BYTE *p;
  WORD wSize;
  POLYGON huge *pPolygon;
  LEVEL_DATA *pLevel;

  if (index < 0 || index > pWndData->nAnimations - 1)
  {
    return;
  }

  //
  //  Update the animation index of affected polygons.
  //

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pPolygon = pLevel->pPolygonData;

    for(j = 0; j < pLevel->nPolygons; ++j)
    {
      if (pPolygon->animation_index == index)
      {
        pPolygon->animation_index = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pPolygon->animation_index > index)
      {
        --pPolygon->animation_index;
      }

      if (pPolygon->damage_animation_index == index)
      {
        pPolygon->damage_animation_index = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pPolygon->damage_animation_index > index)
      {
        --pPolygon->damage_animation_index;
      }

      ++pPolygon;
    }

    ++pLevel;
  }

  //
  //  Update the animation index of affected sprites.
  //

  for(i = 0; i < pWndData->nSprites; ++i)
  {
    if (pWndData->pSpriteData[i].animation_index == index)
    {
      pWndData->pSpriteData[i].animation_index = -1;
    }
    else if (pWndData->pSpriteData[i].animation_index > index)
    {
      --pWndData->pSpriteData[i].animation_index;
    }

    if (pWndData->pSpriteData[i].damage_animation_index == index)
    {
      pWndData->pSpriteData[i].damage_animation_index = -1;
    }
    else if (pWndData->pSpriteData[i].damage_animation_index > index)
    {
      --pWndData->pSpriteData[i].damage_animation_index;
    }

    if (pWndData->pSpriteData[i].damage_animation_index_2 == index)
    {
      pWndData->pSpriteData[i].damage_animation_index_2 = -1;
    }
    else if (pWndData->pSpriteData[i].damage_animation_index_2 > index)
    {
      --pWndData->pSpriteData[i].damage_animation_index_2;
    }
  }

  //
  //  Now delete the animation.
  //

  p = (BYTE *)(&pWndData->pAnimationData[index]);

  wSize = (WORD)(pWndData->nAnimations - index - 1) * sizeof(ANIMATION_DATA);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(ANIMATION_DATA), wSize);
  }

  --pWndData->nAnimations;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteAnimation


/*******************************************************************

    NAME:       Object_DeleteLifeForm

    SYNOPSIS:   Delete the life form at the indicated index.

********************************************************************/

VOID Object_DeleteLifeForm(WND_DATA *pWndData, INT index)
{
  INT i, j;
  BYTE *p;
  WORD wSize;
  VERTEX huge *pVertex;
  LEVEL_DATA *pLevel;

  if (index < 0 || index > pWndData->nLifeForms - 1)
  {
    return;
  }

  //
  //  Update the life form index of affected vertices.
  //

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pVertex = pLevel->pVertexData;

    for(j = 0; j < pLevel->nVertices; ++j)
    {
      if (pVertex->lifeform_index == index)
      {
        pVertex->lifeform_index = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pVertex->lifeform_index > index)
      {
        --pVertex->lifeform_index;
      }

      ++pVertex;
    }

    ++pLevel;
  }

  //
  //  Now delete the life form.
  //

  p = (BYTE *)(&pWndData->pLifeFormData[index]);

  wSize = (WORD)(pWndData->nLifeForms - index - 1) * sizeof(LIFEFORM_DATA);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(LIFEFORM_DATA), wSize);
  }

  --pWndData->nLifeForms;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteLifeForm


/*******************************************************************

    NAME:       Object_DeleteItem

    SYNOPSIS:   Delete the item at the indicated index.

********************************************************************/

VOID Object_DeleteItem(WND_DATA *pWndData, INT index)
{
  INT i, j;
  BYTE *p;
  WORD wSize;
  VERTEX huge *pVertex;
  LEVEL_DATA *pLevel;

  if (index < 0 || index > pWndData->nItems - 1)
  {
    return;
  }

  //
  //  Update the item index of affected vertices.
  //

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pVertex = pLevel->pVertexData;

    for(j = 0; j < pLevel->nVertices; ++j)
    {
      if (pVertex->item_index == index)
      {
        pVertex->item_index = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pVertex->item_index > index)
      {
        --pVertex->item_index;
      }

      ++pVertex;
    }

    ++pLevel;
  }

  //
  //  Now delete the item.
  //

  p = (BYTE *)(&pWndData->pItemData[index]);

  wSize = (WORD)(pWndData->nItems - index - 1) * sizeof(ITEM_DATA);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(ITEM_DATA), wSize);
  }

  --pWndData->nItems;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteItem


/*******************************************************************

    NAME:       Object_DeleteSprite

    SYNOPSIS:   Delete the sprite at the indicated index.

********************************************************************/

VOID Object_DeleteSprite(WND_DATA *pWndData, INT index)
{
  INT i, j;
  BYTE *p;
  WORD wSize;
  VERTEX huge *pVertex;
  LEVEL_DATA *pLevel;

  if (index < 0 || index > pWndData->nTextures - 1)
  {
    return;
  }

  //
  //  Update the sprite index of affected vertices.
  //

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pVertex = pLevel->pVertexData;

    for(j = 0; j < pLevel->nVertices; ++j)
    {
      if (pVertex->sprite_index == index)
      {
        pVertex->sprite_index = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pVertex->sprite_index > index)
      {
        --pVertex->sprite_index;
      }

      ++pVertex;
    }

    ++pLevel;
  }

  //
  //  Now delete the sprite.
  //

  p = (BYTE *)(&pWndData->pSpriteData[index]);

  wSize = (pWndData->nSprites - index - 1) * sizeof(SPRITE_DATA);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(SPRITE_DATA), wSize);
  }

  --pWndData->nSprites;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteSprite


/*******************************************************************

    NAME:       Object_DeleteNoise

    SYNOPSIS:   Delete the noise at the indicated index.

********************************************************************/

VOID Object_DeleteNoise(WND_DATA *pWndData, INT index)
{
  INT i, j;
  BYTE *p;
  WORD wSize;
  VERTEX huge *pVertex;
  LEVEL_DATA *pLevel;

  if (index < 0 || index > pWndData->nNoises - 1)
  {
    return;
  }

  //
  //  Update the noise index of affected vertices.
  //

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pVertex = pLevel->pVertexData;

    for(j = 0; j < pLevel->nVertices; ++j)
    {
      if (pVertex->noise_index == index)
      {
        pVertex->noise_index = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pVertex->noise_index > index)
      {
        --pVertex->noise_index;
      }

      ++pVertex;
    }

    ++pLevel;
  }

  //
  //  Now delete the noise.
  //

  p = (BYTE *)(&pWndData->pNoiseData[index]);

  wSize = (pWndData->nNoises - index - 1) * sizeof(NOISE_DATA);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(NOISE_DATA), wSize);
  }

  --pWndData->nNoises;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteNoise


/*******************************************************************

    NAME:       Object_DeleteSound

    SYNOPSIS:   Delete the sound at the indicated index.

********************************************************************/

VOID Object_DeleteSound(WND_DATA *pWndData, INT index)
{
  INT i, j, k, n;
  BYTE *p;
  WORD wSize;
  LEVEL_DATA *pLevel;

  if (index < 0 || index > pWndData->nSounds - 1)
  {
    return;
  }

  //
  //  Update the sound index of affected triggers and motions.
  //

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    for(j = 0; j < pLevel->nTriggers; ++j)
    {
      if (pLevel->pTriggerData[j].sound_index == index)
      {
        pLevel->pTriggerData[j].sound_index = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pLevel->pTriggerData[j].sound_index > index)
      {
        --pLevel->pTriggerData[j].sound_index;
      }
    }

    for(j = 0; j < pLevel->nMotions; ++j)
    {
      if (pLevel->pMotionData[j].sound_index_start == index)
      {
        pLevel->pMotionData[j].sound_index_start = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pLevel->pMotionData[j].sound_index_start > index)
      {
        --pLevel->pMotionData[j].sound_index_start;
      }

      if (pLevel->pMotionData[j].sound_index_run == index)
      {
        pLevel->pMotionData[j].sound_index_run = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pLevel->pMotionData[j].sound_index_run > index)
      {
        --pLevel->pMotionData[j].sound_index_run;
      }

      if (pLevel->pMotionData[j].sound_index_stop == index)
      {
        pLevel->pMotionData[j].sound_index_stop = -1;

        pLevel->bRebuildZone = TRUE;
      }
      else if (pLevel->pMotionData[j].sound_index_stop > index)
      {
        --pLevel->pMotionData[j].sound_index_stop;
      }
    }

    ++pLevel;
  }

  //
  //  Update the sound index of affected animations.
  //

  for(i = 0; i < pWndData->nAnimations; ++i)
  {
    n = pWndData->pAnimationData[i].num_elements;

    for(j = 0; j < n; ++j)
    {
      if (pWndData->pAnimationData[i].index[j] != -1)
      {
        if (pWndData->pAnimationData[i].index[j] & AF_SOUND_RESOURCE)
        {
          k = pWndData->pAnimationData[i].index[j] & ~AF_SOUND_RESOURCE;

          if (k == index)
          {
            pWndData->pAnimationData[i].index[j] = -1;
          }
          else if (k > index)
          {
            pWndData->pAnimationData[i].index[j] = AF_SOUND_RESOURCE | (k - 1);
          }
        }
      }
    }
  }

  //
  //  Update the sound index of affected life forms.
  //

  for(i = 0; i < pWndData->nLifeForms; ++i)
  {
    n = GetNumLifeFormElements(pWndData->pLifeFormData[i].lifeform_type);
    k = GetNumLifeFormBitmaps(pWndData->pLifeFormData[i].lifeform_type);

    for(j = k; j < n; ++j)
    {
      if (pWndData->pLifeFormData[i].index[j] == index)
      {
        pWndData->pLifeFormData[i].index[j] = -1;
      }
      else if (pWndData->pLifeFormData[i].index[j] > index)
      {
        --pWndData->pLifeFormData[i].index[j];
      }
    }
  }

  //
  //  Update the sound index of affected items.
  //

  for(i = 0; i < pWndData->nItems; ++i)
  {
    n = GetNumItemElements(pWndData->pItemData[i].item_type);
    k = GetNumItemBitmaps(pWndData->pItemData[i].item_type);

    for(j = k; j < n; ++j)
    {
      if (pWndData->pItemData[i].index[j] == index)
      {
        pWndData->pItemData[i].index[j] = -1;
      }
      else if (pWndData->pItemData[i].index[j] > index)
      {
        --pWndData->pItemData[i].index[j];
      }
    }
  }

  //
  //  Update the sound index of affected noises.
  //

  for(i = 0; i < pWndData->nNoises; ++i)
  {
    if (pWndData->pNoiseData[i].sound_index == index)
    {
      pWndData->pNoiseData[i].sound_index = -1;
    }
    else if (pWndData->pNoiseData[i].sound_index > index)
    {
      --pWndData->pNoiseData[i].sound_index;
    }
  }

  //
  //  Now delete the sound.
  //

  p = (BYTE *)(&pWndData->pSoundData[index]);

  wSize = (pWndData->nSounds - index - 1) * sizeof(SOUND_DATA);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(SOUND_DATA), wSize);
  }

  --pWndData->nSounds;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pLevel->bRebuildZone = TRUE;

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteSound


/*******************************************************************

    NAME:       Object_DeleteLevel

    SYNOPSIS:   Delete the level at the indicated index.

********************************************************************/

VOID Object_DeleteLevel(WND_DATA *pWndData, INT index)
{
  INT i;
  BYTE *p;
  WORD wSize;

  if (index < 0 || index > pWndData->nLevels - 1)
  {
    return;
  }

  //
  //  Adjust the zone data header level index for other levels.
  //

  for(i = index + 1; i < pWndData->nLevels; ++i)
  {
    if (pWndData->pZoneData[i].data)
    {
      --(((ZONE_DATA_HDR *)pWndData->pZoneData[i].data)->level_index);
    }
  }

  //
  //  Delete the zone data for the desired index.
  //

  if (pWndData->pZoneData[index].data)
  {
    GlobalFreePtr(pWndData->pZoneData[index].data);
    pWndData->pZoneData[index].data = NULL;
    pWndData->pZoneData[index].size = 0;
  }

  if (pWndData->nLevels > 1)
  {
    //
    //  Delete the associated zone data.
    //

    p = (BYTE *)(pWndData->pZoneData + index);

    wSize = (WORD)(pWndData->nLevels - index - 1) * sizeof(ZONE_DATA);

    if (wSize > 0)
    {
      _fmemmove(p, p + sizeof(ZONE_DATA), wSize);
    }

    _fmemset(pWndData->pZoneData + pWndData->nLevels - 1, 0, sizeof(ZONE_DATA));

    //
    //  Now delete the level data.
    //

    Frame_FreeLevel(pWndData, index);

    p = (BYTE *)(pWndData->pLevelData + index);

    wSize = (WORD)(pWndData->nLevels - index - 1) * sizeof(LEVEL_DATA);

    if (wSize > 0)
    {
      _fmemmove(p, p + sizeof(LEVEL_DATA), wSize);
    }

    --pWndData->nLevels;
  }
  else
  {
    //
    //  Don't delete the last level, just clear it out.
    //

    pWndData->pLevelData[0].nPolygons = 0;
    pWndData->pLevelData[0].nVertices = 0;
    pWndData->pLevelData[0].nTriggers = 0;
    pWndData->pLevelData[0].nMotions = 0;
    pWndData->pLevelData[0].nGroups = 0;
    pWndData->pLevelData[0].nMarks = 0;
    strcpy(pWndData->pLevelData[0].level_name,"Untitled");
  }

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return;

} // Object_DeleteLevel


/*******************************************************************

    NAME:       Object_InsertLevel

    SYNOPSIS:   Insert a level just before the indicated index.

********************************************************************/

BOOL Object_InsertLevel(WND_DATA *pWndData, INT index)
{
  INT i;
  BYTE *p;
  WORD wSize;

  if (pWndData->nLevels == MAX_LEVELS)
  {
    return FALSE;
  }

  if (index < 0 || index > pWndData->nLevels)
  {
    return FALSE;
  }

  //
  //  Make room for the new level in the level and zone data arrays.
  //

  p = (BYTE *)(pWndData->pLevelData + index);

  wSize = (WORD)(pWndData->nLevels - index) * sizeof(LEVEL_DATA);

  if (wSize > 0)
  {
    _fmemmove(p + sizeof(LEVEL_DATA), p, wSize);
  }

  _fmemset(p, 0, sizeof(LEVEL_DATA));

  p = (BYTE *)(pWndData->pZoneData + index);

  wSize = (WORD)(pWndData->nLevels - index) * sizeof(ZONE_DATA);

  if (wSize > 0)
  {
    _fmemmove(p + sizeof(ZONE_DATA), p, wSize);
  }

  _fmemset(p, 0, sizeof(ZONE_DATA));

  ++pWndData->nLevels;

  //
  //  Allocate and initialize the level data.
  //

  if (Frame_NewLevel(pWndData, index) == FALSE)
  {
    //
    //  Unable to allocate memory, remove the level entry.
    //

    p = (BYTE *)(pWndData->pLevelData + index);

    wSize = (WORD)(pWndData->nLevels - index - 1) * sizeof(LEVEL_DATA);

    if (wSize > 0)
    {
      _fmemmove(p, p + sizeof(LEVEL_DATA), wSize);
    }

    p = (BYTE *)(pWndData->pZoneData + index);

    wSize = (WORD)(pWndData->nLevels - index - 1) * sizeof(ZONE_DATA);

    if (wSize > 0)
    {
      _fmemmove(p, p + sizeof(ZONE_DATA), wSize);
    }

    --pWndData->nLevels;

    return FALSE;
  }

  //
  //  Adjust the zone data header level index for other levels.
  //

  for(i = index + 1; i < pWndData->nLevels; ++i)
  {
    if (pWndData->pZoneData[i].data)
    {
      ++(((ZONE_DATA_HDR *)pWndData->pZoneData[i].data)->level_index);
    }
  }

  Object_DeleteUndoData(hwndMDIClient, pWndData);

  return TRUE;

} // Object_InsertLevel


/*******************************************************************

    NAME:       Object_ResetPixelSize

    SYNOPSIS:   Reset the pixel size to that called for in the
                associated group for all unlinked polygons.

********************************************************************/

VOID Object_ResetPixelSize(HWND hwnd, WND_DATA *pWndData)
{
  INT i, j;
  VERTEX huge *pVertex;
  POLYGON huge *pPolygon;
  LEVEL_DATA *pLevel;
  double bx1, by1, bx2, by2, bdx, bdy;
  double xp1, yp1, zp1, xp2, yp2, zp2, dx, dy, dz;
  double scale, d1, d2, pixel_size;

  pLevel = pWndData->pLevelData;

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pPolygon = pLevel->pPolygonData;

    for(j = 0; j < pLevel->nPolygons; ++j)
    {
      if ((pPolygon->flags & PF_LINKED) == 0)
      {
        pixel_size = 0.0;

        if (pPolygon->flags & (PF_ANIMATED | PF_MOTION_ANIMATED))
        {
          if (pPolygon->animation_index != -1)
          {
            pixel_size = pWndData->pAnimationData[pPolygon->animation_index].pixel_size;
          }
        }
        else if (pPolygon->flags & PF_DAMAGE_ANIMATED)
        {
          if (pPolygon->damage_animation_index != -1)
          {
            pixel_size = pWndData->pAnimationData[pPolygon->damage_animation_index].pixel_size;
          }
        }
        else
        {
          if (pPolygon->texture_index != -1)
          {
            pixel_size = pWndData->pTextureData[pPolygon->texture_index].pixel_size;
          }
        }

        if (pixel_size > 0.0)
        {
          pVertex = pLevel->pVertexData + pPolygon->vertices[0];

          xp1 = (double)pVertex->x;
          yp1 = (double)pVertex->y;
          zp1 = (double)pVertex->z;

          pVertex = pLevel->pVertexData + pPolygon->vertices[1];

          xp2 = (double)pVertex->x;
          yp2 = (double)pVertex->y;
          zp2 = (double)pVertex->z;

          dx = xp2 - xp1;
          dy = yp2 - yp1;
          dz = zp2 - zp1;

          d1 = sqrt(dx * dx + dy * dy + dz * dz);

          if (d1 == 0.0)
          {
            ClearSelections(pLevel);
            MsgBox(hwndMDIClient, MB_ICONEXCLAMATION, "d1 is zero!");
            break;
          }

          bx1 = (double)pPolygon->bmx[0];
          by1 = (double)pPolygon->bmy[0];
          bx2 = (double)pPolygon->bmx[1];
          by2 = (double)pPolygon->bmy[1];

          bdx = bx2 - bx1;
          bdy = by2 - by1;
   
          d2 = sqrt(bdx * bdx + bdy * bdy);

          if (d2 == 0.0)
          {
            ClearSelections(pLevel);
            MsgBox(hwndMDIClient, MB_ICONEXCLAMATION, "d2 is zero!");
            break;
          }

          scale = d1 / (d2 * pixel_size);

          bx2 = bx1 + bdx * scale;
          by2 = by1 + bdy * scale;

          pPolygon->bmx[1] = (float)bx2;
          pPolygon->bmy[1] = (float)by2;
        }
      }

      ++pPolygon;
    }

    pLevel->bRebuildZone = TRUE;

    ++pLevel;
  }

  return;

} // Object_ResetPixelSize


//
//  Private functions.
//

/*******************************************************************

    NAME:       Object_ClearWindow

    SYNOPSIS:   Clear the window.

    ENTRY:      hdc - Device context.

                rc - The client window rectangle (including controls).

                nMapMode - MM_TEXT (screen) or MM_LOENGLISH (printer).

********************************************************************/

VOID Object_ClearWindow(HDC hdc, RECT *rc, INT nMapMode)
{
  INT x1, y1, x2, y2;
  HPEN hPenBlack, hPenWhite, hPenLtGray, hPenOld;
  HBRUSH hBrushBlack, hBrushLtGray, hBrushOld;

  //
  //  Clear the client area.
  //

  if (nMapMode == MM_TEXT)
  {
    x1 = rc->left + CONTROL_AREA_WIDTH1;
    y1 = rc->top + CONTROL_AREA_HEIGHT1;
    x2 = rc->right - CONTROL_AREA_WIDTH2;
    y2 = rc->bottom - CONTROL_AREA_HEIGHT2;
  }
  else
  {
    x1 = rc->left;
    y1 = rc->top;
    x2 = rc->right;
    y2 = rc->bottom;
  }

  hPenLtGray = CreatePen(PS_SOLID, 1, RGB(192,192,192));
  hPenBlack = GetStockObject(BLACK_PEN);
  hPenWhite = GetStockObject(WHITE_PEN);
  hBrushLtGray = GetStockObject(LTGRAY_BRUSH);
  hBrushBlack = GetStockObject(BLACK_BRUSH);

  hPenOld = SelectObject(hdc, hPenBlack);
  hBrushOld = SelectObject(hdc, hBrushBlack);

//  Rectangle( hdc, x1, y1, x2, y2 );

  if (nMapMode == MM_TEXT)
  {
    SelectObject(hdc, hPenLtGray);
    SelectObject(hdc, hBrushLtGray);

    Rectangle(hdc, rc->left, rc->top, x1, rc->bottom);
    Rectangle(hdc, x2, rc->top, rc->right, rc->bottom);
    Rectangle(hdc, x1, rc->top, x2, y1);
    Rectangle(hdc, x1, y2, x2, rc->bottom);

    SelectObject(hdc, hPenWhite);

    MoveTo(hdc, rc->left, rc->top);
    LineTo(hdc, rc->right, rc->top);

    MoveTo(hdc, x1, y1 - 1);
    LineTo(hdc, x2, y1 - 1);

    MoveTo(hdc, x1, y2);
    LineTo(hdc, x2, y2);

    MoveTo(hdc, rc->left, rc->bottom - 1);
    LineTo(hdc, rc->right, rc->bottom - 1);

    MoveTo(hdc, x1 - 1, rc->top);
    LineTo(hdc, x1 - 1, rc->bottom);

    MoveTo(hdc, x2, rc->top);
    LineTo(hdc, x2, rc->bottom);
  }

  //
  //  Clean up.
  //
  
  SelectObject(hdc, hPenOld);
  SelectObject(hdc, hBrushOld);
  DeleteObject(hPenLtGray);

  return;
  
} // Object_ClearWindow


/*******************************************************************

    NAME:       Object_ShowTitle

    SYNOPSIS:   Show the text given in the title area..

    ENTRY:      hdc - Device context.

                rc - The client window rectangle (including controls).

                pszFormat - A printf-like format string.

                ... - Other printf-like arguments as needed.

********************************************************************/

VOID Object_ShowTitle(HDC hdc, RECT *rc, LPSTR pszFormat, ...)
{
  INT x1, y1, x2, y2, n;
  CHAR szOutput[MAX_PRINTF_OUTPUT];
  HDC hdcMem;
  HFONT hFontArial, hFontOld;
  HPEN hPenLtGray, hPenOld;
  HBRUSH hBrushLtGray, hBrushOld;
  HBITMAP hbmMsg, hbmOld;
  va_list ArgList;

  va_start(ArgList, pszFormat);
  wvsprintf(szOutput, pszFormat, ArgList);
  va_end(ArgList);

  x1 = CONTROL_AREA_WIDTH1;
  y1 = rc->top + 1;
  x2 = rc->right - CONTROL_AREA_WIDTH2;
  y2 = rc->top + CONTROL_AREA_HEIGHT1 - 1;

  hdcMem = CreateCompatibleDC(hdc);
  hbmMsg = CreateCompatibleBitmap(hdc, x2 - x1, y2 - y1);

  hFontArial = CreateFont(-14,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
  hPenLtGray = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
  hBrushLtGray = CreateSolidBrush(RGB(192, 192, 192));

  hFontOld = SelectObject(hdcMem, hFontArial);
  hbmOld = SelectObject(hdcMem, hbmMsg);
  hPenOld = SelectObject(hdcMem, hPenLtGray);
  hBrushOld = SelectObject(hdcMem, hBrushLtGray);

  SetBkMode(hdcMem, TRANSPARENT);
  SetTextAlign(hdcMem, TA_LEFT);

  Rectangle(hdcMem, 0, 0, x2 - x1, y2 - y1);

  n = strlen(szOutput);

  if (n > 0)
  {
    TextOut(hdcMem, 4, 4, szOutput, n);
  }

  BitBlt(hdc, x1, y1, x2 - x1, y2 - y1, hdcMem, 0, 0, SRCCOPY);

  SelectObject(hdcMem, hBrushOld);
  SelectObject(hdcMem, hPenOld);
  SelectObject(hdcMem, hbmOld);
  SelectObject(hdcMem, hFontOld);

  DeleteObject(hBrushLtGray);
  DeleteObject(hPenLtGray);
  DeleteObject(hFontArial);
  DeleteObject(hbmMsg);

  DeleteDC(hdcMem);

} // Object_ShowTitle


/*******************************************************************

    NAME:       Object_ShowMessage

    SYNOPSIS:   Show the text given in the message area..

    ENTRY:      hdc - Device context.

                rc - The client window rectangle (including controls).

                pszFormat - A printf-like format string.

                ... - Other printf-like arguments as needed.

********************************************************************/

VOID Object_ShowMessage(HDC hdc, RECT *rc, LPSTR pszFormat, ...)
{
  INT x1, y1, x2, y2, n;
  CHAR szOutput[MAX_PRINTF_OUTPUT];
  HDC hdcMem;
  HFONT hFontArial, hFontOld;
  HPEN hPenLtGray, hPenOld;
  HBRUSH hBrushLtGray, hBrushOld;
  HBITMAP hbmMsg, hbmOld;
  va_list ArgList;

  va_start(ArgList, pszFormat);
  wvsprintf(szOutput, pszFormat, ArgList);
  va_end(ArgList);

  x1 = CONTROL_AREA_WIDTH1;
  y1 = rc->bottom - CONTROL_AREA_HEIGHT2 + 1;
  x2 = rc->right - CONTROL_AREA_WIDTH2;
  y2 = rc->bottom - 1;

  hdcMem = CreateCompatibleDC(hdc);
  hbmMsg = CreateCompatibleBitmap(hdc, x2 - x1, y2 - y1);

  hFontArial = CreateFont(-14,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
  hPenLtGray = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
  hBrushLtGray = CreateSolidBrush(RGB(192, 192, 192));

  hFontOld = SelectObject(hdcMem, hFontArial);
  hbmOld = SelectObject(hdcMem, hbmMsg);
  hPenOld = SelectObject(hdcMem, hPenLtGray);
  hBrushOld = SelectObject(hdcMem, hBrushLtGray);

  SetBkMode(hdcMem, TRANSPARENT);
  SetTextAlign(hdcMem, TA_LEFT);

  Rectangle(hdcMem, 0, 0, x2 - x1, y2 - y1);

  n = strlen(szOutput);

  if (n > 0)
  {
    TextOut(hdcMem, 4, 4, szOutput, n);
  }

  BitBlt(hdc, x1, y1, x2 - x1, y2 - y1, hdcMem, 0, 0, SRCCOPY);

  SelectObject(hdcMem, hBrushOld);
  SelectObject(hdcMem, hPenOld);
  SelectObject(hdcMem, hbmOld);
  SelectObject(hdcMem, hFontOld);

  DeleteObject(hBrushLtGray);
  DeleteObject(hPenLtGray);
  DeleteObject(hFontArial);
  DeleteObject(hbmMsg);

  DeleteDC(hdcMem);

} // Object_ShowMessage


/*******************************************************************

    NAME:       Object_GetButtonRects

    SYNOPSIS:   Get retangles for the button controls.

    ENTRY:      rc - The client window rectangle (including controls).
    
                btnRect - Receives the rectangles.

********************************************************************/

VOID Object_GetButtonRects(RECT *rc, RECT *btnRect)
{
  INT i, x, y, xr, yr;

  //
  //  The "Pointer" button.
  //

  xr = rc->left + CONTROL_AREA_WIDTH1 / 2 - BTN_WIDTH1 - BTN_MARGIN3 / 2;
  yr = rc->top + CONTROL_AREA_MARGIN;

  btnRect[MODE_POINTER].left = xr;
  btnRect[MODE_POINTER].top = yr;

  //
  //  The "Select Vertex" button.
  //

  x = xr + BTN_WIDTH1 + BTN_MARGIN3;

  btnRect[MODE_SELECT_VERTEX].left = x;
  btnRect[MODE_SELECT_VERTEX].top = yr;

  //
  //  The "Select Polygon" button.
  //

  y = yr + BTN_HEIGHT1 + BTN_MARGIN3;

  btnRect[MODE_SELECT_POLYGON].left = xr;
  btnRect[MODE_SELECT_POLYGON].top = y;

  //
  //  The "Select Group" button.
  //

  btnRect[MODE_SELECT_GROUP].left = x;
  btnRect[MODE_SELECT_GROUP].top = y;

  //
  //  The "New Vertex" button.
  //

  y = yr + BTN_HEIGHT1 * 2 + BTN_MARGIN3 + BTN_MARGIN4;

  btnRect[MODE_NEW_VERTEX].left = xr;
  btnRect[MODE_NEW_VERTEX].top = y;

  //
  //  The "New Polygon" button.
  //

  btnRect[MODE_NEW_POLYGON].left = x;
  btnRect[MODE_NEW_POLYGON].top = y;

  //
  //  The "Rotate Polygon" button.
  //

  y = yr + BTN_HEIGHT1 * 3 + BTN_MARGIN3 + BTN_MARGIN4 * 2;

  btnRect[MODE_ROTATE_POLYGON].left = xr;
  btnRect[MODE_ROTATE_POLYGON].top = y;

  //
  //  The "Flip Polygon" button.
  //

  btnRect[MODE_FLIP_POLYGON].left = x;
  btnRect[MODE_FLIP_POLYGON].top = y;

  //
  //  The "Link Polygon" button.
  //

  y = yr + BTN_HEIGHT1 * 4 + BTN_MARGIN3 + BTN_MARGIN4 * 3;

  btnRect[MODE_LINK_POLYGON].left = xr;
  btnRect[MODE_LINK_POLYGON].top = y;

  //
  //  The "Pan Up" button.
  //

  xr = rc->right - CONTROL_AREA_WIDTH2 / 2 - BTN_WIDTH2 / 2;
  yr = rc->top + CONTROL_AREA_MARGIN;

  btnRect[VIEW_PAN_UP].left = xr;
  btnRect[VIEW_PAN_UP].top = yr;

  //
  //  The "Tilt Down" button.
  //

  y = yr + BTN_HEIGHT2 + BTN_MARGIN;

  btnRect[VIEW_TILT_DOWN].left = xr;
  btnRect[VIEW_TILT_DOWN].top = y;

  //
  //  The "Zoom In" button.
  //

  y = yr + BTN_HEIGHT2 * 2 + BTN_MARGIN + BTN_MARGIN2;

  btnRect[VIEW_ZOOM_IN].left = xr;
  btnRect[VIEW_ZOOM_IN].top = y;

  //
  //  The "Zoom Out" button.
  //

  y = yr + BTN_HEIGHT2 * 3 + BTN_MARGIN * 2 + BTN_MARGIN2;

  btnRect[VIEW_ZOOM_OUT].left = xr;
  btnRect[VIEW_ZOOM_OUT].top = y;

  //
  //  The "Tilt Up" button.
  //

  y = yr + BTN_HEIGHT2 * 4 + BTN_MARGIN * 2 + BTN_MARGIN2 * 2;

  btnRect[VIEW_TILT_UP].left = xr;
  btnRect[VIEW_TILT_UP].top = y;

  //
  //  The "Pan Down" button.
  //

  y = yr + BTN_HEIGHT2 * 5 + BTN_MARGIN * 3 + BTN_MARGIN2 * 2;

  btnRect[VIEW_PAN_DOWN].left = xr;
  btnRect[VIEW_PAN_DOWN].top = y;

  //
  //  The "Pan Left" button.
  //

  x = xr - BTN_WIDTH2 * 2 - BTN_MARGIN - BTN_MARGIN2;
  y = yr + BTN_HEIGHT2 * 3 + BTN_MARGIN * 2 + BTN_MARGIN2 - BTN_MARGIN / 2 - BTN_HEIGHT2 / 2;

  btnRect[VIEW_PAN_LEFT].left = x;
  btnRect[VIEW_PAN_LEFT].top = y;

  //
  //  The "Rotate Left" button.
  //

  x += BTN_WIDTH2 + BTN_MARGIN;

  btnRect[VIEW_ROTATE_LEFT].left = x;
  btnRect[VIEW_ROTATE_LEFT].top = y;

  //
  //  The "Rotate Right" button.
  //

  x = xr + BTN_WIDTH2 + BTN_MARGIN2;

  btnRect[VIEW_ROTATE_RIGHT].left = x;
  btnRect[VIEW_ROTATE_RIGHT].top = y;

  //
  //  The "Pan Right" button.
  //

  x += BTN_WIDTH2 + BTN_MARGIN;

  btnRect[VIEW_PAN_RIGHT].left = x;
  btnRect[VIEW_PAN_RIGHT].top = y;

  //
  //  The "-X" button.
  //

  x = rc->right - CONTROL_AREA_WIDTH2 + STATUS_COLUMN1_POS;
  y = rc->top + GRID_CONTROLS_OFFSET;

  btnRect[GRID_X_DECREASE].left = x;
  btnRect[GRID_X_DECREASE].top = y;

  //
  //  The "+X" button.
  //

  btnRect[GRID_X_INCREASE].left = x + BTN_WIDTH2 + BTN_MARGIN2;
  btnRect[GRID_X_INCREASE].top = y;

  //
  //  The "-Y" button.
  //

  btnRect[GRID_Y_DECREASE].left = x;
  btnRect[GRID_Y_DECREASE].top = y + STATUS_VERT_SPACING;

  //
  //  The "+Y" button.
  //

  btnRect[GRID_Y_INCREASE].left = x + BTN_WIDTH2 + BTN_MARGIN2;
  btnRect[GRID_Y_INCREASE].top = y + STATUS_VERT_SPACING;

  //
  //  The "-Z" button.
  //

  btnRect[GRID_Z_DECREASE].left = x;
  btnRect[GRID_Z_DECREASE].top = y + STATUS_VERT_SPACING * 2;

  //
  //  The "+Z" button.
  //

  btnRect[GRID_Z_INCREASE].left = x + BTN_WIDTH2 + BTN_MARGIN2;
  btnRect[GRID_Z_INCREASE].top = y + STATUS_VERT_SPACING * 2;

  //
  //  The "Rotate Left" button.
  //

  btnRect[GRID_ROTATE_LEFT].left = x;
  btnRect[GRID_ROTATE_LEFT].top = y + STATUS_VERT_SPACING * 3;

  //
  //  The "Rotate Right" button.
  //

  btnRect[GRID_ROTATE_RIGHT].left = x + BTN_WIDTH2 + BTN_MARGIN2;
  btnRect[GRID_ROTATE_RIGHT].top = y + STATUS_VERT_SPACING * 3;

  //
  //  The "Tilt Down" button.
  //

  btnRect[GRID_TILT_DOWN].left = x;
  btnRect[GRID_TILT_DOWN].top = y + STATUS_VERT_SPACING * 4;

  //
  //  The "Tilt Up" button.
  //

  btnRect[GRID_TILT_UP].left = x + BTN_WIDTH2 + BTN_MARGIN2;
  btnRect[GRID_TILT_UP].top = y + STATUS_VERT_SPACING * 4;

  //
  //  Setup the rest.
  //

  for(i = 0; i < NUM_CONTROLS; ++i)
  {
    if (i < NUM_MODE_CONTROLS)
    {
      btnRect[i].right = btnRect[i].left + BTN_WIDTH1;
      btnRect[i].bottom = btnRect[i].top + BTN_HEIGHT1;
    }
    else
    {
      btnRect[i].right = btnRect[i].left + BTN_WIDTH2;
      btnRect[i].bottom = btnRect[i].top + BTN_HEIGHT2;
    }
  }

  return;
  
} // Object_GetButtonRects


/*******************************************************************

    NAME:       Object_DrawButtons

    SYNOPSIS:   Draw the button controls.

    ENTRY:      hdc - Device context.

                rc - The client window rectangle (including controls).

********************************************************************/

VOID Object_DrawButtons(HDC hdc, RECT *rc)
{
  INT x, y;
  HDC hdcMem;
  HFONT hFontArial, hFontOld;
  HBITMAP hbmOld;
  RECT btnRect[NUM_CONTROLS];

  //
  //  Get the button rectangles.
  //

  Object_GetButtonRects(rc, btnRect);

  //
  //  Create a compatible memory DC to hold the bitmaps.
  //

  hdcMem = CreateCompatibleDC(hdc);

  //
  //  Draw the "Pointer" button.
  //

  x = btnRect[MODE_POINTER].left;
  y = btnRect[MODE_POINTER].top;

  hbmOld = SelectObject(hdcMem, hbmSelectPointerNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Select Vertex" button.
  //

  x = btnRect[MODE_SELECT_VERTEX].left;
  y = btnRect[MODE_SELECT_VERTEX].top;

  SelectObject(hdcMem, hbmSelectVertexNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Select Polygon" button.
  //

  x = btnRect[MODE_SELECT_POLYGON].left;
  y = btnRect[MODE_SELECT_POLYGON].top;

  SelectObject(hdcMem, hbmSelectPolygonNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Select Group" button.
  //

  x = btnRect[MODE_SELECT_GROUP].left;
  y = btnRect[MODE_SELECT_GROUP].top;

  SelectObject(hdcMem, hbmSelectGroupNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "New Vertex" button.
  //

  x = btnRect[MODE_NEW_VERTEX].left;
  y = btnRect[MODE_NEW_VERTEX].top;

  SelectObject(hdcMem, hbmNewVertexNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "New Polygon" button.
  //

  x = btnRect[MODE_NEW_POLYGON].left;
  y = btnRect[MODE_NEW_POLYGON].top;

  SelectObject(hdcMem, hbmNewPolygonNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Rotate Polygon" button.
  //

  x = btnRect[MODE_ROTATE_POLYGON].left;
  y = btnRect[MODE_ROTATE_POLYGON].top;

  SelectObject(hdcMem, hbmRotatePolygonNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Flip Polygon" button.
  //

  x = btnRect[MODE_FLIP_POLYGON].left;
  y = btnRect[MODE_FLIP_POLYGON].top;

  SelectObject(hdcMem, hbmFlipPolygonNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Link Polygon" button.
  //

  x = btnRect[MODE_LINK_POLYGON].left;
  y = btnRect[MODE_LINK_POLYGON].top;

  SelectObject(hdcMem, hbmLinkPolygonNormal);

  BitBlt(hdc, x, y, BTN_WIDTH1, BTN_HEIGHT1, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Pan Up" button.
  //

  x = btnRect[VIEW_PAN_UP].left;
  y = btnRect[VIEW_PAN_UP].top;

  SelectObject(hdcMem, hbmUpNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Tilt Down" button.
  //

  x = btnRect[VIEW_TILT_DOWN].left;
  y = btnRect[VIEW_TILT_DOWN].top;

  SelectObject(hdcMem, hbmTiltDownNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Zoom In" button.
  //

  x = btnRect[VIEW_ZOOM_IN].left;
  y = btnRect[VIEW_ZOOM_IN].top;

  SelectObject(hdcMem, hbmZoomInNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Zoom Out" button.
  //

  x = btnRect[VIEW_ZOOM_OUT].left;
  y = btnRect[VIEW_ZOOM_OUT].top;

  SelectObject(hdcMem, hbmZoomOutNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Tilt Up" button.
  //

  x = btnRect[VIEW_TILT_UP].left;
  y = btnRect[VIEW_TILT_UP].top;

  SelectObject(hdcMem, hbmTiltUpNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Pan Down" button.
  //

  x = btnRect[VIEW_PAN_DOWN].left;
  y = btnRect[VIEW_PAN_DOWN].top;

  SelectObject(hdcMem, hbmDownNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Pan Left" button.
  //

  x = btnRect[VIEW_PAN_LEFT].left;
  y = btnRect[VIEW_PAN_LEFT].top;

  SelectObject(hdcMem, hbmLeftNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Rotate Left" button.
  //

  x = btnRect[VIEW_ROTATE_LEFT].left;
  y = btnRect[VIEW_ROTATE_LEFT].top;

  SelectObject(hdcMem, hbmRotateLeftNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Rotate Right" button.
  //

  x = btnRect[VIEW_ROTATE_RIGHT].left;
  y = btnRect[VIEW_ROTATE_RIGHT].top;

  SelectObject(hdcMem, hbmRotateRightNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Pan Right" button.
  //

  x = btnRect[VIEW_PAN_RIGHT].left;
  y = btnRect[VIEW_PAN_RIGHT].top;

  SelectObject(hdcMem, hbmRightNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "-X" button.
  //

  x = btnRect[GRID_X_DECREASE].left;
  y = btnRect[GRID_X_DECREASE].top;

  SelectObject(hdcMem, hbmDownNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "+X" button.
  //

  x = btnRect[GRID_X_INCREASE].left;
  y = btnRect[GRID_X_INCREASE].top;

  SelectObject(hdcMem, hbmUpNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "-Y" button.
  //

  x = btnRect[GRID_Y_DECREASE].left;
  y = btnRect[GRID_Y_DECREASE].top;

  SelectObject(hdcMem, hbmDownNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "+Y" button.
  //

  x = btnRect[GRID_Y_INCREASE].left;
  y = btnRect[GRID_Y_INCREASE].top;

  SelectObject(hdcMem, hbmUpNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "-Z" button.
  //

  x = btnRect[GRID_Z_DECREASE].left;
  y = btnRect[GRID_Z_DECREASE].top;

  SelectObject(hdcMem, hbmDownNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "+Z" button.
  //

  x = btnRect[GRID_Z_INCREASE].left;
  y = btnRect[GRID_Z_INCREASE].top;

  SelectObject(hdcMem, hbmUpNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Rotate Left" button.
  //

  x = btnRect[GRID_ROTATE_LEFT].left;
  y = btnRect[GRID_ROTATE_LEFT].top;

  SelectObject(hdcMem, hbmRotateLeftNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Rotate Right" button.
  //

  x = btnRect[GRID_ROTATE_RIGHT].left;
  y = btnRect[GRID_ROTATE_RIGHT].top;

  SelectObject(hdcMem, hbmRotateRightNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Tilt Down" button.
  //

  x = btnRect[GRID_TILT_DOWN].left;
  y = btnRect[GRID_TILT_DOWN].top;

  SelectObject(hdcMem, hbmTiltDownNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the "Tilt Up" button.
  //

  x = btnRect[GRID_TILT_UP].left;
  y = btnRect[GRID_TILT_UP].top;

  SelectObject(hdcMem, hbmTiltUpNormal);

  BitBlt(hdc, x, y, BTN_WIDTH2, BTN_HEIGHT2, hdcMem, 0, 0, SRCCOPY);

  //
  //  Draw the labels.
  //

  hFontArial = CreateFont(-12,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");

  SetBkMode(hdc, TRANSPARENT);
  SetTextAlign(hdc, TA_LEFT);

  hFontOld = SelectObject(hdc, hFontArial);

  x = btnRect[VIEW_PAN_LEFT].left;
  y = btnRect[VIEW_PAN_UP].top;

  TextOut(hdc, x, y, "View", 4);

  x = btnRect[GRID_X_DECREASE].left;
  y = btnRect[GRID_X_DECREASE].top - BTN_HEIGHT2;

  TextOut(hdc, x, y, "Grid", 4);

  SelectObject(hdc, hFontOld);
  DeleteObject(hFontArial);

  //
  //  Clean up.
  //

  SelectObject(hdcMem, hbmOld);
  DeleteDC(hdcMem);
  
  return;
  
} // Object_DrawButtons


/*******************************************************************

    NAME:       Object_GetStatusRects

    SYNOPSIS:   Get retangles for the status controls.

    ENTRY:      rc - The client window rectangle (including controls).
    
                stsRect - Receives the rectangles.

********************************************************************/

VOID Object_GetStatusRects(RECT *rc, RECT *stsRect)
{
  INT x, y;

  //
  //  View plane "theta" status window (rotation).
  //

  x = rc->right - CONTROL_AREA_WIDTH2 + STATUS_COLUMN1_POS;
  y = rc->top + VIEW_STATUS_OFFSET;

  stsRect[VIEW_STATUS_THETA].left = x;
  stsRect[VIEW_STATUS_THETA].top = y;
  stsRect[VIEW_STATUS_THETA].right = x + STATUS_COLUMN1_WIDTH;
  stsRect[VIEW_STATUS_THETA].bottom = y + STATUS_HEIGHT;

  //
  //  View plane "phi" status window (elevation).
  //

  y += STATUS_VERT_SPACING;

  stsRect[VIEW_STATUS_PHI].left = x;
  stsRect[VIEW_STATUS_PHI].top = y;
  stsRect[VIEW_STATUS_PHI].right = x + STATUS_COLUMN1_WIDTH;
  stsRect[VIEW_STATUS_PHI].bottom = y + STATUS_HEIGHT;

  //
  //  View plane "x" status window.
  //

  x = rc->right - CONTROL_AREA_WIDTH2 + STATUS_COLUMN2_POS;
  y = rc->top + VIEW_STATUS_OFFSET;

  stsRect[VIEW_STATUS_X].left = x;
  stsRect[VIEW_STATUS_X].top = y;
  stsRect[VIEW_STATUS_X].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[VIEW_STATUS_X].bottom = y + STATUS_HEIGHT;

  //
  //  View plane "y" status window.
  //

  y += STATUS_VERT_SPACING;

  stsRect[VIEW_STATUS_Y].left = x;
  stsRect[VIEW_STATUS_Y].top = y;
  stsRect[VIEW_STATUS_Y].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[VIEW_STATUS_Y].bottom = y + STATUS_HEIGHT;

  //
  //  View plane "z" status window.
  //

  y += STATUS_VERT_SPACING;

  stsRect[VIEW_STATUS_Z].left = x;
  stsRect[VIEW_STATUS_Z].top = y;
  stsRect[VIEW_STATUS_Z].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[VIEW_STATUS_Z].bottom = y + STATUS_HEIGHT;

  //
  //  Grid plane "x" status window.
  //

  x = rc->right - CONTROL_AREA_WIDTH2 + STATUS_COLUMN2_POS;
  y = rc->top + GRID_STATUS_OFFSET;

  stsRect[GRID_STATUS_X].left = x;
  stsRect[GRID_STATUS_X].top = y;
  stsRect[GRID_STATUS_X].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[GRID_STATUS_X].bottom = y + STATUS_HEIGHT;

  //
  //  Grid plane "y" status window.
  //

  y += STATUS_VERT_SPACING;

  stsRect[GRID_STATUS_Y].left = x;
  stsRect[GRID_STATUS_Y].top = y;
  stsRect[GRID_STATUS_Y].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[GRID_STATUS_Y].bottom = y + STATUS_HEIGHT;

  //
  //  Grid plane "z" status window.
  //

  y += STATUS_VERT_SPACING;

  stsRect[GRID_STATUS_Z].left = x;
  stsRect[GRID_STATUS_Z].top = y;
  stsRect[GRID_STATUS_Z].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[GRID_STATUS_Z].bottom = y + STATUS_HEIGHT;

  //
  //  Grid plane "theta" status window (rotation).
  //

  y += STATUS_VERT_SPACING;

  stsRect[GRID_STATUS_THETA].left = x;
  stsRect[GRID_STATUS_THETA].top = y;
  stsRect[GRID_STATUS_THETA].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[GRID_STATUS_THETA].bottom = y + STATUS_HEIGHT;

  //
  //  Gird plane "phi" status window (elevation).
  //

  y += STATUS_VERT_SPACING;

  stsRect[GRID_STATUS_PHI].left = x;
  stsRect[GRID_STATUS_PHI].top = y;
  stsRect[GRID_STATUS_PHI].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[GRID_STATUS_PHI].bottom = y + STATUS_HEIGHT;

  //
  //  Cursor absolute "x" status window.
  //

  x = rc->left + CURSOR_STATUS_COL_POS;
  y = rc->top + CURSOR_STATUS_OFFSET;

  stsRect[CURSOR_STATUS_ABS_X].left = x;
  stsRect[CURSOR_STATUS_ABS_X].top = y;
  stsRect[CURSOR_STATUS_ABS_X].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[CURSOR_STATUS_ABS_X].bottom = y + STATUS_HEIGHT;

  //
  //  Cursor absolute "y" status window.
  //

  y += STATUS_VERT_SPACING;

  stsRect[CURSOR_STATUS_ABS_Y].left = x;
  stsRect[CURSOR_STATUS_ABS_Y].top = y;
  stsRect[CURSOR_STATUS_ABS_Y].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[CURSOR_STATUS_ABS_Y].bottom = y + STATUS_HEIGHT;

  //
  //  Cursor absolute "z" status window.
  //

  y += STATUS_VERT_SPACING;

  stsRect[CURSOR_STATUS_ABS_Z].left = x;
  stsRect[CURSOR_STATUS_ABS_Z].top = y;
  stsRect[CURSOR_STATUS_ABS_Z].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[CURSOR_STATUS_ABS_Z].bottom = y + STATUS_HEIGHT;

  //
  //  Cursor relative "x" status window.
  //

  y += STATUS_HEIGHT * 3;

  stsRect[CURSOR_STATUS_REL_X].left = x;
  stsRect[CURSOR_STATUS_REL_X].top = y;
  stsRect[CURSOR_STATUS_REL_X].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[CURSOR_STATUS_REL_X].bottom = y + STATUS_HEIGHT;

  //
  //  Cursor relative "y" status window.
  //

  y += STATUS_VERT_SPACING;

  stsRect[CURSOR_STATUS_REL_Y].left = x;
  stsRect[CURSOR_STATUS_REL_Y].top = y;
  stsRect[CURSOR_STATUS_REL_Y].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[CURSOR_STATUS_REL_Y].bottom = y + STATUS_HEIGHT;

  //
  //  Cursor relative "z" status window.
  //

  y += STATUS_VERT_SPACING;

  stsRect[CURSOR_STATUS_REL_Z].left = x;
  stsRect[CURSOR_STATUS_REL_Z].top = y;
  stsRect[CURSOR_STATUS_REL_Z].right = x + STATUS_COLUMN2_WIDTH;
  stsRect[CURSOR_STATUS_REL_Z].bottom = y + STATUS_HEIGHT;

  return;
  
} // Object_GetStatusRects


/*******************************************************************

    NAME:       Object_DrawStatusWindows

    SYNOPSIS:   Draw the view and drawing plane status windows.

    ENTRY:      hdc - Device context.

                rc - The client window rectangle (including controls).

********************************************************************/

VOID Object_DrawStatusWindows(HDC hdc, RECT *rc)
{
  INT i, x, y;
  RECT stsRect[NUM_STATUS_WINDOWS];
  HFONT hFontSymbol, hFontArial, hFontArialBold, hFontOld;
  HPEN hPenDkGray, hPenWhite, hPenOld;

  //
  //  Get the status window rectangles.
  //

  Object_GetStatusRects(rc, stsRect);

  //
  //  Draw each status window.
  //

  hPenDkGray = GetStockObject(BLACK_PEN);
  hPenWhite = GetStockObject(WHITE_PEN);

  hPenOld = SelectObject(hdc, hPenDkGray);

  for(i = 0; i < NUM_STATUS_WINDOWS; ++i)
  {
    x = stsRect[i].left;
    y = stsRect[i].top;

    MoveTo(hdc, stsRect[i].left, stsRect[i].top);
    LineTo(hdc, stsRect[i].left, stsRect[i].bottom);
    MoveTo(hdc, stsRect[i].left+1, stsRect[i].top);
    LineTo(hdc, stsRect[i].right, stsRect[i].top);

    SelectObject(hdc, hPenWhite);

    MoveTo(hdc, stsRect[i].left+1, stsRect[i].bottom-1);
    LineTo(hdc, stsRect[i].right, stsRect[i].bottom-1);
    MoveTo(hdc, stsRect[i].right-1, stsRect[i].top+1);
    LineTo(hdc, stsRect[i].right-1, stsRect[i].bottom-1);

    SelectObject(hdc, hPenDkGray);
  }

  SelectObject(hdc, hPenOld);

  //
  //  Draw the status window labels.
  //

  hFontSymbol = CreateFont(-14,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Symbol");
  hFontArial = CreateFont(-14,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
  hFontArialBold = CreateFont(-12,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");

  SetBkMode(hdc, TRANSPARENT);
  SetTextAlign(hdc, TA_LEFT);

  hFontOld = SelectObject(hdc, hFontSymbol);

  x = stsRect[VIEW_STATUS_THETA].left;
  y = stsRect[VIEW_STATUS_THETA].top;

  TextOut(hdc, x - 12, y, "q", 1);

  x = stsRect[VIEW_STATUS_PHI].left;
  y = stsRect[VIEW_STATUS_PHI].top;

  TextOut(hdc, x - 12, y, "f", 1);

  x = stsRect[GRID_STATUS_THETA].left;
  y = stsRect[GRID_STATUS_THETA].top;

  TextOut(hdc, x - 12, y, "q", 1);

  x = stsRect[GRID_STATUS_PHI].left;
  y = stsRect[GRID_STATUS_PHI].top;

  TextOut(hdc, x - 12, y, "f", 1);

  SelectObject(hdc, hFontArial);

  x = stsRect[VIEW_STATUS_X].left;
  y = stsRect[VIEW_STATUS_X].top;

  TextOut(hdc, x - 12, y, "X", 1);

  x = stsRect[VIEW_STATUS_Y].left;
  y = stsRect[VIEW_STATUS_Y].top;

  TextOut(hdc, x - 12, y, "Y", 1);

  x = stsRect[VIEW_STATUS_Z].left;
  y = stsRect[VIEW_STATUS_Z].top;

  TextOut(hdc, x - 12, y, "Z", 1);

  x = stsRect[GRID_STATUS_X].left;
  y = stsRect[GRID_STATUS_X].top;

  TextOut(hdc, x - 12, y, "X", 1);

  x = stsRect[GRID_STATUS_Y].left;
  y = stsRect[GRID_STATUS_Y].top;

  TextOut(hdc, x - 12, y, "Y", 1);

  x = stsRect[GRID_STATUS_Z].left;
  y = stsRect[GRID_STATUS_Z].top;

  TextOut(hdc, x - 12, y, "Z", 1);

  x = stsRect[CURSOR_STATUS_ABS_X].left;
  y = stsRect[CURSOR_STATUS_ABS_X].top;

  TextOut(hdc, x - 12, y, "X", 1);

  x = stsRect[CURSOR_STATUS_ABS_Y].left;
  y = stsRect[CURSOR_STATUS_ABS_Y].top;

  TextOut(hdc, x - 12, y, "Y", 1);

  x = stsRect[CURSOR_STATUS_ABS_Z].left;
  y = stsRect[CURSOR_STATUS_ABS_Z].top;

  TextOut(hdc, x - 12, y, "Z", 1);

  x = stsRect[CURSOR_STATUS_REL_X].left;
  y = stsRect[CURSOR_STATUS_REL_X].top;

  TextOut(hdc, x - 12, y, "X", 1);

  x = stsRect[CURSOR_STATUS_REL_Y].left;
  y = stsRect[CURSOR_STATUS_REL_Y].top;

  TextOut(hdc, x - 12, y, "Y", 1);

  x = stsRect[CURSOR_STATUS_REL_Z].left;
  y = stsRect[CURSOR_STATUS_REL_Z].top;

  TextOut(hdc, x - 12, y, "Z", 1);

  SelectObject(hdc, hFontArialBold);

  x = stsRect[CURSOR_STATUS_ABS_X].left;
  y = stsRect[CURSOR_STATUS_ABS_X].top;

  TextOut(hdc, x - 12, y - 16, "Absolute", 8);

  x = stsRect[CURSOR_STATUS_REL_X].left;
  y = stsRect[CURSOR_STATUS_REL_X].top;

  TextOut(hdc, x - 12, y - 16, "Relative", 8);

  SelectObject(hdc, hFontOld);
  DeleteObject(hFontArialBold);
  DeleteObject(hFontArial);
  DeleteObject(hFontSymbol);
  
  return;
  
} // Object_DrawStatusWindows


/*******************************************************************

    NAME:       Object_ClearStatusWindow

    SYNOPSIS:   Clear the indicated status window.

    ENTRY:      nIndex - The status window index.
                
********************************************************************/

VOID Object_ClearStatusWindow(HDC hdc, RECT *rc, WND_DATA *pWndData, INT nIndex)
{
  INT x1, y1, x2, y2;
  RECT stsRect[NUM_STATUS_WINDOWS];
  HPEN hPenLtGray, hPenOld;
  HBRUSH hBrushLtGray, hBrushOld;

  //
  //  Get the status window rectangles.
  //

  Object_GetStatusRects(rc, stsRect);

  //
  //  Draw the status window.
  //

  hPenLtGray = CreatePen(PS_SOLID, 1, RGB(192,192,192));
  hBrushLtGray = CreateSolidBrush(RGB(192,192,192));

  hPenOld = SelectObject(hdc, hPenLtGray);
  hBrushOld = SelectObject(hdc, hBrushLtGray);

  x1 = stsRect[nIndex].left;
  y1 = stsRect[nIndex].top;
  x2 = stsRect[nIndex].right;
  y2 = stsRect[nIndex].bottom;

  Rectangle(hdc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);

  SelectObject(hdc, hBrushOld);
  SelectObject(hdc, hPenOld);
  DeleteObject(hBrushLtGray);
  DeleteObject(hPenLtGray);

  return;

} // Object_ClearStatusWindow


/*******************************************************************

    NAME:       Object_UpdateStatusWindow

    SYNOPSIS:   Update the indicated status window.

    ENTRY:      nIndex - The status window index.
                
********************************************************************/

VOID Object_UpdateStatusWindow(HDC hdc, RECT *rc, WND_DATA *pWndData, INT nIndex)
{
  INT x1, y1, x2, y2;
  RECT stsRect[NUM_STATUS_WINDOWS];
  CHAR szBuf[80];
  HFONT hFontArial, hFontOld;
  HPEN hPenLtGray, hPenOld;
  HBRUSH hBrushLtGray, hBrushOld;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  //
  //  Get the status window rectangles.
  //

  Object_GetStatusRects(rc, stsRect);

  //
  //  Draw the status window.
  //

  hFontArial = CreateFont(-14,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
  hPenLtGray = CreatePen(PS_SOLID, 1, RGB(192,192,192));
  hBrushLtGray = CreateSolidBrush(RGB(192,192,192));

  SetBkMode(hdc, TRANSPARENT);
  SetTextAlign(hdc, TA_LEFT);

  hFontOld = SelectObject(hdc, hFontArial);
  hPenOld = SelectObject(hdc, hPenLtGray);
  hBrushOld = SelectObject(hdc, hBrushLtGray);

  if (nIndex == VIEW_STATUS_THETA)
  {
    DoubleToString(szBuf, (double)pLevel->view_rotation * 180.0 / PI, 1);
  }
  else if (nIndex == VIEW_STATUS_PHI)
  {
    DoubleToString(szBuf, (double)pLevel->view_elevation * 180.0 / PI, 1);
  }
  else if (nIndex == VIEW_STATUS_X)
  {
    wsprintf(szBuf, "%ld", pLevel->view_x);
  }
  else if (nIndex == VIEW_STATUS_Y)
  {
    wsprintf(szBuf, "%ld", pLevel->view_y);
  }
  else if (nIndex == VIEW_STATUS_Z)
  {
    wsprintf(szBuf, "%ld", pLevel->view_z);
  }
  else if (nIndex == GRID_STATUS_THETA)
  {
    DoubleToString( szBuf, (double)pLevel->grid_rotation * 180.0 / PI, 1);
  }
  else if (nIndex == GRID_STATUS_PHI)
  {
    DoubleToString(szBuf, (double)pLevel->grid_elevation * 180.0 / PI, 1);
  }
  else if (nIndex == GRID_STATUS_X)
  {
    wsprintf(szBuf, "%ld", pLevel->grid_x);
  }
  else if (nIndex == GRID_STATUS_Y)
  {
    wsprintf(szBuf, "%ld", pLevel->grid_y);
  }
  else if (nIndex == GRID_STATUS_Z)
  {
    wsprintf(szBuf, "%ld", pLevel->grid_z);
  }
  else if (nIndex == CURSOR_STATUS_ABS_X)
  {
    wsprintf(szBuf, "%ld", pWndData->cursor_abs_x);
  }
  else if (nIndex == CURSOR_STATUS_ABS_Y)
  {        
    wsprintf(szBuf, "%ld", pWndData->cursor_abs_y);
  }
  else if (nIndex == CURSOR_STATUS_ABS_Z)
  {
    wsprintf(szBuf, "%ld", pWndData->cursor_abs_z);
  }
  else if (nIndex == CURSOR_STATUS_REL_X)
  {
    wsprintf( szBuf, "%ld", pWndData->cursor_rel_x);
  }
  else if (nIndex == CURSOR_STATUS_REL_Y )
  {
    wsprintf(szBuf, "%ld", pWndData->cursor_rel_y);
  }
  else // if ( nIndex == CURSOR_STATUS_REL_Z )
  {
    wsprintf(szBuf, "%ld", pWndData->cursor_rel_z);
  }

  x1 = stsRect[nIndex].left;
  y1 = stsRect[nIndex].top;
  x2 = stsRect[nIndex].right;
  y2 = stsRect[nIndex].bottom;

  Rectangle(hdc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);

  TextOut(hdc, x1 + 1, y1 - 1, szBuf, _fstrlen(szBuf));

  //
  //  Show the level title
  //

  Object_ShowTitle(hdc, rc, "%s", pLevel->level_name);

  //
  //  Cleanup
  //

  SelectObject(hdc, hBrushOld);
  SelectObject(hdc, hPenOld);
  SelectObject(hdc, hFontOld);
  DeleteObject(hBrushLtGray);
  DeleteObject(hPenLtGray);
  DeleteObject(hFontArial);

  return;

} // Object_UpdateStatusWindow


/*******************************************************************

    NAME:       Object_ControlPressed

    SYNOPSIS:   The indicated control has been pressed so take
                action.

    ENTRY:      nIndex - The control index.
                
********************************************************************/

VOID Object_ControlPressed(HDC hdc, RECT *rc, WND_DATA *pWndData, INT nIndex)
{
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  if (nIndex == MODE_POINTER)
  {
    nMode = MODE_POINTER;
  }
  else if (nIndex == MODE_SELECT_VERTEX)
  {
    nMode = MODE_SELECT_VERTEX;
  }
  else if (nIndex == MODE_SELECT_POLYGON)
  {
    nMode = MODE_SELECT_POLYGON;
  }
  else if (nIndex == MODE_SELECT_GROUP)
  {
    nMode = MODE_SELECT_GROUP;
  }
  else if (nIndex == MODE_NEW_VERTEX)
  {
    nMode = MODE_NEW_VERTEX;
  }
  else if (nIndex == MODE_NEW_POLYGON)
  {
    nMode = MODE_NEW_POLYGON;
  }
  else if (nIndex == MODE_ROTATE_POLYGON)
  {
    nMode = MODE_ROTATE_POLYGON;
  }
  else if (nIndex == MODE_FLIP_POLYGON)
  {
    nMode = MODE_FLIP_POLYGON;
  }
  else if (nIndex == MODE_LINK_POLYGON)
  {
    nMode = MODE_LINK_POLYGON;
  }
  else if (nIndex == VIEW_PAN_UP || nIndex == VIEW_PAN_DOWN)
  {
    double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
    double sin_gtheta, cos_gtheta, sin_gphi, cos_gphi;
    double d, dx, dy, dz;

    if (nIndex == VIEW_PAN_UP)
    {
      d = (double)pLevel->view_delta;
    }
    else
    {
      d = -(double)pLevel->view_delta;
    }

    if (!pLevel->bMaintainGridDist)
    {
      sin_vtheta = sin(pLevel->view_rotation);
      cos_vtheta = cos(pLevel->view_rotation);
      sin_vphi   = sin(pLevel->view_elevation);
      cos_vphi   = cos(pLevel->view_elevation);

      dx = d*sin_vphi*cos_vtheta;
      dy = d*sin_vphi*sin_vtheta;
      dz = d*cos_vphi;
    }
    else
    {
      sin_vtheta = sin(pLevel->view_rotation-pLevel->grid_rotation);
      cos_vtheta = cos(pLevel->view_rotation-pLevel->grid_rotation);
      sin_gtheta = sin(pLevel->grid_rotation);
      cos_gtheta = cos(pLevel->grid_rotation);
      sin_gphi   = sin(pLevel->grid_elevation);
      cos_gphi   = cos(pLevel->grid_elevation);

      dx =  d*cos_vtheta*cos_gphi*cos_gtheta-d*sin_vtheta*sin_gtheta;
      dy =  d*cos_vtheta*cos_gphi*sin_gtheta+d*sin_vtheta*cos_gtheta;
      dz = -d*cos_vtheta*sin_gphi;
    }

    pLevel->view_x += (long)dx;
    pLevel->view_y += (long)dy;
    pLevel->view_z += (long)dz;

    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_X);
    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_Y);
    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_Z);
  }
  else if (nIndex == VIEW_PAN_LEFT || nIndex == VIEW_PAN_RIGHT)
  {
    double sin_vtheta, cos_vtheta;
    double sin_gtheta, cos_gtheta, sin_gphi, cos_gphi;
    double d, dx, dy, dz;

    if (nIndex == VIEW_PAN_LEFT)
    {
      d = (double)pLevel->view_delta;
    }
    else
    {
      d = -(double)pLevel->view_delta;
    }

    if (!pLevel->bMaintainGridDist)
    {
      sin_vtheta = sin(pLevel->view_rotation);
      cos_vtheta = cos(pLevel->view_rotation);

      dx = -d*sin_vtheta;
      dy =  d*cos_vtheta;
      dz =  0.0;
    }
    else
    {
      sin_vtheta = sin(pLevel->view_rotation-pLevel->grid_rotation);
      cos_vtheta = cos(pLevel->view_rotation-pLevel->grid_rotation);
      sin_gtheta = sin(pLevel->grid_rotation);
      cos_gtheta = cos(pLevel->grid_rotation);
      sin_gphi   = sin(pLevel->grid_elevation);
      cos_gphi   = cos(pLevel->grid_elevation);

      dx = -d*sin_vtheta*cos_gphi*cos_gtheta-d*cos_vtheta*sin_gtheta;
      dy = -d*sin_vtheta*cos_gphi*sin_gtheta+d*cos_vtheta*cos_gtheta;
      dz =  d*sin_vtheta*sin_gphi;
    }

    pLevel->view_x += (long)dx;
    pLevel->view_y += (long)dy;
    pLevel->view_z += (long)dz;

    Object_UpdateStatusWindow( hdc, rc, pWndData, VIEW_STATUS_X );
    Object_UpdateStatusWindow( hdc, rc, pWndData, VIEW_STATUS_Y );
    Object_UpdateStatusWindow( hdc, rc, pWndData, VIEW_STATUS_Z );
  }
  else if (nIndex == VIEW_TILT_UP)
  {
    double delta;

    delta = PI / 64;

    pLevel->view_elevation += delta;

    if (pLevel->view_elevation > PI / 2.0)
    {
      pLevel->view_elevation = PI / 2.0;
    }

    pLevel->view_elevation = delta * Round(pLevel->view_elevation / delta);

    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_PHI);
  }
  else if (nIndex == VIEW_TILT_DOWN)
  {
    double delta;

    delta = PI / 64;

    pLevel->view_elevation -= delta;

    if (pLevel->view_elevation < -PI / 2.0)
    {
      pLevel->view_elevation = -PI / 2.0;
    }

    pLevel->view_elevation = delta * Round(pLevel->view_elevation / delta);

    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_PHI);
  }
  else if (nIndex == VIEW_ROTATE_LEFT || nIndex == VIEW_ROTATE_RIGHT)
  {
//  double sin_gtheta, cos_gtheta, sin_gphi, cos_gphi;
    double dr, d, t, dx, dy, delta;
//  double dz, rx, ry;

    delta = PI / 64;

    if (nIndex == VIEW_ROTATE_LEFT)
    {
      dr = delta;
    }
    else
    {
      dr = -delta;
    }

    if (pLevel->bLockViewToGrid)
    {
      dx = (double)pLevel->view_x - (double)pLevel->grid_x;
      dy = (double)pLevel->view_y - (double)pLevel->grid_y;

      d = sqrt(dx * dx + dy * dy);

      if (dx == 0.0)
      {
        if (dy < 0.0)
        {
          t = -PI / 2.0;
        }
        else
        {
          t = PI / 2.0;
        }
      }
      else
      {
        t = atan(dy / dx);

        if ((dx < 0.0 && dy > 0.0) || (dx < 0.0 && dy < 0.0))
        {
          t += PI;
        }
      }

      t += PI;

      dx = d * (cos(t) - cos(t + dr));
      dy = d * (sin(t) - sin(t + dr));

//    sin_gtheta = sin(-pLevel->grid_rotation);
//    cos_gtheta = cos(-pLevel->grid_rotation);
//    sin_gphi   = sin(pLevel->grid_elevation);
//    cos_gphi   = cos(pLevel->grid_elevation);

//    rx = (dx * cos_gtheta - dy * sin_gtheta) * cos_gphi;
//    ry = (dx * sin_gtheta + dy * cos_gtheta);
//    dz = (dy * sin_gtheta - dx * cos_gtheta) * sin_gphi;

//    sin_gtheta = sin(pLevel->grid_rotation);
//    cos_gtheta = cos(pLevel->grid_rotation);

//    dx = rx * cos_gtheta - ry * sin_gtheta;
//    dy = rx * sin_gtheta + ry * cos_gtheta;

      pLevel->view_x += (long)dx;
      pLevel->view_y += (long)dy;
//    pLevel->view_z += (long)dz;

      Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_X);
      Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_Y);
//    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_Z);
    }

    pLevel->view_rotation += dr;

    if (pLevel->view_rotation > PI * 2.0)
    {
      pLevel->view_rotation -= PI * 2.0;
    }

    if (pLevel->view_rotation < 0.0)
    {
      pLevel->view_rotation += PI * 2.0;
    }

    pLevel->view_rotation = delta * Round(pLevel->view_rotation / delta);

    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_THETA);
  }
  else if (nIndex == VIEW_ZOOM_IN)
  {
    double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
    double dx, dy, dz;

    sin_vtheta = sin(pLevel->view_rotation);
    cos_vtheta = cos(pLevel->view_rotation);
    sin_vphi   = sin(pLevel->view_elevation);
    cos_vphi   = cos(pLevel->view_elevation);

    dx = (double)pLevel->view_delta*cos_vphi*cos_vtheta;
    dy = (double)pLevel->view_delta*cos_vphi*sin_vtheta;
    dz = -(double)pLevel->view_delta*sin_vphi;

    pLevel->view_x += (long)dx;
    pLevel->view_y += (long)dy;
    pLevel->view_z += (long)dz;

    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_X);
    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_Y);
    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_Z);
  }
  else if (nIndex == VIEW_ZOOM_OUT)
  {
    double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
    double dx, dy, dz;

    sin_vtheta = sin(pLevel->view_rotation);
    cos_vtheta = cos(pLevel->view_rotation);
    sin_vphi   = sin(pLevel->view_elevation);
    cos_vphi   = cos(pLevel->view_elevation);

    dx = -(double)pLevel->view_delta*cos_vphi*cos_vtheta;
    dy = -(double)pLevel->view_delta*cos_vphi*sin_vtheta;
    dz = (double)pLevel->view_delta*sin_vphi;

    pLevel->view_x += (long)dx;
    pLevel->view_y += (long)dy;
    pLevel->view_z += (long)dz;

    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_X);
    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_Y);
    Object_UpdateStatusWindow(hdc, rc, pWndData, VIEW_STATUS_Z);
  }
  else if (nIndex == GRID_Y_INCREASE)
  {
    pLevel->grid_y += pLevel->grid_delta;

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_Y);
  }
  else if (nIndex == GRID_Y_DECREASE)
  {
    pLevel->grid_y -= pLevel->grid_delta;

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_Y);
  }
  else if (nIndex == GRID_X_DECREASE)
  {
    pLevel->grid_x -= pLevel->grid_delta;

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_X);
  }
  else if (nIndex == GRID_X_INCREASE)
  {
    pLevel->grid_x += pLevel->grid_delta;

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_X);
  }
  else if (nIndex == GRID_TILT_UP)
  {
    double delta;

    delta = PI / 64.0;

    pLevel->grid_elevation += delta;

    if (pLevel->grid_elevation > PI / 2.0)
    {
      pLevel->grid_elevation = PI / 2.0;
    }

    pLevel->grid_elevation = delta * Round(pLevel->grid_elevation / delta);

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_PHI);
  }
  else if (nIndex == GRID_TILT_DOWN)
  {
    double delta;

    delta = PI / 64.0;

    pLevel->grid_elevation -= delta;

    if (pLevel->grid_elevation < -PI / 2.0)
    {
      pLevel->grid_elevation = -PI / 2.0;
    }

    pLevel->grid_elevation = delta * Round(pLevel->grid_elevation / delta);

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_PHI);
  }
  else if (nIndex == GRID_ROTATE_LEFT)
  {
    double delta;

    delta = PI / 64.0;

    pLevel->grid_rotation += delta;

    if (pLevel->grid_rotation > PI * 2.0)
    {
      pLevel->grid_rotation -= PI * 2.0;
    }

    pLevel->grid_rotation = delta * Round(pLevel->grid_rotation / delta);

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_THETA);
  }
  else if (nIndex == GRID_ROTATE_RIGHT)
  {
    double delta;

    delta = PI / 64.0;

    pLevel->grid_rotation -= delta;

    if (pLevel->grid_rotation < 0.0)
    {
      pLevel->grid_rotation += PI * 2.0;
    }

    pLevel->grid_rotation = delta * Round(pLevel->grid_rotation / delta);

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_THETA);
  }
  else if (nIndex == GRID_Z_INCREASE)
  {
    pLevel->grid_z += pLevel->grid_delta;

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_Z);
  }
  else // if (nIndex == GRID_Z_DECREASE)
  {
    pLevel->grid_z -= pLevel->grid_delta;

    Object_UpdateStatusWindow(hdc, rc, pWndData, GRID_STATUS_Z);
  }

  if (nIndex >= NUM_MODE_CONTROLS)
  {
    Object_RegenerateView( rc->right - rc->left,
                           rc->bottom - rc->top,
                           pWndData );

    Object_DrawView(hdc, pWndData->hbmView, pWndData->bm_width, pWndData->bm_height);
  }

  return;

} // Object_ControlPressed


/*******************************************************************

    NAME:       Object_DrawLine

    SYNOPSIS:   Draw a 3D line and clip it to the 2D view.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

                pLine - First and second points in an array of
                  doubles (xp1,yp1,zp1,xp2,yp2,zp2).

********************************************************************/

VOID Object_DrawLine(HDC hdc, WND_DATA *pWndData, double *pLine, INT x_off, INT y_off)
{
  double r, s, t, m, xp1, yp1, zp1, xp2, yp2, zp2;
  double xMax, yMax, x_scale, y_scale, x_adj, y_adj;
  double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  x_adj = (double)(pWndData->bm_width / 2);
  y_adj = (double)(pWndData->bm_height / 2);

  x_scale = 500.0; // (double)xLen;
  y_scale = 500.0; // (double)yLen;

  xMax = (double)pWndData->bm_width - 0.000001;
  yMax = (double)pWndData->bm_height - 0.000001;

  xp1 = pLine[0] - (double)pLevel->view_x;
  yp1 = pLine[1] - (double)pLevel->view_y;
  zp1 = pLine[2] - (double)pLevel->view_z;
  xp2 = pLine[3] - (double)pLevel->view_x;
  yp2 = pLine[4] - (double)pLevel->view_y;
  zp2 = pLine[5] - (double)pLevel->view_z;

  sin_vtheta = sin(-pLevel->view_rotation);
  cos_vtheta = cos(-pLevel->view_rotation);
  sin_vphi   = sin(-pLevel->view_elevation);
  cos_vphi   = cos(-pLevel->view_elevation);

  //
  //  Translate the points to the current view.
  //

  r = (xp1*cos_vtheta-yp1*sin_vtheta)*cos_vphi+zp1*sin_vphi;
  s = xp1*sin_vtheta+yp1*cos_vtheta;
  t = zp1*cos_vphi-(xp1*cos_vtheta-yp1*sin_vtheta)*sin_vphi;

  xp1 = r;
  yp1 = s;
  zp1 = t;

  r = (xp2*cos_vtheta-yp2*sin_vtheta)*cos_vphi+zp2*sin_vphi;
  s = xp2*sin_vtheta+yp2*cos_vtheta;
  t = zp2*cos_vphi-(xp2*cos_vtheta-yp2*sin_vtheta)*sin_vphi;

  xp2 = r;
  yp2 = s;
  zp2 = t;

  //
  //  Skip it if the line is completely behind the view plane.
  //

  if ( xp1 < (double)pLevel->view_depth &&
       xp2 < (double)pLevel->view_depth )
  {
    return;
  }

  //
  //  Clip any part of the line behind the view plane.
  //

  if (xp1 < (double)pLevel->view_depth)
  {
    m = (zp2 - zp1) / (xp2 - xp1);
    zp1 = m * ((double)pLevel->view_depth - xp1) + zp1;
    m = (yp2 - yp1) / (xp2 - xp1);
    yp1 = m * ((double)pLevel->view_depth - xp1) + yp1;
    xp1 = (double)pLevel->view_depth;
  }

  if (xp2 < (double)pLevel->view_depth)
  {
    m = (zp2 - zp1) / (xp2 - xp1);
    zp2 = m * ((double)pLevel->view_depth - xp1) + zp1;
    m = (yp2 - yp1) / (xp2 - xp1);
    yp2 = m * ((double)pLevel->view_depth - xp1) + yp1;
    xp2 = (double)pLevel->view_depth;
  }

  if ( xp1 < (double)pLevel->view_depth ||
       xp2 < (double)pLevel->view_depth )
  {
    return;
  }

  //
  //  Calculate the view window position (in pixels).
  //

  r = 1 / xp1;
  xp1 = x_adj - x_scale * yp1 * r;
  yp1 = y_adj - y_scale * zp1 * r;

  r = 1 / xp2;
  xp2 = x_adj - x_scale * yp2 * r;
  yp2 = y_adj - y_scale * zp2 * r;

  //
  //  If the line is completely above, below, left of, or right of the
  //  view window then it is trivially invisible. If not, it may still
  //  be invisible.
  //

  if ( (xp1 < 0.0 && xp2 < 0.0) ||
       (yp1 < 0.0 && yp2 < 0.0) ||
       (xp1 > xMax && xp2 > xMax) ||
       (yp1 > yMax && yp2 > yMax) )
  {
    return;
  }

  //
  //  If the line is completely within the view window then no further
  //  clipping is required and we can just draw it.
  //
  
  if ( xp1 >= 0.0 && xp1 <= xMax &&
       xp2 >= 0.0 && xp2 <= xMax &&
       yp1 >= 0.0 && yp1 <= yMax &&
       yp2 >= 0.0 && yp2 <= yMax )
  {
    MoveTo(hdc, (int)xp1 + x_off, (int)yp1 + y_off);
    LineTo(hdc, (int)xp2 + x_off, (int)yp2 + y_off);
    return;
  }

  //
  //  The line is either partially visible or invisible. First handle
  //  verticle or horizontal lines as special cases (a zero or infinite
  //  slope causes problems).
  //

  m = xp2 - xp1;

  if (fabs(m) < 0.000001)               // if line is vertical or nearly so
  {
    if (xp1 < 0.0 || xp1 > xMax)
    {
      return;
    }

    if (yp1 < 0.0)
    {
      yp1 = 0.0;
    }
    else if (yp1 > yMax)
    {
      yp1 = yMax;
    }

    if (yp2 < 0.0)
    {
      yp2 = 0.0;
    }
    else if (yp2 > yMax)
    {
      yp2 = yMax;
    }

    MoveTo(hdc, (int)xp1 + x_off, (int)yp1 + y_off);
    LineTo(hdc, (int)xp1 + x_off, (int)yp2 + y_off);
    return;
  }

  m = (yp2 - yp1) / m;

  if (fabs(m) < 0.000001)               // if line is horizontal or nearly so
  {
    if (yp1 < 0.0 || yp1 > yMax)
    {
      return;
    }

    if (xp1 < 0.0)
    {
      xp1 = 0.0;
    }
    else if (xp1 > xMax)
    {
      xp1 = xMax;
    }

    if (xp2 < 0.0)
    {
      xp2 = 0.0;
    }
    else if (xp2 > xMax)
    {
      xp2 = xMax;
    }

    MoveTo(hdc, (int)xp1 + x_off, (int)yp1 + y_off);
    LineTo(hdc, (int)xp2 + x_off, (int)yp1 + y_off);

    return;
  }

  //
  //  The line is not horizontal or vertical so use the first point and the
  //  slope to calculate the clipped points.
  //

  if (xp1 < 0.0)
  {
    yp1 = -m * xp1 + yp1;               // y = m * (x - xp1) + yp1, x = 0.0
    xp1 = 0.0;
  }
  else if (xp2 < 0.0)
  {
    yp2 = -m * xp1 + yp1;               // y = m * (x - xp1) + yp1, x = 0.0
    xp2 = 0.0;
  }

  if (xp1 > xMax)
  {
    yp1 = m * (xMax - xp1) + yp1;       // y = m * (x - xp1) + yp1, x = xMax
    xp1 = xMax;
  }
  else if (xp2 > xMax)
  {
    yp2 = m * (xMax - xp1) + yp1;       // y = m * (x - xp1) + yp1, x = xMax
    xp2 = xMax;
  }

  if (yp1 < 0.0)
  {
    xp1 = xp1 - yp1 / m;                // x = xp1 + (y - yp1) / m, y = 0.0
    yp1 = 0.0;
  }
  else if (yp2 < 0.0)
  {
    xp2 = xp1 - yp1 / m;                // x = xp1 + (y - yp1) / m, y = 0.0
    yp2 = 0.0;
  }

  if (yp1 > yMax)
  {
    xp1 = xp1 + (yMax - yp1) / m;       // x = xp1 + (y - yp1) / m, y = yMax
    yp1 = yMax;
  }
  else if (yp2 > yMax)
  {
    xp2 = xp1 + (yMax - yp1) / m;       // x = xp1 + (y - yp1) / m, y = yMax
    yp2 = yMax;
  }

  //
  //  If either point is still out of range then the line is not visible.
  //

  if ( xp1 < 0.0 || xp2 < 0.0 || yp1 < 0.0 || yp2 < 0.0 ||
       xp1 > xMax || xp2 > xMax || yp1 > yMax || yp2 > yMax )
  {
    return;
  }

  //
  //  Draw the clipped line.
  //

  MoveTo(hdc, (int)xp1 + x_off, (int)yp1 + y_off);
  LineTo(hdc, (int)xp2 + x_off, (int)yp2 + y_off);
  return;
  
} // Object_DrawLine


/*******************************************************************

    NAME:       Object_DrawGrid

    SYNOPSIS:   Draw the drawing plane grid.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

********************************************************************/

VOID Object_DrawGrid(HDC hdc, WND_DATA *pWndData)
{
  INT i, j, n;
  HPEN hPenBlue, hPenGreen, hPenOld;
  HBRUSH hBrushOld;
  double r, s, t, xp1, yp1, xp2, yp2, line[6];
  double sin_gtheta, cos_gtheta, sin_gphi, cos_gphi;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  hPenBlue = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
  hPenGreen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
  hPenOld = SelectObject(hdc, GetStockObject(BLACK_PEN));
  hBrushOld = SelectObject(hdc, GetStockObject(BLACK_BRUSH));

  Rectangle(hdc, 0, 0, pWndData->bm_width, pWndData->bm_height);

  SelectObject(hdc, hPenBlue);

  sin_gtheta = sin(pLevel->grid_rotation);
  cos_gtheta = cos(pLevel->grid_rotation);
  sin_gphi   = sin(pLevel->grid_elevation);
  cos_gphi   = cos(pLevel->grid_elevation);

  n = (INT)(pLevel->grid_size / pLevel->grid_spacing + 1L) / 2;

  for(i = 0; i < 2; ++i)                // horz lines first, then vert
  {
    for(j = -n; j <= n; ++j)
    {
      //
      //  Calculate the untranslated end points for this grid line.
      //

      if (j == 0)
      {
        SelectObject(hdc, hPenGreen);
      }

      r = (double)j * (double)pLevel->grid_spacing;
      s = (double)(pLevel->grid_size / 2L);

      if (i == 0)
      {
        xp1 = -s;
        yp1 = r;

        xp2 = s;
        yp2 = r;
      }
      else
      {
        xp1 = r;
        yp1 = -s;

        xp2 = r;
        yp2 = s;
      }

      //
      //  Translate the line to the current grid settings.
      //

      r = xp1*cos_gphi*cos_gtheta-yp1*sin_gtheta;
      s = xp1*cos_gphi*sin_gtheta+yp1*cos_gtheta;
      t = -xp1*sin_gphi;

      line[0] = r + (double)pLevel->grid_x;
      line[1] = s + (double)pLevel->grid_y;
      line[2] = t + (double)pLevel->grid_z;
  
      r = xp2*cos_gphi*cos_gtheta-yp2*sin_gtheta;
      s = xp2*cos_gphi*sin_gtheta+yp2*cos_gtheta;
      t = -xp2*sin_gphi;

      line[3] = r + (double)pLevel->grid_x;
      line[4] = s + (double)pLevel->grid_y;
      line[5] = t + (double)pLevel->grid_z;

      //
      //  Draw the line.
      //

      Object_DrawLine(hdc, pWndData, line, 0, 0);

      if ( j == 0 )
      {
        SelectObject(hdc, hPenBlue);
      }
    }
  }

  SelectObject(hdc, hBrushOld);
  SelectObject(hdc, hPenOld);
  DeleteObject(hPenGreen);
  DeleteObject(hPenBlue);
  
  return;
  
} // Object_DrawGrid


/*******************************************************************

    NAME:       Object_DrawVertex

    SYNOPSIS:   Draw a single vertex with the current pen and brush.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

                x - Vertex x position.

                y - Vertex y position.

                z - Vertex z position.
                
                nOrder - If > 0 then show order number.
                
                nDir - If > 0 then show direction vector.

                               4 3 2
                                \|/ 
                               5-+-1
                                /|\
                               6 7 8

********************************************************************/

VOID Object_DrawVertex(HDC hdc, WND_DATA *pWndData, LONG x, LONG y, LONG z, INT nOrder, INT nDir)
{
  char szBuf[40];
  int xi, yi;
  double r, s, t, xp, yp, zp;
  double xMax, yMax, x_scale, y_scale, x_adj, y_adj;
  double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  x_adj = (double)(pWndData->bm_width / 2);
  y_adj = (double)(pWndData->bm_height / 2);

  x_scale = 500.0; // (double)xLen;
  y_scale = 500.0; // (double)yLen;

  xMax = (double)pWndData->bm_width - 0.000001;
  yMax = (double)pWndData->bm_height - 0.000001;

  xp = (double)x - (double)pLevel->view_x;
  yp = (double)y - (double)pLevel->view_y;
  zp = (double)z - (double)pLevel->view_z;

  sin_vtheta = sin(-pLevel->view_rotation);
  cos_vtheta = cos(-pLevel->view_rotation);
  sin_vphi   = sin(-pLevel->view_elevation);
  cos_vphi   = cos(-pLevel->view_elevation);

  //
  //  Translate the point to the current view.
  //

  r = (xp*cos_vtheta-yp*sin_vtheta)*cos_vphi+zp*sin_vphi;
  s = xp*sin_vtheta+yp*cos_vtheta;
  t = zp*cos_vphi-(xp*cos_vtheta-yp*sin_vtheta)*sin_vphi;

  //
  //  Skip it if the point is behind the view plane.
  //

  if (r < (double)pLevel->view_depth)
  {
    return;
  }

  r = 1 / r;
  xp = x_adj - x_scale * s * r;
  yp = y_adj - y_scale * t * r;

  //
  //  If the point is outside the view window then skip it.
  //

  if (xp < 0.0 || yp < 0.0 || xp > xMax || yp > yMax)
  {
    return;
  }

  xi = (int)xp;
  yi = (int)yp;

  if (nOrder > 0)
  {
    TEXTMETRIC tm;
    GetTextMetrics(hdc, &tm);
    wsprintf(szBuf, "%d", nOrder);
    Ellipse(hdc, xi - 8, yi - 8, xi + 8, yi + 8);
    SetTextAlign(hdc, TA_CENTER);
    TextOut(hdc, xi, yi - tm.tmHeight / 2, szBuf, strlen(szBuf));
  }
  else
  {
    Ellipse(hdc, xi - 4, yi - 4, xi + 4, yi + 4);
  }

  if (nDir > 0 && nDir <= 8)
  {
    MoveTo(hdc, xi, yi);

    if (nDir == 1)
    {
      LineTo(hdc, xi + 8, yi);
    }
    else if (nDir == 2)
    {
      LineTo(hdc, xi + 6, yi - 6);
    }
    else if (nDir == 3)
    {
      LineTo(hdc, xi, yi - 8);
    }
    else if (nDir == 4)
    {
      LineTo(hdc, xi - 6, yi - 6);
    }
    else if (nDir == 5)
    {
      LineTo(hdc, xi - 8, yi);
    }
    else if (nDir == 6)
    {
      LineTo(hdc, xi - 6, yi + 6);
    }
    else if (nDir == 7)
    {
      LineTo(hdc, xi, yi + 8);
    }
    else
    {
      LineTo(hdc, xi + 6, yi + 6);
    }
  }

} // Object_DrawVertex


/*******************************************************************

    NAME:       Object_DrawVertices

    SYNOPSIS:   Draw the vertices.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

********************************************************************/

VOID Object_DrawVertices(HDC hdc, WND_DATA *pWndData)
{
  INT i, color;
  VERTEX huge *pVertex;
  LEVEL_DATA *pLevel;
  HPEN hPenRed, hPenGreen, hPenYellow, hPenCyan;
  HPEN hPenMagenta, hPenGrey, hPenOld;
  HBRUSH hBrushRed, hBrushGreen, hBrushYellow, hBrushCyan;
  HBRUSH hBrushMagenta, hBrushGrey, hBrushOld;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  //
  //  Create the various pens and brushs we may need.
  //

  hPenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
  hBrushRed = CreateSolidBrush(RGB(255, 0, 0));

  hPenGreen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
  hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));

  hPenYellow = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
  hBrushYellow = CreateSolidBrush(RGB(255, 255, 0));

  hPenCyan = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
  hBrushCyan = CreateSolidBrush(RGB(0, 255, 255));

  hPenMagenta = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
  hBrushMagenta = CreateSolidBrush(RGB(255, 0, 255));

  hPenGrey = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
  hBrushGrey = CreateSolidBrush(RGB(192, 192, 192));

  hPenOld = SelectObject(hdc, hPenRed);
  hBrushOld = SelectObject(hdc, hBrushRed);

  color = 5;

  pVertex = pLevel->pVertexData;

  for(i = 0; i < pLevel->nVertices; ++i, ++pVertex)
  {
    if (pVertex->flags & (VF_HIDDEN | VF_SELECTED))
    {
      continue;
    }

    if (pVertex->flags & VF_SPRITE)
    {
      if (color != 1)
      {
        color = 1;
        SelectObject(hdc, hPenGrey);
        SelectObject(hdc, hBrushGrey);
      }  
    }
    else if (pVertex->flags & (VF_LIFEFORM | VF_PLAYER_START))
    {
      if (color != 2)
      {
        color = 2;
        SelectObject(hdc, hPenCyan);
        SelectObject(hdc, hBrushCyan);
      }  
    }
    else if (pVertex->flags & (VF_TRIGGER | VF_NOISE))
    {
      if (color != 3)
      {
        color = 3;
        SelectObject(hdc, hPenMagenta);
        SelectObject(hdc, hBrushMagenta);
      }  
    }
    else if (pVertex->flags & VF_ITEM)
    {
      if (color != 4)
      {
        color = 4;
        SelectObject(hdc, hPenYellow);
        SelectObject(hdc, hBrushYellow);
      }  
    }
    else
    {
      if (color != 5)
      {
        color = 5;
        SelectObject(hdc, hPenRed);
        SelectObject(hdc, hBrushRed);
      }  
    }

    Object_DrawVertex( hdc,
                       pWndData,
                       (LONG)Round(pVertex->x),
                       (LONG)Round(pVertex->y),
                       (LONG)Round(pVertex->z),
                       0, 0 );
  }

  SelectObject(hdc, hPenGreen);
  SelectObject(hdc, hBrushGreen);

  pVertex = pLevel->pVertexData;

  for(i = 0; i < pLevel->nVertices; ++i, ++pVertex)
  {
    if (pVertex->flags & VF_HIDDEN)
    {
      continue;
    }

    if (pVertex->flags & VF_SELECTED)
    {
      Object_DrawVertex( hdc,
                         pWndData,
                         (LONG)Round(pVertex->x),
                         (LONG)Round(pVertex->y),
                         (LONG)Round(pVertex->z),
                         0, 0 );
    }
  }

  SelectObject(hdc, hBrushOld);
  SelectObject(hdc, hPenOld);

  DeleteObject(hBrushGrey);
  DeleteObject(hPenGrey);

  DeleteObject(hBrushMagenta);
  DeleteObject(hPenMagenta);

  DeleteObject(hBrushCyan);
  DeleteObject(hPenCyan);

  DeleteObject(hBrushYellow);
  DeleteObject(hPenYellow);

  DeleteObject(hBrushGreen);
  DeleteObject(hPenGreen);

  DeleteObject(hPenRed);
  DeleteObject(hBrushRed);

} // Object_DrawVertices


/*******************************************************************

    NAME:       Object_DrawPolygon

    SYNOPSIS:   Draw the polyogn.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

                nVertices - Number of vertices in the polygon.

                pVertices - Pointer to the vertices.

********************************************************************/

VOID Object_DrawPolygon(HDC hdc, WND_DATA *pWndData, INT nVertices, WORD *pVertices)
{
  INT i;
  VERTEX huge *pVertex;
  double point[(MAX_VERTICES+1)*3];

  for(i = 0; i < nVertices; ++i)
  {
    pVertex = pWndData->pLevelData[pWndData->level_index].pVertexData + pVertices[i];

    point[i*3]   = (double)pVertex->x;
    point[i*3+1] = (double)pVertex->y;
    point[i*3+2] = (double)pVertex->z;
  }

  point[i*3] = point[0];
  point[i*3+1] = point[1];
  point[i*3+2] = point[2];

  for(i = 0; i < nVertices; ++i)
  {
    Object_DrawLine(hdc, pWndData, &point[i*3], 0, 0);
  }

} // Object_DrawPolygon


/*******************************************************************

    NAME:       Object_DrawPolygons

    SYNOPSIS:   Draw the polygons.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

********************************************************************/

VOID Object_DrawPolygons(HDC hdc, WND_DATA *pWndData)
{
  INT i, nVertices;
  WORD *pVertices;
  POLYGON huge *pPolygon;
  LEVEL_DATA *pLevel;
  HPEN hPenRed, hPenGreen, hPenOld;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  //
  //  Create the various pens we may need.
  //

  hPenRed = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
  hPenGreen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));

  hPenOld = SelectObject(hdc, hPenRed);

  pPolygon = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i, ++pPolygon)
  {
    if (pPolygon->flags & (PF_HIDDEN | PF_SELECTED))
    {
      continue;
    }

    nVertices = (pPolygon->flags & 7) + 3;
    pVertices = pPolygon->vertices;

    Object_DrawPolygon(hdc, pWndData, nVertices, pVertices);

    if (pPolygon->flags & PF_LINKED)
    {
      Object_DrawLinkArrow( hdc,
                            pWndData,
                            pPolygon,
                            pLevel->pPolygonData + pPolygon->polygon_link );
    }
  }

  SelectObject(hdc, hPenGreen);

  pPolygon = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i, ++pPolygon)
  {
    if (pPolygon->flags & PF_HIDDEN)
    {
      continue;
    }

    if (pPolygon->flags & PF_SELECTED)
    {
      nVertices = (pPolygon->flags & 7) + 3;
      pVertices = pPolygon->vertices;

      Object_DrawPolygon(hdc, pWndData, nVertices, pVertices);

      if (pPolygon->flags & PF_LINKED)
      {
        Object_DrawLinkArrow( hdc,
                              pWndData,
                              pPolygon,
                              pLevel->pPolygonData + pPolygon->polygon_link );
      }
    }
  }

  SelectObject(hdc, hPenOld);
  DeleteObject(hPenGreen);
  DeleteObject(hPenRed);

  return;

} // Object_DrawPolygons


/*******************************************************************

    NAME:       Object_CalcCursorPosition

    SYNOPSIS:   Calculate the cusor absolute and relative positions
                for the view window pixel coordinates given.

    ENTRY:      pWndData - The window data pointer.

                x - View window x position.

                y - View window y position.

********************************************************************/

VOID Object_CalcCursorPosition(WND_DATA *pWndData, INT x, INT y)
{
  double xp1, yp1, zp1, xp2, yp2, zp2, xp3, yp3, zp3;
  double r, s, t, a, b, c, d;
  double sin_gtheta, cos_gtheta, sin_gphi, cos_gphi;
  double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
  double x_adj, y_adj, x_scale, y_scale;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  x_adj = (double)(pWndData->bm_width / 2);
  y_adj = (double)(pWndData->bm_height / 2);

  x_scale = 500.0; // (double)xLen;
  y_scale = 500.0; // (double)yLen;

  sin_vtheta = sin(-pLevel->view_rotation);
  cos_vtheta = cos(-pLevel->view_rotation);
  sin_vphi   = sin(-pLevel->view_elevation);
  cos_vphi   = cos(-pLevel->view_elevation);

  sin_gtheta = sin(pLevel->grid_rotation);
  cos_gtheta = cos(pLevel->grid_rotation);
  sin_gphi   = sin(pLevel->grid_elevation);
  cos_gphi   = cos(pLevel->grid_elevation);

  //
  //  Calculate the grid plane coefficients in untranslated view space.
  //

  xp1 = (double)pLevel->grid_x;
  yp1 = (double)pLevel->grid_y;
  zp1 = (double)pLevel->grid_z;

  r = xp1 - pLevel->view_x;
  s = yp1 - pLevel->view_y;
  t = zp1 - pLevel->view_z;

  xp1 = (r * cos_vtheta - s * sin_vtheta) * cos_vphi + t * sin_vphi;
  yp1 = (r * sin_vtheta + s * cos_vtheta);
  zp1 = (s * sin_vtheta - r * cos_vtheta) * sin_vphi + t * cos_vphi;

  r = -1000.0;
  s = 1000.0;

  xp2 = (double)pLevel->grid_x + r * cos_gphi * cos_gtheta - s * sin_gtheta;
  yp2 = (double)pLevel->grid_y + r * cos_gphi * sin_gtheta + s * cos_gtheta;
  zp2 = (double)pLevel->grid_z - r * sin_gphi;

  r = xp2 - pLevel->view_x;
  s = yp2 - pLevel->view_y;
  t = zp2 - pLevel->view_z;

  xp2 = (r * cos_vtheta - s * sin_vtheta) * cos_vphi + t * sin_vphi;
  yp2 = (r * sin_vtheta + s * cos_vtheta);
  zp2 = (s * sin_vtheta - r * cos_vtheta) * sin_vphi + t * cos_vphi;

  r = 1000.0;
  s = 1000.0;

  xp3 = (double)pLevel->grid_x + r * cos_gphi * cos_gtheta - s * sin_gtheta;
  yp3 = (double)pLevel->grid_y + r * cos_gphi * sin_gtheta + s * cos_gtheta;
  zp3 = (double)pLevel->grid_z - r * sin_gphi;

  r = xp3 - pLevel->view_x;
  s = yp3 - pLevel->view_y;
  t = zp3 - pLevel->view_z;

  xp3 = (r * cos_vtheta - s * sin_vtheta) * cos_vphi + t * sin_vphi;
  yp3 = (r * sin_vtheta + s * cos_vtheta);
  zp3 = (s * sin_vtheta - r * cos_vtheta) * sin_vphi + t * cos_vphi;

  a =  yp1 * (zp3 - zp2) - zp1 * (yp3 - yp2) + yp3 * zp2 - yp2 * zp3;
  b = -xp1 * (zp3 - zp2) + zp1 * (xp3 - xp2) - xp3 * zp2 + xp2 * zp3;
  c =  xp1 * (yp3 - yp2) - yp1 * (xp3 - xp2) + xp3 * yp2 - xp2 * yp3;
  d = -(a * xp1 + b * yp1 + c * zp1);

  //
  //  Calculate the grid intersection point in untranslated view space.
  //

  r = ( x_adj - (double)x ) / x_scale;
  s = ( y_adj - (double)y ) / y_scale;
  t = a + b * r + c * s;

  if ( fabs(t) < 0.000001 )
  {
    xp1 = 0.0;
  }
  else
  {
    xp1 = -d / t;
  }

  yp1 = xp1 * r;
  zp1 = xp1 * s;

  //
  //  Convert from untranslated view space to untranslated grid space.
  //

  sin_vtheta = sin(pLevel->view_rotation);
  cos_vtheta = cos(pLevel->view_rotation);
  sin_vphi   = sin(pLevel->view_elevation);
  cos_vphi   = cos(pLevel->view_elevation);

  sin_gtheta = sin(-pLevel->grid_rotation);
  cos_gtheta = cos(-pLevel->grid_rotation);
  sin_gphi   = sin(-pLevel->grid_elevation);
  cos_gphi   = cos(-pLevel->grid_elevation);

  r = xp1;
  s = yp1;
  t = zp1;

  xp1 = pLevel->view_x + (r * cos_vphi + t * sin_vphi) * cos_vtheta - s * sin_vtheta;
  yp1 = pLevel->view_y + (r * cos_vphi + t * sin_vphi) * sin_vtheta + s * cos_vtheta;
  zp1 = pLevel->view_z + (t * cos_vphi - r * sin_vphi);

  r = xp1 - pLevel->grid_x;
  s = yp1 - pLevel->grid_y;
  t = zp1 - pLevel->grid_z;

  xp1 = (r * cos_gtheta - s * sin_gtheta) * cos_gphi + t * sin_gphi;
  yp1 = (r * sin_gtheta + s * cos_gtheta);
  zp1 = (s * sin_gtheta - r * cos_gtheta) * sin_gphi + t * cos_gphi;

  // ... of course zp1 must be zero!

  if (pLevel->bSnapToGrid)
  {
    r = (double)pLevel->grid_spacing;
    s = 1.0 / r;

    xp1 = r * Round(xp1 * s);
    yp1 = r * Round(yp1 * s);
  }
  else
  {
    xp1 = Round(xp1);
    yp1 = Round(yp1);
  }

  if (xp1 > 999999.0)
  {
    xp1 = 999999.0;
  }
  else if (xp1 < -999999.0)
  {
    xp1 = -999999.0;
  }

  if (yp1 > 999999.0)
  {
    yp1 = 999999.0;
  }
  else if (yp1 < -999999.0)
  {
    yp1 = -999999.0;
  }

  pWndData->cursor_rel_x = (long)xp1;
  pWndData->cursor_rel_y = (long)yp1;
  pWndData->cursor_rel_z = 0L;

  //
  //  Convert from grid space to real space.
  //

  sin_gtheta = sin(pLevel->grid_rotation);
  cos_gtheta = cos(pLevel->grid_rotation);
  sin_gphi   = sin(pLevel->grid_elevation);
  cos_gphi   = cos(pLevel->grid_elevation);

  r = xp1;
  s = yp1;

  xp1 = (double)pLevel->grid_x + r * cos_gphi * cos_gtheta - s * sin_gtheta;
  yp1 = (double)pLevel->grid_y + r * cos_gphi * sin_gtheta + s * cos_gtheta;
  zp1 = (double)pLevel->grid_z - r * sin_gphi;

  pWndData->cursor_abs_x = (long)Round(xp1);
  pWndData->cursor_abs_y = (long)Round(yp1);
  pWndData->cursor_abs_z = (long)Round(zp1);
  
  return;
  
} // Object_CalcCursorPosition


/*******************************************************************

    NAME:       Object_DrawCursorLines

    SYNOPSIS:   Draw cursor lines in the drawing plane grid for
                the view window pixel coordinates given.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

                x - View window x position.

                y - View window y position.

********************************************************************/

VOID Object_DrawCursorLines(HDC hdc, WND_DATA *pWndData, INT x, INT y)
{
  HPEN hPenViolet, hPenOld;
  double r, s, line[6];
  double sin_gtheta, cos_gtheta, sin_gphi, cos_gphi;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  hPenViolet = CreatePen(PS_SOLID, 1, RGB(255, 0, 255));
  hPenOld = SelectObject(hdc, GetStockObject(BLACK_PEN));

  SelectObject(hdc, hPenViolet);

  Object_CalcCursorPosition(pWndData, x, y);

  //
  //  Calculate the coodinates of the cursor lines.
  //

  sin_gtheta = sin(pLevel->grid_rotation);
  cos_gtheta = cos(pLevel->grid_rotation);
  sin_gphi   = sin(pLevel->grid_elevation);
  cos_gphi   = cos(pLevel->grid_elevation);

  r = pWndData->cursor_rel_x - (double)pLevel->grid_size;
  s = pWndData->cursor_rel_y;

  line[0] = (double)pLevel->grid_x + r * cos_gphi * cos_gtheta - s * sin_gtheta;
  line[1] = (double)pLevel->grid_y + r * cos_gphi * sin_gtheta + s * cos_gtheta;
  line[2] = (double)pLevel->grid_z - r * sin_gphi;

  r = pWndData->cursor_rel_x + (double)pLevel->grid_size;
  s = pWndData->cursor_rel_y;

  line[3] = (double)pLevel->grid_x + r * cos_gphi * cos_gtheta - s * sin_gtheta;
  line[4] = (double)pLevel->grid_y + r * cos_gphi * sin_gtheta + s * cos_gtheta;
  line[5] = (double)pLevel->grid_z - r * sin_gphi;

  Object_DrawLine(hdc, pWndData, line, 0, 0);

  r = pWndData->cursor_rel_x;
  s = pWndData->cursor_rel_y - (double)pLevel->grid_size;

  line[0] = (double)pLevel->grid_x + r * cos_gphi * cos_gtheta - s * sin_gtheta;
  line[1] = (double)pLevel->grid_y + r * cos_gphi * sin_gtheta + s * cos_gtheta;
  line[2] = (double)pLevel->grid_z - r * sin_gphi;

  r = pWndData->cursor_rel_x;
  s = pWndData->cursor_rel_y + (double)pLevel->grid_size;

  line[3] = (double)pLevel->grid_x + r * cos_gphi * cos_gtheta - s * sin_gtheta;
  line[4] = (double)pLevel->grid_y + r * cos_gphi * sin_gtheta + s * cos_gtheta;
  line[5] = (double)pLevel->grid_z - r * sin_gphi;

  Object_DrawLine(hdc, pWndData, line, 0, 0);

  //
  //  Cleanup.
  //

  SelectObject(hdc, hPenOld);
  DeleteObject(hPenViolet);
  
  return;
  
} // Object_DrawCursorLines


/*******************************************************************

    NAME:       Object_DrawSelectedVertex

    SYNOPSIS:   Draw the closest selected vertex for the view
                window pixel coordinates given.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

                x - View window x position.

                y - View window y position.

********************************************************************/

VOID Object_DrawSelectedVertex(HDC hdc, WND_DATA *pWndData, INT x, INT y)
{
  INT i;
  HPEN hPenOld;
  HBRUSH hBrushOld;
  VERTEX huge *pVertex;
  LEVEL_DATA *pLevel;
  LONG xl, yl, zl;
  double xp1, yp1, zp1, xp2, yp2, zp2, xp, yp, zp;
  double r, s, t, a1, a2, a3, b1, b2, b3, d;
  double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
  double x_adj, y_adj, x_scale, y_scale;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  hPenOld = SelectObject(hdc, GetStockObject(WHITE_PEN));
  hBrushOld = SelectObject(hdc, GetStockObject(NULL_BRUSH));

  x_adj = (double)(pWndData->bm_width / 2);
  y_adj = (double)(pWndData->bm_height / 2);

  x_scale = 500.0; // (double)xLen;
  y_scale = 500.0; // (double)yLen;

  //
  //  Calculate the line-of-sight endpoints in untranslated view space.
  //

  r = (x_adj - (double)x) / x_scale;
  s = (y_adj - (double)y) / y_scale;

  xp1 = (double)pLevel->view_depth;
  yp1 = xp1 * r;
  zp1 = xp1 * s;

  xp2 = (double)pLevel->view_depth * 2.0;
  yp2 = xp2 * r;
  zp2 = xp2 * s;

  //
  //  Convert from untranslated view space to real space.
  //

  sin_vtheta = sin(pLevel->view_rotation);
  cos_vtheta = cos(pLevel->view_rotation);
  sin_vphi   = sin(pLevel->view_elevation);
  cos_vphi   = cos(pLevel->view_elevation);

  r = xp1;
  s = yp1;
  t = zp1;

  xp1 = pLevel->view_x + (r * cos_vphi + t * sin_vphi) * cos_vtheta - s * sin_vtheta;
  yp1 = pLevel->view_y + (r * cos_vphi + t * sin_vphi) * sin_vtheta + s * cos_vtheta;
  zp1 = pLevel->view_z + (t * cos_vphi - r * sin_vphi);

  r = xp2;
  s = yp2;
  t = zp2;

  xp2 = pLevel->view_x + (r * cos_vphi + t * sin_vphi) * cos_vtheta - s * sin_vtheta;
  yp2 = pLevel->view_y + (r * cos_vphi + t * sin_vphi) * sin_vtheta + s * cos_vtheta;
  zp2 = pLevel->view_z + (t * cos_vphi - r * sin_vphi);

  //
  //  Find the closest vertex.
  //

  t = 0.0;
  pWndData->bSelVertex = FALSE;

  pVertex = pLevel->pVertexData;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if ((pVertex->flags & VF_HIDDEN) == 0)
    {
      //
      //  Make sure the vertex is not behind the view plane.
      //

      xp = (double)pVertex->x;
      yp = (double)pVertex->y;
      zp = (double)pVertex->z;

      b1 = xp2 - xp1;
      b2 = yp2 - yp1;
      b3 = zp2 - zp1;

      d = -(b1 * xp1 + b2 * yp1 + b3 * zp1);
      r = b1 * xp2 + b2 * yp2 + b3 * zp2 + d;
      s = b1 * xp + b2 * yp + b3 * zp + d;

      if (r * s >= 0.0)
      {
        //
        //  Compute the distance from the vertex to the line.
        //

        a1 = xp - xp1;
        a2 = yp - yp1;
        a3 = zp - zp1;

        r = a1 * b1 + a2 * b2 + a3 * b3;
        s = a1 * a1 + a2 * a2 + a3 * a3 - (r * r) / (b1 * b1 + b2 * b2 + b3 * b3);

        if (!pWndData->bSelVertex || s < t)
        {
          pWndData->bSelVertex = TRUE;
          pWndData->nSelVertex = i;
          t = s;
          xl = (LONG)Round(xp);
          yl = (LONG)Round(yp);
          zl = (LONG)Round(zp);
        }
      }
    }

    ++pVertex;
  }

  if (pWndData->bSelVertex)
  {
    pVertex = pLevel->pVertexData + pWndData->nSelVertex;

    if (pVertex->flags & VF_LIFEFORM)
    {
      i = (pVertex->orientation & 7) + 1;
    }
    else
    {
      i = 0;
    }

    Object_DrawVertex(hdc, pWndData, xl, yl, zl, 0, i);

    pWndData->cursor_abs_x = xl;
    pWndData->cursor_abs_y = yl;
    pWndData->cursor_abs_z = zl;
  }

  //
  //  Cleanup.
  //

  SelectObject(hdc, hBrushOld);
  SelectObject(hdc, hPenOld);
  
  return;
  
} // Object_DrawSelectedVertex


/*******************************************************************

    NAME:       Object_DrawSelectedPolygon

    SYNOPSIS:   Draw the closest selected polygon for the view
                window pixel coordinates given.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

                x - View window x position.

                y - View window y position.

********************************************************************/

VOID Object_DrawSelectedPolygon(HDC hdc, WND_DATA *pWndData, INT x, INT y)
{
  INT i, j, k, n, nv;
  WORD *pv;
  HPEN hPenOld;
  POLYGON huge *pPolygon;
  LEVEL_DATA *pLevel;
  double xp1, yp1, zp1, xp2, yp2, zp2, xp[3], yp[3], zp[3];
  double r, s, t, a, b, c, d, min_t, xi, yi, zi;
  double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
  double x_adj, y_adj, x_scale, y_scale;
  double qx, qy, qz, rx, ry, rz, px, py, pz;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  hPenOld = SelectObject(hdc, GetStockObject(WHITE_PEN));

  x_adj = (double)(pWndData->bm_width / 2);
  y_adj = (double)(pWndData->bm_height / 2);

  x_scale = 500.0; // (double)xLen;
  y_scale = 500.0; // (double)yLen;

  //
  //  Calculate the line-of-sight endpoints in untranslated view space.
  //

  r = (x_adj - (double)x) / x_scale;
  s = (y_adj - (double)y) / y_scale;

  xp1 = (double)pLevel->view_depth;
  yp1 = xp1 * r;
  zp1 = xp1 * s;

  xp2 = (double)pLevel->view_depth * 2.0;
  yp2 = xp2 * r;
  zp2 = xp2 * s;

  //
  //  Convert from untranslated view space to real space.
  //

  sin_vtheta = sin(pLevel->view_rotation);
  cos_vtheta = cos(pLevel->view_rotation);
  sin_vphi   = sin(pLevel->view_elevation);
  cos_vphi   = cos(pLevel->view_elevation);

  r = xp1;
  s = yp1;
  t = zp1;

  xp1 = pLevel->view_x + (r * cos_vphi + t * sin_vphi) * cos_vtheta - s * sin_vtheta;
  yp1 = pLevel->view_y + (r * cos_vphi + t * sin_vphi) * sin_vtheta + s * cos_vtheta;
  zp1 = pLevel->view_z + (t * cos_vphi - r * sin_vphi);

  r = xp2;
  s = yp2;
  t = zp2;

  xp2 = pLevel->view_x + (r * cos_vphi + t * sin_vphi) * cos_vtheta - s * sin_vtheta;
  yp2 = pLevel->view_y + (r * cos_vphi + t * sin_vphi) * sin_vtheta + s * cos_vtheta;
  zp2 = pLevel->view_z + (t * cos_vphi - r * sin_vphi);

  //
  //  Find the closest polygon.
  //

  pWndData->bSelPolygon = FALSE;
  min_t = 0.0;

  pPolygon = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if ((pPolygon->flags & VF_HIDDEN) == 0)
    {
      for(j = 0; j < 3; ++j)
      {
        xp[j] = (double)((pLevel->pVertexData + pPolygon->vertices[j])->x);
        yp[j] = (double)((pLevel->pVertexData + pPolygon->vertices[j])->y);
        zp[j] = (double)((pLevel->pVertexData + pPolygon->vertices[j])->z);
      }

      a =  yp[0] * (zp[2] - zp[1]) - zp[0] * (yp[2] - yp[1]) + yp[2] * zp[1] - yp[1] * zp[2];
      b = -xp[0] * (zp[2] - zp[1]) + zp[0] * (xp[2] - xp[1]) - xp[2] * zp[1] + xp[1] * zp[2];
      c =  xp[0] * (yp[2] - yp[1]) - yp[0] * (xp[2] - xp[1]) + xp[2] * yp[1] - xp[1] * yp[2];
      d = -(a * xp[0] + b * yp[0] + c * zp[0]);

      r = a * xp1 + b * yp1 + c * zp1 + d;
      s = a * (xp1 - xp2) + b * (yp1 - yp2) + c * (zp1 - zp2);

      if (s != 0.0)
      {
        t = r / s;

        if (t > 0.0)                    // if not behind the view plane
        {
          xi = xp1 + t * (xp2 - xp1);
          yi = yp1 + t * (yp2 - yp1);
          zi = zp1 + t * (zp2 - zp1);

          if (PointInPolygon(pPolygon, pLevel->pVertexData, xi, yi, zi))
          {
            if (!pWndData->bSelPolygon || t < min_t)
            {
              pWndData->bSelPolygon = TRUE;
              pWndData->nSelPolygon = i;
              min_t = t;
              nv = (pPolygon->flags & 7) + 3;
              pv = pPolygon->vertices;
            }
          }
        }
      }
    }

    ++pPolygon;
  }

  //
  //  If linking polygons guard against infinite linked loops.
  //

  if (nMode == MODE_LINK_POLYGON)
  {
    if (pWndData->bSelPolygon && pWndData->bLnkPolygon)
    {
      pPolygon = pLevel->pPolygonData + pWndData->nSelPolygon;

      while(pPolygon->flags & PF_LINKED)
      {
        if (pPolygon->polygon_link == pWndData->nLnkPolygon)
        {
          pWndData->bSelPolygon = FALSE;
          break;
        }

        pPolygon = pLevel->pPolygonData + pPolygon->polygon_link;
      }
    }
  }

  //
  //  If linking polygons, the polygons must have a common edge.
  //

  if (nMode == MODE_LINK_POLYGON)
  {
    if (pWndData->bSelPolygon && pWndData->bLnkPolygon)
    {
      pWndData->bSelPolygon = FALSE;

      pPolygon = pLevel->pPolygonData + pWndData->nLnkPolygon;
      n = (pPolygon->flags & 7) + 3;

      for(i = 0; i < n; ++i)
      {
        qx = (double)((pLevel->pVertexData + pPolygon->vertices[i])->x);
        qy = (double)((pLevel->pVertexData + pPolygon->vertices[i])->y);
        qz = (double)((pLevel->pVertexData + pPolygon->vertices[i])->z);

        j = (i + 1) % n;

        rx = (double)((pLevel->pVertexData + pPolygon->vertices[j])->x);
        ry = (double)((pLevel->pVertexData + pPolygon->vertices[j])->y);
        rz = (double)((pLevel->pVertexData + pPolygon->vertices[j])->z);

        for(j = 0; j < nv; ++j)
        {
          px = (double)((pLevel->pVertexData + pv[j])->x);
          py = (double)((pLevel->pVertexData + pv[j])->y);
          pz = (double)((pLevel->pVertexData + pv[j])->z);

          a = (px - qx) * (px - qx) + (py - qy) * (py - qy) + (pz - qz) * (pz - qz);
          b = (rx - qx) * (rx - qx) + (ry - qy) * (ry - qy) + (rz - qz) * (rz - qz);
          r = (px - qx) * (rx - qx) + (py - qy) * (ry - qy) + (pz - qz) * (rz - qz);

          if (fabs(a - r * r / b) > 1.0)
          {
            continue;   
          }

          k = (j + 1) % nv;

          px = (double)((pLevel->pVertexData + pv[k])->x);
          py = (double)((pLevel->pVertexData + pv[k])->y);
          pz = (double)((pLevel->pVertexData + pv[k])->z);

          a = (px - qx) * (px - qx) + (py - qy) * (py - qy) + (pz - qz) * (pz - qz);
          b = (rx - qx) * (rx - qx) + (ry - qy) * (ry - qy) + (rz - qz) * (rz - qz);
          r = (px - qx) * (rx - qx) + (py - qy) * (ry - qy) + (pz - qz) * (rz - qz);

          if (fabs(a - r * r / b) < 1.0)
          {
            pWndData->bSelPolygon = TRUE;
            break;
          }
        }

        if (pWndData->bSelPolygon)
        {
          break;
        }
      }
    }
  }

  //
  //  If linking polygons, show the link polygon.
  //

  if (nMode == MODE_LINK_POLYGON && pWndData->bLnkPolygon)
  {
    pPolygon = pLevel->pPolygonData + pWndData->nLnkPolygon;
    n = (pPolygon->flags & 7) + 3;

    Object_DrawPolygon(hdc, pWndData, n, pPolygon->vertices);

    if (pWndData->bSelPolygon && pWndData->nLnkPolygon != pWndData->nSelPolygon)
    {
      Object_DrawLinkArrow( hdc,
                            pWndData,
                            pPolygon,
                            pLevel->pPolygonData + pWndData->nSelPolygon );
    }
  }

  //
  //  If the cursor is over an eligible polygon, hilite the polygon.
  //

  if (pWndData->bSelPolygon)
  {
    Object_DrawPolygon(hdc, pWndData, nv, pv);

    //
    //  If rotating or flipping the polygon, show the vertex order.
    //

    if (nMode == MODE_ROTATE_POLYGON || nMode == MODE_FLIP_POLYGON)
    {
      HFONT hFontArial, hFontOld;
      HBRUSH hBrushOld;

      hFontArial = CreateFont(-12,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
      SetBkMode(hdc, TRANSPARENT);
      hFontOld = SelectObject(hdc, hFontArial);
      hBrushOld = SelectObject(hdc, GetStockObject(WHITE_BRUSH));

      pPolygon = pLevel->pPolygonData + pWndData->nSelPolygon;
      n = (pPolygon->flags & 7) + 3;

      for(i = 0; i < n; ++i)
      {
        qx = (double)((pLevel->pVertexData + pPolygon->vertices[i])->x);
        qy = (double)((pLevel->pVertexData + pPolygon->vertices[i])->y);
        qz = (double)((pLevel->pVertexData + pPolygon->vertices[i])->z);

        Object_DrawVertex( hdc,
                           pWndData,
                           (LONG)Round(qx),
                           (LONG)Round(qy),
                           (LONG)Round(qz),
                           i + 1, 0 );
      }

      SelectObject(hdc, hBrushOld);
      SelectObject(hdc, hFontOld);
      DeleteObject(hFontArial);
    }
  }

  //
  //  Cleanup.
  //

  SelectObject(hdc, hPenOld);
  
  return;
  
} // Object_DrawSelectedPolygon


/*******************************************************************

    NAME:       Object_IsVertexIncluded

    SYNOPSIS:   Determine if the vertex at the offset given is
                included within the selection box.

    ENTRY:      pWndData - The window data pointer.

                nIndex - Index of the vertex.

                pt - If not NULL receives the vertex pixel coords

    EXIT:       TRUE if included, FALSE if not.

********************************************************************/

BOOL Object_IsVertexIncluded(WND_DATA *pWndData, INT nIndex, POINT *pt)
{
  VERTEX huge *pVertex;
  POINT tmp_pt;
  double r, s, t, xp, yp, zp;
  double xMax, yMax, x_scale, y_scale, x_adj, y_adj;
  double sin_vtheta, cos_vtheta, sin_vphi, cos_vphi;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pVertex = pWndData->pLevelData[pWndData->level_index].pVertexData + nIndex;

  if (pVertex->flags & VF_HIDDEN)
  {
    return FALSE;
  }

  x_adj = (double)(pWndData->bm_width / 2);
  y_adj = (double)(pWndData->bm_height / 2);

  x_scale = 500.0; // (double)xLen;
  y_scale = 500.0; // (double)yLen;

  xMax = (double)pWndData->bm_width - 0.000001;
  yMax = (double)pWndData->bm_height - 0.000001;

  sin_vtheta = sin(-pLevel->view_rotation);
  cos_vtheta = cos(-pLevel->view_rotation);
  sin_vphi   = sin(-pLevel->view_elevation);
  cos_vphi   = cos(-pLevel->view_elevation);

  xp = (double)pVertex->x;
  yp = (double)pVertex->y;
  zp = (double)pVertex->z;

  xp -= (double)pLevel->view_x;
  yp -= (double)pLevel->view_y;
  zp -= (double)pLevel->view_z;

  //
  //  Translate the point to the current view.
  //

  r = (xp*cos_vtheta-yp*sin_vtheta)*cos_vphi+zp*sin_vphi;
  s = xp*sin_vtheta+yp*cos_vtheta;
  t = zp*cos_vphi-(xp*cos_vtheta-yp*sin_vtheta)*sin_vphi;

  //
  //  Skip it if the point is behind the view plane.
  //

  if (r < (double)pLevel->view_depth)
  {
    return FALSE;
  }

  r = 1 / r;
  xp = x_adj - x_scale * s * r;
  yp = y_adj - y_scale * t * r;

  //
  //  If the point is outside the selection box then skip it.
  //

  if (xp < 0.0 || yp < 0.0 || xp > xMax || yp > yMax)
  {
    return FALSE;
  }

  tmp_pt.x = (INT)xp;
  tmp_pt.y = (INT)yp;

  if (!PtInRect(&pWndData->sbox, tmp_pt))
  {
    return FALSE;
  }

  if (pt)
  {
    pt->x = tmp_pt.x;
    pt->y = tmp_pt.y;
  }

  return TRUE;
  
} // Object_IsVertexIncluded


/*******************************************************************

    NAME:       Object_DrawSelectedObjects

    SYNOPSIS:   Draw any vertices within the selection box. If hdc
                is NULL then select any vertices within the selection
                box and any polygons using those vertices.

    ENTRY:      hdc - Device context (or NULL if selecting).

                pWndData - The window data pointer.

********************************************************************/

VOID Object_DrawSelectedObjects(HDC hdc, WND_DATA *pWndData)
{
  INT i, j, n;
  POLYGON huge *pPolygon;
  LEVEL_DATA *pLevel;
  HPEN hPenOld;
  HBRUSH hBrushOld;
  POINT pt;

  pLevel = pWndData->pLevelData + pWndData->level_index;

  if (hdc)
  {
    hPenOld = SelectObject(hdc, GetStockObject(WHITE_PEN));
    hBrushOld = SelectObject(hdc, GetStockObject(NULL_BRUSH));
  }

  //
  //  Select each polygon which has a vertex in the selection box.
  //

  if (hdc == NULL)
  {
    pPolygon = pLevel->pPolygonData;

    for(i = 0; i < pLevel->nPolygons; ++i)
    {
      if ((pPolygon->flags & PF_HIDDEN) == 0)
      {
        n = (pPolygon->flags & 7) + 3;

        for(j = 0; j < n; ++j)
        {
          if (Object_IsVertexIncluded(pWndData, pPolygon->vertices[j], NULL))
          {
            pPolygon->flags |= PF_SELECTED;
            break;
          }
        }
      }

      ++pPolygon;
    }
  }

  //
  //  Draw or select each vertex in the selection box.
  //

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (Object_IsVertexIncluded(pWndData, i, &pt))
    {
      if (hdc)
      {
        Ellipse(hdc, pt.x - 4, pt.y - 4, pt.x + 4, pt.y + 4);
      }
      else
      {
        (pLevel->pVertexData + i)->flags |= VF_SELECTED;
      }
    }
  }

  if (hdc)
  {
    //
    //  Cleanup.
    //

    SelectObject(hdc, hBrushOld);
    SelectObject(hdc, hPenOld);
  }

  return;
  
} // Object_DrawSelectedObjects


/*******************************************************************

    NAME:       Object_DrawLinkArrow

    SYNOPSIS:   Draw the link arrow for the linked polygon given.
                The arrow lies in the plane of the linked polygon
                and points from the linked polygon to the link
                polygon.

    ENTRY:      hdc - Device context.

                pWndData - The window data pointer.

                pLinked - Pointer to the linked polygon.

                pLink - Pointer to the link polygon.

********************************************************************/

VOID Object_DrawLinkArrow(HDC hdc, WND_DATA *pWndData, POLYGON *pLinked, POLYGON *pLink)
{
  INT i, j, n1, n2;
  WORD *p1, *p2;
  LEVEL_DATA *pLevel;
  VERTEX v1, v2, v3, v4, *pv1, *pv2, huge *pVertex;
  double a, b, r, a1, a2, a3, b1, b2, b3, c1, c2, c3, line[6];
  double d1, d2, d3, d4, d5, d6, dx, dy, dz;

  //
  //  Find common vertices between the polygons.
  //

  pLevel = pWndData->pLevelData + pWndData->level_index;

  n1 = (pLinked->flags & 7) + 3;
  p1 = pLinked->vertices;

  n2 = (pLink->flags & 7) + 3;
  p2 = pLink->vertices;

  for(i = 0; i < n1; ++i)
  {
    pVertex = pLevel->pVertexData + p1[i];

    v1.x = pVertex->x;
    v1.y = pVertex->y;
    v1.z = pVertex->z;

    pVertex = pLevel->pVertexData + p1[(i+1)%n1];

    v2.x = pVertex->x;
    v2.y = pVertex->y;
    v2.z = pVertex->z;

    for(j = 0; j < n2; ++j)
    {
      pVertex = pLevel->pVertexData + p2[j];

      v3.x = pVertex->x;
      v3.y = pVertex->y;
      v3.z = pVertex->z;

      a = (v3.x - v1.x) * (v3.x - v1.x) + (v3.y - v1.y) * (v3.y - v1.y) + (v3.z - v1.z) * (v3.z - v1.z);
      b = (v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z);
      r = (v3.x - v1.x) * (v2.x - v1.x) + (v3.y - v1.y) * (v2.y - v1.y) + (v3.z - v1.z) * (v2.z - v1.z);

      if (fabs(a - r * r / b) > 1.0)
      {
        continue;   
      }

      pVertex = pLevel->pVertexData + p2[(j+1)%n2];

      v4.x = pVertex->x;
      v4.y = pVertex->y;
      v4.z = pVertex->z;

      a = (v4.x - v1.x) * (v4.x - v1.x) + (v4.y - v1.y) * (v4.y - v1.y) + (v4.z - v1.z) * (v4.z - v1.z);
      b = (v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z);
      r = (v4.x - v1.x) * (v2.x - v1.x) + (v4.y - v1.y) * (v2.y - v1.y) + (v4.z - v1.z) * (v2.z - v1.z);

      if (fabs(a - r * r / b) > 1.0)
      {
        continue;
      }

      dx = v1.x - v3.x;
      dy = v1.y - v3.y;
      dz = v1.z - v3.z;

      d1 = dx * dx + dy * dy + dz * dz;

      dx = v1.x - v4.x;
      dy = v1.y - v4.y;
      dz = v1.z - v4.z;

      d2 = dx * dx + dy * dy + dz * dz;

      dx = v2.x - v3.x;
      dy = v2.y - v3.y;
      dz = v2.z - v3.z;

      d3 = dx * dx + dy * dy + dz * dz;

      dx = v2.x - v4.x;
      dy = v2.y - v4.y;
      dz = v2.z - v4.z;

      d4 = dx * dx + dy * dy + dz * dz;

      dx = v1.x - v2.x;
      dy = v1.y - v2.y;
      dz = v1.z - v2.z;

      d5 = dx * dx + dy * dy + dz * dz;

      dx = v3.x - v4.x;
      dy = v3.y - v4.y;
      dz = v3.z - v4.z;

      d6 = dx * dx + dy * dy + dz * dz;

      if (d1 >= d2 && d1 >= d3 && d1 >= d4 && d1 >= d5 && d1 >= d6)
      {
        pv1 = &v2;
        pv2 = &v4;
      }
      else if (d2 >= d1 && d2 >= d3 && d2 >= d4 && d2 >= d5 && d2 >= d6)
      {
        pv1 = &v2;
        pv2 = &v3;
      }
      else if (d3 >= d1 && d3 >= d2 && d3 >= d4 && d3 >= d5 && d3 >= d6)
      {
        pv1 = &v1;
        pv2 = &v4;
      }
      else if (d4 >= d1 && d4 >= d2 && d4 >= d3 && d4 >= d5 && d4 >= d6)
      {
        pv1 = &v1;
        pv2 = &v3;
      }
      else if (d5 >= d1 && d5 >= d2 && d5 >= d3 && d5 >= d4 && d5 >= d6)
      {
        pv1 = &v3;
        pv2 = &v4;
      }
      else
      {
        pv1 = &v1;
        pv2 = &v2;
      }

      dx = pv2->x - pv1->x;
      dy = pv2->y - pv1->y;
      dz = pv2->z - pv1->z;

      d1 = sqrt(dx * dx + dy * dy + dz * dz);

      if (d1 < 1.0)
      {
        continue;
      }

      line[0] = (pv1->x + pv2->x) / 2;
      line[1] = (pv1->y + pv2->y) / 2;
      line[2] = (pv1->z + pv2->z) / 2;

      dx = line[0] - v1.x;
      dy = line[1] - v1.y;
      dz = line[2] - v1.z;

      d1 = sqrt(dx * dx + dy * dy + dz * dz);

      dx = line[0] - v2.x;
      dy = line[1] - v2.y;
      dz = line[2] - v2.z;

      d2 = sqrt(dx * dx + dy * dy + dz * dz);

      dx = v1.x - v2.x;
      dy = v1.y - v2.y;
      dz = v1.z - v2.z;

      d3 = sqrt(dx * dx + dy * dy + dz * dz);

      if (d1 + d2 < d3 + 1.0)
      {
        break;
      }
    }

    if (j < n2)
    {
      break;
    }
  }

  //
  //  If the common vertices were not found then forget it.
  //

  if (i == n1)
  {
    return;
  }

  //
  //  Get a third vertex for the linked polygon.
  //

  pVertex = pLevel->pVertexData + p1[(i+2)%n1];

  v3.x = pVertex->x;
  v3.y = pVertex->y;
  v3.z = pVertex->z;

  //
  //  Calculate the normal vector for the linked polygon.
  //

  a1 = v1.y * (v3.z - v2.z) - v1.z * (v3.y - v2.y) + v3.y * v2.z - v2.y * v3.z;
  a2 = v1.z * (v3.x - v2.x) - v1.x * (v3.z - v2.z) - v3.x * v2.z + v2.x * v3.z;
  a3 = v1.x * (v3.y - v2.y) - v1.y * (v3.x - v2.x) + v3.x * v2.y - v2.x * v3.y;

  //
  //  Calculate the arrow vector.
  //

  b1 = v2.x - v1.x;
  b2 = v2.y - v1.y;
  b3 = v2.z - v1.z;

  r = sqrt(b1 * b1 + b2 * b2 + b3 * b3);

  b1 = b1 / r;
  b2 = b2 / r;
  b3 = b3 / r;

  c1 = a2 * b3 - a3 * b2;
  c2 = a3 * b1 - a1 * b3;
  c3 = a1 * b2 - a2 * b1;

  r = sqrt(c1 * c1 + c2 * c2 + c3 * c3);

  c1 = -c1 / r;
  c2 = -c2 / r;
  c3 = -c3 / r;

  line[3] = line[0] + c1 * 250;
  line[4] = line[1] + c2 * 250;
  line[5] = line[2] + c3 * 250;

  Object_DrawLine(hdc, pWndData, line, 0, 0);

  line[3] = line[0] + c1 * 100 + b1 * 100;
  line[4] = line[1] + c2 * 100 + b2 * 100;
  line[5] = line[2] + c3 * 100 + b3 * 100;

  Object_DrawLine(hdc, pWndData, line, 0, 0);

  line[3] = line[0] + c1 * 100 - b1 * 100;
  line[4] = line[1] + c2 * 100 - b2 * 100;
  line[5] = line[2] + c3 * 100 - b3 * 100;

  Object_DrawLine(hdc, pWndData, line, 0, 0);

  return;
  
} // Object_DrawLinkArrow


/*******************************************************************

    NAME:       Object_RegenerateView

    SYNOPSIS:   Regenerate the view bitmap.

    ENTRY:      cx - client area width.

                cy - client area height.

                pWndData - Object window data pointer.

********************************************************************/

VOID Object_RegenerateView(INT cx, INT cy, WND_DATA *pWndData)
{
  INT width, height;
  HDC hdc, hdcMem;
  HBITMAP hbmOld;

  width = cx - CONTROL_AREA_WIDTH1 - CONTROL_AREA_WIDTH2;
  height = cy - CONTROL_AREA_HEIGHT1 - CONTROL_AREA_HEIGHT2;

  if (pWndData->bm_width != width || pWndData->bm_height != height)
  {
    if (pWndData->hbmView)
    {
      DeleteObject(pWndData->hbmView);
    }

    pWndData->hbmView = NULL;
  }

  hdc = GetDC(NULL);

  if (pWndData->hbmView == NULL)
  {
//  pWndData->hbmView = CreateBitmap(width, height, 1, 8, NULL);
    pWndData->hbmView = CreateCompatibleBitmap(hdc, width, height);
    pWndData->bm_width = width;
    pWndData->bm_height = height;
  }

  hdcMem = CreateCompatibleDC(hdc);
  hbmOld = SelectObject(hdcMem, pWndData->hbmView);

  Object_DrawGrid(hdcMem, pWndData);

  Object_DrawVertices(hdcMem, pWndData);

  Object_DrawPolygons(hdcMem, pWndData);

  SelectObject(hdcMem, hbmOld);
  DeleteDC(hdcMem);

  ReleaseDC(NULL, hdc);

} // Object_RegenerateView


/*******************************************************************

    NAME:       Object_DrawView

    SYNOPSIS:   Draw the current view on screen.

    ENTRY:      hdc - Device context (not a memory DC).

                hbm - The handle to the bitmap.

                width - The width of the bitmap.

                height - The height of the bitmap.

********************************************************************/

VOID Object_DrawView(HDC hdc, HBITMAP hbm, INT width, INT height)
{
  HDC hdcMem;
  HBITMAP hbmOld;

  if (hbm == NULL)
  {
    return;
  }

  hdcMem = CreateCompatibleDC(hdc);
  hbmOld = SelectObject(hdcMem, hbm);

  BitBlt( hdc,
          CONTROL_AREA_WIDTH1,
          CONTROL_AREA_HEIGHT1,
          width,
          height,
          hdcMem,
          0,
          0,
          SRCCOPY );

  SelectObject(hdcMem, hbmOld);
  DeleteDC(hdcMem);

} // Object_DrawView


/*******************************************************************

    NAME:       Object_Cleanup

    SYNOPSIS:   Cleanup the data by combining close vertices.

********************************************************************/

VOID Object_Cleanup(HWND hwnd, WND_DATA *pWndData)
{
  INT i;
  LEVEL_DATA *pLevel;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  pLevel = pWndData->pLevelData + pWndData->level_index;

  //
  //  Check for invalid indecies.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if ((pLevel->pPolygonData + i)->group_index >= pLevel->nGroups)
    {
      (pLevel->pPolygonData + i)->group_index = -1;
    }
  }

  //
  //  Remove duplicate vertices.
  //

  DeleteDuplicateVertices(pLevel, FALSE);

  //
  //  Remove duplicate polygons.
  //

  DeleteDuplicatePolygons(pLevel);

#if 0

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    nDupe = FindPolygon(pLevel, i + 1, pLevel->pPolygonData + i);

    if (nDupe != -1)
    {
      ReplacePolygon(pLevel, nDupe, i);
      DeletePolygon(pLevel, nDupe);
    }

    ++i;
  }

#endif

  SetCursor(hcurSave);

  return;

} // Object_Cleanup


/*******************************************************************

    NAME:       Object_WndProc

    SYNOPSIS:   Window procedure.

    ENTRY:      hwnd - Window handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    LRESULT - Depends on the actual message.

********************************************************************/

LRESULT CALLBACK __export Object_WndProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam )
{
  switch(nMessage)
  {
    HANDLE_MSG(hwnd, WM_CREATE,          Object_OnCreate      );
    HANDLE_MSG(hwnd, WM_CLOSE,           Wnd_OnClose          );
    HANDLE_MSG(hwnd, WM_DESTROY,         Object_OnDestroy     );
    HANDLE_MSG(hwnd, WM_QUERYENDSESSION, Wnd_OnQueryEndSession);
    HANDLE_MSG(hwnd, WM_TIMER,           Object_OnTimer       );
    HANDLE_MSG(hwnd, WM_SIZE,            Object_OnSize        );
    HANDLE_MSG(hwnd, WM_PAINT,           Object_OnPaint       );
    HANDLE_MSG(hwnd, WM_LBUTTONDOWN,     Object_OnLButtonDown );
    HANDLE_MSG(hwnd, WM_MOUSEMOVE,       Object_OnMouseMove   );
    HANDLE_MSG(hwnd, WM_LBUTTONUP,       Object_OnLButtonUp   );
    HANDLE_MSG(hwnd, WM_KEYDOWN,         Object_OnKey         );
    HANDLE_MSG(hwnd, WM_KEYUP,           Object_OnKey         );
//  HANDLE_MSG(hwnd, WM_CHAR,            Object_OnChar        );
  }

  return DefMDIChildProc(hwnd, nMessage, wParam, lParam);

} // Object_WndProc


/*******************************************************************

    NAME:       Object_OnCreate

    SYNOPSIS:   Handles WM_CREATE messages.

    ENTRY:      hwnd - Window handle.
    
                lpCreateStruct - Points to a CREATESTRUCT data
                  structure containing information about the window
                  being created.

    RETURNS:    TRUE to allow window to be created, FALSE to stop it.
                
********************************************************************/

BOOL Object_OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct)
{
  //
  //  Create the timer.
  //

  if (SetTimer(hwnd, OBJWND_TIMER_ID, 100, NULL) == 0)
  {
    MsgBox(hwnd, MB_ICONSTOP, "Unable to create timer.");

    return FALSE;
  }

  return TRUE;
  
} // Object_OnCreate


/*******************************************************************

    NAME:       Object_OnDestroy

    SYNOPSIS:   Handles WM_DESTROY message.

    ENTRY:      hwnd - Window handle.

********************************************************************/

VOID Object_OnDestroy(HWND hwnd)
{
  WND_DATA *pWndData;

  KillTimer(hwnd, OBJWND_TIMER_ID);

  pWndData = WNDPTR(hwnd);

  if (pWndData != NULL)
  {
    Frame_FreeWindowData(pWndData);
  }

  if (bGotCursor)
  {
    bGotCursor = FALSE;

#ifdef USE_CUSTOM_CURSOR

    ReleaseCapture();
    ShowCursor(TRUE);

#endif

  }

  //
  //  Let the default handler do the rest.
  //

  Wnd_OnDestroy(hwnd);

} // Object_OnDestroy


/*******************************************************************

    NAME:       Object_OnTimer

    SYNOPSIS:   Handles WM_TIMER messages.

    ENTRY:      hwnd - Window handle.

                id - Timer ID.

********************************************************************/

VOID Object_OnTimer(HWND hwnd, UINT id)
{
  if (bControlActive && nControlIndex >= NUM_MODE_CONTROLS)
  {
    if (nAutoRepeatDelay == 0)
    {
      WND_DATA *pWndData;

      pWndData = WNDPTR(hwnd);

      if (pWndData != NULL)
      {
        if (pWndData->loaded)
        {
          HDC hdc;
          RECT rc;
  
          hdc = GetDC(hwnd);
          GetClientRect(hwnd, &rc);
          Object_ControlPressed(hdc, &rc, pWndData, nControlIndex);
          ReleaseDC(hwnd, hdc);
        }
      }
    }
    else
    {
      --nAutoRepeatDelay;
    }
  }

//InvalidateRect(hwnd, NULL, TRUE);

  return;

} // Object_OnTimer


/*******************************************************************

    NAME:       Object_OnSize

    SYNOPSIS:   Handles WM_SIZE messages.

    ENTRY:      hwnd - Window handle.

                state - type of sizing requested

                cx - client area width

                cy - client area height

********************************************************************/

VOID Object_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
#if 1 // 6/14/97 - Why was this commented out?

  WND_DATA *pWndData;

  pWndData = WNDPTR(hwnd);

  if (pWndData != NULL)
  {
    if (pWndData->loaded)
    {
      Object_RegenerateView(cx, cy, pWndData);
    }
  }

#endif

  FORWARD_WM_SIZE(hwnd, state, cx, cy, DefMDIChildProc);

} // Object_OnSize


/*******************************************************************

    NAME:       Object_OnPaint

    SYNOPSIS:   Handles WM_PAINT messages.

    ENTRY:      hwnd - Window handle.

********************************************************************/

VOID Object_OnPaint(HWND hwnd)
{
  HDC hdc;
  PAINTSTRUCT psPaint;
  WND_DATA *pWndData;
  
  hdc = BeginPaint(hwnd, &psPaint);

  pWndData = WNDPTR(hwnd);

  if (pWndData != NULL)
  {
    if (pWndData->loaded)
    {
      INT i;
      RECT rc;

      GetClientRect(hwnd, &rc);
      Object_DrawPage(hdc, &rc, MM_TEXT, pWndData);

      for(i = 0; i < NUM_STATUS_WINDOWS; ++i)
      {
        Object_UpdateStatusWindow(hdc, &rc, pWndData, i);
      }

      if (pWndData->hbmView == NULL)
      {
        Object_RegenerateView( rc.right - rc.left,
                               rc.bottom - rc.top,
                               pWndData );
      }

      Object_DrawView( hdc,
                       pWndData->hbmView,
                       pWndData->bm_width,
                       pWndData->bm_height );
    }
  }

  EndPaint(hwnd, &psPaint);

} // Object_OnPaint


/*******************************************************************

    NAME:       Object_OnLButtonDown

    SYNOPSIS:   Handles WM_LBUTTONDOWN messages.

    ENTRY:      hwnd - Window handle.
                
                fDoubleClick - TRUE for double click.
                
                x - Cursor x position relative to the upper-left
                    corner of the window.
                
                y - Cursor y position relative to the upper-left
                    corner of the window.

                keyFlags - MK_CONTROL, MK_MBUTTON, MK_RBUTTON, MK_SHIFT
                
********************************************************************/

VOID Object_OnLButtonDown( HWND hwnd,
                           BOOL fDoubleClick,
                           INT  x,
                           INT  y,
                           UINT keyFlags )
{
  INT i;
  DWORD dwSize;
  POINT pt;
  HDC hdc, hdcMem;
  HBITMAP hbmOld;
  RECT rc, rcView, btnRect[NUM_CONTROLS];
  WND_DATA *pWndData;
  LEVEL_DATA *pLevel;
  
  pWndData = WNDPTR(hwnd);

  if (pWndData == NULL)
  {
    return;
  }

  if (!pWndData->loaded)
  {
    return;
  }

  pLevel = pWndData->pLevelData + pWndData->level_index;

  //
  //  Check if the mouse was in the view window.
  //

  pt.x = x;
  pt.y = y;

  GetClientRect(hwnd, &rc);

  rcView.left = rc.left + CONTROL_AREA_WIDTH1;
  rcView.top = rc.top + CONTROL_AREA_HEIGHT1;
  rcView.right = rc.right - CONTROL_AREA_WIDTH2;
  rcView.bottom = rc.bottom - CONTROL_AREA_HEIGHT2;

  if (PtInRect(&rcView, pt))
  {
    x -= CONTROL_AREA_WIDTH1;
    y -= CONTROL_AREA_HEIGHT1;

    if (nMode == MODE_POINTER)
    {
      if (CountAllSelections(pLevel))
      {
        dwSize = (DWORD)sizeof(VERTEX) * (DWORD)pLevel->nVertices;

        pWndData->pDragData = (BYTE huge *)GlobalAllocPtr(GPTR, dwSize);

        if (pWndData->pDragData)
        {
          Object_CalcCursorPosition(pWndData, x, y);

          pWndData->drag_start_x = pWndData->cursor_abs_x;
          pWndData->drag_start_y = pWndData->cursor_abs_y;
          pWndData->drag_start_z = pWndData->cursor_abs_z;

        /*ExtendPolygonSelections(pLevel);*/

          hmemcpy( pWndData->pDragData, pLevel->pVertexData, dwSize );

          Object_CreateUndoData( hwnd, pWndData );
        }
      }
    }
    else if (nMode == MODE_NEW_VERTEX)
    {
      NewVertex( pLevel,
                 pWndData->cursor_abs_x,
                 pWndData->cursor_abs_y,
                 pWndData->cursor_abs_z );

      Object_ViewChanged(hwnd, pWndData);

      Object_CreateUndoData(hwnd, pWndData);
    }
    else if (nMode == MODE_SELECT_VERTEX)
    {
      if (pWndData->bSelVertex)
      {
        (pLevel->pVertexData + pWndData->nSelVertex)->flags ^= VF_SELECTED;
      }

      Object_ViewChanged(hwnd, pWndData);
    }
    else if (nMode == MODE_SELECT_POLYGON)
    {
      if (pWndData->bSelPolygon)
      {
        (pLevel->pPolygonData + pWndData->nSelPolygon)->flags ^= PF_SELECTED;
      }

      Object_ViewChanged(hwnd, pWndData);
    }
    else if (nMode == MODE_SELECT_GROUP)
    {
      pWndData->sbox.left = x;
      pWndData->sbox.right = x;
      pWndData->sbox.top = y;
      pWndData->sbox.bottom = y;
    }
    else if (nMode == MODE_NEW_POLYGON)
    {
      if (pWndData->bSelVertex)
      {
        for(i = 0; i < pWndData->nSelVertices; ++i)
        {
          if (pWndData->nSelVertex == pWndData->nSelVert[i])
          {
            break;
          }
        }

        if (i < pWndData->nSelVertices || pWndData->nSelVertices == MAX_VERTICES)
        {
          if (pWndData->nSelVertices >= 3)
          {
            Object_CreateUndoData(hwnd, pWndData);
            NewPolygon(pLevel, pWndData->nSelVertices, pWndData->nSelVert);
            pWndData->nSelVertices = 0;
          }
        }
        else
        {
          pWndData->nSelVert[pWndData->nSelVertices++] = pWndData->nSelVertex;
        }

        Object_ViewChanged(hwnd, pWndData);
      }
    }
    else if (nMode == MODE_ROTATE_POLYGON)
    {
      if (pWndData->bSelPolygon)
      {
        RotatePolygon(pLevel->pPolygonData + pWndData->nSelPolygon, 1);
        SendMessage(hwnd, WM_MOUSEMOVE, 0, MAKELONG(pt.x, pt.y));
      }
    }
    else if (nMode == MODE_FLIP_POLYGON)
    {
      if (pWndData->bSelPolygon)
      {
        FlipPolygon(pLevel->pPolygonData + pWndData->nSelPolygon);
        SendMessage(hwnd, WM_MOUSEMOVE, 0, MAKELONG(pt.x, pt.y));
      }
    }
    else if (nMode == MODE_LINK_POLYGON)
    {
      if (pWndData->bSelPolygon && pWndData->bLnkPolygon)
      {
        Object_CreateUndoData(hwnd, pWndData);

        if (pWndData->nLnkPolygon == pWndData->nSelPolygon)
        {
          UnlinkPolygon(pWndData, pLevel, pWndData->nLnkPolygon);
        }
        else
        {
          POLYGON huge *pPolygon;

          pPolygon = pLevel->pPolygonData + pWndData->nLnkPolygon;

          pPolygon->flags |= PF_LINKED;
          pPolygon->polygon_link = pWndData->nSelPolygon;
        }

        Object_ViewChanged(hwnd, pWndData);

        pWndData->bLnkPolygon = FALSE;
        pWndData->bSelPolygon = FALSE;
      }
      else if (pWndData->bSelPolygon)
      {
        pWndData->nLnkPolygon = pWndData->nSelPolygon;
        pWndData->bLnkPolygon = TRUE;
        pWndData->bSelPolygon = FALSE;
      }
    }

    return;
  }

  //
  //  Check if any of the buttons has been pressed.
  //

  Object_GetButtonRects(&rc, btnRect);

  for(i = 0; i < NUM_CONTROLS; ++i)
  {
    if (PtInRect(&btnRect[i], pt))
    {
      break;
    }
  }

  if (i < NUM_CONTROLS)
  {
    hdc = GetDC(hwnd);
    hdcMem = CreateCompatibleDC(hdc);

    if (i == MODE_POINTER)
    {
      hbmOld = SelectObject(hdcMem, hbmSelectPointerPressed);
    }
    else if (i == MODE_SELECT_VERTEX)
    {
      hbmOld = SelectObject(hdcMem, hbmSelectVertexPressed);
    }
    else if (i == MODE_SELECT_POLYGON)
    {
      hbmOld = SelectObject(hdcMem, hbmSelectPolygonPressed);
    }
    else if (i == MODE_SELECT_GROUP)
    {
      hbmOld = SelectObject(hdcMem, hbmSelectGroupPressed);
    }
    else if (i == MODE_NEW_VERTEX)
    {
      hbmOld = SelectObject(hdcMem, hbmNewVertexPressed);
    }
    else if (i == MODE_NEW_POLYGON)
    {
      hbmOld = SelectObject(hdcMem, hbmNewPolygonPressed);
    }
    else if (i == MODE_ROTATE_POLYGON)
    {
      hbmOld = SelectObject(hdcMem, hbmRotatePolygonPressed);
    }
    else if (i == MODE_FLIP_POLYGON)
    {
      hbmOld = SelectObject(hdcMem, hbmFlipPolygonPressed);
    }
    else if (i == MODE_LINK_POLYGON)
    {
      pWndData->bLnkPolygon = FALSE;
      pWndData->bSelPolygon = FALSE;
      hbmOld = SelectObject(hdcMem, hbmLinkPolygonPressed);
    }
    else if (i == VIEW_PAN_UP)
    {
      hbmOld = SelectObject(hdcMem, hbmUpPressed);
    }
    else if (i == VIEW_PAN_DOWN)
    {
      hbmOld = SelectObject(hdcMem, hbmDownPressed);
    }
    else if (i == VIEW_PAN_LEFT)
    {
      hbmOld = SelectObject(hdcMem, hbmLeftPressed);
    }
    else if (i == VIEW_PAN_RIGHT)
    {
      hbmOld = SelectObject(hdcMem, hbmRightPressed);
    }
    else if (i == VIEW_TILT_UP)
    {
      hbmOld = SelectObject(hdcMem, hbmTiltUpPressed);
    }
    else if (i == VIEW_TILT_DOWN)
    {
      hbmOld = SelectObject(hdcMem, hbmTiltDownPressed);
    }
    else if (i == VIEW_ROTATE_LEFT)
    {
      hbmOld = SelectObject(hdcMem, hbmRotateLeftPressed);
    }
    else if (i == VIEW_ROTATE_RIGHT)
    {
      hbmOld = SelectObject(hdcMem, hbmRotateRightPressed);
    }
    else if (i == VIEW_ZOOM_IN)
    {
      hbmOld = SelectObject(hdcMem, hbmZoomInPressed);
    }
    else if (i == VIEW_ZOOM_OUT)
    {
      hbmOld = SelectObject(hdcMem, hbmZoomOutPressed);
    }
    else if (i == GRID_Y_INCREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmUpPressed);
    }
    else if (i == GRID_Y_DECREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmDownPressed);
    }
    else if (i == GRID_X_DECREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmDownPressed);
    }
    else if (i == GRID_X_INCREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmUpPressed);
    }
    else if (i == GRID_TILT_UP)
    {
      hbmOld = SelectObject(hdcMem, hbmTiltUpPressed);
    }
    else if (i == GRID_TILT_DOWN)
    {
      hbmOld = SelectObject(hdcMem, hbmTiltDownPressed);
    }
    else if (i == GRID_ROTATE_LEFT)
    {
      hbmOld = SelectObject(hdcMem, hbmRotateLeftPressed);
    }
    else if (i == GRID_ROTATE_RIGHT)
    {
      hbmOld = SelectObject(hdcMem, hbmRotateRightPressed);
    }
    else if (i == GRID_Z_INCREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmUpPressed);
    }
    else // if (i == GRID_Z_DECREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmDownPressed);
    }

    BitBlt( hdc,
            btnRect[i].left,
            btnRect[i].top,
            i < NUM_MODE_CONTROLS ? BTN_WIDTH1 : BTN_WIDTH2,
            i < NUM_MODE_CONTROLS ? BTN_HEIGHT1 : BTN_HEIGHT2,
            hdcMem,
            0,
            0,
            SRCCOPY );

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);

    nControlIndex = i;
    bControlActive = TRUE;
    nAutoRepeatDelay = AUTO_REPEAT_DELAY;

    Object_ControlPressed(hdc, &rc, pWndData, nControlIndex);

    ReleaseDC(hwnd, hdc);

#ifdef USE_CUSTOM_CURSOR

    SetCapture(hwnd);

#endif

  }
  else
  {
    //
    //  Process with the default message handler.
    //
  
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, keyFlags, DefMDIChildProc);
  }

} // Object_OnLButtonDown


/*******************************************************************

    NAME:       Object_OnMouseMove

    SYNOPSIS:   Handles WM_MOUSEMOVE messages.

    ENTRY:      hwnd - Window handle.
                
                x - Cursor x position in screen coordinates.
                
                y - Cursor y position in screen coordinates.

                keyFlags - MK_CONTROL, MK_MBUTTON, MK_RBUTTON, MK_SHIFT
                
********************************************************************/

VOID Object_OnMouseMove( HWND hwnd,
                         INT  x,
                         INT  y,
                         UINT keyFlags )
{
  INT i, n;
  CHAR szBuf[128];
  DWORD dwSize;
  VERTEX huge *pVertex;
  POLYGON huge *pPolygon;
  HDC hdc, hdcSrc, hdcDst;
  HBITMAP hbmNew, hbmSrcOld, hbmDstOld;
  HPEN hPenCursor, hPenOld;
  HBRUSH hBrushNull, hBrushOld;
  POINT pt;
  RECT rcClient, rcView;
  WND_DATA *pWndData;
  LEVEL_DATA *pLevel;
  double point[6];

  pWndData = WNDPTR(hwnd);

  if (pWndData == NULL)
  {
    return;
  }

  if (!pWndData->loaded)
  {
    return;
  }

  pLevel = pWndData->pLevelData + pWndData->level_index;

  pt.x = x;
  pt.y = y;

  GetClientRect(hwnd, &rcClient);

  rcView.left = rcClient.left + CONTROL_AREA_WIDTH1;
  rcView.top = rcClient.top + CONTROL_AREA_HEIGHT1;
  rcView.right = rcClient.right - CONTROL_AREA_WIDTH2;
  rcView.bottom = rcClient.bottom - CONTROL_AREA_HEIGHT2;

  if (PtInRect(&rcView, pt))
  {
    x -= CONTROL_AREA_WIDTH1;
    y -= CONTROL_AREA_HEIGHT1;

    if (nMode == MODE_POINTER)
    {
      if (pWndData->pDragData)
      {
        long cursor_x, cursor_y, cursor_z, ldx, ldy, ldz;

        Object_CalcCursorPosition(pWndData, x, y);

        ldx = pWndData->cursor_abs_x - pWndData->drag_start_x;
        ldy = pWndData->cursor_abs_y - pWndData->drag_start_y;
        ldz = pWndData->cursor_abs_z - pWndData->drag_start_z;

        cursor_x = pWndData->cursor_rel_x;
        cursor_y = pWndData->cursor_rel_y;
        cursor_z = pWndData->cursor_rel_z;

        pWndData->cursor_rel_x = ldx;
        pWndData->cursor_rel_y = ldy;
        pWndData->cursor_rel_z = ldz;

        hdc = GetDC(hwnd);

        Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_X);
        Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Y);
        Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Z);

        ReleaseDC(hwnd, hdc);

        pWndData->cursor_rel_x = cursor_x;
        pWndData->cursor_rel_y = cursor_y;
        pWndData->cursor_rel_z = cursor_z;

        dwSize = (DWORD)sizeof(VERTEX) * (DWORD)pLevel->nVertices;

        hmemcpy(pLevel->pVertexData, pWndData->pDragData, dwSize);

        MoveSelectedVertices(pLevel, ldx, ldy, ldz);

        Object_ViewChanged(hwnd, pWndData);
      }
    }
    else if (nMode == MODE_NEW_VERTEX)
    {
      if (!bGotCursor)
      {

#ifdef USE_CUSTOM_CURSOR

        ShowCursor(FALSE);
        SetCapture(hwnd);

#endif

        bGotCursor = TRUE;
      }

      hdc = GetDC(hwnd);

      hdcSrc = CreateCompatibleDC(hdc);
      hdcDst = CreateCompatibleDC(hdc);

      hbmNew = CreateCompatibleBitmap(hdc, pWndData->bm_width, pWndData->bm_height);

      hbmSrcOld = SelectObject(hdcSrc, pWndData->hbmView);
      hbmDstOld = SelectObject(hdcDst, hbmNew);

      BitBlt( hdcDst,
              0,
              0,
              pWndData->bm_width,
              pWndData->bm_height,
              hdcSrc,
              0,
              0,
              SRCCOPY );

      Object_DrawCursorLines(hdcDst, pWndData, x, y);

#ifdef USE_CUSTOM_CURSOR

      hPenCursor = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
      hPenOld = SelectObject(hdcDst, hPenCursor);

      MoveTo(hdcDst, x - 10, y);
      LineTo(hdcDst, x + 10, y);
      MoveTo(hdcDst, x, y - 10);
      LineTo(hdcDst, x, y + 10);

      SelectObject(hdcDst, hPenOld);
      DeleteObject(hPenCursor);

#endif

      SelectObject(hdcDst, hbmDstOld);
      SelectObject(hdcSrc, hbmSrcOld);

      Object_DrawView(hdc, hbmNew, pWndData->bm_width, pWndData->bm_height);

      DeleteObject(hbmNew);

      DeleteDC(hdcDst);
      DeleteDC(hdcSrc);

      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_X);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_Y);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_Z);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_X);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Y);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Z);

      ReleaseDC(hwnd, hdc);
    }
    else if (nMode == MODE_SELECT_VERTEX || nMode == MODE_NEW_POLYGON)
    {
      hdc = GetDC(hwnd);

      if (!bGotCursor)
      {

#ifdef USE_CUSTOM_CURSOR

        ShowCursor(FALSE);
        SetCapture(hwnd);

#endif

        bGotCursor = TRUE;

        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_X);
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Y);
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Z);
/*
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_X);
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Y);
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Z);
*/
      }

      hdcSrc = CreateCompatibleDC(hdc);
      hdcDst = CreateCompatibleDC(hdc);
      hbmNew = CreateCompatibleBitmap(hdc, pWndData->bm_width, pWndData->bm_height);

      //
      //  Copy the current view to the new bitmap.
      //

      hbmSrcOld = SelectObject(hdcSrc, pWndData->hbmView);
      hbmDstOld = SelectObject(hdcDst, hbmNew);

      BitBlt( hdcDst,
              0,
              0,
              pWndData->bm_width,
              pWndData->bm_height,
              hdcSrc,
              0,
              0,
              SRCCOPY );

      //
      //  Highlight the selected vertex.
      //

      Object_DrawSelectedVertex(hdcDst, pWndData, x, y);

      //
      //  If drawing a new polygon, draw partial polygon.
      //

      if (nMode == MODE_NEW_POLYGON && pWndData->nSelVertices > 0)
      {
        hPenCursor = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        hPenOld = SelectObject(hdcDst, hPenCursor);

        n = pWndData->nSelVertices - 1;

        if (pWndData->bSelVertex)
        {
          if (pWndData->nSelVertex != pWndData->nSelVert[n])
          {
            ++n;
          }
        }

        for(i = 0; i < n; ++i)
        {
          pVertex = pLevel->pVertexData + pWndData->nSelVert[i];

          point[0] = (double)pVertex->x;
          point[1] = (double)pVertex->y;
          point[2] = (double)pVertex->z;

          if (i < pWndData->nSelVertices - 1)
          {
            pVertex = pLevel->pVertexData + pWndData->nSelVert[i+1];
          }
          else
          {
            pVertex = pLevel->pVertexData + pWndData->nSelVertex;
          }

          point[3] = (double)pVertex->x;
          point[4] = (double)pVertex->y;
          point[5] = (double)pVertex->z;

          Object_DrawLine(hdcDst, pWndData, point, 0, 0);
        }

        SelectObject(hdcDst, hPenOld);
        DeleteObject(hPenCursor);
      }

#ifdef USE_CUSTOM_CURSOR

      //
      //  Draw the crosshair cursor on the bitmap.
      //

      hPenCursor = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
      hPenOld = SelectObject(hdcDst, hPenCursor);

      MoveTo(hdcDst, x - 10, y);
      LineTo(hdcDst, x + 10, y);
      MoveTo(hdcDst, x, y - 10);
      LineTo(hdcDst, x, y + 10);

      SelectObject(hdcDst, hPenOld);
      DeleteObject(hPenCursor);

#endif

      //
      //  Draw the resulting bitmap.
      //

      SelectObject(hdcDst, hbmDstOld);
      SelectObject(hdcSrc, hbmSrcOld);

      Object_DrawView(hdc, hbmNew, pWndData->bm_width, pWndData->bm_height);

      DeleteObject(hbmNew);

      DeleteDC(hdcDst);
      DeleteDC(hdcSrc);

      //
      //  Update the status windows.
      //

      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_X);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_Y);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_Z);

/*
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_X);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Y);
      Object_UpdateStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Z);
*/

      if (nMode == MODE_SELECT_VERTEX && pWndData->bSelVertex)
      {
        pVertex = pLevel->pVertexData + pWndData->nSelVertex;

        if (pVertex->flags & VF_SPRITE)
        {
          wsprintf( szBuf, "Sprite \"%s\"",
            pWndData->pSpriteData[pVertex->sprite_index].sprite_name);
        }
        else if (pVertex->flags & VF_LIFEFORM)
        {
          wsprintf( szBuf, "Life Form [%c%c%c] \"%s\"",
            (pVertex->flags & VF_DIFFICULTY_LEVEL_1) ? '1':'X',
            (pVertex->flags & VF_DIFFICULTY_LEVEL_2) ? '2':'X',
            (pVertex->flags & VF_DIFFICULTY_LEVEL_3) ? '3':'X',
            pWndData->pLifeFormData[pVertex->lifeform_index].lifeform_name);
        }
        else if (pVertex->flags & VF_ITEM)
        {
          wsprintf( szBuf, "Item [%c%c%c] \"%s\"",
            (pVertex->flags & VF_DIFFICULTY_LEVEL_1) ? '1':'X',
            (pVertex->flags & VF_DIFFICULTY_LEVEL_2) ? '2':'X',
            (pVertex->flags & VF_DIFFICULTY_LEVEL_3) ? '3':'X',
            pWndData->pItemData[pVertex->item_index].item_name);
        }
        else if (pVertex->flags & VF_NOISE)
        {
          wsprintf( szBuf, "Noise \"%s\"",
            pWndData->pNoiseData[pVertex->noise_index].noise_name);
        }
        else if (pVertex->flags & VF_TRIGGER)
        {
          wsprintf( szBuf, "Trigger \"%s\"",
            pLevel->pTriggerData[pVertex->trigger_index].trigger_name);
        }
        else if (pVertex->flags & VF_PLAYER_START)
        {
          wsprintf( szBuf, "Player Start");
        }
        else
        {
          wsprintf( szBuf, "Vertex");
        }

        Object_ShowMessage(hdc, &rcClient, szBuf);
      }
/*
      else
      {
        Object_ShowMessage(hdc, &rcClient, "");
      }
*/
      ReleaseDC(hwnd, hdc);
    }
    else if (nMode == MODE_SELECT_GROUP)
    {
      if (keyFlags & MK_LBUTTON)
      {
        hdc = GetDC(hwnd);

        hdcSrc = CreateCompatibleDC(hdc);
        hdcDst = CreateCompatibleDC(hdc);

        hbmNew = CreateCompatibleBitmap(hdc, pWndData->bm_width, pWndData->bm_height);

        //
        //  Copy the current view to the new bitmap.
        //

        hbmSrcOld = SelectObject(hdcSrc, pWndData->hbmView);
        hbmDstOld = SelectObject(hdcDst, hbmNew);

        BitBlt( hdcDst,
                0,
                0,
                pWndData->bm_width,
                pWndData->bm_height,
                hdcSrc,
                0,
                0,
                SRCCOPY );

        //
        //  Update the selection box.
        //

        if (x < (pWndData->sbox.left + pWndData->sbox.right) / 2)
        {
          pWndData->sbox.left = x;
        }
        else
        {
          pWndData->sbox.right = x;
        }

        if (y < (pWndData->sbox.top + pWndData->sbox.bottom) / 2)
        {
          pWndData->sbox.top = y;
        }
        else
        {
          pWndData->sbox.bottom = y;
        }

        if (pWndData->sbox.left < 0)
        {
          pWndData->sbox.left = 0;
        }

        if (pWndData->sbox.right > pWndData->bm_width)
        {
          pWndData->sbox.right = pWndData->bm_width;
        }        

        if (pWndData->sbox.top < 0)
        {
          pWndData->sbox.top = 0;
        }

        if (pWndData->sbox.bottom > pWndData->bm_height)
        {
          pWndData->sbox.bottom = pWndData->bm_height;
        }        

        //
        //  Highlight the selected vertex.
        //

        Object_DrawSelectedObjects(hdcDst, pWndData);

        //
        //  Draw the selection box on the bitmap.
        //

        hPenCursor = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        hBrushNull = GetStockObject(NULL_BRUSH);
        hPenOld = SelectObject(hdcDst, hPenCursor);
        hBrushOld = SelectObject(hdcDst, hBrushNull);

        Rectangle( hdcDst,
                   pWndData->sbox.left,
                   pWndData->sbox.top,
                   pWndData->sbox.right,
                   pWndData->sbox.bottom );

        SelectObject(hdcDst, hBrushOld);
        SelectObject(hdcDst, hPenOld);
        DeleteObject(hPenCursor);

        //
        //  Draw the resulting bitmap.
        //

        SelectObject(hdcDst, hbmDstOld);
        SelectObject(hdcSrc, hbmSrcOld);

        Object_DrawView(hdc, hbmNew, pWndData->bm_width, pWndData->bm_height);

        DeleteObject(hbmNew);

        DeleteDC(hdcDst);
        DeleteDC(hdcSrc);

        ReleaseDC(hwnd, hdc);
      }
    }
    else if (nMode == MODE_SELECT_POLYGON || nMode == MODE_LINK_POLYGON ||
             nMode == MODE_ROTATE_POLYGON || nMode == MODE_FLIP_POLYGON)
    {
      hdc = GetDC(hwnd);

      if (!bGotCursor)
      {

#ifdef USE_CUSTOM_CURSOR

        ShowCursor(FALSE);
        SetCapture(hwnd);

#endif

        bGotCursor = TRUE;

        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_X);
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_Y);
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_Z);

        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_X);
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Y);
        Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Z);
      }

      if (nMode == MODE_LINK_POLYGON)
      {
        if (pWndData->bLnkPolygon && pWndData->bSelPolygon)
        {
          if (pWndData->nLnkPolygon == pWndData->nSelPolygon)
          {
            Object_ShowMessage(hdc, &rcClient, "Press left mouse button to remove polygon link");
          }
          else
          {
            Object_ShowMessage(hdc, &rcClient, "Press left mouse button to establish polygon link");
          }
        }
        else if (!pWndData->bLnkPolygon && pWndData->bSelPolygon)
        {
          Object_ShowMessage(hdc, &rcClient, "Press left mouse button to select polygon to be linked");
        }
        else
        {
          Object_ShowMessage(hdc, &rcClient, "Move the mouse to select a polygon");
        }
      }

      hdcSrc = CreateCompatibleDC(hdc);
      hdcDst = CreateCompatibleDC(hdc);

      hbmNew = CreateCompatibleBitmap(hdc, pWndData->bm_width, pWndData->bm_height);

      //
      //  Copy the current view to the new bitmap.
      //

      hbmSrcOld = SelectObject(hdcSrc, pWndData->hbmView);
      hbmDstOld = SelectObject(hdcDst, hbmNew);

      BitBlt( hdcDst,
              0,
              0,
              pWndData->bm_width,
              pWndData->bm_height,
              hdcSrc,
              0,
              0,
              SRCCOPY );

      //
      //  Highlight the selected polygon.
      //

      Object_DrawSelectedPolygon(hdcDst, pWndData, x, y);

#ifdef USE_CUSTOM_CURSOR

      //
      //  Draw the crosshair cursor on the bitmap.
      //

      hPenCursor = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
      hPenOld = SelectObject(hdcDst, hPenCursor);

      MoveTo(hdcDst, x - 10, y);
      LineTo(hdcDst, x + 10, y);
      MoveTo(hdcDst, x, y - 10);
      LineTo(hdcDst, x, y + 10);

      SelectObject(hdcDst, hPenOld);
      DeleteObject(hPenCursor);

#endif

      //
      //  Draw the resulting bitmap.
      //

      SelectObject(hdcDst, hbmDstOld);
      SelectObject(hdcSrc, hbmSrcOld);

      Object_DrawView(hdc, hbmNew, pWndData->bm_width, pWndData->bm_height);

      DeleteObject(hbmNew);

      DeleteDC(hdcDst);
      DeleteDC(hdcSrc);

      if (nMode != MODE_LINK_POLYGON && pWndData->bSelPolygon)
      {
        CHAR szMsg1[16], szMsg2[16], szMsg3[64];

        szMsg1[0] = '\0';
        szMsg2[0] = '\0';

        pPolygon = pLevel->pPolygonData + pWndData->nSelPolygon;

        if (pPolygon->flags & PF_LINKED)
        {
          wsprintf( szMsg1, "Linked, ");

          while(pPolygon->flags & PF_LINKED)
          {
            pPolygon = pLevel->pPolygonData + pPolygon->polygon_link;
          }
        }

        if (pPolygon->flags & PF_LINKED)
        {
          wsprintf( szMsg2, "Secret, ");
        }

        if (pPolygon->flags & PF_ANIMATED)
        {
          wsprintf( szMsg3, "Animated \"%s\"",
            pWndData->pAnimationData[pPolygon->animation_index].animation_name);
        }
        else if (pPolygon->flags & PF_MOTION_ANIMATED)
        {
          wsprintf( szMsg3, "Motion Animated \"%s\"",
            pWndData->pAnimationData[pPolygon->animation_index].animation_name);
        }
        else if (pPolygon->flags & PF_INVISIBLE)
        {
          wsprintf( szMsg3, "Invisible");
        }
        else
        {
          wsprintf( szMsg3, "Textured \"%s\"",
            pWndData->pTextureData[pPolygon->texture_index].texture_name);
        }

        wsprintf( szBuf, "%s%s%s", szMsg1, szMsg2, szMsg3 );

        Object_ShowMessage(hdc, &rcClient, szBuf);
      }
/*
      else
      {
        Object_ShowMessage(hdc, &rcClient, "");
      }
*/
      ReleaseDC(hwnd, hdc);
    }
  }
  else
  {
    if (bGotCursor)
    {
      hdc = GetDC(hwnd);

      Object_DrawView(hdc, pWndData->hbmView, pWndData->bm_width, pWndData->bm_height);

      Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_X);
      Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_Y);
      Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_ABS_Z);

      Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_X);
      Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Y);
      Object_ClearStatusWindow(hdc, &rcClient, pWndData, CURSOR_STATUS_REL_Z);

      ReleaseDC(hwnd, hdc);

#ifdef USE_CUSTOM_CURSOR

      ShowCursor(TRUE);
      ReleaseCapture();

#endif

      bGotCursor = FALSE;
    }

    FORWARD_WM_MOUSEMOVE(hwnd, x, y, keyFlags, DefMDIChildProc);
  }
  
} // Object_OnMouseMove


/*******************************************************************

    NAME:       Object_OnLButtonUp

    SYNOPSIS:   Handles WM_LBUTTONUP messages.

    ENTRY:      hwnd - Window handle.
                
                x - Cursor x position relative to the upper-left
                    corner of the window.
                
                y - Cursor y position relative to the upper-left
                    corner of the window.

                keyFlags - MK_CONTROL, MK_MBUTTON, MK_RBUTTON, MK_SHIFT
                
********************************************************************/

VOID Object_OnLButtonUp( HWND hwnd,
                         INT  x,
                         INT  y,
                         UINT keyFlags )
{
  HDC hdc, hdcMem;
  CHAR szBuf[128];
  HBITMAP hbmOld;
  WND_DATA *pWndData;

  //
  //  Get a pointer to the data.
  //
  
  pWndData = WNDPTR(hwnd);

  if (pWndData == NULL)
  {
    return;
  }

  if (!pWndData->loaded)
  {
    return;
  }

  if (bControlActive)
  {
    RECT rc, btnRect[NUM_CONTROLS];

    //
    //  Get the button rectangles.
    //

    GetClientRect(hwnd, &rc);
    Object_GetButtonRects(&rc, btnRect);

    hdc = GetDC(hwnd);
    hdcMem = CreateCompatibleDC(hdc);

    if (nControlIndex == MODE_POINTER)
    {
      wsprintf( szBuf, "Vertices = %d of %d   Polygons = %d of %d",
        pWndData->pLevelData[pWndData->level_index].nVertices,
        MAX_NUM_VERTICES,
        pWndData->pLevelData[pWndData->level_index].nPolygons,
        MAX_NUM_POLYGONS );

      Object_ShowMessage(hdc, &rc, szBuf);

      hbmOld = SelectObject(hdcMem, hbmSelectPointerNormal);
    }
    else if (nControlIndex == MODE_SELECT_VERTEX)
    {
      Object_ShowMessage(hdc, &rc, "Press the left mouse button to select/deselect a vertex");
      hbmOld = SelectObject(hdcMem, hbmSelectVertexNormal);
    }
    else if (nControlIndex == MODE_SELECT_POLYGON)
    {
      Object_ShowMessage(hdc, &rc, "Press the left mouse button to select/deselect a polygon");
      hbmOld = SelectObject(hdcMem, hbmSelectPolygonNormal);
    }
    else if (nControlIndex == MODE_SELECT_GROUP)
    {
      Object_ShowMessage(hdc, &rc, "Hold the left mouse button down to drag out a selection box");
      hbmOld = SelectObject(hdcMem, hbmSelectGroupNormal);
    }
    else if (nControlIndex == MODE_NEW_VERTEX)
    {
      Object_ShowMessage(hdc, &rc, "Press the left mouse button to create a vertex");
      hbmOld = SelectObject(hdcMem, hbmNewVertexNormal);
    }
    else if (nControlIndex == MODE_NEW_POLYGON)
    {
      pWndData->nSelVertices = 0;
      Object_ShowMessage(hdc, &rc, "Press the left mouse button to select each polygon vertex");
      hbmOld = SelectObject(hdcMem, hbmNewPolygonNormal);
    }
    else if (nControlIndex == MODE_ROTATE_POLYGON)
    {
      pWndData->nSelVertices = 0;
      Object_ShowMessage(hdc, &rc, "Press the left mouse button to rotate the polygon vertices clockwise");
      hbmOld = SelectObject(hdcMem, hbmRotatePolygonNormal);
    }
    else if (nControlIndex == MODE_FLIP_POLYGON)
    {
      pWndData->nSelVertices = 0;
      Object_ShowMessage(hdc, &rc, "Press the left mouse button to flip the visible side of the polygon");
      hbmOld = SelectObject(hdcMem, hbmFlipPolygonNormal);
    }
    else if (nControlIndex == MODE_LINK_POLYGON)
    {
      Object_ShowMessage(hdc, &rc, "Move the mouse to select a polygon");
      hbmOld = SelectObject(hdcMem, hbmLinkPolygonNormal);
    }
    else if (nControlIndex == VIEW_PAN_UP)
    {
      hbmOld = SelectObject(hdcMem, hbmUpNormal);
    }
    else if (nControlIndex == VIEW_PAN_DOWN)
    {
      hbmOld = SelectObject(hdcMem, hbmDownNormal);
    }
    else if (nControlIndex == VIEW_PAN_LEFT)
    {
      hbmOld = SelectObject(hdcMem, hbmLeftNormal);
    }
    else if (nControlIndex == VIEW_PAN_RIGHT)
    {
      hbmOld = SelectObject(hdcMem, hbmRightNormal);
    }
    else if (nControlIndex == VIEW_TILT_UP)
    {
      hbmOld = SelectObject(hdcMem, hbmTiltUpNormal);
    }
    else if (nControlIndex == VIEW_TILT_DOWN)
    {
      hbmOld = SelectObject(hdcMem, hbmTiltDownNormal);
    }
    else if (nControlIndex == VIEW_ROTATE_LEFT)
    {
      hbmOld = SelectObject(hdcMem, hbmRotateLeftNormal);
    }
    else if (nControlIndex == VIEW_ROTATE_RIGHT)
    {
      hbmOld = SelectObject(hdcMem, hbmRotateRightNormal);
    }
    else if (nControlIndex == VIEW_ZOOM_IN)
    {
      hbmOld = SelectObject(hdcMem, hbmZoomInNormal);
    }
    else if (nControlIndex == VIEW_ZOOM_OUT)
    {
      hbmOld = SelectObject(hdcMem, hbmZoomOutNormal);
    }
    else if (nControlIndex == GRID_Y_INCREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmUpNormal);
    }
    else if (nControlIndex == GRID_Y_DECREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmDownNormal);
    }
    else if (nControlIndex == GRID_X_DECREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmDownNormal);
    }
    else if (nControlIndex == GRID_X_INCREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmUpNormal);
    }                       
    else if (nControlIndex == GRID_TILT_UP)
    {
      hbmOld = SelectObject(hdcMem, hbmTiltUpNormal);
    }
    else if (nControlIndex == GRID_TILT_DOWN)
    {
      hbmOld = SelectObject(hdcMem, hbmTiltDownNormal);
    }
    else if (nControlIndex == GRID_ROTATE_LEFT)
    {
      hbmOld = SelectObject(hdcMem, hbmRotateLeftNormal);
    }
    else if (nControlIndex == GRID_ROTATE_RIGHT)
    {
      hbmOld = SelectObject(hdcMem, hbmRotateRightNormal);
    }
    else if (nControlIndex == GRID_Z_INCREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmUpNormal);
    }
    else // if (nControlIndex == GRID_Z_DECREASE)
    {
      hbmOld = SelectObject(hdcMem, hbmDownNormal);
    }

    BitBlt( hdc,
            btnRect[nControlIndex].left,
            btnRect[nControlIndex].top,
            nControlIndex < NUM_MODE_CONTROLS ? BTN_WIDTH1 : BTN_WIDTH2,
            nControlIndex < NUM_MODE_CONTROLS ? BTN_HEIGHT1 : BTN_HEIGHT2,
            hdcMem,
            0,
            0,
            SRCCOPY );

    SelectObject(hdcMem, hbmOld);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);

    bControlActive = FALSE;

#ifdef USE_CUSTOM_CURSOR

    ReleaseCapture();

#endif

//  InvalidateRect(hwnd, NULL, TRUE);
  }
  else
  {
    if (pWndData->pDragData)
    {
      GlobalFreePtr(pWndData->pDragData);
      pWndData->pDragData = NULL;

      if (nMode == MODE_POINTER)
      {
        if (pWndData->cursor_abs_x != pWndData->drag_start_x ||
            pWndData->cursor_abs_y != pWndData->drag_start_y ||
            pWndData->cursor_abs_z != pWndData->drag_start_z)
        {
          DeleteDuplicateVertices(pWndData->pLevelData + pWndData->level_index, TRUE);
          Object_ResetPixelSize(hwnd, pWndData);
        }
      }
    }

    if (nMode == MODE_SELECT_GROUP)
    {
      if ( pWndData->sbox.right > pWndData->sbox.left &&
           pWndData->sbox.bottom > pWndData->sbox.top )
      {
        Object_DrawSelectedObjects(NULL, pWndData);

        pWndData->sbox.left = 0;
        pWndData->sbox.right = 0;
        pWndData->sbox.top = 0;
        pWndData->sbox.bottom = 0;

        Object_ViewChanged(hwnd, pWndData);
      }
    }

    //
    //  Process with the default message handler.
    //
  
    FORWARD_WM_LBUTTONUP(hwnd, x, y, keyFlags, DefMDIChildProc);
  }
  
} // Object_OnLButtonUp


/*******************************************************************

    NAME:       Object_OnKey

    SYNOPSIS:   Handles WM_KEYDOWN and WM_KEYUP messages.

    ENTRY:      hwnd - Window handle.
                
                vk - The virtual key code
                
                fDown - TRUE if WM_KEYDOWN, FALSE if WM_KEYUP
                
                cRepeat - Repeat count if key is being held down.
                
                flags - See WM_KEYDOWN or WM_KEYUP in SDK docs.

********************************************************************/

VOID Object_OnKey(HWND hwnd, UINT vk, BOOL fDown, INT cRepeat, UINT flags)
{
  HDC hdc;
  RECT rc;
  WND_DATA *pWndData;
  LEVEL_DATA *pLevel;
  static BOOL bControlKey;
  static BOOL bShiftKey;

  if (vk == VK_CONTROL)
  {
    bControlKey = fDown;
    return;
  }

  if (vk == VK_SHIFT)
  {
    bShiftKey = fDown;
    return;
  }

  if (!fDown)
  {
    FORWARD_WM_KEYUP(hwnd, vk, cRepeat, flags, DefMDIChildProc);
    return;
  }

  //
  //  Get a pointer to the data.
  //
  
  pWndData = WNDPTR(hwnd);

  if (pWndData == NULL)
  {
    return;
  }

  if (!pWndData->loaded)
  {
    return;
  }

  pLevel = pWndData->pLevelData + pWndData->level_index;

  //
  //  Respond to the keypress.
  //
  
  hdc = GetDC(hwnd);
  GetClientRect(hwnd, &rc);

  if (vk == VK_UP || vk == VK_DOWN)
  {
    if (bShiftKey)
    {
      double delta;

      delta = PI / 64.0;

      if (vk == VK_UP)
      {
        pLevel->view_elevation += delta * (double)cRepeat;

        if (pLevel->view_elevation > PI / 2.0)
        {
          pLevel->view_elevation = PI / 2.0;
        }
      }
      else
      {
        pLevel->view_elevation -= delta * (double)cRepeat;

        if (pLevel->view_elevation < -PI / 2.0)
        {
          pLevel->view_elevation = -PI / 2.0;
        }
      }

      pLevel->view_elevation = delta * Round(pLevel->view_elevation / delta);

      Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_PHI);
    }
    else
    {
      double sin_vtheta, cos_vtheta;
      double sin_gtheta, cos_gtheta, sin_gphi, cos_gphi;
      double d, dx, dy, dz;

      if (vk == VK_UP)
      {
        d = (double)pLevel->view_delta;
      }
      else
      {
        d = -(double)pLevel->view_delta;
      }

      d = d * (double)cRepeat;

      if (bControlKey)
      {               
        dx = 0.0;
        dy = 0.0;
        dz = d;
      }
      else
      {
        sin_vtheta = sin(pLevel->view_rotation-pLevel->grid_rotation);
        cos_vtheta = cos(pLevel->view_rotation-pLevel->grid_rotation);
        sin_gtheta = sin(pLevel->grid_rotation);
        cos_gtheta = cos(pLevel->grid_rotation);
        sin_gphi   = sin(pLevel->grid_elevation);
        cos_gphi   = cos(pLevel->grid_elevation);

        dx =  d*cos_vtheta*cos_gphi*cos_gtheta-d*sin_vtheta*sin_gtheta;
        dy =  d*cos_vtheta*cos_gphi*sin_gtheta+d*sin_vtheta*cos_gtheta;
        dz = -d*cos_vtheta*sin_gphi;
      }

      pLevel->view_x += (long)dx;
      pLevel->view_y += (long)dy;
      pLevel->view_z += (long)dz;

      Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_X);
      Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_Y);
      Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_Z);
    }
  }
  else if (vk == VK_LEFT || vk == VK_RIGHT)
  {
    double dr, delta;

    delta = PI / 64;

    if (vk == VK_LEFT)
    {
      dr = delta;
    }
    else
    {
      dr = -delta;
    }

    dr = dr * (double)cRepeat;

    pLevel->view_rotation += dr;

    if (pLevel->view_rotation > PI * 2.0)
    {
      pLevel->view_rotation -= PI * 2.0;
    }

    if (pLevel->view_rotation < 0.0)
    {
      pLevel->view_rotation += PI * 2.0;
    }

    pLevel->view_rotation = delta * Round(pLevel->view_rotation / delta);

    Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_THETA);
  }
  else if (vk == VK_F8)
  {
    Object_Cleanup(hwnd, pWndData);
  }
  else
  {
    FORWARD_WM_KEYDOWN(hwnd, vk, cRepeat, flags, DefMDIChildProc);
  }

  Object_RegenerateView( rc.right - rc.left,
                         rc.bottom - rc.top,
                         pWndData );

  Object_DrawView(hdc, pWndData->hbmView, pWndData->bm_width, pWndData->bm_height);

  ReleaseDC(hwnd, hdc);

  return;
  
} // Object_OnKey

#if 0

/*******************************************************************

    NAME:       Object_OnChar

    SYNOPSIS:   Handles WM_CHAR messages.

    ENTRY:      hwnd - Window handle.
                
                vk - The virtual key code
                
                cRepeat - Repeat count if key is being held down.

********************************************************************/

VOID Object_OnChar(HWND hwnd, UINT vk, INT cRepeat)
{
  HDC hdc;
  RECT rc;
  WND_DATA *pWndData;
  LEVEL_DATA *pLevel;
  BOOL bControlKey;
  BOOL bShiftKey;

  bControlKey = (GetKeyState( VK_CONTROL ) < 0) ? TRUE : FALSE;
  bShiftKey = (GetKeyState( VK_SHIFT ) < 0) ? TRUE : FALSE;

  //
  //  Get a pointer to the data.
  //
  
  pWndData = WNDPTR(hwnd);

  if (pWndData == NULL)
  {
    return;
  }

  if (!pWndData->loaded)
  {
    return;
  }

  pLevel = pWndData->pLevelData + pWndData->level_index;

  //
  //  Respond to the keypress.
  //
  
  hdc = GetDC(hwnd);
  GetClientRect(hwnd, &rc);

  if (vk == VK_UP || vk == VK_DOWN)
  {
    double delta;

    delta = PI / 64.0;

    if (bShiftKey)
    {
      if (vk == VK_UP)
      {
        pLevel->view_elevation += delta * (double)cRepeat;

        if (pLevel->view_elevation > PI / 2.0)
        {
          pLevel->view_elevation = PI / 2.0;
        }
      }
      else
      {
        pLevel->view_elevation -= delta * (double)cRepeat;

        if (pLevel->view_elevation < -PI / 2.0)
        {
          pLevel->view_elevation = -PI / 2.0;
        }
      }

      pLevel->view_elevation = delta * Round(pLevel->view_elevation / delta);

      Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_PHI);
    }
    else
    {
      double sin_vtheta, cos_vtheta;
      double sin_gtheta, cos_gtheta, sin_gphi, cos_gphi;
      double d, dx, dy, dz;

      if (vk == VK_UP)
      {
        d = (double)pLevel->view_delta;
      }
      else
      {
        d = -(double)pLevel->view_delta;
      }

      d = d * (double)cRepeat;

      if (bControlKey)
      {
        dx = 0.0;
        dy = 0.0;
        dz = d;
      }
      else
      {
        sin_vtheta = sin(pLevel->view_rotation-pLevel->grid_rotation);
        cos_vtheta = cos(pLevel->view_rotation-pLevel->grid_rotation);
        sin_gtheta = sin(pLevel->grid_rotation);
        cos_gtheta = cos(pLevel->grid_rotation);
        sin_gphi   = sin(pLevel->grid_elevation);
        cos_gphi   = cos(pLevel->grid_elevation);

        dx =  d*cos_vtheta*cos_gphi*cos_gtheta-d*sin_vtheta*sin_gtheta;
        dy =  d*cos_vtheta*cos_gphi*sin_gtheta+d*sin_vtheta*cos_gtheta;
        dz = -d*cos_vtheta*sin_gphi;
      }

      pLevel->view_x += (long)dx;
      pLevel->view_y += (long)dy;
      pLevel->view_z += (long)dz;

      Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_X);
      Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_Y);
      Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_Z);
    }
  }
  else if (vk == VK_LEFT || vk == VK_RIGHT)
  {
    double dr, delta;

    delta = PI / 64.0;

    if (vk == VK_LEFT)
    {
      dr = delta;
    }
    else
    {
      dr = -delta;
    }

    dr = dr * (double)cRepeat;

    pLevel->view_rotation += dr;

    if (pLevel->view_rotation > PI * 2.0)
    {
      pLevel->view_rotation -= PI * 2.0;
    }

    if (pLevel->view_rotation < 0.0)
    {
      pLevel->view_rotation += PI * 2.0;
    }

    pLevel->view_rotation = delta * Round(pLevel->view_rotation / delta);

    Object_UpdateStatusWindow(hdc, &rc, pWndData, VIEW_STATUS_THETA);
  }

  Object_RegenerateView( rc.right - rc.left,
                         rc.bottom - rc.top,
                         pWndData );

  Object_DrawView(hdc, pWndData->hbmView, pWndData->bm_width, pWndData->bm_height);

  ReleaseDC(hwnd, hdc);

  return;
  
} // Object_OnChar

#endif
