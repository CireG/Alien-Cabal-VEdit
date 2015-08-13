/*******************************************************************

    write.c

    Routines for writing graphical object (GOB) files.

********************************************************************/


#include "vedit.h"


//
//  Private prototypes.
//

BOOL WriteFileInfo(INT fh, WND_DATA *pWndData);

BOOL WritePaletteData(INT fh, WND_DATA *pWndData);

BOOL WriteBitmapData(INT fh, BITMAP_DATA *pBitmap);

BOOL WriteTextureData(INT fh, TEXTURE_DATA *pTexture);

BOOL WriteSoundData(INT fh, SOUND_DATA *pSound);

BOOL WriteAnimationData(INT fh, ANIMATION_DATA *pAnimation);

BOOL WriteLifeFormData(INT fh, LIFEFORM_DATA *pLifeForm);

BOOL WriteItemData(INT fh, ITEM_DATA *pItem);

BOOL WriteSpriteData(INT fh, SPRITE_DATA *pSprite);

BOOL WriteNoiseData(INT fh, NOISE_DATA *pNoise);

BOOL WriteLevelData(INT fh, LEVEL_DATA *pLevel);

BOOL WriteZoneData(INT fh, ZONE_DATA *pZone);

BOOL CheckLevelData(WND_DATA *pWndData, INT index);

void GetZoneDataVertices(LEVEL_DATA *pLevel, POLYGON *p, ZONE_DATA_VERTEX *zdv);

void AdjustBitmapVertices(INT xlen, INT ylen, INT n, ZONE_DATA_VERTEX *zdv);

BOOL CreateZonePolygon(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_POLYGON *zdp);

BOOL CreateZoneSprite(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_SPRITE *zds);

BOOL CreateZoneLifeForm(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_LIFEFORM *zdl);

BOOL CreateZoneItem(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_ITEM *zdi);

BOOL CreateZoneNoise(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_NOISE *zdn);

BOOL CreateZoneTrigger(LEVEL_DATA *pLevel, INT i, ZONE_DATA_TRIGGER *zdt);

BOOL CreateZoneMotion(LEVEL_DATA *pLevel, INT i, ZONE_DATA_MOTION *zdm);

BOOL CreateZoneData(WND_DATA *pWndData, INT index);


//
//  Public functions.
//

/*******************************************************************

    NAME:       RebuildZoneData

    SYNOPSIS:   Rebuild zone data for all levels.

    ENTRY:      pWndData - The window data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL RebuildZoneData(WND_DATA *pWndData)
{
  INT i;
  HCURSOR hcurSave;

  //
  //  Check the level data first.
  //

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    if (CheckLevelData(pWndData, i) == FALSE)
    {
      pWndData->level_index = i;
      return FALSE;
    }
  }

  hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

  //
  //  Delete any duplicate vertices or polygons.
  //

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    DeleteDuplicateVertices(pWndData->pLevelData + i, FALSE);
    DeleteDuplicatePolygons(pWndData->pLevelData + i);
  }

  //
  //  Reset the pixel sizes.
  //

  Object_ResetPixelSize(NULL, pWndData);

  //
  //  Build the zone data.
  //

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    if (pWndData->pLevelData[i].bRebuildZone)
    {
      if (CreateZoneData(pWndData, i) == FALSE)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Not enough memory to create zone data for %s",
                (LPSTR)pWndData->pLevelData[i].level_name );

        continue;
      }
    }
  }

  SetCursor(hcurSave);

  return TRUE;
  
} // RebuildZoneData


/*******************************************************************

    NAME:       SaveObjectFile

    SYNOPSIS:   Save the object file given.

    ENTRY:      lpszFile - Full DOS name (with drive and path) of
                  the object file.
    
                pWndData - Object window to be saved.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL SaveObjectFile(LPSTR lpszFile, WND_DATA *pWndData)
{
  INT i;
  HFILE fh;
  HCURSOR hcurSave;

  //
  //  Make sure the level data is ok first.
  //

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    if (pWndData->pZoneData[i].data == NULL)
    {
      pWndData->pLevelData[i].bRebuildZone = TRUE;
    }

    if (pWndData->pLevelData[i].bRebuildZone)
    {
      if (CheckLevelData(pWndData, i) == FALSE)
      {
        pWndData->level_index = i;
        return FALSE;
      }
    }
  }

  //
  //  Create the file.
  //

  fh = _lcreat(lpszFile, 0);

  if (fh == HFILE_ERROR)
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to create %s",
            (LPSTR)lpszFile );

    return FALSE;
  }

  hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

  //
  //  Save the file information.
  //

  if (WriteFileInfo(fh, pWndData) == FALSE)
  {
    _lclose(fh);
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to write to %s",
            (LPSTR)lpszFile );

    return FALSE;
  }

  //
  //  Save the palette data.
  //

  if (WritePaletteData(fh, pWndData) == FALSE)
  {
    _lclose(fh);
    SetCursor(hcurSave);

    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to write to %s",
            (LPSTR)lpszFile );

    return FALSE;
  }

  //
  //  Save the bitmap data.
  //

  for(i = 0; i < pWndData->nBitmaps; ++i)
  {
    if (WriteBitmapData(fh, pWndData->pBitmapData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save the texture data.
  //

  for(i = 0; i < pWndData->nTextures; ++i)
  {
    if (WriteTextureData(fh, pWndData->pTextureData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save the sound data.
  //

  for(i = 0; i < pWndData->nSounds; ++i)
  {
    if (WriteSoundData(fh, pWndData->pSoundData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save the animation data.
  //

  for(i = 0; i < pWndData->nAnimations; ++i)
  {
    if (WriteAnimationData(fh, pWndData->pAnimationData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save the life form data.
  //

  for(i = 0; i < pWndData->nLifeForms; ++i)
  {
    if (WriteLifeFormData(fh, pWndData->pLifeFormData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save the item data.
  //

  for(i = 0; i < pWndData->nItems; ++i)
  {
    if (WriteItemData(fh, pWndData->pItemData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save the sprite data.
  //

  for(i = 0; i < pWndData->nSprites; ++i)
  {
    if (WriteSpriteData(fh, pWndData->pSpriteData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save the noise data.
  //

  for(i = 0; i < pWndData->nNoises; ++i)
  {
    if (WriteNoiseData(fh, pWndData->pNoiseData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save each level data structure.
  //

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    if (WriteLevelData(fh, pWndData->pLevelData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Save each level data structure as zone data.
  //

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    if (pWndData->pLevelData[i].bRebuildZone)
    {
      if (CreateZoneData(pWndData, i) == FALSE)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Not enough memory to create zone data for %s",
                (LPSTR)pWndData->pLevelData[i].level_name );

        continue;
      }
    }

    if (WriteZoneData(fh, pWndData->pZoneData + i) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to write to %s",
              (LPSTR)lpszFile );

      return FALSE;
    }
  }

  //
  //  Close the file.
  //

  _lclose(fh);
  SetCursor(hcurSave);

  return TRUE;
  
} // SaveObjectFile


//
//  Private functions.
//

/*******************************************************************

    NAME:       WriteFileInfo

    SYNOPSIS:   Write file information.

    ENTRY:      fh - Destination file handle.
    
                pWndData - Window data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteFileInfo(INT fh, WND_DATA *pWndData)
{
  FILE_INFO_HDR hdr;
  struct dosdate_t dos_date;

  memset(&hdr, 0, sizeof(hdr));

  hdr.id = ID_FILE_INFO;
  hdr.size = sizeof(hdr);

  _fstrcpy(hdr.name, pWndData->file_name);
  _fstrcpy(hdr.author, pWndData->file_author);

#if 1

  _fstrcpy(hdr.version, szVersion);

  _dos_getdate(&dos_date);

  wsprintf( hdr.date,
            "%02d/%02d/%02d",
            dos_date.month,
            dos_date.day,
            dos_date.year % 100 );

#else

  _fstrcpy(hdr.date, pWndData->file_date);
  _fstrcpy(hdr.version, pWndData->file_version);

#endif

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteFileInfo


/*******************************************************************

    NAME:       WritePaletteData

    SYNOPSIS:   Write palette data.

    ENTRY:      fh - Destination file handle.
    
                pWndData - Window data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WritePaletteData(INT fh, WND_DATA *pWndData)
{
  INT i;
  PALETTE_DATA_HDR hdr;

  //
  //  Save the palette data.
  //

  hdr.id = ID_PALETTE_DATA;
  hdr.size = sizeof(hdr);

  for(i = 0; i < 256; ++i)
  {
    hdr.red[i] = pWndData->pPalette->red[i];
    hdr.green[i] = pWndData->pPalette->green[i];
    hdr.blue[i] = pWndData->pPalette->blue[i];
  }
                                   
  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  return TRUE;
  
} // WritePaletteData


/*******************************************************************

    NAME:       WriteBitmapData

    SYNOPSIS:   Write bitmap data.

    ENTRY:      fh - Destination file handle.
    
                pBitmap - Bitmap data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteBitmapData(INT fh, BITMAP_DATA *pBitmap)
{
  DWORD dwSize;
  BITMAP_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  dwSize = (DWORD)pBitmap->x_len * (DWORD)pBitmap->y_len;

  hdr.id = ID_BITMAP_DATA;
  hdr.size = (long)sizeof(hdr) + (long)dwSize;
  hdr.hdr_size = sizeof(hdr);
  hdr.x_len = pBitmap->x_len;
  hdr.y_len = pBitmap->y_len;
  hdr.flags = pBitmap->flags;

  _fstrcpy(hdr.bitmap_name, pBitmap->bitmap_name);

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  if (_hwrite(fh, pBitmap->data, (long)dwSize) != (long)dwSize)
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteBitmapData


/*******************************************************************

    NAME:       WriteTextureData

    SYNOPSIS:   Write texture data.

    ENTRY:      fh - Destination file handle.
    
                pTexture - Texture data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteTextureData(INT fh, TEXTURE_DATA *pTexture)
{
  TEXTURE_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  hdr.id = ID_TEXTURE_DATA;
  hdr.size = sizeof(hdr);
  hdr.bitmap_index = pTexture->bitmap_index;
  hdr.color_index = pTexture->color_index;
  hdr.pixel_size = pTexture->pixel_size;

  _fstrcpy(hdr.texture_name, pTexture->texture_name);

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteTextureData


/*******************************************************************

    NAME:       WriteSoundData

    SYNOPSIS:   Write sound data.

    ENTRY:      fh - Destination file handle.
    
                pSound - Sound data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteSoundData(INT fh, SOUND_DATA *pSound)
{
  DWORD dwSize;
  SOUND_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  dwSize = (DWORD)pSound->length;

  hdr.id = ID_SOUND_DATA;
  hdr.size = (long)sizeof(hdr) + (long)dwSize;
  hdr.hdr_size = sizeof(hdr);
  hdr.flags = pSound->flags;

  _fstrcpy(hdr.sound_name, pSound->sound_name);

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  if (_hwrite(fh, pSound->data, (long)dwSize) != (long)dwSize)
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteSoundData


/*******************************************************************

    NAME:       WriteAnimationData

    SYNOPSIS:   Write animation data.

    ENTRY:      fh - Destination file handle.
    
                pAnimation - Animation data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteAnimationData(INT fh, ANIMATION_DATA *pAnimation)
{
  INT i;
  ANIMATION_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  hdr.id = ID_ANIMATION_DATA;
  hdr.size = sizeof(hdr);
  hdr.num_elements = pAnimation->num_elements;
  hdr.delay = pAnimation->delay;
  hdr.pixel_size = pAnimation->pixel_size;
  hdr.first_element = pAnimation->first_element;

  _fstrcpy(hdr.animation_name, pAnimation->animation_name);

  for(i = 0; i < hdr.num_elements; ++i)
  {
    hdr.index[i] = pAnimation->index[i];
  }

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteAnimationData


/*******************************************************************

    NAME:       WriteLifeFormData

    SYNOPSIS:   Write life form data.

    ENTRY:      fh - Destination file handle.
    
                pLifeForm - Life form data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteLifeFormData(INT fh, LIFEFORM_DATA *pLifeForm)
{
  INT i;
  LIFEFORM_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  hdr.id = ID_LIFEFORM_DATA;
  hdr.size = sizeof(hdr);
  hdr.lifeform_type = pLifeForm->lifeform_type;
  hdr.pixel_size = pLifeForm->pixel_size;

  _fstrcpy(hdr.lifeform_name, pLifeForm->lifeform_name);

  for(i = 0; i < MAX_ELEMENTS; ++i)
  {
    hdr.index[i] = pLifeForm->index[i];
  }

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteLifeFormData


/*******************************************************************

    NAME:       WriteItemData

    SYNOPSIS:   Write item data.

    ENTRY:      fh - Destination file handle.
    
                pItem - Item data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteItemData(INT fh, ITEM_DATA *pItem)
{
  INT i;
  ITEM_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  hdr.id = ID_ITEM_DATA;
  hdr.size = sizeof(hdr);
  hdr.item_type = pItem->item_type;
  hdr.pixel_size = pItem->pixel_size;

  _fstrcpy(hdr.item_name, pItem->item_name);

  for(i = 0; i < MAX_ELEMENTS; ++i)
  {
    hdr.index[i] = pItem->index[i];
  }

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteItemData


/*******************************************************************

    NAME:       WriteSpriteData

    SYNOPSIS:   Write sprite data.

    ENTRY:      fh - Destination file handle.
    
                pSprite - Sprite data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteSpriteData(INT fh, SPRITE_DATA *pSprite)
{
  SPRITE_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  hdr.id = ID_SPRITE_DATA;
  hdr.size = sizeof(hdr);
  hdr.flags = pSprite->flags;
  hdr.bitmap_index = pSprite->bitmap_index;
  hdr.animation_index = pSprite->animation_index;
  hdr.damage_animation_index = pSprite->damage_animation_index;
  hdr.damage_animation_index_2 = pSprite->damage_animation_index_2;
  hdr.pixel_size = pSprite->pixel_size;

  _fstrcpy(hdr.sprite_name, pSprite->sprite_name);

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteSpriteData


/*******************************************************************

    NAME:       WriteNoiseData

    SYNOPSIS:   Write noise data.

    ENTRY:      fh - Destination file handle.
    
                pNoise - Noise data pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteNoiseData(INT fh, NOISE_DATA *pNoise)
{
  NOISE_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  hdr.id = ID_NOISE_DATA;
  hdr.size = sizeof(hdr);
  hdr.sound_index = pNoise->sound_index;
  hdr.delay = pNoise->delay;
  hdr.radius = pNoise->radius;
  hdr.height = pNoise->height;

  _fstrcpy(hdr.noise_name, pNoise->noise_name);

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  return TRUE;
  
} // WriteNoiseData


/*******************************************************************

    NAME:       WriteLevelData

    SYNOPSIS:   Write level data.

    ENTRY:      fh - Destination file handle.

                pLevel - Level data pointer.
                
    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteLevelData(INT fh, LEVEL_DATA *pLevel)
{
  DWORD dwSize;
  LEVEL_DATA_HDR hdr;

  memset(&hdr, 0, sizeof(hdr));

  hdr.id = ID_LEVEL_DATA;
  hdr.size = (long)sizeof(hdr) +
             (long)sizeof(VERTEX) * (long)pLevel->nVertices +
             (long)sizeof(POLYGON) * (long)pLevel->nPolygons +
             (long)sizeof(TRIGGER) * (long)pLevel->nTriggers +
             (long)sizeof(MOTION) * (long)pLevel->nMotions +
             (long)sizeof(GROUP) * (long)pLevel->nGroups +
             (long)sizeof(MARK) * (long)pLevel->nMarks;
  hdr.hdr_size = sizeof(hdr);

  hdr.view_home_rotation = pLevel->view_home_rotation;
  hdr.view_home_elevation = pLevel->view_home_elevation;
  hdr.view_home_x = pLevel->view_home_x;
  hdr.view_home_y = pLevel->view_home_y;
  hdr.view_home_z = pLevel->view_home_z;
  hdr.view_rotation = pLevel->view_rotation;
  hdr.view_elevation = pLevel->view_elevation;
  hdr.view_x = pLevel->view_x;
  hdr.view_y = pLevel->view_y;
  hdr.view_z = pLevel->view_z;
  hdr.view_delta = pLevel->view_delta;
  hdr.view_depth = pLevel->view_depth;
  hdr.view_width = pLevel->view_width;
  hdr.view_height = pLevel->view_height;
  hdr.grid_x = pLevel->grid_x;
  hdr.grid_y = pLevel->grid_y;
  hdr.grid_z = pLevel->grid_z;
  hdr.grid_rotation = pLevel->grid_rotation;
  hdr.grid_elevation = pLevel->grid_elevation;
  hdr.grid_delta = pLevel->grid_delta;
  hdr.grid_spacing = pLevel->grid_spacing;
  hdr.grid_size = pLevel->grid_size;
  hdr.bMaintainGridDist = pLevel->bMaintainGridDist;
  hdr.bLockViewToGrid = pLevel->bLockViewToGrid;
  hdr.bSnapToGrid = pLevel->bSnapToGrid;

  hdr.nVertices = pLevel->nVertices;
  hdr.nPolygons = pLevel->nPolygons;
  hdr.nTriggers = pLevel->nTriggers;
  hdr.nMotions = pLevel->nMotions;
  hdr.nGroups = pLevel->nGroups;
  hdr.nMarks = pLevel->nMarks;
  hdr.nLastMark = pLevel->nLastMark;

  hdr.flags = pLevel->flags;
  hdr.bkg_bitmap_index = pLevel->bkg_bitmap_index;
  hdr.sol_bitmap_index = pLevel->sol_bitmap_index;
  hdr.eol_bitmap_index = pLevel->eol_bitmap_index;

  _fstrcpy(hdr.level_name, pLevel->level_name);
  _fstrcpy(hdr.level_author, pLevel->level_author);
  _fstrcpy(hdr.sol_text, pLevel->sol_text);
  _fstrcpy(hdr.eol_text, pLevel->eol_text);

  if (_lwrite(fh, &hdr, sizeof(hdr)) != sizeof(hdr))
  {
    return FALSE;
  }

  if (pLevel->nVertices > 0)
  {
    dwSize = (DWORD)sizeof(VERTEX) * (DWORD)pLevel->nVertices;

    if (_hwrite(fh, pLevel->pVertexData, (long)dwSize) != (long)dwSize)
    {
      return FALSE;
    }
  }

  if (pLevel->nPolygons > 0)
  {
    dwSize = (DWORD)sizeof(POLYGON) * (DWORD)pLevel->nPolygons;

    if (_hwrite(fh, pLevel->pPolygonData, (long)dwSize) != (long)dwSize)
    {
      return FALSE;
    }
  }

  if (pLevel->nTriggers > 0)
  {
    dwSize = (DWORD)sizeof(TRIGGER) * (DWORD)pLevel->nTriggers;

    if (_lwrite(fh, pLevel->pTriggerData, (WORD)dwSize) != (WORD)dwSize)
    {
      return FALSE;
    }
  }

  if (pLevel->nMotions > 0)
  {
    dwSize = (DWORD)sizeof(MOTION) * (DWORD)pLevel->nMotions;

    if (_lwrite(fh, pLevel->pMotionData, (WORD)dwSize) != (WORD)dwSize)
    {
      return FALSE;
    }
  }

  if (pLevel->nGroups > 0)
  {
    dwSize = (DWORD)sizeof(GROUP) * (DWORD)pLevel->nGroups;

    if (_lwrite(fh, pLevel->pGroupData, (WORD)dwSize) != (WORD)dwSize)
    {
      return FALSE;
    }
  }

  if (pLevel->nMarks > 0)
  {
    dwSize = (DWORD)sizeof(MARK) * (DWORD)pLevel->nMarks;

    if (_lwrite(fh, pLevel->pMarkData, (WORD)dwSize) != (WORD)dwSize)
    {
      return FALSE;
    }
  }

  return TRUE;
  
} // WriteLevelData


/*******************************************************************

    NAME:       WriteZoneData

    SYNOPSIS:   Write zone data.

    ENTRY:      fh - Destination file handle.

                pZone - Zone data pointer.
                
    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL WriteZoneData(INT fh, ZONE_DATA *pZone)
{
  if (pZone->data == NULL)
  {
    return FALSE;
  }

  if (_hwrite(fh, pZone->data, pZone->size) != pZone->size)
  {
    return FALSE;
  }

  return TRUE;

} // WriteZoneData


/*******************************************************************

    NAME:       CheckLevelData

    SYNOPSIS:   Check for any problems with level data that would
                prevent the creation of valid zone data.

    ENTRY:      pWndData - Window data pointer.

                index - Level data index.
                
    RETURNS:    TRUE if successful, FALSE on error.

********************************************************************/

BOOL CheckLevelData(WND_DATA *pWndData, INT index)
{
  INT i, j, n;
  POLYGON huge *pPolygon;
  VERTEX huge *pVertex;
  TEXTURE_DATA *pTexture;
  ANIMATION_DATA *pAnimation;
  TRIGGER *pTrigger;
  MOTION *pMotion;
  LIFEFORM_DATA *pLifeForm;
  ITEM_DATA *pItem;
  SPRITE_DATA *pSprite;
  NOISE_DATA *pNoise;
  LEVEL_DATA *pLevel;

  pLevel = pWndData->pLevelData + index;

  //
  //  Check the level bitmaps.
  //

  if (pLevel->flags & LF_BKG_BITMAP)
  {
    if (pLevel->bkg_bitmap_index == -1)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Level \"%s\" has no background bitmap assigned",
              pLevel->level_name );

      return FALSE;
    }

    if (pWndData->pBitmapData[pLevel->bkg_bitmap_index].x_len != 320 ||
        pWndData->pBitmapData[pLevel->bkg_bitmap_index].y_len != 200)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Level \"%s\" background bitmap is not 320x200",
              pLevel->level_name );

      return FALSE;
    }
  }

  if (pLevel->flags & LF_SOL_BITMAP)
  {
    if (pLevel->sol_bitmap_index == -1)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Level \"%s\" has no start of level bitmap assigned",
              pLevel->level_name );

      return FALSE;
    }
  }

  if (pLevel->flags & LF_EOL_BITMAP)
  {
    if (pLevel->eol_bitmap_index == -1)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Level \"%s\" has no end of level bitmap assigned",
              pLevel->level_name );

      return FALSE;
    }
  }

  //
  //  Check the texture data.
  //

  for(i = 0; i < pWndData->nTextures; ++i)
  {
    pTexture = pWndData->pTextureData + i;

    if (pTexture->bitmap_index == -1)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Texture \"%s\" has no bitmap assigned",
              pTexture->texture_name );

      return FALSE;
    }
  }

  //
  //  Check the animation data.
  //

  for(i = 0; i < pWndData->nAnimations; ++i)
  {
    pAnimation = pWndData->pAnimationData + i;

    for(j = 0; j < pAnimation->num_elements; ++j)
    {
      if (pAnimation->index[j] == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Animation \"%s\" is not fully defined",
                pAnimation->animation_name );

        return FALSE;
      }
    }

    if (GetFirstBitmapIndex(pAnimation) == -1)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Animation \"%s\" has no bitmaps",
              pAnimation->animation_name );

      return FALSE;
    }
  }

  //
  //  Check the motion data.
  //

  for(i = 0; i < pLevel->nMotions; ++i)
  {
    pMotion = pLevel->pMotionData + i;

    if (pMotion->flags & MF_START_SOUND)
    {
      if (pMotion->sound_index_start == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Motion \"%s\" has no start sound assigned",
                pMotion->motion_name );

        return FALSE;
      }
    }

    if (pMotion->flags & MF_RUN_SOUND)
    {
      if (pMotion->sound_index_run == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Motion \"%s\" has no run sound assigned",
                pMotion->motion_name );

        return FALSE;
      }
    }

    if (pMotion->flags & MF_STOP_SOUND)
    {
      if (pMotion->sound_index_stop == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Motion \"%s\" has no stop sound assigned",
                pMotion->motion_name );

        return FALSE;
      }
    }
  }

  //
  //  Check the life form data.
  //

  for(i = 0; i < pWndData->nLifeForms; ++i)
  {
    pLifeForm = pWndData->pLifeFormData + i;

    n = GetNumLifeFormElements(pLifeForm->lifeform_type);

    for(j = 0; j < n; ++j)
    {
      if (pLifeForm->index[j] == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Life form \"%s\" is not fully defined",
                pLifeForm->lifeform_name );

        return FALSE;
      }
    }
  }

  //
  //  Check the item data.
  //

  for(i = 0; i < pWndData->nItems; ++i)
  {
    pItem = pWndData->pItemData + i;

    n = GetNumItemElements(pItem->item_type);

    for(j = 0; j < n; ++j)
    {
      if (pItem->index[j] == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Item \"%s\" is not fully defined",
                pItem->item_name );

        return FALSE;
      }
    }
  }

  //
  //  Check the sprite data.
  //

  for(i = 0; i < pWndData->nSprites; ++i)
  {
    pSprite = pWndData->pSpriteData + i;

    if (pSprite->flags & SF_ANIMATED)
    {
      if (pSprite->animation_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Sprite \"%s\" has no animation assigned",
                pSprite->sprite_name );

        return FALSE;
      }
    }
    else
    {
      if (pSprite->bitmap_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Sprite \"%s\" has no bitmap assigned",
                pSprite->sprite_name );

        return FALSE;
      }
    }

    if (pSprite->flags & SF_DAMAGE_ANIMATED)
    {
      if (pSprite->damage_animation_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Sprite \"%s\" has no damage animation assigned",
                pSprite->sprite_name );

        return FALSE;
      }
    }

    if (pSprite->flags & SF_DAMAGE_ANIMATED_2)
    {
      if (pSprite->damage_animation_index_2 == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Sprite \"%s\" has no secondary damage animation assigned",
                pSprite->sprite_name );

        return FALSE;
      }
    }
  }

  //
  //  Check the noise data.
  //

  for(i = 0; i < pWndData->nNoises; ++i)
  {
    pNoise = pWndData->pNoiseData + i;

    if (pNoise->sound_index == -1)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Noise \"%s\" has no sound assigned",
              pNoise->noise_name );

      return FALSE;
    }
  }

  //
  //  Check the trigger data.
  //

  for(i = 0; i < pLevel->nTriggers; ++i)
  {
    pTrigger = pLevel->pTriggerData + i;

    if (pTrigger->flags & TF_SOUND)
    {
      if (pTrigger->sound_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Trigger \"%s\" has no sound assigned",
                pTrigger->trigger_name );

        return FALSE;
      }
    }

    if (pTrigger->flags & TF_ITEM_REQUIRED)
    {
      if (pTrigger->item_type == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Trigger \"%s\" has no item assigned",
                pTrigger->trigger_name );

        return FALSE;
      }
    }
  }

  //
  //  Check the polygons.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    pPolygon = pLevel->pPolygonData + i;

    if (!IsPolygonFlat(pPolygon, pLevel->pVertexData))
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Polygon is not flat" );

      ClearSelections(pLevel);
      pPolygon->flags |= PF_SELECTED;

      return FALSE;
    }

    if (!IsPolygonConvex(pPolygon, pLevel->pVertexData))
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Polygon is not convex" );

      ClearSelections(pLevel);
      pPolygon->flags |= PF_SELECTED;

      return FALSE;
    }

    if (pPolygon->flags & PF_INVISIBLE)
    {
      pPolygon->flags &= ~(PF_LINKED | PF_MOVING_POLYGON | PF_MOTION_ANIMATED | PF_ANIMATED | PF_DAMAGE_ANIMATED);
    }

    if (pPolygon->flags & (PF_MOVING_POLYGON | PF_MOTION_ANIMATED))
    {
      if (pPolygon->motion_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Polygon has no motion assigned" );

        ClearSelections(pLevel);
        pPolygon->flags |= PF_SELECTED;

        return FALSE;
      }
    }

    if (pPolygon->flags & (PF_ANIMATED | PF_MOTION_ANIMATED))
    {
      while(pPolygon->flags & PF_LINKED)
      {
        pPolygon = pLevel->pPolygonData + pPolygon->polygon_link;
      }

      if (pPolygon->animation_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Polygon has no animation assigned" );

        ClearSelections(pLevel);
        pPolygon->flags |= PF_SELECTED;

        return FALSE;
      }
    }
    else if ((pPolygon->flags & PF_INVISIBLE) == 0)
    {
      while(pPolygon->flags & PF_LINKED)
      {
        pPolygon = pLevel->pPolygonData + pPolygon->polygon_link;
      }

      if (pPolygon->texture_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Polygon has no texture assigned" );

        ClearSelections(pLevel);
        pPolygon->flags |= PF_SELECTED;

        return FALSE;
      }
    }

    if (pPolygon->flags & PF_DAMAGE_ANIMATED)
    {
      while(pPolygon->flags & PF_LINKED)
      {
        pPolygon = pLevel->pPolygonData + pPolygon->polygon_link;
      }

      if (pPolygon->damage_animation_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Polygon has no damage animation assigned" );

        ClearSelections(pLevel);
        pPolygon->flags |= PF_SELECTED;

        return FALSE;
      }
    }
  }

  //
  //  Check the vertices.
  //

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    pVertex = pLevel->pVertexData + i;

    if (pVertex->flags & VF_SPRITE)
    {
      if (pVertex->sprite_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Vertex is missing sprite assignment" );

        ClearSelections(pLevel);
        pVertex->flags |= VF_SELECTED;

        return FALSE;
      }
    }
    else if (pVertex->flags & VF_LIFEFORM)
    {
      if (pVertex->lifeform_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Vertex is missing life form assignment" );

        ClearSelections(pLevel);
        pVertex->flags |= VF_SELECTED;

        return FALSE;
      }
    }
    else if (pVertex->flags & VF_ITEM)
    {
      if (pVertex->item_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Vertex is missing item assignment" );

        ClearSelections(pLevel);
        pVertex->flags |= VF_SELECTED;

        return FALSE;
      }
    }
    else if (pVertex->flags & VF_NOISE)
    {
      if (pVertex->noise_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Vertex is missing noise assignment" );

        ClearSelections(pLevel);
        pVertex->flags |= VF_SELECTED;

        return FALSE;
      }
    }
    else if (pVertex->flags & VF_TRIGGER)
    {
      if (pVertex->trigger_index == -1)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Vertex is missing trigger assignment" );

        ClearSelections(pLevel);
        pVertex->flags |= VF_SELECTED;

        return FALSE;
      }
    }
  }

  return TRUE;

} // CheckLevelData


/*******************************************************************

    NAME:       GetZoneDataVertices

    SYNOPSIS:   Calculate the zone data vertices from the polygon.
                The polygon given must be unlinked.

    ENTRY:      pLevel - Pointer to level data.

                p - Pointer to polygon to be converted.
    
                zdv - The zone data vertex array.

    RETURNS:    None

********************************************************************/

void GetZoneDataVertices(LEVEL_DATA *pLevel, POLYGON *p, ZONE_DATA_VERTEX *zdv)
{
  INT j, n;
  TEXEL v1, v2, v;

  n = (p->flags & 7) + 3;

  v1.x   = (double)pLevel->pVertexData[p->vertices[0]].x;
  v1.y   = (double)pLevel->pVertexData[p->vertices[0]].y;
  v1.z   = (double)pLevel->pVertexData[p->vertices[0]].z;
  v1.bmx = (double)p->bmx[0];
  v1.bmy = (double)p->bmy[0];

  v2.x   = (double)pLevel->pVertexData[p->vertices[1]].x;
  v2.y   = (double)pLevel->pVertexData[p->vertices[1]].y;
  v2.z   = (double)pLevel->pVertexData[p->vertices[1]].z;
  v2.bmx = (double)p->bmx[1];
  v2.bmy = (double)p->bmy[1];

  for(j = 0; j < n; ++j)
  {
    v.x = (double)pLevel->pVertexData[p->vertices[j]].x;
    v.y = (double)pLevel->pVertexData[p->vertices[j]].y;
    v.z = (double)pLevel->pVertexData[p->vertices[j]].z;
        
    zdv[j].object_x = (float)Round(v.x);
    zdv[j].object_y = (float)Round(v.y);
    zdv[j].object_z = (float)Round(v.z);

    ComputeBitmapCoordinates(&v1, &v2, &v);

    zdv[j].bitmap_x = (float)v.bmx;
    zdv[j].bitmap_y = (float)v.bmy;
  }

  return;

} // GetZoneDataVertices


/*******************************************************************

    NAME:       AdjustBitmapVertices

    SYNOPSIS:   Adjust the bitmap vertices.

    ENTRY:      xlen - The width of the applied bitmap.

                ylen - The height of the applied bitmap.

                n - Number of vertices.
    
                zdv - The zone data vertex array.

    RETURNS:    None

********************************************************************/

void AdjustBitmapVertices(INT xlen, INT ylen, INT n, ZONE_DATA_VERTEX *zdv)
{
  INT j;
  LONG bmx, bmy;
  double x, y;

  //
  //  Adjust bitmap vertices to eliminate any negatives.
  //

  x = (double)zdv[0].bitmap_x;
  y = (double)zdv[0].bitmap_y;

  for(j = 1; j < n; ++j)
  {
    if ((double)zdv[j].bitmap_x < x)
    {
      x = (double)zdv[j].bitmap_x;
    }

    if ((double)zdv[j].bitmap_y < y)
    {
      y = (double)zdv[j].bitmap_y;
    }
  }

  if (x < 0.0)
  {
    bmx = ((long)(-x) / (long)xlen + 1L) * (long)xlen;

    for(j = 0; j < n; ++j)
    {
      zdv[j].bitmap_x += (float)bmx;
    }
  }

  if (y < 0.0)
  {
    bmy = ((long)(-y) / (long)ylen + 1L) * (long)ylen;

    for(j = 0; j < n; ++j)
    {
      zdv[j].bitmap_y += (float)bmy;
    }
  }

  //
  //  Reduce the bitmap vertices to the lowest values possible.
  //

  bmx = (long)zdv[0].bitmap_x;
  bmy = (long)zdv[0].bitmap_y;

  for(j = 1; j < n; ++j)
  {
    if ((long)zdv[j].bitmap_x < bmx)
    {
      bmx = (long)zdv[j].bitmap_x;
    }

    if ((long)zdv[j].bitmap_y < bmy)
    {
      bmy = (long)zdv[j].bitmap_y;
    }
  }

  bmx = (bmx / (long)xlen) * (long)xlen;
  bmy = (bmy / (long)ylen) * (long)ylen;

  for(j = 0; j < n; ++j)
  {
    zdv[j].bitmap_x -= (float)bmx;
    zdv[j].bitmap_y -= (float)bmy;
  }

#if 0

  //
  // Note:
  //
  // This is now done in VScape code since it is not known at this time if
  // the bitmap is to be converted to a new size for speed considerations.
  //

  //
  //  Now add xlen and ylen to each bitmap coordinate. This
  //  prevents negatives or zeros near the edges of the bitmaps
  //  which then prevents divide by zero or overflows in the
  //  assembly code (i.e. the lbx/lw and lby/lw calculations).
  //

  for(j = 0; j < n; ++j)
  {
    zdv[j].bitmap_x += (float)xlen;
    zdv[j].bitmap_y += (float)ylen;
  }

#endif

  return;

} // AdjustBitmapVertices


/*******************************************************************

    NAME:       CreateZonePolygon

    SYNOPSIS:   Create a zone data polygon.

    ENTRY:      pWndData - Pointer to window data.

                pLevel - Pointer to level data.

                i - Index of the polygon to be converted.
    
                zdp - The zone data polygon buffer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL CreateZonePolygon(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_POLYGON *zdp)
{
  INT j, n, xlen, ylen;
  POLYGON polygon, huge *p;
  ANIMATION_DATA *pAnimation;
  double x1, y1, z1, x2, y2, z2, x3, y3, z3, a, b, c, d;

  _fmemset(zdp, 0, sizeof(ZONE_DATA_POLYGON));

  p = pLevel->pPolygonData + i;

  n = (p->flags & 7) + 3;

  _fmemset(&polygon, 0, sizeof(POLYGON));

  if (p->flags & PF_LINKED)
  {
    ConvertToUnlinked(pLevel, i, &polygon);
  }
  else
  {
    hmemcpy(&polygon, p, sizeof(POLYGON));
  }

  if (polygon.flags & (PF_ANIMATED | PF_MOTION_ANIMATED))
  {
    if (polygon.animation_index == -1)
    {
      return FALSE;
    }

    pAnimation = pWndData->pAnimationData + polygon.animation_index;

    zdp->animation_index[0] = polygon.animation_index;
    zdp->current_element_index[0] = pAnimation->first_element;
    zdp->current_element_delay[0] = pAnimation->delay;
    zdp->bitmap_index = GetFirstBitmapIndex(pAnimation);
    zdp->flags |= FLAG_ANIMATED;
  }
  else if (polygon.flags & PF_INVISIBLE)
  {
    zdp->bitmap_index = -1;
  }
  else
  {
    if (polygon.texture_index == -1)
    {
      return FALSE;
    }

    zdp->bitmap_index = pWndData->pTextureData[polygon.texture_index].bitmap_index;
  }

  if (polygon.flags & (PF_MOVING_POLYGON | PF_MOTION_ANIMATED))
  {
    if (polygon.motion_index == -1)
    {
      return FALSE;
    }

    if (polygon.flags & PF_MOVING_POLYGON)
    {
      zdp->flags |= FLAG_MOVING_POLYGON;
    }

    zdp->motion_index = polygon.motion_index;
  }
  else
  {
    zdp->motion_index = -1;
  }

  if (polygon.flags & PF_DAMAGE_ANIMATED)
  {
    if (polygon.damage_animation_index == -1)
    {
      return FALSE;
    }

    pAnimation = pWndData->pAnimationData + polygon.damage_animation_index;

    zdp->animation_index[1] = polygon.damage_animation_index;
    zdp->current_element_index[1] = -1;
    zdp->current_element_delay[1] = pAnimation->delay;
    zdp->flags |= FLAG_DAMAGE_ANIMATED;
  }

  zdp->type = (unsigned int)n;

  if (p->flags & PF_DOUBLE_SIDED)
  {
    zdp->flags |= FLAG_DOUBLE_SIDED;
  }

  if (p->flags & PF_INVISIBLE)
  {
    zdp->flags |= FLAG_INVISIBLE;
  }

  if (p->flags & PF_WALK_THROUGH)
  {
    zdp->flags |= FLAG_WALK_THROUGH;
  }

  if (p->flags & PF_SECRET_PLACE)
  {
    zdp->flags |= FLAG_SECRET;
  }

  if ((polygon.flags & PF_INVISIBLE) == 0)
  {
    zdp->bitmap_dx = polygon.texture_dx;
    zdp->bitmap_dy = polygon.texture_dy;

    if (zdp->bitmap_dx != 0.0 || zdp->bitmap_dy != 0.0)
    {
      zdp->flags |= FLAG_MOVING_BITMAP;
    }

    xlen = pWndData->pBitmapData[zdp->bitmap_index].x_len;
    ylen = pWndData->pBitmapData[zdp->bitmap_index].y_len;

    GetZoneDataVertices(pLevel, &polygon, zdp->vertices);
    AdjustBitmapVertices(xlen, ylen, n, zdp->vertices);
  }
  else
  {
    for(j = 0; j < n; ++j)
    {
      zdp->vertices[j].object_x = pLevel->pVertexData[p->vertices[j]].x;
      zdp->vertices[j].object_y = pLevel->pVertexData[p->vertices[j]].y;
      zdp->vertices[j].object_z = pLevel->pVertexData[p->vertices[j]].z;
    }
  }

  //
  //  Generate the polygon plane equation coefficients.
  //

  x1 = (double)zdp->vertices[0].object_x;
  y1 = (double)zdp->vertices[0].object_y;
  z1 = (double)zdp->vertices[0].object_z;

  x2 = (double)zdp->vertices[1].object_x;
  y2 = (double)zdp->vertices[1].object_y;
  z2 = (double)zdp->vertices[1].object_z;

  x3 = (double)zdp->vertices[2].object_x;
  y3 = (double)zdp->vertices[2].object_y;
  z3 = (double)zdp->vertices[2].object_z;

  a =  y1 * (z3 - z2) - z1 * (y3 - y2) + y3 * z2 - y2 * z3;
  b = -x1 * (z3 - z2) + z1 * (x3 - x2) - x3 * z2 + x2 * z3;
  c =  x1 * (y3 - y2) - y1 * (x3 - x2) + x3 * y2 - x2 * y3;
  d = sqrt(a * a + b * b + c * c);

  zdp->a = (float)(a / d);
  zdp->b = (float)(b / d);
  zdp->c = (float)(c / d);
  zdp->d = (float)(-(a * x1 + b * y1 + c * z1) / d);

  zdp->vis_list_index = -1;
  zdp->mot_list_index = -1;

  if ((polygon.flags & PF_INVISIBLE) == 0)
  {
    if ((p->flags & PF_LINKED) == 0)
    {
      for(j = 0; j < n; ++j)
      {
        zdp->vertices[j].bitmap_x += p->texture_x_offset;
        zdp->vertices[j].bitmap_y += p->texture_y_offset;
      }
    }
  }

  return TRUE;

} // CreateZonePolygon


/*******************************************************************

    NAME:       CreateZoneSprite

    SYNOPSIS:   Create a zone data sprite.

    ENTRY:      pWndData - Pointer to window data.

                pLevel - Pointer to level data.

                i - Index of the vertex to be converted.
    
                zds - The zone data sprite buffer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL CreateZoneSprite(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_SPRITE *zds)
{
  VERTEX huge *pVertex;
  SPRITE_DATA *pSprite;
  ANIMATION_DATA *pAnimation;
  double x, y, z, w, h;

  _fmemset(zds, 0, sizeof(ZONE_DATA_SPRITE));

  pVertex = pLevel->pVertexData + i;

  if (pVertex->sprite_index == -1)
  {
    return FALSE;
  }

  pSprite = pWndData->pSpriteData + pVertex->sprite_index;

  if (pSprite->flags & SF_ANIMATED)
  {
    if (pSprite->animation_index == -1)
    {
      return FALSE;
    }

    pAnimation = pWndData->pAnimationData + pSprite->animation_index;

    zds->animation_index[0] = pSprite->animation_index;
    zds->current_element_index[0] = pAnimation->first_element;
    zds->current_element_delay[0] = pAnimation->delay;
    zds->bitmap_index = GetFirstBitmapIndex(pAnimation);
    zds->flags |= FLAG_ANIMATED;
  }
  else
  {
    if (pSprite->bitmap_index == -1)
    {
      return FALSE;
    }

    zds->bitmap_index = pSprite->bitmap_index;
  }

  if (pSprite->flags & SF_DAMAGE_ANIMATED)
  {
    if (pSprite->damage_animation_index == -1)
    {
      return FALSE;
    }

    pAnimation = pWndData->pAnimationData + pSprite->damage_animation_index;

    zds->animation_index[1] = pSprite->damage_animation_index;
    zds->current_element_index[1] = -1;
    zds->current_element_delay[1] = pAnimation->delay;
    zds->flags |= FLAG_DAMAGE_ANIMATED;
  }

  if (pSprite->flags & SF_DAMAGE_ANIMATED_2)
  {
    ANIMATION_DATA *pAnimation;

    if (pSprite->damage_animation_index_2 == -1)
    {
      return FALSE;
    }

    pAnimation = pWndData->pAnimationData + pSprite->damage_animation_index_2;

    zds->animation_index[2] = pSprite->damage_animation_index_2;
    zds->current_element_index[2] = -1;
    zds->current_element_delay[2] = pAnimation->delay;
    zds->flags |= FLAG_DAMAGE_ANIMATED_2;
  }

  if (pSprite->flags & SF_DESTRUCTABLE)
  {
    zds->flags |= FLAG_DESTRUCTABLE;
  }

  if (pSprite->flags & SF_HANGING)
  {
    zds->flags |= FLAG_HANGING;
  }

  if (pSprite->flags & SF_WALK_THROUGH)
  {
    zds->flags |= FLAG_WALK_THROUGH;
  }

  zds->mm_per_pixel = (float)pSprite->pixel_size;

  x = (double)pVertex->x;
  y = (double)pVertex->y;
  z = (double)pVertex->z;

  w = (double)pWndData->pBitmapData[zds->bitmap_index].x_len * zds->mm_per_pixel;
  h = (double)pWndData->pBitmapData[zds->bitmap_index].y_len * zds->mm_per_pixel;

  zds->type = ZD_SPRITE;
  zds->x = (float)Round(x);
  zds->y = (float)Round(y);
  zds->z = (float)Round(z);
  zds->height = (float)h;
  zds->width = (float)w;

  return TRUE;

} // CreateZoneSprite


/*******************************************************************

    NAME:       CreateZoneLifeForm

    SYNOPSIS:   Create a zone data life form.

    ENTRY:      pWndData - Pointer to window data.

                pLevel - Pointer to level data.

                i - Index of the vertex to be converted.
    
                zdl - The zone data life form buffer.

    RETURNS:    Number of frames used by life form or 0 on error.

********************************************************************/

INT CreateZoneLifeForm(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_LIFEFORM *zdl)
{
  INT j, n;
  VERTEX huge *pVertex;
  LIFEFORM_DATA *pLifeForm;
  double x, y, z, w, h;

  _fmemset(zdl, 0, sizeof(ZONE_DATA_LIFEFORM));

  pVertex = pLevel->pVertexData + i;

  if (pVertex->lifeform_index == -1)
  {
    return FALSE;
  }

  pLifeForm = pWndData->pLifeFormData + pVertex->lifeform_index;

  j = GetNumLifeFormBitmaps( pLifeForm->lifeform_type );
  n = GetNumLifeFormElements( pLifeForm->lifeform_type );

  while(j < n)
  {
    if (pLifeForm->index[j] == -1)
    {
      return FALSE;
    }

    ++j;
  }

  for(j = 0; j < n; ++j)
  {
    zdl->index[j] = pLifeForm->index[j];
  }

  zdl->type = ZD_LIFEFORM;
  zdl->lifeform_type = pLifeForm->lifeform_type;

  if (pVertex->flags & VF_DIFFICULTY_LEVEL_1)
  {
    zdl->flags |= FLAG_DIFFICULTY_LEVEL_1;
  }

  if (pVertex->flags & VF_DIFFICULTY_LEVEL_2)
  {
    zdl->flags |= FLAG_DIFFICULTY_LEVEL_2;
  }

  if (pVertex->flags & VF_DIFFICULTY_LEVEL_3)
  {
    zdl->flags |= FLAG_DIFFICULTY_LEVEL_3;
  }

  zdl->mm_per_pixel = (float)pLifeForm->pixel_size;

  x = (double)pVertex->x;
  y = (double)pVertex->y;
  z = (double)pVertex->z;

  w = (double)pWndData->pBitmapData[zdl->index[0]].x_len * zdl->mm_per_pixel;
  h = (double)pWndData->pBitmapData[zdl->index[0]].y_len * zdl->mm_per_pixel;

  zdl->x = (float)Round(x);
  zdl->y = (float)Round(y);
  zdl->z = (float)Round(z);
  zdl->height = (float)h;
  zdl->width = (float)w;
  zdl->walking_dir = pVertex->orientation;
  zdl->health = 100;

  return TRUE;

} // CreateZoneLifeForm


/*******************************************************************

    NAME:       CreateZoneItem

    SYNOPSIS:   Create a zone data item.

    ENTRY:      pWndData - Pointer to window data.

                pLevel - Pointer to level data.

                i - Index of the vertex to be converted.
    
                zdi - The zone data item buffer.

    RETURNS:    Number of frames used by item or 0 on error.

********************************************************************/

INT CreateZoneItem(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_ITEM *zdi)
{
  INT j, n;
  VERTEX huge *pVertex;
  ITEM_DATA *pItem;
  double x, y, z, w, h;

  _fmemset(zdi, 0, sizeof(ZONE_DATA_ITEM));

  pVertex = pLevel->pVertexData + i;

  if (pVertex->item_index == -1)
  {
    return FALSE;
  }

  pItem = pWndData->pItemData + pVertex->item_index;

  j = GetNumItemBitmaps( pItem->item_type );
  n = GetNumItemElements( pItem->item_type );

  while(j < n)
  {
    if (pItem->index[j] == -1)
    {
      return FALSE;
    }

    ++j;
  }

  for(j = 0; j < n; ++j)
  {
    zdi->index[j] = pItem->index[j];
  }

  zdi->type = ZD_ITEM;
  zdi->item_type = pItem->item_type;

  if (pVertex->flags & VF_DIFFICULTY_LEVEL_1)
  {
    zdi->flags |= FLAG_DIFFICULTY_LEVEL_1;
  }

  if (pVertex->flags & VF_DIFFICULTY_LEVEL_2)
  {
    zdi->flags |= FLAG_DIFFICULTY_LEVEL_2;
  }

  if (pVertex->flags & VF_DIFFICULTY_LEVEL_3)
  {
    zdi->flags |= FLAG_DIFFICULTY_LEVEL_3;
  }

  zdi->mm_per_pixel = (float)pItem->pixel_size;

  x = (double)pVertex->x;
  y = (double)pVertex->y;
  z = (double)pVertex->z;

  w = (double)pWndData->pBitmapData[zdi->index[0]].x_len * zdi->mm_per_pixel;
  h = (double)pWndData->pBitmapData[zdi->index[0]].y_len * zdi->mm_per_pixel;

  zdi->x = (float)Round(x);
  zdi->y = (float)Round(y);
  zdi->z = (float)Round(z);
  zdi->height = (float)h;
  zdi->width = (float)w;

  return TRUE;

} // CreateZoneItem


/*******************************************************************

    NAME:       CreateZoneNoise

    SYNOPSIS:   Create a zone data noise source.

    ENTRY:      pWndData - Pointer to window data.

                pLevel - Pointer to level data.

                i - Index of the vertex to be converted.
    
                zdn - The zone data noise buffer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

INT CreateZoneNoise(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT i, ZONE_DATA_NOISE *zdn)
{
  VERTEX huge *pVertex;
  NOISE_DATA *pNoise;
  double x, y, z, r, h;

  _fmemset(zdn, 0, sizeof(ZONE_DATA_NOISE));

  pVertex = pLevel->pVertexData + i;

  if (pVertex->noise_index == -1)
  {
    return FALSE;
  }

  pNoise = pWndData->pNoiseData + pVertex->noise_index;

  if (pNoise->sound_index == -1)
  {
    return FALSE;
  }

  x = (double)pVertex->x;
  y = (double)pVertex->y;
  z = (double)pVertex->z;

  r = (double)pNoise->radius;
  h = (double)pNoise->height;

  zdn->type = ZD_NOISE;
  zdn->sound_index = pNoise->sound_index;
  zdn->delay = pNoise->delay;
  zdn->delay_remaining = pNoise->delay;

  zdn->x = (float)Round(x);
  zdn->y = (float)Round(y);
  zdn->z = (float)Round(z);
  zdn->radius = (float)Round(r);
  zdn->height = (float)Round(h);

  return TRUE;

} // CreateZoneNoise


/*******************************************************************

    NAME:       CreateZoneTrigger

    SYNOPSIS:   Create a zone data trigger.

    ENTRY:      pLevel - Pointer to level data.

                i - Index of the trigger to be converted.
    
                zdt - The zone data trigger buffer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL CreateZoneTrigger(LEVEL_DATA *pLevel, INT i, ZONE_DATA_TRIGGER *zdt)
{
  int j;
  VERTEX huge *pVertex;
  TRIGGER *pTrigger;

  _fmemset(zdt, 0, sizeof(ZONE_DATA_TRIGGER));

  pVertex = pLevel->pVertexData + i;

  if (pVertex->trigger_index == -1)
  {
    return FALSE;
  }

  pTrigger = pLevel->pTriggerData + pVertex->trigger_index;

  zdt->type = ZD_TRIGGER;

  if (pTrigger->flags & TF_PROXIMITY_TRIGGER)
  {
    zdt->flags |= FLAG_PROXIMITY_TRIGGER;
  }

  if (pTrigger->flags & TF_USER_TRIGGER)
  {
    zdt->flags |= FLAG_USER_TRIGGER;
  }

  if (pTrigger->flags & TF_ITEM_REQUIRED)
  {
    zdt->flags |= FLAG_ITEM_REQUIRED;
  }

  if (pTrigger->flags & TF_END_LEVEL)
  {
    zdt->flags |= FLAG_END_LEVEL;
  }

  if (pTrigger->flags & TF_END_GAME)
  {
    zdt->flags |= FLAG_END_GAME;
  }

  if (pTrigger->flags & TF_SOUND)
  {
    zdt->sound_index = pTrigger->sound_index;

    if (zdt->sound_index == -1)
    {
      return FALSE;
    }
  }
  else
  {
    zdt->sound_index = -1;
  }

  zdt->x = pVertex->x;
  zdt->y = pVertex->y;
  zdt->z = pVertex->z;
  zdt->radius = pTrigger->radius;
  zdt->height = pTrigger->height;
  zdt->item_type = pTrigger->item_type;

  for(j = 0; j < pLevel->nMotions; ++j)
  {
    if (pLevel->pMotionData[j].trigger_index == pVertex->trigger_index)
    {
      zdt->motion_index[zdt->num_motions++] = j;
    }
  }

  return TRUE;

} // CreateZoneTrigger


/*******************************************************************

    NAME:       CreateZoneMotion

    SYNOPSIS:   Create a zone data motion.

    ENTRY:      pLevel - Pointer to level data.

                i - Index of the motion to be converted.
    
                zdm - The zone data motion buffer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL CreateZoneMotion(LEVEL_DATA *pLevel, INT i, ZONE_DATA_MOTION *zdm)
{
  MOTION *pMotion;

  _fmemset(zdm, 0, sizeof(ZONE_DATA_MOTION));

  pMotion = pLevel->pMotionData + i;

  zdm->type = ZD_MOTION;

  if (pMotion->flags & MF_START_SOUND)
  {
    zdm->flags |= FLAG_START_SOUND;

    zdm->sound_index_start = pMotion->sound_index_start;

    if (zdm->sound_index_start == -1)
    {
      return FALSE;
    }
  }
  else
  {
    zdm->sound_index_start = -1;
  }

  if (pMotion->flags & MF_RUN_SOUND)
  {
    zdm->flags |= FLAG_RUN_SOUND;

    zdm->sound_index_run = pMotion->sound_index_run;

    if (zdm->sound_index_run == -1)
    {
      return FALSE;
    }
  }
  else
  {
    zdm->sound_index_run = -1;
  }

  if (pMotion->flags & MF_STOP_SOUND)
  {
    zdm->flags |= FLAG_STOP_SOUND;

    zdm->sound_index_stop = pMotion->sound_index_stop;

    if (zdm->sound_index_stop == -1)
    {
      return FALSE;
    }
  }
  else
  {
    zdm->sound_index_stop = -1;
  }

  if (pMotion->flags & MF_AUTO_RUN)
  {
    zdm->flags |= FLAG_AUTO_RUN;
  }

  if (pMotion->flags & MF_AUTO_RETURN)
  {
    zdm->flags |= FLAG_AUTO_RETURN;
  }

  if (pMotion->flags & MF_TRIGGER_RETURN)
  {
    zdm->flags |= FLAG_TRIGGER_RETURN;
  }

  zdm->dx = pMotion->dx;
  zdm->dy = pMotion->dy;
  zdm->dz = pMotion->dz;

  zdm->num_steps = pMotion->num_steps;
  zdm->return_delay = pMotion->return_delay;

  return TRUE;

} // CreateZoneMotion


/*******************************************************************

    NAME:       CreateZoneData

    SYNOPSIS:   Create zone data. This data will consist of a zone
                data header plus the zone data.

    ENTRY:      pWndData - Window data pointer.

                index - Level data index.
                
    RETURNS:    TRUE if successful, FALSE on error.

********************************************************************/

BOOL CreateZoneData(WND_DATA *pWndData, INT index)
{
  INT i, j, n;
  long build_number;
  WORD wSize;
  BYTE huge *d, *s;
  POLYGON huge *pPolygon;
  VERTEX huge *pVertex;
  ZONE_DATA_HDR zoneDataHdr;
  ZONE_DATA_POLYGON zone_polygon;
  ZONE_DATA_LIFEFORM zone_lifeform;
  ZONE_DATA_ITEM zone_item;
  ZONE_DATA_SPRITE zone_sprite;
  ZONE_DATA_NOISE zone_noise;
  ZONE_DATA_TRIGGER zone_trigger;
  ZONE_DATA_MOTION zone_motion;
  LEVEL_DATA *pLevel;
  ZONE_DATA *pZone;

  pLevel = pWndData->pLevelData + index;
  pZone  = pWndData->pZoneData + index;

  build_number = 1;

  if (pZone->data)
  {
    build_number = ((ZONE_DATA_HDR *)(pZone->data))->build_number + 1;
    GlobalFreePtr(pZone->data);
    pZone->data = NULL;
  }

  memset(&zoneDataHdr, 0, sizeof(zoneDataHdr));

  zoneDataHdr.id = ID_ZONE_DATA;
  zoneDataHdr.size = sizeof(zoneDataHdr) + 2; // + 2 for NULL terminator
  zoneDataHdr.level_index = index;

  zoneDataHdr.player_x = (float)0.0;
  zoneDataHdr.player_y = (float)0.0;
  zoneDataHdr.player_z = (float)0.0;
  zoneDataHdr.player_orientation = (float)0.0;

  zoneDataHdr.build_number = build_number;

  if (pLevel->flags & LF_BKG_BITMAP)
  {
    zoneDataHdr.bkg_bitmap_index = pLevel->bkg_bitmap_index;
  }
  else
  {
    zoneDataHdr.bkg_bitmap_index = -1;
  }

  if (pLevel->flags & LF_SOL_BITMAP)
  {
    zoneDataHdr.sol_bitmap_index = pLevel->sol_bitmap_index;
  }
  else
  {
    zoneDataHdr.sol_bitmap_index = -1;
  }

  if (pLevel->flags & LF_EOL_BITMAP)
  {
    zoneDataHdr.eol_bitmap_index = pLevel->eol_bitmap_index;
  }
  else
  {
    zoneDataHdr.eol_bitmap_index = -1;
  }

  _fstrcpy(zoneDataHdr.level_name,pLevel->level_name);
  _fstrcpy(zoneDataHdr.level_author,pLevel->level_author);

  if (pLevel->flags & LF_SOL_TEXT)
  {
    _fstrcpy(zoneDataHdr.sol_text,pLevel->sol_text);
  }

  if (pLevel->flags & LF_EOL_TEXT)
  {
    _fstrcpy(zoneDataHdr.eol_text,pLevel->eol_text);
  }

  //
  //  Calculate the size of the zone data.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    pPolygon = pLevel->pPolygonData + i;

    n = (pPolygon->flags & 7) + 3;

    zoneDataHdr.size += sizeof(ZONE_DATA_POLYGON);
    zoneDataHdr.size -= (MAX_CLIPPED_VERTICES - n) * sizeof(ZONE_DATA_VERTEX);
  }

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    pVertex = pLevel->pVertexData + i;

    if (pVertex->flags & VF_SPRITE)
    {
      SPRITE_DATA *pSprite;

      if (pVertex->sprite_index == -1)
      {
        continue;
      }

      pSprite = pWndData->pSpriteData + pVertex->sprite_index;

      zoneDataHdr.size += sizeof(ZONE_DATA_SPRITE);
    }
    else if (pVertex->flags & VF_LIFEFORM)
    {
      j = pVertex->lifeform_index;
      n = GetNumLifeFormElements(pWndData->pLifeFormData[j].lifeform_type);

      zoneDataHdr.size += sizeof(ZONE_DATA_LIFEFORM);
      zoneDataHdr.size -= (MAX_ELEMENTS - n) * sizeof(int);
    }
    else if (pVertex->flags & VF_ITEM)
    {
      j = pVertex->item_index;
      n = GetNumItemElements(pWndData->pItemData[j].item_type);

      zoneDataHdr.size += sizeof(ZONE_DATA_ITEM);
      zoneDataHdr.size -= (MAX_ELEMENTS - n) * sizeof(int);
    }
    else if (pVertex->flags & VF_NOISE)
    {
      if (pVertex->noise_index == -1)
      {
        continue;
      }

      zoneDataHdr.size += sizeof(ZONE_DATA_NOISE);
    }
    else if (pVertex->flags & VF_TRIGGER)
    {
      TRIGGER *pTrigger;

      if (pVertex->trigger_index == -1)
      {
        continue;
      }

      pTrigger = pLevel->pTriggerData + pVertex->trigger_index;

      n = 0;

      for(j = 0; j < pLevel->nMotions; ++j)
      {
        if (pLevel->pMotionData[j].trigger_index == pVertex->trigger_index)
        {
          ++n;
        }
      }

      if (n == 0 && (pTrigger->flags & (TF_SOUND | TF_END_LEVEL | TF_END_GAME)) == 0)
      {
        continue;                       // this trigger is not being used
      }

      zoneDataHdr.size += sizeof(ZONE_DATA_TRIGGER);
      zoneDataHdr.size -= (MAX_MOTIONS_PER_TRIGGER - n) * sizeof(int);
    }
    else if (pVertex->flags & VF_PLAYER_START)
    {
      zoneDataHdr.player_x = pVertex->x;
      zoneDataHdr.player_y = pVertex->y;
      zoneDataHdr.player_z = pVertex->z;
      zoneDataHdr.player_orientation = (float)(pVertex->orientation * PI / 4.0);
    }
  }

  zoneDataHdr.size += sizeof(ZONE_DATA_MOTION) * pLevel->nMotions;

  zoneDataHdr.zone_size = zoneDataHdr.size - sizeof(zoneDataHdr);

  //
  //  Allocate memory for the zone data.
  //

  pZone->data = (BYTE huge *)GlobalAllocPtr(GPTR,zoneDataHdr.size);

  if (pZone->data == NULL)
  {
    return FALSE;
  }

  pZone->size = zoneDataHdr.size;

  d = pZone->data;

  s = (BYTE *)&zoneDataHdr;
  wSize = sizeof(zoneDataHdr);

  while(wSize > 0)
  {
    *d++ = *s++;
    --wSize;
  }

  //
  //  Create polygon based zone data.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    pPolygon = pLevel->pPolygonData + i;

    CreateZonePolygon(pWndData, pLevel, i, &zone_polygon);

    s = (BYTE *)&zone_polygon;
    n = (pPolygon->flags & 7) + 3;
    wSize = sizeof(ZONE_DATA_POLYGON) - (MAX_CLIPPED_VERTICES - n) * sizeof(ZONE_DATA_VERTEX);

    while(wSize > 0)
    {
      *d++ = *s++;
      --wSize;
    }
  }

  //
  //  Write vertex based zone data.
  //

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    pVertex = pLevel->pVertexData + i;

    if (pVertex->flags & VF_SPRITE)
    {
      SPRITE_DATA *pSprite;

      if (pVertex->sprite_index == -1)
      {
        continue;
      }

      pSprite = pWndData->pSpriteData + pVertex->sprite_index;

      CreateZoneSprite(pWndData, pLevel, i, &zone_sprite);

      s = (BYTE *)&zone_sprite;
      wSize = sizeof(ZONE_DATA_SPRITE);
    }
    else if (pVertex->flags & VF_LIFEFORM)
    {
      CreateZoneLifeForm(pWndData, pLevel, i, &zone_lifeform);

      j = pVertex->lifeform_index;

      s = (BYTE *)&zone_lifeform;
      n = GetNumLifeFormElements(pWndData->pLifeFormData[j].lifeform_type);
      wSize = sizeof(ZONE_DATA_LIFEFORM) - (MAX_ELEMENTS - n) * sizeof(int);
    }
    else if (pVertex->flags & VF_ITEM)
    {
      CreateZoneItem(pWndData, pLevel, i, &zone_item);

      j = pVertex->item_index;

      s = (BYTE *)&zone_item;
      n = GetNumItemElements(pWndData->pItemData[j].item_type);
      wSize = sizeof(ZONE_DATA_ITEM) - (MAX_ELEMENTS - n) * sizeof(int);
    }
    else if (pVertex->flags & VF_NOISE)
    {
      if (pVertex->noise_index == -1)
      {
        continue;
      }

      CreateZoneNoise(pWndData, pLevel, i, &zone_noise);

      s = (BYTE *)&zone_noise;
      wSize = sizeof(ZONE_DATA_NOISE);
    }
    else if (pVertex->flags & VF_TRIGGER)
    {
      TRIGGER *pTrigger;

      if (pVertex->trigger_index == -1)
      {
        continue;
      }

      pTrigger = pLevel->pTriggerData + pVertex->trigger_index;

      n = 0;

      for(j = 0; j < pLevel->nMotions; ++j)
      {
        if (pLevel->pMotionData[j].trigger_index == pVertex->trigger_index)
        {
          ++n;
        }
      }

      if (n == 0 && (pTrigger->flags & (TF_SOUND | TF_END_LEVEL | TF_END_GAME)) == 0)
      {
        continue;                       // this trigger is not being used
      }

      CreateZoneTrigger(pLevel, i, &zone_trigger);

      s = (BYTE *)&zone_trigger;
      wSize = sizeof(ZONE_DATA_TRIGGER) - (MAX_MOTIONS_PER_TRIGGER - n) * sizeof(int);
    }
    else
    {
      continue;
    }

    while(wSize > 0)
    {
      *d++ = *s++;
      --wSize;
    }
  }

  //
  //  Write zone motion data.
  //

  for(i = 0; i < pLevel->nMotions; ++i)
  {
    CreateZoneMotion(pLevel, i, &zone_motion);

    s = (BYTE *)&zone_motion;
    wSize = sizeof(ZONE_DATA_MOTION);

    while(wSize > 0)
    {
      *d++ = *s++;
      --wSize;
    }
  }

  //
  //  Write null terminator.
  //

  *d++ = '\0';
  *d++ = '\0';

  pLevel->bRebuildZone = FALSE;

  return TRUE;

} // CreateZoneData
