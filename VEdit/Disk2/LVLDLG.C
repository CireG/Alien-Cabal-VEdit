/*******************************************************************

    lvldlg.c

    Routines for displaying and managing the levels dialog.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL CALLBACK __export ViewLevels_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam );

BOOL ViewLevels_CopyLevel(HWND hwnd, INT index);

VOID ViewLevels_PasteLevel(HWND hwnd, INT index);

VOID ViewLevels_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify );

BOOL ViewLevels_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

int EditLevelDialog(HWND hwndParent);

BOOL CALLBACK __export EditLevel_DlgProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam );

VOID EditLevel_OnCommand( HWND hwnd,
                          INT  id,
                          HWND hwndCtrl,
                          UINT codeNotify );

BOOL EditLevel_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);

int SelectLevelDialog(HWND hwndParent);

BOOL CALLBACK __export SelectLevel_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam );

VOID SelectLevel_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify );

BOOL SelectLevel_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);


//
//  Private data.
//

static BOOL bChange;
static WND_DATA *po;
static WND_DATA *pi;
static CHAR szNewLevelName[32];
static CHAR szNewSolText[MAX_LEVEL_TEXT];
static CHAR szNewEolText[MAX_LEVEL_TEXT];
static WORD wNewFlags;
static INT nNewBkgBitmapIndex;
static INT nNewSolBitmapIndex;
static INT nNewEolBitmapIndex;
static INT nTextContent;
static INT nListContent;
static CHAR szImportFileName[256];
static INT nInsertIndex;
static CHAR *szFilter = "GOB files (*.gob)\000*.gob\000"
                        "All Files (*.*)\000*.*\000";


//
//  Public functions.
//

/*******************************************************************

    NAME:       ViewLevelsDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int ViewLevelsDialog(HWND hwndParent, WND_DATA *pWndData)
{
  int nResult;
  FARPROC pfnProc;

  bChange = FALSE;

  po = pWndData;
  
  pfnProc = MakeProcInstance((FARPROC)ViewLevels_DlgProc, hInst);

  nResult = DialogBox( hInst,
                       IDD_VIEW_LEVELS,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance(pfnProc);

  if (bChange)
  {
    Object_DeleteUndoData(hwndParent, pWndData);
  }
    
  return nResult;
    
} // ViewLevelsDialog


//
//  Private functions.
//

/*******************************************************************

    NAME:       ViewLevels_CopyLevel

    SYNOPSIS:   Copy a level to the clipboard.

    ENTRY:      hwnd - Dialog box window handle.

                index - index of the level to be copied.

********************************************************************/

BOOL ViewLevels_CopyLevel(HWND hwnd, INT index)
{
  HANDLE hData;
  BYTE huge *pData;
  LEVEL_DATA *pLevel;
  DWORD dwSize;
  CB_INFO cb;
  HCURSOR hcurSave;

  pLevel = po->pLevelData + index;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  _fmemset(&cb, 0, sizeof(cb));

  _fstrcpy(cb.level_name, pLevel->level_name);
  _fstrcpy(cb.level_author, pLevel->level_author);
  _fstrcpy(cb.sol_text, pLevel->sol_text);
  _fstrcpy(cb.eol_text, pLevel->eol_text);

  cb.flags = pLevel->flags;
  cb.bkg_bitmap_index = pLevel->bkg_bitmap_index;
  cb.sol_bitmap_index = pLevel->sol_bitmap_index;
  cb.eol_bitmap_index = pLevel->eol_bitmap_index;

  cb.num_vertices = pLevel->nVertices;
  cb.num_polygons = pLevel->nPolygons;
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
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return FALSE;
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

  if (cb.num_vertices > 0)
  {
    dwSize = (DWORD)cb.num_vertices * (DWORD)sizeof(VERTEX);
    hmemcpy(pData, pLevel->pVertexData, dwSize);
    pData += dwSize;
  }

  if (cb.num_polygons > 0)
  {
    dwSize = (DWORD)cb.num_polygons * (DWORD)sizeof(POLYGON);
    hmemcpy(pData, pLevel->pPolygonData, dwSize);
    pData += dwSize;
  }

  if (cb.num_triggers > 0)
  {
    dwSize = (DWORD)cb.num_triggers * (DWORD)sizeof(TRIGGER);
    hmemcpy(pData, pLevel->pTriggerData, dwSize);
    pData += dwSize;
  }

  if (cb.num_motions > 0)
  {
    dwSize = (DWORD)cb.num_motions * (DWORD)sizeof(MOTION);
    hmemcpy(pData, pLevel->pMotionData, dwSize);
    pData += dwSize;
  }

  if (cb.num_groups > 0)
  {
    dwSize = (DWORD)cb.num_groups * (DWORD)sizeof(GROUP);
    hmemcpy(pData, pLevel->pGroupData, dwSize);
    pData += dwSize;
  }

  if (cb.num_marks > 0)
  {
    dwSize = (DWORD)cb.num_marks * (DWORD)sizeof(MARK);
    hmemcpy(pData, pLevel->pMarkData, dwSize);
    pData += dwSize;
  }

  GlobalUnlock(hData);

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

    return FALSE;
  }

  if (!EmptyClipboard())
  {
    CloseClipboard();
    GlobalFree(hData);
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to empty the clipboard" );

    return FALSE;
  }

  if (!SetClipboardData(uFormatCode, hData))
  {
    CloseClipboard();
    GlobalFree(hData);
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable set the clipboard data" );

    return FALSE;
  }

  CloseClipboard();
  SetCursor(hcurSave);

  return TRUE;

} // ViewLevels_CopyLevel


/*******************************************************************

    NAME:       ViewLevels_PasteLevel

    SYNOPSIS:   Replace the level with clipboard data.

    ENTRY:      hwnd - Dialog box window handle.

                index - index of the level to be copied.

********************************************************************/

VOID ViewLevels_PasteLevel(HWND hwnd, INT index)
{
  INT i;
  DWORD dwSize;
  HANDLE hData;
  BYTE huge *pData;
  LEVEL_DATA *pLevel;
  CB_INFO cb;
  VERTEX huge *pVertex;
  POLYGON huge *pPolygon;
  HCURSOR hcurSave;

  pLevel = po->pLevelData + index;

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

  if (cb.num_vertices > MAX_NUM_VERTICES ||
      cb.num_polygons > MAX_NUM_POLYGONS ||
      cb.num_triggers > MAX_TRIGGERS ||
      cb.num_motions > MAX_MOTIONS ||
      cb.num_groups > MAX_GROUPS ||
      cb.num_marks > MAX_MARKS)
  {
    GlobalUnlock(hData);
    CloseClipboard();
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Not enough array space" );

    return;
  }

  //
  //  Copy the clipboard data.
  //

  _fstrcpy(pLevel->level_name, cb.level_name);
  _fstrcpy(pLevel->level_author, cb.level_author);
  _fstrcpy(pLevel->sol_text, cb.sol_text);
  _fstrcpy(pLevel->eol_text, cb.eol_text);

  pLevel->flags = cb.flags;
  pLevel->bkg_bitmap_index = cb.bkg_bitmap_index;
  pLevel->sol_bitmap_index = cb.sol_bitmap_index;
  pLevel->eol_bitmap_index = cb.eol_bitmap_index;

  pLevel->nVertices = cb.num_vertices;
  pLevel->nPolygons = cb.num_polygons;
  pLevel->nTriggers = cb.num_triggers;
  pLevel->nMotions = cb.num_motions;
  pLevel->nGroups = cb.num_groups;
  pLevel->nMarks = cb.num_marks;

  if (cb.num_vertices > 0)
  {
    dwSize = (DWORD)sizeof(VERTEX) * (DWORD)cb.num_vertices;
    hmemcpy(pLevel->pVertexData, pData, dwSize);
    pData += dwSize;
  }

  if (cb.num_polygons > 0)
  {
    dwSize = (DWORD)sizeof(POLYGON) * (DWORD)cb.num_polygons;
    hmemcpy(pLevel->pPolygonData, pData, dwSize);
    pData += dwSize;
  }

  if (cb.num_triggers > 0)
  {
    dwSize = (DWORD)sizeof(TRIGGER) * (DWORD)cb.num_triggers;
    hmemcpy(pLevel->pTriggerData, pData, dwSize);
    pData += dwSize;
  }

  if (cb.num_motions > 0)
  {
    dwSize = (DWORD)sizeof(MOTION) * (DWORD)cb.num_motions;
    hmemcpy(pLevel->pMotionData, pData, dwSize);
    pData += dwSize;
  }

  if (cb.num_groups > 0)
  {
    dwSize = (DWORD)sizeof(GROUP) * (DWORD)cb.num_groups;
    hmemcpy(pLevel->pGroupData, pData, dwSize);
    pData += dwSize;
  }

  if (cb.num_marks > 0)
  {
    dwSize = (DWORD)sizeof(MARK) * (DWORD)cb.num_marks;
    hmemcpy(pLevel->pMarkData, pData, dwSize);
    pData += dwSize;
  }

  GlobalUnlock(hData);
  CloseClipboard();

  //
  //  Check for invalid indecies.
  //

  pVertex = pLevel->pVertexData;

  for(i = 0; i < pLevel->nVertices; ++i, ++pVertex)
  {
    if (pVertex->lifeform_index >= po->nLifeForms)
    {
      pVertex->lifeform_index = -1;
    }

    if (pVertex->item_index >= po->nItems)
    {
      pVertex->item_index = -1;
    }

    if (pVertex->sprite_index >= po->nSprites)
    {
      pVertex->sprite_index = -1;
    }

    if (pVertex->noise_index >= po->nNoises)
    {
      pVertex->noise_index = -1;
    }

    if (pVertex->trigger_index >= pLevel->nTriggers)
    {
      pVertex->trigger_index = -1;
    }

    if (pVertex->group_index >= pLevel->nGroups)
    {
      pVertex->group_index = -1;
    }
  }

  pPolygon = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i, ++pPolygon)
  {
    if (pPolygon->texture_index >= po->nTextures)
    {
      pPolygon->texture_index = -1;
    }

    if (pPolygon->animation_index >= po->nAnimations)
    {
      pPolygon->animation_index = -1;
    }

    if (pPolygon->damage_animation_index >= po->nAnimations)
    {
      pPolygon->damage_animation_index = -1;
    }

    if (pPolygon->group_index >= pLevel->nGroups)
    {
      pPolygon->group_index = -1;
    }

    if (pPolygon->motion_index >= pLevel->nMotions)
    {
      pPolygon->motion_index = -1;
    }
  }

  pLevel->bRebuildZone = TRUE;

  SetCursor(hcurSave);

  return;

} // ViewLevels_PasteLevel


/*******************************************************************

    NAME:       ResolveSound

    SYNOPSIS:   Resolve any resource requirements for the imported
                sound given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                index - The sound index.

    RETURNS:    The new sound index.

********************************************************************/

INT ResolveSound(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, INT index)
{
  INT i;
  DWORD dwSize;
  SOUND_DATA tmp;
  SOUND_DATA *p;

  if (index < 0 || index >= pSrcWndData->nSounds)
  {
    return -1;
  }

  //
  //  Create a temporary sound structure and resolve the resources.
  //

  _fmemcpy(&tmp, pSrcWndData->pSoundData + index, sizeof(tmp));

  //
  //  Try to find a sound matching the temporary sound structure.
  //

  for(i = 0; i < pDstWndData->nSounds; ++i)
  {
    p = pDstWndData->pSoundData + i;

    if (p->flags == tmp.flags)
    {
      if (p->length == tmp.length)
      {
        dwSize = (DWORD)p->length;

        if (dwSize > 65535L)
        {
          dwSize = 65535L;
        }

        if (_fmemcmp(p->data, tmp.data, (WORD)dwSize) == 0)
        {
          return i;
        }
      }
    }
  }

  //
  //  No matches so attempt to add the temporary sound structure.
  //

  if (pDstWndData->nSounds < MAX_SOUNDS)
  {
    i = pDstWndData->nSounds;
    p = pDstWndData->pSoundData + i;

    _fmemcpy(p, &tmp, sizeof(tmp));

    dwSize = (DWORD)p->length;

    p->data = (BYTE huge *)GlobalAllocPtr(GPTR, dwSize);

    if (p->data == NULL)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Out of memory" );

      return -1;
    }

    hmemcpy(p->data, tmp.data, dwSize);

    ++pDstWndData->nSounds;

    return i;
  }

  //
  //  No room left. The sound cannot be resolved.
  //

  return -1;
  
} // ResolveSound


/*******************************************************************

    NAME:       ResolveNoise

    SYNOPSIS:   Resolve any resource requirements for the imported
                noise given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                index - The noise index.

    RETURNS:    The new noise index.

********************************************************************/

INT ResolveNoise(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, INT index)
{
  INT i;
  NOISE_DATA tmp;
  NOISE_DATA *p;

  if (index < 0 || index >= pSrcWndData->nNoises)
  {
    return -1;
  }

  //
  //  Create a temporary noise structure and resolve the resources.
  //

  _fmemcpy(&tmp, pSrcWndData->pNoiseData + index, sizeof(tmp));

  tmp.sound_index = ResolveSound(pDstWndData, pSrcWndData, tmp.sound_index);

  //
  //  Try to find a noise matching the temporary noise structure.
  //

  for(i = 0; i < pDstWndData->nNoises; ++i)
  {
    p = pDstWndData->pNoiseData + i;

    if (p->delay == tmp.delay)
    {
      if ((INT)(p->radius + 0.5) == (INT)(tmp.radius + 0.5))
      {
        if ((INT)(p->height + 0.5) == (INT)(tmp.height + 0.5))
        {
          if (p->sound_index == tmp.sound_index)
          {
            return i;
          }
        }
      }
    }
  }

  //
  //  No matches so attempt to add the temporary noise structure.
  //

  if (pDstWndData->nNoises < MAX_NOISES)
  {
    i = pDstWndData->nNoises++;

    _fmemcpy(pDstWndData->pNoiseData + i, &tmp, sizeof(tmp));

    return i;
  }

  //
  //  No room left. The noise cannot be resolved.
  //

  return -1;
  
} // ResolveNoise


/*******************************************************************

    NAME:       ResolveBitmap

    SYNOPSIS:   Resolve any resource requirements for the imported
                bitmap given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                index - The bitmap index.

    RETURNS:    The new bitmap index.

********************************************************************/

INT ResolveBitmap(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, INT index)
{
  INT i;
  DWORD dwSize;
  BITMAP_DATA tmp;
  BITMAP_DATA *p;

  if (index < 0 || index >= pSrcWndData->nBitmaps)
  {
    return -1;
  }

  //
  //  Create a temporary bitmap structure and resolve the resources.
  //

  _fmemcpy(&tmp, pSrcWndData->pBitmapData + index, sizeof(tmp));

  //
  //  Try to find a bitmap matching the temporary bitmap structure.
  //

  for(i = 0; i < pDstWndData->nBitmaps; ++i)
  {
    p = pDstWndData->pBitmapData + i;

    if (p->flags == tmp.flags)
    {
      if (p->x_len == tmp.x_len)
      {
        if (p->y_len == tmp.y_len)
        {
          dwSize = (DWORD)p->x_len * (DWORD)p->y_len;

          if (dwSize > 65535L)
          {
            dwSize = 65535L;
          }

          if (_fmemcmp(p->data, tmp.data, (WORD)dwSize) == 0)
          {
            return i;
          }
        }
      }
    }
  }

  //
  //  No matches so attempt to add the temporary bitmap structure.
  //

  if (pDstWndData->nBitmaps < MAX_BITMAPS)
  {
    i = pDstWndData->nBitmaps;
    p = pDstWndData->pBitmapData + i;

    _fmemcpy(p, &tmp, sizeof(tmp));

    dwSize = (DWORD)p->x_len * (DWORD)p->y_len;

    p->data = (BYTE *)GlobalAllocPtr(GPTR, dwSize);

    if (p->data == NULL)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Out of memory" );

      return -1;
    }

    hmemcpy(p->data, tmp.data, dwSize);

    ++pDstWndData->nBitmaps;

    return i;
  }

  //
  //  No room left. The bitmap cannot be resolved.
  //

  return -1;
  
} // ResolveBitmap


/*******************************************************************

    NAME:       ResolveTexture

    SYNOPSIS:   Resolve any resource requirements for the imported
                texture given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                index - The texture index.

    RETURNS:    The new texture index.

********************************************************************/

INT ResolveTexture(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, INT index)
{
  INT i;
  TEXTURE_DATA tmp;
  TEXTURE_DATA *p;

  if (index < 0 || index >= pSrcWndData->nTextures)
  {
    return -1;
  }

  //
  //  Create a temporary texture structure and resolve the resources.
  //

  _fmemcpy(&tmp, pSrcWndData->pTextureData + index, sizeof(tmp));

  tmp.bitmap_index = ResolveBitmap(pDstWndData, pSrcWndData, tmp.bitmap_index);

  //
  //  Try to find a texture matching the temporary texture structure.
  //

  for(i = 0; i < pDstWndData->nTextures; ++i)
  {
    p = pDstWndData->pTextureData + i;

    if (p->flags == tmp.flags)
    {
      if (p->color_index == tmp.color_index)
      {
        if ((INT)(p->pixel_size * 10.0 + 0.5) == (INT)(tmp.pixel_size * 10.0 + 0.5))
        {
          if (p->bitmap_index == tmp.bitmap_index)
          {
            return i;
          }
        }
      }
    }
  }

  //
  //  No matches so attempt to add the temporary texture structure.
  //

  if (pDstWndData->nTextures < MAX_TEXTURES)
  {
    i = pDstWndData->nTextures++;

    _fmemcpy(pDstWndData->pTextureData + i, &tmp, sizeof(tmp));

    return i;
  }

  //
  //  No room left. The texture cannot be resolved.
  //

  return -1;
  
} // ResolveTexture


/*******************************************************************

    NAME:       ResolveAnimation

    SYNOPSIS:   Resolve any resource requirements for the imported
                animation given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                index - The animation index.

    RETURNS:    The new animation index.

********************************************************************/

INT ResolveAnimation(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, INT index)
{
  INT i, j;
  ANIMATION_DATA tmp;
  ANIMATION_DATA *p;

  if (index < 0 || index >= pSrcWndData->nAnimations)
  {
    return -1;
  }

  //
  //  Create a temporary animation structure and resolve the resources.
  //

  _fmemcpy(&tmp, pSrcWndData->pAnimationData + index, sizeof(tmp));

  for(i = 0; i < tmp.num_elements; ++i)
  {
    if (tmp.index[i] != -1)
    {
      if (tmp.index[i] & AF_SOUND_RESOURCE)
      {
        tmp.index[i] = ResolveSound(pDstWndData, pSrcWndData, tmp.index[i] & ~AF_SOUND_RESOURCE) | AF_SOUND_RESOURCE;
      }
      else
      {
        tmp.index[i] = ResolveBitmap(pDstWndData, pSrcWndData, tmp.index[i]);
      }
    }
  }

  //
  //  Try to find an animation matching the temporary animation structure.
  //

  for(i = 0; i < pDstWndData->nAnimations; ++i)
  {
    p = pDstWndData->pAnimationData + i;

    if (p->num_elements == tmp.num_elements)
    {
      if (p->first_element == tmp.first_element)
      {
        if (p->delay == tmp.delay)
        {
          if ((INT)(p->pixel_size * 10.0 + 0.5) == (INT)(tmp.pixel_size * 10.0 + 0.5))
          {
            for(j = 0; j < p->num_elements; ++j)
            {
              if (p->index[j] != tmp.index[j])
              {
                break;
              }
            }

            if (j == p->num_elements)
            {
              return i;
            }
          }
        }
      }
    }
  }

  //
  //  No matches so attempt to add the temporary animation structure.
  //

  if (pDstWndData->nAnimations < MAX_ANIMATIONS)
  {
    i = pDstWndData->nAnimations++;

    _fmemcpy(pDstWndData->pAnimationData + i, &tmp, sizeof(tmp));

    return i;
  }

  //
  //  No room left. The animation cannot be resolved.
  //

  return -1;
  
} // ResolveAnimation


/*******************************************************************

    NAME:       ResolveLifeform

    SYNOPSIS:   Resolve any resource requirements for the imported
                life form given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                index - The life form index.

    RETURNS:    The new life form index.

********************************************************************/

INT ResolveLifeform(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, INT index)
{
  INT i, j, num_elements, num_bitmaps;
  LIFEFORM_DATA tmp;
  LIFEFORM_DATA *p;

  if (index < 0 || index >= pSrcWndData->nLifeForms)
  {
    return -1;
  }

  //
  //  Create a temporary life form structure and resolve the resources.
  //

  _fmemcpy(&tmp, pSrcWndData->pLifeFormData + index, sizeof(tmp));

  num_elements = GetNumLifeFormElements(tmp.lifeform_type);
  num_bitmaps = GetNumLifeFormBitmaps(tmp.lifeform_type);

  for(i = 0; i < num_elements; ++i)
  {
    if (i < num_bitmaps)
    {
      tmp.index[i] = ResolveBitmap(pDstWndData, pSrcWndData, tmp.index[i]);
    }
    else
    {
      tmp.index[i] = ResolveSound(pDstWndData, pSrcWndData, tmp.index[i]);
    }
  }

  //
  //  Try to find a life form matching the temporary life form structure.
  //

  for(i = 0; i < pDstWndData->nLifeForms; ++i)
  {
    p = pDstWndData->pLifeFormData + i;

    if (p->lifeform_type == tmp.lifeform_type)
    {
      if ((INT)(p->pixel_size * 10.0 + 0.5) == (INT)(tmp.pixel_size * 10.0 + 0.5))
      {
        for(j = 0; j < num_elements; ++j)
        {
          if (p->index[j] != tmp.index[j])
          {
            break;
          }
        }

        if (j == num_elements)
        {
          return i;
        }
      }
    }
  }

  //
  //  No matches so attempt to add the temporary life form structure.
  //

  if (pDstWndData->nLifeForms < MAX_LIFEFORMS)
  {
    i = pDstWndData->nLifeForms++;

    _fmemcpy(pDstWndData->pLifeFormData + i, &tmp, sizeof(tmp));

    return i;
  }

  //
  //  No room left. The life form cannot be resolved.
  //

  return -1;
  
} // ResolveLifeform


/*******************************************************************

    NAME:       ResolveItem

    SYNOPSIS:   Resolve any resource requirements for the imported
                item given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                index - The item index.

    RETURNS:    The new item index.

********************************************************************/

INT ResolveItem(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, INT index)
{
  INT i, j, num_elements, num_bitmaps;
  ITEM_DATA tmp;
  ITEM_DATA *p;

  if (index < 0 || index >= pSrcWndData->nItems)
  {
    return -1;
  }

  //
  //  Create a temporary item structure and resolve the resources.
  //

  _fmemcpy(&tmp, pSrcWndData->pItemData + index, sizeof(tmp));

  num_elements = GetNumItemElements(tmp.item_type);
  num_bitmaps = GetNumItemBitmaps(tmp.item_type);

  for(i = 0; i < num_elements; ++i)
  {
    if (i < num_bitmaps)
    {
      tmp.index[i] = ResolveBitmap(pDstWndData, pSrcWndData, tmp.index[i]);
    }
    else
    {
      tmp.index[i] = ResolveSound(pDstWndData, pSrcWndData, tmp.index[i]);
    }
  }

  //
  //  Try to find an item matching the temporary item structure.
  //

  for(i = 0; i < pDstWndData->nItems; ++i)
  {
    p = pDstWndData->pItemData + i;

    if (p->item_type == tmp.item_type)
    {
      if ((INT)(p->pixel_size * 10.0 + 0.5) == (INT)(tmp.pixel_size * 10.0 + 0.5))
      {
        for(j = 0; j < num_elements; ++j)
        {
          if (p->index[j] != tmp.index[j])
          {
            break;
          }
        }

        if (j == num_elements)
        {
          return i;
        }
      }
    }
  }

  //
  //  No matches so attempt to add the temporary item structure.
  //

  if (pDstWndData->nItems < MAX_ITEMS)
  {
    i = pDstWndData->nItems++;

    _fmemcpy(pDstWndData->pItemData + i, &tmp, sizeof(tmp));

    return i;
  }

  //
  //  No room left. The item cannot be resolved.
  //

  return -1;
  
} // ResolveItem


/*******************************************************************

    NAME:       ResolveSprite

    SYNOPSIS:   Resolve any resource requirements for the imported
                sprite given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                index - The sprite index.

    RETURNS:    The new sprite index.

********************************************************************/

INT ResolveSprite(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, INT index)
{
  INT i;
  SPRITE_DATA tmp;
  SPRITE_DATA *p;

  if (index < 0 || index >= pSrcWndData->nSprites)
  {
    return -1;
  }

  //
  //  Create a temporary sprite structure and resolve the resources.
  //

  _fmemcpy(&tmp, pSrcWndData->pSpriteData + index, sizeof(tmp));

  if (tmp.flags & SF_ANIMATED)
  {
    tmp.animation_index = ResolveAnimation(pDstWndData, pSrcWndData, tmp.animation_index);
    tmp.bitmap_index = -1;
  }
  else
  {
    tmp.animation_index = -1;
    tmp.bitmap_index = ResolveBitmap(pDstWndData, pSrcWndData, tmp.bitmap_index);
  }

  if (tmp.flags & SF_DAMAGE_ANIMATED)
  {
    tmp.damage_animation_index = ResolveAnimation(pDstWndData, pSrcWndData, tmp.damage_animation_index);
  }
  else
  {
    tmp.damage_animation_index = -1;
  }

  if (tmp.flags & SF_DAMAGE_ANIMATED_2)
  {
    tmp.damage_animation_index_2 = ResolveAnimation(pDstWndData, pSrcWndData, tmp.damage_animation_index_2);
  }
  else
  {
    tmp.damage_animation_index_2 = -1;
  }

  //
  //  Try to find a sprite matching the temporary sprite structure.
  //

  for(i = 0; i < pDstWndData->nSprites; ++i)
  {
    p = pDstWndData->pSpriteData + i;

    if (p->flags == tmp.flags)
    {
      if ((INT)(p->pixel_size * 10.0 + 0.5) == (INT)(tmp.pixel_size * 10.0 + 0.5))
      {
        if (p->flags & SF_ANIMATED)
        {
          if (p->animation_index != tmp.animation_index)
          {
            continue;
          }
        }
        else if (p->bitmap_index != tmp.bitmap_index)
        {
          continue;
        }

        if (p->flags & SF_DAMAGE_ANIMATED)
        {
          if (p->damage_animation_index != tmp.damage_animation_index)
          {
            continue;
          }
        }

        if (p->flags & SF_DAMAGE_ANIMATED_2)
        {
          if (p->damage_animation_index_2 != tmp.damage_animation_index_2)
          {
            continue;
          }
        }

        return i;
      }
    }
  }

  //
  //  No matches so attempt to add the temporary sprite structure.
  //

  if (pDstWndData->nSprites < MAX_SPRITES)
  {
    i = pDstWndData->nSprites++;

    _fmemcpy(pDstWndData->pSpriteData + i, &tmp, sizeof(tmp));

    return i;
  }

  //
  //  No room left. The sprite cannot be resolved.
  //

  return -1;
  
} // ResolveSprite


/*******************************************************************

    NAME:       ResolveVertex

    SYNOPSIS:   Resolve any resource requirements for the imported
                vertex given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                pVertex - Vertex pointer.

********************************************************************/

VOID ResolveVertex(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, VERTEX huge *pVertex)
{
  if (pVertex->flags & VF_LIFEFORM)
  {
    pVertex->lifeform_index = ResolveLifeform(pDstWndData, pSrcWndData, pVertex->lifeform_index);
    pVertex->item_index = -1;
    pVertex->sprite_index = -1;
    pVertex->noise_index = -1;
  }
  else if (pVertex->flags & VF_ITEM)
  {
    pVertex->lifeform_index = -1;
    pVertex->item_index = ResolveItem(pDstWndData, pSrcWndData, pVertex->item_index);
    pVertex->sprite_index = -1;
    pVertex->noise_index = -1;
  }
  else if (pVertex->flags & VF_SPRITE)
  {
    pVertex->lifeform_index = -1;
    pVertex->item_index = -1;
    pVertex->sprite_index = ResolveSprite(pDstWndData, pSrcWndData, pVertex->sprite_index);
    pVertex->noise_index = -1;
  }
  else if (pVertex->flags & VF_NOISE)
  {
    pVertex->lifeform_index = -1;
    pVertex->item_index = -1;
    pVertex->sprite_index = -1;
    pVertex->noise_index = ResolveNoise(pDstWndData, pSrcWndData, pVertex->noise_index);
  }
  else
  {
    pVertex->lifeform_index = -1;
    pVertex->item_index = -1;
    pVertex->sprite_index = -1;
    pVertex->noise_index = -1;
  }

  return;
  
} // ResolveVertex


/*******************************************************************

    NAME:       ResolvePolygon

    SYNOPSIS:   Resolve any resource requirements for the imported
                polygon given.

    ENTRY:      pDstWndData - The destination window data pointer.

                pSrcWndData - The source window data pointer.
    
                pPolygon - Polygon pointer.

********************************************************************/

VOID ResolvePolygon(WND_DATA *pDstWndData, WND_DATA *pSrcWndData, POLYGON huge *pPolygon)
{
  if (pPolygon->flags & PF_INVISIBLE)
  {
    pPolygon->texture_index = -1;
    pPolygon->animation_index = -1;
    pPolygon->damage_animation_index = -1;
    pPolygon->flags &= ~(PF_ANIMATED | PF_DAMAGE_ANIMATED);
  }
  else
  {
    if (pPolygon->flags & (PF_ANIMATED | PF_MOTION_ANIMATED))
    {
      pPolygon->animation_index = ResolveAnimation(pDstWndData, pSrcWndData, pPolygon->animation_index);
      pPolygon->texture_index = -1;
    }
    else
    {
      pPolygon->texture_index = ResolveTexture(pDstWndData, pSrcWndData, pPolygon->texture_index);
      pPolygon->animation_index = -1;
    }

    if (pPolygon->flags & PF_DAMAGE_ANIMATED)
    {
      pPolygon->damage_animation_index = ResolveAnimation(pDstWndData, pSrcWndData, pPolygon->damage_animation_index);
    }
    else
    {
      pPolygon->damage_animation_index = -1;
    }
  }

  return;
  
} // ResolvePolygon


/*******************************************************************

    NAME:       GetImportFileName

    SYNOPSIS:   Show the open dialog box and allow the user to
                select a GOB file name to import.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

BOOL GetImportFileName(HWND hwndParent)
{
  INT n;
  BOOL bResult;
  OPENFILENAME ofn;
  CHAR szDirName[256], szFileTitle[256];

  _fstrcpy(szDirName, szPath);

  n = _fstrlen(szDirName);

  if (n > 0)
  {
    if (szDirName[n-1] == '\\')
    {
      szDirName[n-1] = '\0';
    }
  }
  
  szImportFileName[0] = '\0';

  _fmemset(&ofn, 0, sizeof(OPENFILENAME));

  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = hwndParent;
  ofn.hInstance = hInst;
  ofn.lpstrFilter = szFilter;
  ofn.nFilterIndex = 1;
  ofn.lpstrFile= szImportFileName;
  ofn.nMaxFile = sizeof(szImportFileName);
  ofn.lpstrFileTitle = szFileTitle;
  ofn.nMaxFileTitle = sizeof(szFileTitle);
  ofn.lpstrInitialDir = szDirName;
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

  bResult = GetOpenFileName(&ofn);
    
  if (!bResult)
  {
    DWORD dwError;
    
    dwError = CommDlgExtendedError();
    
    if (dwError)
    {
      MsgBox( hwndParent,
              MB_ICONEXCLAMATION,
              "Common dialog error %lu",
              dwError );
    }
              
    return FALSE;
  }
      
  return TRUE;
  
} // GetImportFileName


/*******************************************************************

    NAME:       ImportLevel

    SYNOPSIS:   Import the desired level from the import data.

    ENTRY:      hwndParent - The parent window for this dialog.
    
                index - Index at which to insert the level;

********************************************************************/

BOOL ImportLevel(HWND hwndParent, INT index)
{
  INT i;
  LEVEL_DATA *pLevel, *pTmpLevel;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

  //
  //  Insert a new level just before the desired index.
  //

  if (Object_InsertLevel(po, index) == FALSE)
  {
    SetCursor(hcurSave);
    return FALSE;
  }

  //
  //  Copy the level data to the new level.
  //

  pLevel = po->pLevelData + index;
  pTmpLevel = pi->pLevelData + pi->level_index;

  hmemcpy(pLevel->pVertexData, pTmpLevel->pVertexData, (DWORD)sizeof(VERTEX) * (DWORD)pTmpLevel->nVertices);
  hmemcpy(pLevel->pPolygonData, pTmpLevel->pPolygonData, (DWORD)sizeof(POLYGON) * (DWORD)pTmpLevel->nPolygons);
  hmemcpy(pLevel->pTriggerData, pTmpLevel->pTriggerData, (DWORD)sizeof(TRIGGER) * (DWORD)pTmpLevel->nTriggers);
  hmemcpy(pLevel->pMotionData, pTmpLevel->pMotionData, (DWORD)sizeof(MOTION) * (DWORD)pTmpLevel->nMotions);
  hmemcpy(pLevel->pGroupData, pTmpLevel->pGroupData, (DWORD)sizeof(GROUP) * (DWORD)pTmpLevel->nGroups);
  hmemcpy(pLevel->pMarkData, pTmpLevel->pMarkData, (DWORD)sizeof(MARK) * (DWORD)pTmpLevel->nMarks);

  pLevel->view_home_rotation = pTmpLevel->view_home_rotation;
  pLevel->view_home_elevation = pTmpLevel->view_home_elevation;
  pLevel->view_home_x = pTmpLevel->view_home_x;
  pLevel->view_home_y = pTmpLevel->view_home_y;
  pLevel->view_home_z = pTmpLevel->view_home_z;
  pLevel->view_rotation = pTmpLevel->view_rotation;
  pLevel->view_elevation = pTmpLevel->view_elevation;
  pLevel->view_x = pTmpLevel->view_x;
  pLevel->view_y = pTmpLevel->view_y;
  pLevel->view_z = pTmpLevel->view_z;
  pLevel->view_delta = pTmpLevel->view_delta;
  pLevel->view_depth = pTmpLevel->view_depth;
  pLevel->view_width = pTmpLevel->view_width;
  pLevel->view_height = pTmpLevel->view_height;
  pLevel->grid_x = pTmpLevel->grid_x;
  pLevel->grid_y = pTmpLevel->grid_y;
  pLevel->grid_z = pTmpLevel->grid_z;
  pLevel->grid_rotation = pTmpLevel->grid_rotation;
  pLevel->grid_elevation = pTmpLevel->grid_elevation;
  pLevel->grid_delta = pTmpLevel->grid_delta;
  pLevel->grid_spacing = pTmpLevel->grid_spacing;
  pLevel->grid_size = pTmpLevel->grid_size;
  pLevel->bMaintainGridDist = pTmpLevel->bMaintainGridDist;
  pLevel->bLockViewToGrid = pTmpLevel->bLockViewToGrid;
  pLevel->bSnapToGrid = pTmpLevel->bSnapToGrid;

  pLevel->nVertices = pTmpLevel->nVertices;
  pLevel->nPolygons = pTmpLevel->nPolygons;
  pLevel->nTriggers = pTmpLevel->nTriggers;
  pLevel->nMotions = pTmpLevel->nMotions;
  pLevel->nGroups = pTmpLevel->nGroups;
  pLevel->nMarks = pTmpLevel->nMarks;
  pLevel->nLastMark = pTmpLevel->nLastMark;

  pLevel->flags = pTmpLevel->flags;
  pLevel->bkg_bitmap_index = ResolveBitmap(po, pi, pTmpLevel->bkg_bitmap_index);
  pLevel->sol_bitmap_index = ResolveBitmap(po, pi, pTmpLevel->sol_bitmap_index);
  pLevel->eol_bitmap_index = ResolveBitmap(po, pi, pTmpLevel->eol_bitmap_index);

  _fstrcpy(pLevel->level_name, pTmpLevel->level_name);
  _fstrcpy(pLevel->level_author, pTmpLevel->level_author);
  _fstrcpy(pLevel->sol_text, pTmpLevel->sol_text);
  _fstrcpy(pLevel->eol_text, pTmpLevel->eol_text);

  //
  //  Resolve any vertex resources.
  //

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    ResolveVertex(po, pi, pLevel->pVertexData + i);
  }

  //
  //  Resolve any polygon resources.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    ResolvePolygon(po, pi, pLevel->pPolygonData + i);
  }

  //
  //  Resolve any motion resources.
  //

  for(i = 0; i < pLevel->nMotions; ++i)
  {
    pLevel->pMotionData[i].sound_index_start = ResolveSound(po, pi, pLevel->pMotionData[i].sound_index_start);
    pLevel->pMotionData[i].sound_index_run   = ResolveSound(po, pi, pLevel->pMotionData[i].sound_index_run);
    pLevel->pMotionData[i].sound_index_stop  = ResolveSound(po, pi, pLevel->pMotionData[i].sound_index_stop);
  }

  //
  //  Resolve any trigger resources.
  //

  for(i = 0; i < pLevel->nTriggers; ++i)
  {
    pLevel->pTriggerData[i].sound_index = ResolveSound(po, pi, pLevel->pTriggerData[i].sound_index);
  }

  //
  //  Return success.
  //

  SetCursor(hcurSave);

  return TRUE;
  
} // ImportLevel


/*******************************************************************

    NAME:       ViewLevels_ReloadListBox

    SYNOPSIS:   Reload the list box with level names.

    ENTRY:      hwndList - List box window handle.

********************************************************************/

VOID ViewLevels_ReloadListBox(HWND hwndList)
{
  INT i, j;

  j = ListBox_GetTopIndex(hwndList);

  ListBox_ResetContent(hwndList);

  for(i = 0; i < po->nLevels; ++i)
  {
    ListBox_AddString(hwndList, po->pLevelData[i].level_name);
  }

  ListBox_AddString(hwndList, "");

  ListBox_SetTopIndex(hwndList, j);

  return;

} // ViewLevels_ReloadListBox


/*******************************************************************

    NAME:       ViewLevels_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export ViewLevels_DlgProc( HWND   hwnd,
                                           UINT   nMessage,
                                           WPARAM wParam,
                                           LPARAM lParam )
{
  switch(nMessage)
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    ViewLevels_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, ViewLevels_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor   );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy    );
  }

  return FALSE;

} // ViewLevels_DlgProc


/*******************************************************************

    NAME:       ViewLevels_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/

VOID ViewLevels_OnCommand( HWND hwnd,
                           INT  id,
                           HWND hwndCtrl,
                           UINT codeNotify )
{
  INT i;
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDC_NEW_LEVEL:

      if (po->nLevels == MAX_LEVELS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d levels has been reached.",
                MAX_LEVELS );

        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_LEVEL_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult < 0 || lResult > po->nLevels)
      {
        break;
      }

      i = (INT)lResult;

      _fmemset(szNewLevelName, 0, sizeof(szNewLevelName));
      _fmemset(szNewSolText, 0, sizeof(szNewSolText));
      _fmemset(szNewEolText, 0, sizeof(szNewEolText));

      wNewFlags = 0;
      nNewBkgBitmapIndex = -1;
      nNewSolBitmapIndex = -1;
      nNewEolBitmapIndex = -1;

      if (EditLevelDialog(hwnd) == IDCANCEL)
      {
        break;
      }

      if (Object_InsertLevel(po, i) == FALSE)
      {
        break;
      }

      _fstrcpy(po->pLevelData[i].level_name, szNewLevelName);
      _fstrcpy(po->pLevelData[i].sol_text, szNewSolText);
      _fstrcpy(po->pLevelData[i].eol_text, szNewEolText);

      po->pLevelData[i].flags = wNewFlags;
      po->pLevelData[i].bkg_bitmap_index = nNewBkgBitmapIndex;
      po->pLevelData[i].sol_bitmap_index = nNewSolBitmapIndex;
      po->pLevelData[i].eol_bitmap_index = nNewEolBitmapIndex;

      ViewLevels_ReloadListBox(hwndList);

      ListBox_SetCurSel(hwndList, i);

      bChange = TRUE;

      break;

    case IDC_IMPORT_LEVEL:

      if (po->nLevels == MAX_LEVELS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d levels has been reached.",
                MAX_LEVELS );

        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_LEVEL_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult <= po->nLevels)
      {
        i = (INT)lResult;

        if (GetImportFileName(hwnd))
        {
          pi = Frame_CreateWindowData(0);

          if (pi)
          {
            if (LoadObjectFile(szImportFileName, pi))
            {
              if (SelectLevelDialog(hwnd) != IDCANCEL)
              {
                InvalidateRect(hwnd, NULL, FALSE);
                UpdateWindow(hwnd);

                if (ImportLevel(hwnd, i))
                {
                  ViewLevels_ReloadListBox(hwndList);
                  ListBox_SetCurSel(hwndList, i + 1);
                }

                bChange = TRUE;
              }
            }

            Frame_FreeWindowData(pi);
          }
        }
      }

      break;

    case IDC_CUT_LEVEL:

      hwndList = GetDlgItem(hwnd, IDC_LEVEL_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < po->nLevels)
      {
        i = (INT)lResult;

        if (ViewLevels_CopyLevel(hwnd, i))
        {
          EnableWindow(GetDlgItem(hwnd, IDC_PASTE_LEVEL), TRUE);
        }

        Object_DeleteLevel(po, i);

        //
        //  Note: The last level is not actually deleted, it is just
        //        cleared out, and the name is changed to "Untitled".
        //

        ViewLevels_ReloadListBox(hwndList);

        if (i == 0)
        {
          ListBox_SetCurSel(hwndList, 0);
        }
        else
        {
          ListBox_SetCurSel(hwndList, i - 1);
        }

        bChange = TRUE;
      }

      break;

    case IDC_COPY_LEVEL:

      hwndList = GetDlgItem(hwnd, IDC_LEVEL_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < po->nLevels)
      {
        i = (INT)lResult;

        if (ViewLevels_CopyLevel(hwnd, i))
        {
          EnableWindow(GetDlgItem(hwnd, IDC_PASTE_LEVEL), TRUE);
        }
      }

      break;

    case IDC_PASTE_LEVEL:

      if (po->nLevels == MAX_LEVELS)
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The limit of %d levels has been reached.",
                MAX_LEVELS );

        break;
      }

      hwndList = GetDlgItem(hwnd, IDC_LEVEL_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult < 0 || lResult > po->nLevels)
      {
        break;
      }

      i = (INT)lResult;

      if (Object_InsertLevel(po, i) == FALSE)
      {
        break;
      }

      ViewLevels_PasteLevel(hwnd, i);

      ViewLevels_ReloadListBox(hwndList);

      ListBox_SetCurSel(hwndList, i);

      bChange = TRUE;

      break;

    case IDC_DELETE_LEVEL:

      hwndList = GetDlgItem(hwnd, IDC_LEVEL_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < po->nLevels)
      {
        i = (INT)lResult;

        Object_DeleteLevel(po, i);

        //
        //  Note: The last level is not actually deleted, it is just
        //        cleared out, and the name is changed to "Untitled".
        //

        ViewLevels_ReloadListBox(hwndList);

        if (i == 0)
        {
          ListBox_SetCurSel(hwndList, 0);
        }
        else
        {
          ListBox_SetCurSel(hwndList, i - 1);
        }

        bChange = TRUE;
      }

      break;

    case IDC_EDIT_LEVEL:

      hwndList = GetDlgItem(hwnd, IDC_LEVEL_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < po->nLevels)
      {
        i = (INT)lResult;

        _fstrcpy(szNewLevelName, po->pLevelData[i].level_name);
        _fstrcpy(szNewSolText, po->pLevelData[i].sol_text);
        _fstrcpy(szNewEolText, po->pLevelData[i].eol_text);

        wNewFlags = po->pLevelData[i].flags;
        nNewBkgBitmapIndex = po->pLevelData[i].bkg_bitmap_index;
        nNewSolBitmapIndex = po->pLevelData[i].sol_bitmap_index;
        nNewEolBitmapIndex = po->pLevelData[i].eol_bitmap_index;

        if (EditLevelDialog(hwnd) == IDCANCEL)
        {
          break;
        }

        _fstrcpy(po->pLevelData[i].level_name, szNewLevelName);
        _fstrcpy(po->pLevelData[i].sol_text, szNewSolText);
        _fstrcpy(po->pLevelData[i].eol_text, szNewEolText);

        po->pLevelData[i].flags = wNewFlags;
        po->pLevelData[i].bkg_bitmap_index = nNewBkgBitmapIndex;
        po->pLevelData[i].sol_bitmap_index = nNewSolBitmapIndex;
        po->pLevelData[i].eol_bitmap_index = nNewEolBitmapIndex;

        ViewLevels_ReloadListBox(hwndList);

        ListBox_SetCurSel(hwndList, i);

        bChange = TRUE;
      }

      break;
      
    case IDOK: case IDCANCEL:

      if (hPalCommon)
      {
        DeleteObject(hPalCommon);
        hPalCommon = NULL;
      }

      if (id == IDOK)
      {
        hwndList = GetDlgItem(hwnd, IDC_LEVEL_LIST);

        lResult = ListBox_GetCurSel(hwndList);

        if (lResult >= 0 && lResult < po->nLevels)
        {
          po->level_index = (INT)lResult;
        }

        bChange = TRUE;
      }

      if (po->level_index < 0 || po->level_index >= po->nLevels)
      {
        po->level_index = 0;
      }

      EndDialog(hwnd, id);
      break;
  }
  
} // ViewLevels_OnCommand


/*******************************************************************

    NAME:       ViewLevels_OnInitDialog

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

BOOL ViewLevels_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_PASTE_LEVEL);

  if (IsClipboardFormatAvailable(uFormatCode) == FALSE)
  {
    EnableWindow(hwndCtrl, FALSE);
  }

  hwndCtrl = GetDlgItem(hwnd, IDC_LEVEL_LIST);

  ViewLevels_ReloadListBox(hwndCtrl);

  ListBox_SetCurSel(hwndCtrl, po->level_index);

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // ViewLevels_OnInitDialog


/*******************************************************************

    NAME:       EditLevelDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int EditLevelDialog(HWND hwndParent)
{
  int nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance((FARPROC)EditLevel_DlgProc, hInst);

  nResult = DialogBox( hInst,
                       IDD_EDIT_LEVEL,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance(pfnProc);
    
  return nResult;
    
} // EditLevelDialog


/*******************************************************************

    NAME:       EditLevel_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export EditLevel_DlgProc( HWND   hwnd,
                                          UINT   nMessage,
                                          WPARAM wParam,
                                          LPARAM lParam )
{
  switch(nMessage)
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    EditLevel_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, EditLevel_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor  );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy   );
  }

  return FALSE;

} // EditLevel_DlgProc


/*******************************************************************

    NAME:       EditLevel_InitList

    SYNOPSIS:   Initialize the list box and label depending on the
                current list content.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditLevel_InitList(HWND hwnd)
{
  INT i, nBmpIndex;
  LPSTR lpszLabel;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_LEVEL_LIST);

  ListBox_ResetContent(hwndCtrl);

  for(i = 0; i < po->nBitmaps; ++i)
  {
    ListBox_AddString(hwndCtrl, po->pBitmapData[i].bitmap_name);
  }

  if (nListContent == 0)
  {
    lpszLabel = "Bkg Bitmap:";
    nBmpIndex = nNewBkgBitmapIndex;
  }
  else if (nListContent == 1)
  {
    lpszLabel = "SOL Bitmap:";
    nBmpIndex = nNewSolBitmapIndex;
  }
  else
  {
    lpszLabel = "EOL Bitmap:";
    nBmpIndex = nNewEolBitmapIndex;
  }

  if (nBmpIndex != -1)
  {
    ListBox_SetCurSel(hwndCtrl, nBmpIndex);
  }

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_LEVEL_LIST_STATIC);
  Static_SetText(hwndCtrl, lpszLabel);

  if (nBmpIndex >= 0 && nBmpIndex < po->nBitmaps)
  {
    PreviewBitmap( hwnd,
                   IDC_EDIT_LEVEL_BITMAP_PREVIEW,
                   IDC_EDIT_LEVEL_BITMAP_SIZE,
                   &po->pBitmapData[nBmpIndex],
                   po->pPalette );
  }
  else
  {
    PreviewBitmap( hwnd,
                   IDC_EDIT_LEVEL_BITMAP_PREVIEW,
                   IDC_EDIT_LEVEL_BITMAP_SIZE,
                   NULL,
                   NULL );
  }

  return;

} // EditLevel_InitList


/*******************************************************************

    NAME:       EditLevel_InitText

    SYNOPSIS:   Initialize the text box and label depending on the
                current text content.

    ENTRY:      hwnd - The dialog box window handle. 

********************************************************************/

VOID EditLevel_InitText(HWND hwnd)
{
  LPSTR lpszLabel;
  LPSTR lpszText;
  HWND hwndCtrl;

  if (nTextContent == 0)
  {
    lpszLabel = "Start of Level Text:";
    lpszText = szNewSolText;
  }
  else
  {
    lpszLabel = "End of Level Text:";
    lpszText = szNewEolText;
  }

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_LEVEL_TEXT_STATIC);
  Static_SetText(hwndCtrl, lpszLabel);

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_LEVEL_TEXT);
  Edit_SetText(hwndCtrl, lpszText);

  return;

} // EditLevel_InitText


/*******************************************************************

    NAME:       EditLevel_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/


VOID EditLevel_OnCommand( HWND hwnd,
                          INT  id,
                          HWND hwndCtrl,
                          UINT codeNotify )
{
  INT i;
  LRESULT lResult;

  switch(id)
  {
    case IDC_EDIT_LEVEL_TEXT:

      if (nTextContent == 0)
      {
        if (codeNotify == EN_CHANGE)
        {
          SendMessage( hwndCtrl,
                       WM_GETTEXT,
                       (WPARAM)sizeof(szNewSolText),
                       (LPARAM)((LPSTR)szNewSolText) );
        }
      }
      else
      {
        if (codeNotify == EN_CHANGE)
        {
          SendMessage( hwndCtrl,
                       WM_GETTEXT,
                       (WPARAM)sizeof(szNewEolText),
                       (LPARAM)((LPSTR)szNewEolText) );
        }
      }
      break;

    case IDC_EDIT_LEVEL_LIST:

      if (codeNotify == LBN_SELCHANGE)
      {
        lResult = ListBox_GetCurSel(hwndCtrl);

        i = (INT)lResult;

        if (i >= 0 && i < po->nBitmaps)
        {
          if (nListContent == 0)
          {
            nNewBkgBitmapIndex = i;
          }
          else if (nListContent == 1)
          {
            nNewSolBitmapIndex = i;
          }
          else
          {
            nNewEolBitmapIndex = i;
          }

          PreviewBitmap( hwnd,
                         IDC_EDIT_LEVEL_BITMAP_PREVIEW,
                         IDC_EDIT_LEVEL_BITMAP_SIZE,
                         &po->pBitmapData[i],
                         po->pPalette );
        }
        else
        {
          PreviewBitmap( hwnd,
                         IDC_EDIT_LEVEL_BITMAP_PREVIEW,
                         IDC_EDIT_LEVEL_BITMAP_SIZE,
                         NULL,
                         NULL );
        }
      }
      break;

    case IDC_EDIT_LEVEL_NAME:

      if (codeNotify == EN_CHANGE)
      {
        SendMessage( hwndCtrl,
                     WM_GETTEXT,
                     (WPARAM)sizeof(szNewLevelName),
                     (LPARAM)((LPSTR)szNewLevelName) );
      }
      break;
      
    case IDC_EDIT_LEVEL_SOL_TEXT:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~LF_SOL_TEXT;
        }
        else
        {
          wNewFlags |= LF_SOL_TEXT;
          nTextContent = 0;
          EditLevel_InitText(hwnd);
        }
      }
      break;
      
    case IDC_EDIT_LEVEL_EOL_TEXT:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~LF_EOL_TEXT;
        }
        else
        {
          wNewFlags |= LF_EOL_TEXT;
          nTextContent = 1;
          EditLevel_InitText(hwnd);
        }
      }
      break;
      
    case IDC_EDIT_LEVEL_BKG_BITMAP:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~LF_BKG_BITMAP;
        }
        else
        {
          wNewFlags |= LF_BKG_BITMAP;
          nListContent = 0;
          EditLevel_InitList(hwnd);
        }
      }
      break;
      
    case IDC_EDIT_LEVEL_SOL_BITMAP:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~LF_SOL_BITMAP;
        }
        else
        {
          wNewFlags |= LF_SOL_BITMAP;
          nListContent = 1;
          EditLevel_InitList(hwnd);
        }
      }
      break;
      
    case IDC_EDIT_LEVEL_EOL_BITMAP:

      if (codeNotify == BN_CLICKED)
      {
        i = (int)SendMessage(hwndCtrl, BM_GETCHECK, (WPARAM)0, (LPARAM)0);

        if (i == 0)
        {
          wNewFlags &= ~LF_EOL_BITMAP;
        }
        else
        {
          wNewFlags |= LF_EOL_BITMAP;
          nListContent = 2;
          EditLevel_InitList(hwnd);
        }
      }
      break;
      
    case IDC_EDIT_LEVEL_SHOW_BITMAPS:

      if (++nListContent == 3)
      {
        nListContent = 0;
      }

      EditLevel_InitList(hwnd);
      break;
      
    case IDC_EDIT_LEVEL_SHOW_TEXT:

      if (++nTextContent == 2)
      {
        nTextContent = 0;
      }

      EditLevel_InitText(hwnd);
      break;
      
    case IDOK:

      if (!szNewLevelName[0])
      {
        MsgBox( hwnd,
                MB_ICONEXCLAMATION,
                "The level must be given a name." );

        break;
      }

      EndDialog(hwnd, id);
      break;
      
    case IDCANCEL:

      EndDialog(hwnd, id);
      break;
  }
  
} // EditLevel_OnCommand


/*******************************************************************

    NAME:       EditLevel_OnInitDialog

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

BOOL EditLevel_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_LEVEL_NAME);
  Edit_LimitText(hwndCtrl, 30);
  Edit_SetText(hwndCtrl, szNewLevelName);

  hwndCtrl = GetDlgItem(hwnd, IDC_EDIT_LEVEL_TEXT);
  Edit_LimitText(hwndCtrl, MAX_LEVEL_TEXT - 1);

  nListContent = 0;
  EditLevel_InitList(hwnd);

  nTextContent = 0;
  EditLevel_InitText(hwnd);

  CheckDlgButton( hwnd,
                  IDC_EDIT_LEVEL_BKG_BITMAP,
                  (wNewFlags & LF_BKG_BITMAP) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_LEVEL_SOL_TEXT,
                  (wNewFlags & LF_SOL_TEXT) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_LEVEL_EOL_TEXT,
                  (wNewFlags & LF_EOL_TEXT) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_LEVEL_SOL_BITMAP,
                  (wNewFlags & LF_SOL_BITMAP) ? 1 : 0 );

  CheckDlgButton( hwnd,
                  IDC_EDIT_LEVEL_EOL_BITMAP,
                  (wNewFlags & LF_EOL_BITMAP) ? 1 : 0 );

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // EditLevel_OnInitDialog


/*******************************************************************

    NAME:       SelectLevelDialog

    SYNOPSIS:   Displays the dialog box.

    ENTRY:      hwndParent - The parent window for this dialog.

********************************************************************/

int SelectLevelDialog(HWND hwndParent)
{
  int nResult;
  FARPROC pfnProc;
  
  pfnProc = MakeProcInstance((FARPROC)SelectLevel_DlgProc, hInst);

  nResult = DialogBox( hInst,
                       IDD_SELECT_LEVEL,
                       hwndParent,
                       (DLGPROC)pfnProc );

  FreeProcInstance(pfnProc);
    
  return nResult;
    
} // SelectLevelDialog


/*******************************************************************

    NAME:       SelectLevel_DlgProc

    SYNOPSIS:   Dialog procedure for the dialog box.

    ENTRY:      hwnd - Dialog box handle.

                nMessage - The message.

                wParam - The first message parameter.

                lParam - The second message parameter.

    RETURNS:    BOOL - TRUE if we handle the message, FALSE otherwise.

********************************************************************/

BOOL CALLBACK __export SelectLevel_DlgProc( HWND   hwnd,
                                            UINT   nMessage,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
  switch(nMessage)
  {
    HANDLE_MSG_VOID( hwnd, WM_COMMAND,    SelectLevel_OnCommand    );
    HANDLE_MSG_BOOL( hwnd, WM_INITDIALOG, SelectLevel_OnInitDialog );
    HANDLE_MSG_BOOL( hwnd, WM_CTLCOLOR,   AllDialogs_OnCtlColor    );
    HANDLE_MSG_VOID( hwnd, WM_DESTROY,    AllDialogs_OnDestroy     );
  }

  return FALSE;

} // SelectLevel_DlgProc


/*******************************************************************

    NAME:       SelectLevel_OnCommand

    SYNOPSIS:   Handles WM_COMMAND messages sent to the dialog box.

    ENTRY:      hwnd - Dialog box window handle.

                id - Identifies the menu/control/accelerator.

                hwndCtrl - Identifies the control sending the command.

                codeNotify - A notification code.  Will be zero for
                    menus, one for accelerators.

********************************************************************/

VOID SelectLevel_OnCommand( HWND hwnd,
                            INT  id,
                            HWND hwndCtrl,
                            UINT codeNotify )
{
  HWND hwndList;
  LRESULT lResult;

  switch(id)
  {
    case IDOK:

      if (hPalCommon)
      {
        DeleteObject(hPalCommon);
        hPalCommon = NULL;
      }

      hwndList = GetDlgItem(hwnd, IDC_SELECT_LEVEL_LIST);

      lResult = ListBox_GetCurSel(hwndList);

      if (lResult >= 0 && lResult < pi->nLevels)
      {
        pi->level_index = (INT)lResult;
      }
      else
      {
        EndDialog(hwnd, IDCANCEL);
        break;
      }

      EndDialog(hwnd, id);
      break;
      
    case IDCANCEL:

      if (hPalCommon)
      {
        DeleteObject(hPalCommon);
        hPalCommon = NULL;
      }

      EndDialog(hwnd, id);
      break;
  }
  
} // SelectLevel_OnCommand


/*******************************************************************

    NAME:       SelectLevel_OnInitDialog

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

BOOL SelectLevel_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
  INT i;
  HWND hwndCtrl;

  hwndCtrl = GetDlgItem(hwnd, IDC_SELECT_LEVEL_LIST);

  ListBox_ResetContent(hwndCtrl);

  for(i = 0; i < pi->nLevels; ++i)
  {
    ListBox_AddString(hwndCtrl, pi->pLevelData[i].level_name);
  }

  ListBox_SetCurSel(hwndCtrl, 0);

  AllDialogs_OnInitDialog(hwnd, hwndFocus, lParam);

  return TRUE;

} // SelectLevel_OnInitDialog
