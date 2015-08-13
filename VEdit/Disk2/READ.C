/*******************************************************************

    read.c

    Routines for reading graphical object (GOB) files.

********************************************************************/


#include "vedit.h"


//
//  Public functions.
//

/*******************************************************************

    NAME:       FilterObject

    SYNOPSIS:   Filter out any anomalies from object data.

    ENTRY:      pWndData - Object window pointer.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL FilterObject(WND_DATA *pWndData)
{
  INT i, j, n, num_elements, num_bitmaps, index;
  WORD wMask;
  LEVEL_DATA *pLevel;
  POLYGON huge *pPolygon;
  VERTEX huge *pVertex;

  wMask = BF_TRANSPARENT;

  for(i = 0; i < pWndData->nBitmaps; ++i)
  {
    pWndData->pBitmapData[i].flags &= wMask;
  }

  wMask = 0;

  for(i = 0; i < pWndData->nTextures; ++i)
  {
    pWndData->pTextureData[i].flags &= wMask;

    if (pWndData->pTextureData[i].bitmap_index < -1 ||
        pWndData->pTextureData[i].bitmap_index >= pWndData->nBitmaps)
    {
      pWndData->pTextureData[i].bitmap_index = -1;
    }
  }

  for(i = 0; i < pWndData->nAnimations; ++i)
  {
    if (pWndData->pAnimationData[i].num_elements < 0 ||
        pWndData->pAnimationData[i].num_elements >= MAX_ELEMENTS)
    {
      pWndData->pAnimationData[i].num_elements = 0;
    }

    for(j = 0; j < pWndData->pAnimationData[i].num_elements; ++j)
    {
      if (pWndData->pAnimationData[i].index[j] != -1)
      {
        index = pWndData->pAnimationData[i].index[j] & ~AF_SOUND_RESOURCE;

        if (pWndData->pAnimationData[i].index[j] & AF_SOUND_RESOURCE)
        {
          if (index < -1 || index >= pWndData->nSounds)
          {
            pWndData->pAnimationData[i].index[j] = -1;
          }
        }
        else if (index < -1 || index >= pWndData->nBitmaps)
        {
          pWndData->pAnimationData[i].index[j] = -1;
        }
      }
    }
  }

  for(i = 0; i < pWndData->nLifeForms; ++i)
  {
    num_elements = GetNumLifeFormElements( pWndData->pLifeFormData[i].lifeform_type );
    num_bitmaps = GetNumLifeFormBitmaps( pWndData->pLifeFormData[i].lifeform_type );

    for(j = 0; j < num_elements; ++j)
    {
      if ( j < num_bitmaps )
      {
        n = pWndData->nBitmaps;
      }
      else
      {
        n = pWndData->nSounds;
      }

      if (pWndData->pLifeFormData[i].index[j] < -1 ||
          pWndData->pLifeFormData[i].index[j] >= n)
      {
        pWndData->pLifeFormData[i].index[j] = -1;
      }
    }
  }

  for(i = 0; i < pWndData->nItems; ++i)
  {
    num_elements = GetNumItemElements( pWndData->pItemData[i].item_type );
    num_bitmaps = GetNumItemBitmaps( pWndData->pItemData[i].item_type );

    for(j = 0; j < num_elements; ++j)
    {
      if ( j < num_bitmaps )
      {
        n = pWndData->nBitmaps;
      }
      else
      {
        n = pWndData->nSounds;
      }

      if (pWndData->pItemData[i].index[j] < -1 ||
          pWndData->pItemData[i].index[j] >= n)
      {
        pWndData->pItemData[i].index[j] = -1;
      }
    }
  }

  wMask = SF_HANGING | SF_WALK_THROUGH | SF_ANIMATED | SF_DAMAGE_ANIMATED |
          SF_DAMAGE_SOUND | SF_DESTRUCTABLE | SF_DAMAGE_ANIMATED_2;

  for(i = 0; i < pWndData->nSprites; ++i)
  {
    pWndData->pSpriteData[i].flags &= wMask;

    if (pWndData->pSpriteData[i].bitmap_index < -1 ||
        pWndData->pSpriteData[i].bitmap_index >= pWndData->nBitmaps)
    {
      pWndData->pSpriteData[i].bitmap_index = -1;
    }

    if (pWndData->pSpriteData[i].animation_index < -1 ||
        pWndData->pSpriteData[i].animation_index >= pWndData->nAnimations)
    {
      pWndData->pSpriteData[i].animation_index = -1;
    }

    if (pWndData->pSpriteData[i].damage_animation_index < -1 ||
        pWndData->pSpriteData[i].damage_animation_index >= pWndData->nAnimations)
    {
      pWndData->pSpriteData[i].damage_animation_index = -1;
    }

    if (pWndData->pSpriteData[i].damage_animation_index_2 < -1 ||
        pWndData->pSpriteData[i].damage_animation_index_2 >= pWndData->nAnimations)
    {
      pWndData->pSpriteData[i].damage_animation_index_2 = -1;
    }
  }

  wMask = 0;

  for(i = 0; i < pWndData->nNoises; ++i)
  {
    pWndData->pNoiseData[i].flags &= wMask;

    if (pWndData->pNoiseData[i].sound_index < -1 ||
        pWndData->pNoiseData[i].sound_index >= pWndData->nSounds)
    {
      pWndData->pNoiseData[i].sound_index = -1;
    }
  }

  for(i = 0; i < pWndData->nLevels; ++i)
  {
    pLevel = pWndData->pLevelData + i;

    if (pLevel->nLastMark < 0 || pLevel->nLastMark >= pLevel->nMarks)
    {
      pLevel->nLastMark = -1;
    }

    wMask = LF_BKG_BITMAP | 
            LF_SOL_BITMAP | LF_SOL_TEXT | 
            LF_EOL_BITMAP | LF_EOL_TEXT;

    pLevel->flags &= wMask;

    if (pLevel->bkg_bitmap_index < -1 ||
        pLevel->bkg_bitmap_index >= pWndData->nBitmaps)
    {
      pLevel->bkg_bitmap_index = -1;
    }

    if (pLevel->sol_bitmap_index < -1 ||
        pLevel->sol_bitmap_index >= pWndData->nBitmaps)
    {
      pLevel->sol_bitmap_index = -1;
    }

    if (pLevel->eol_bitmap_index < -1 ||
        pLevel->eol_bitmap_index >= pWndData->nBitmaps)
    {
      pLevel->eol_bitmap_index = -1;
    }

    wMask = TF_USER_TRIGGER | TF_PROXIMITY_TRIGGER | TF_ITEM_REQUIRED |
            TF_SOUND | TF_END_LEVEL | TF_END_GAME;

    for(j = 0; j < pLevel->nTriggers; ++j)
    {
      pLevel->pTriggerData[j].flags &= wMask;

      if (pLevel->pTriggerData[j].sound_index < -1 ||
          pLevel->pTriggerData[j].sound_index >= pWndData->nSounds)
      {
        pLevel->pTriggerData[j].sound_index = -1;
      }
    }

    wMask = MF_AUTO_RUN | MF_AUTO_RETURN | MF_TRIGGER_RETURN | 
            MF_START_SOUND | MF_RUN_SOUND | MF_STOP_SOUND;

    for(j = 0; j < pLevel->nMotions; ++j)
    {
      pLevel->pMotionData[j].flags &= wMask;

      if (pLevel->pMotionData[j].sound_index_start < -1 ||
          pLevel->pMotionData[j].sound_index_start >= pWndData->nSounds)
      {
        pLevel->pMotionData[j].sound_index_start = -1;
      }

      if (pLevel->pMotionData[j].sound_index_run < -1 ||
          pLevel->pMotionData[j].sound_index_run >= pWndData->nSounds)
      {
        pLevel->pMotionData[j].sound_index_run = -1;
      }

      if (pLevel->pMotionData[j].sound_index_stop < -1 ||
          pLevel->pMotionData[j].sound_index_stop >= pWndData->nSounds)
      {
        pLevel->pMotionData[j].sound_index_stop = -1;
      }
    }

    wMask = 7 | PF_SELECTED | PF_HIDDEN | PF_LINKED | PF_ANIMATED | 
            PF_DOUBLE_SIDED | PF_INVISIBLE | PF_WALK_THROUGH | 
            PF_MOTION_ANIMATED | PF_DAMAGE_ANIMATED | PF_MOVING_POLYGON |
            PF_SECRET_PLACE;

    for(j = 0; j < pLevel->nPolygons; ++j)
    {
      pPolygon = pLevel->pPolygonData + j;

      pPolygon->flags &= wMask;

      if (pPolygon->flags & PF_INVISIBLE)
      {
        pPolygon->flags &= ~(PF_LINKED | PF_MOVING_POLYGON | PF_MOTION_ANIMATED | PF_ANIMATED | PF_DAMAGE_ANIMATED);
      }

      if (pPolygon->texture_index < -1 ||
          pPolygon->texture_index >= pWndData->nTextures)
      {
        pPolygon->texture_index = -1;
      }

      if (pPolygon->animation_index < -1 ||
          pPolygon->animation_index >= pWndData->nAnimations)
      {
        pPolygon->animation_index = -1;
      }

      if (pPolygon->motion_index < -1 ||
          pPolygon->motion_index >= pLevel->nMotions)
      {
        pPolygon->motion_index = -1;
      }

      if (pPolygon->group_index < -1 ||
          pPolygon->group_index >= pLevel->nGroups)
      {
        pPolygon->group_index = -1;
      }

      if (pPolygon->damage_animation_index < -1 ||
          pPolygon->damage_animation_index >= pWndData->nAnimations)
      {
        pPolygon->damage_animation_index = -1;
      }
    }

    wMask = VF_SELECTED | VF_HIDDEN | VF_SPRITE | VF_LIFEFORM | VF_ITEM | 
            VF_TRIGGER | VF_NOISE | VF_PLAYER_START | VF_DIFFICULTY_LEVEL_1 | 
            VF_DIFFICULTY_LEVEL_2 | VF_DIFFICULTY_LEVEL_3;

    for(j = 0; j < pLevel->nVertices; ++j)
    {
      pVertex = pLevel->pVertexData + j;

      pVertex->flags &= wMask;

      pVertex->orientation &= 7;

      if (pVertex->lifeform_index < -1 ||
          pVertex->lifeform_index >= pWndData->nLifeForms)
      {
        pVertex->lifeform_index = -1;
      }

      if (pVertex->item_index < -1 ||
          pVertex->item_index >= pWndData->nItems)
      {
        pVertex->item_index = -1;
      }

      if (pVertex->sprite_index < -1 ||
          pVertex->sprite_index >= pWndData->nSprites)
      {
        pVertex->sprite_index = -1;
      }

      if (pVertex->noise_index < -1 ||
          pVertex->noise_index >= pWndData->nNoises)
      {
        pVertex->noise_index = -1;
      }

      if (pVertex->trigger_index < -1 ||
          pVertex->trigger_index >= pLevel->nTriggers)
      {
        pVertex->trigger_index = -1;
      }

      if (pVertex->group_index < -1 ||
          pVertex->group_index >= pLevel->nGroups)
      {
        pVertex->group_index = -1;
      }
    }
  }

  return TRUE;

} // FilterObject


/*******************************************************************

    NAME:       LoadObjectFile

    SYNOPSIS:   Load the object file given.

    ENTRY:      lpszFile - Full DOS name (with drive and path) of
                  the object file.
    
                pWndData - Object window to receive the object.

    RETURNS:    BOOL - TRUE if successful, FALSE on error.

********************************************************************/

BOOL LoadObjectFile(LPSTR lpszFile, WND_DATA *pWndData)
{
  INT i, j, n, id;
  HFILE fh;
  DWORD dwTagSize, dwSize;
  HCURSOR hcurSave;
  LEVEL_DATA *pLevel;
  ZONE_DATA *pZone;

  //
  //  Open the file.
  //

  fh = _lopen(lpszFile, READ);

  if (fh == HFILE_ERROR)
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to open %s",
            (LPSTR)lpszFile );

    return FALSE;
  }

  hcurSave = SetCursor(LoadCursor(NULL,IDC_WAIT));

  pWndData->nBitmaps = 0;
  pWndData->nTextures = 0;
  pWndData->nAnimations = 0;
  pWndData->nItems = 0;
  pWndData->nSprites = 0;
  pWndData->nNoises = 0;
  pWndData->nLifeForms = 0;
  pWndData->nSounds = 0;

  do
  {
    if (_lread(fh, &id, 2) != 2)
    {
      break;
    }

    if (_lread(fh, &dwTagSize, 4) != 4)
    {
      _lclose(fh);
      SetCursor(hcurSave);

      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to read %s",
              (LPSTR)lpszFile );

      return FALSE;
    }

    if (id == ID_FILE_INFO)
    {
       FILE_INFO_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (dwTagSize > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+6, (WORD)dwTagSize-6) != (WORD)dwTagSize-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      _fstrcpy(pWndData->file_name, hdr.name);
      _fstrcpy(pWndData->file_author, hdr.author);
      _fstrcpy(pWndData->file_date, hdr.date);
      _fstrcpy(pWndData->file_version, hdr.version);
    }
    else if (id == ID_PALETTE_DATA)
    {
      PALETTE_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (dwTagSize > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+6, (WORD)dwTagSize-6) != (WORD)dwTagSize-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      for(i = 0; i < 256; ++i)
      {
        pWndData->pPalette->red[i] = hdr.red[i];
        pWndData->pPalette->green[i] = hdr.green[i];
        pWndData->pPalette->blue[i] = hdr.blue[i];
      }
    }
    else if (id == ID_BITMAP_DATA)
    {
      BITMAP_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if ( _lread(fh, ((BYTE *)&hdr)+6, 2) != 2)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (hdr.hdr_size > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if ( _lread(fh, ((BYTE *)&hdr)+8, (WORD)hdr.hdr_size-8) != (WORD)hdr.hdr_size-8)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (pWndData->nBitmaps == MAX_BITMAPS)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      i = pWndData->nBitmaps;

      dwSize = (DWORD)hdr.x_len * (DWORD)hdr.y_len;

      pWndData->pBitmapData[i].x_len = hdr.x_len;
      pWndData->pBitmapData[i].y_len = hdr.y_len;
      pWndData->pBitmapData[i].flags = hdr.flags;

      _fstrcpy(pWndData->pBitmapData[i].bitmap_name, hdr.bitmap_name);

      pWndData->pBitmapData[i].data = (BYTE *)GlobalAllocPtr(GPTR, dwSize);

      if (pWndData->pBitmapData[i].data == NULL)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_hread(fh, pWndData->pBitmapData[i].data, (long)dwSize) != (long)dwSize)
      {
        GlobalFreePtr(pWndData->pBitmapData[i].data);
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      ++pWndData->nBitmaps;
    }
    else if (id == ID_TEXTURE_DATA)
    {
      TEXTURE_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (dwTagSize > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+6, (WORD)dwTagSize-6) != (WORD)dwTagSize-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (pWndData->nTextures == MAX_TEXTURES)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      i = pWndData->nTextures;

      pWndData->pTextureData[i].bitmap_index = hdr.bitmap_index;
      pWndData->pTextureData[i].color_index = hdr.color_index;
      pWndData->pTextureData[i].pixel_size = hdr.pixel_size;

      _fstrcpy(pWndData->pTextureData[i].texture_name, hdr.texture_name);

      ++pWndData->nTextures;
    }
    else if (id == ID_SOUND_DATA)
    {
      SOUND_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if ( _lread(fh, ((BYTE *)&hdr)+6, 2) != 2)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (hdr.hdr_size > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if ( _lread(fh, ((BYTE *)&hdr)+8, (WORD)hdr.hdr_size-8) != (WORD)hdr.hdr_size-8)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (pWndData->nSounds == MAX_SOUNDS)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      i = pWndData->nSounds;

      dwSize = dwTagSize - (DWORD)hdr.hdr_size;

      pWndData->pSoundData[i].length = (long)dwSize;
      pWndData->pSoundData[i].flags = hdr.flags;

      _fstrcpy(pWndData->pSoundData[i].sound_name, hdr.sound_name);

      pWndData->pSoundData[i].data = (BYTE *)GlobalAllocPtr(GPTR, dwSize);

      if (pWndData->pSoundData[i].data == NULL)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_hread(fh, pWndData->pSoundData[i].data, (long)dwSize) != (long)dwSize)
      {
        GlobalFreePtr(pWndData->pSoundData[i].data);
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      ++pWndData->nSounds;
    }
    else if (id == ID_ANIMATION_DATA)
    {
      ANIMATION_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (dwTagSize > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+6, (WORD)dwTagSize-6) != (WORD)dwTagSize-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (pWndData->nAnimations == MAX_ANIMATIONS)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      i = pWndData->nAnimations;

      pWndData->pAnimationData[i].num_elements = hdr.num_elements;
      pWndData->pAnimationData[i].delay = hdr.delay;
      pWndData->pAnimationData[i].pixel_size = hdr.pixel_size;
      pWndData->pAnimationData[i].first_element = hdr.first_element;

      _fstrcpy(pWndData->pAnimationData[i].animation_name, hdr.animation_name);

      for(j = 0; j < hdr.num_elements; ++j)
      {
        pWndData->pAnimationData[i].index[j] = hdr.index[j];
      }

      ++pWndData->nAnimations;
    }
    else if (id == ID_LIFEFORM_DATA)
    {
      LIFEFORM_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (dwTagSize > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+6, (WORD)dwTagSize-6) != (WORD)dwTagSize-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (pWndData->nLifeForms == MAX_LIFEFORMS)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      i = pWndData->nLifeForms;

      pWndData->pLifeFormData[i].lifeform_type = hdr.lifeform_type;
      pWndData->pLifeFormData[i].pixel_size = hdr.pixel_size;

      _fstrcpy(pWndData->pLifeFormData[i].lifeform_name, hdr.lifeform_name);

      n = GetNumLifeFormElements( hdr.lifeform_type );

      for(j = 0; j < n; ++j)
      {
        pWndData->pLifeFormData[i].index[j] = hdr.index[j];
      }

      ++pWndData->nLifeForms;
    }
    else if (id == ID_ITEM_DATA)
    {
      ITEM_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (dwTagSize > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+6, (WORD)dwTagSize-6) != (WORD)dwTagSize-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (pWndData->nItems == MAX_ITEMS)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      i = pWndData->nItems;

      pWndData->pItemData[i].item_type = hdr.item_type;
      pWndData->pItemData[i].pixel_size = hdr.pixel_size;

      _fstrcpy(pWndData->pItemData[i].item_name, hdr.item_name);

      n = GetNumItemElements( hdr.item_type );

      for(j = 0; j < n; ++j)
      {
        pWndData->pItemData[i].index[j] = hdr.index[j];
      }

      ++pWndData->nItems;
    }
    else if (id == ID_SPRITE_DATA || id == ID_OLD_SPRITE_DATA)
    {
      SPRITE_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (dwTagSize > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+6, (WORD)dwTagSize-6) != (WORD)dwTagSize-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (id == ID_OLD_SPRITE_DATA)
      {
        hdr.damage_animation_index = hdr.damage_animation_index_2;
        hdr.damage_animation_index_2 = -1;
      }

      if (pWndData->nSprites == MAX_SPRITES)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      i = pWndData->nSprites;

      pWndData->pSpriteData[i].flags = hdr.flags;
      pWndData->pSpriteData[i].bitmap_index = hdr.bitmap_index;
      pWndData->pSpriteData[i].animation_index = hdr.animation_index;
      pWndData->pSpriteData[i].damage_animation_index = hdr.damage_animation_index;
      pWndData->pSpriteData[i].damage_animation_index_2 = hdr.damage_animation_index_2;
      pWndData->pSpriteData[i].pixel_size = hdr.pixel_size;

      _fstrcpy(pWndData->pSpriteData[i].sprite_name, hdr.sprite_name);

      ++pWndData->nSprites;
    }
    else if (id == ID_NOISE_DATA)
    {
      NOISE_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (dwTagSize > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+6, (WORD)dwTagSize-6) != (WORD)dwTagSize-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (pWndData->nNoises == MAX_NOISES)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      i = pWndData->nNoises;

      pWndData->pNoiseData[i].sound_index = hdr.sound_index;
      pWndData->pNoiseData[i].delay = hdr.delay;
      pWndData->pNoiseData[i].radius = hdr.radius;
      pWndData->pNoiseData[i].height = hdr.height;

      _fstrcpy(pWndData->pNoiseData[i].noise_name, hdr.noise_name);

      ++pWndData->nNoises;
    }
    else if (id == ID_OLD_LEVEL_DATA)
    {
      OLD_LEVEL_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (_lread(fh, ((BYTE *)&hdr)+6, 2) != 2)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (hdr.hdr_size > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+8, (WORD)hdr.hdr_size-8) != (WORD)hdr.hdr_size-8)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      pLevel = pWndData->pLevelData + pWndData->nLevels;

      if (pWndData->nLevels == MAX_LEVELS)
      {
        MsgBox( NULL,
                MB_ICONEXCLAMATION,
                "Maximum number of levels has been reached" );

        return FALSE;
      }

      if (Frame_NewLevel(pWndData, pWndData->nLevels) == FALSE)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      ++pWndData->nLevels;

      if (hdr.nVertices > 0)
      {
        dwSize = (DWORD)sizeof(VERTEX) * (DWORD)hdr.nVertices;

        if (_hread(fh, pLevel->pVertexData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nPolygons > 0)
      {
        dwSize = (DWORD)sizeof(POLYGON) * (DWORD)hdr.nPolygons;

        if (_hread(fh, pLevel->pPolygonData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nTriggers > 0)
      {
        dwSize = (DWORD)sizeof(TRIGGER) * (DWORD)hdr.nTriggers;

        if (_hread(fh, pLevel->pTriggerData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nMotions > 0)
      {
        dwSize = (DWORD)sizeof(MOTION) * (DWORD)hdr.nMotions;

        if (_hread(fh, pLevel->pMotionData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nGroups > 0)
      {
        dwSize = (DWORD)sizeof(GROUP) * (DWORD)hdr.nGroups;

        if (_hread(fh, pLevel->pGroupData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      pLevel->view_home_rotation = hdr.view_home_rotation;
      pLevel->view_home_elevation = hdr.view_home_elevation;
      pLevel->view_home_x = hdr.view_home_x;
      pLevel->view_home_y = hdr.view_home_y;
      pLevel->view_home_z = hdr.view_home_z;
      pLevel->view_rotation = hdr.view_rotation;
      pLevel->view_elevation = hdr.view_elevation;
      pLevel->view_x = hdr.view_x;
      pLevel->view_y = hdr.view_y;
      pLevel->view_z = hdr.view_z;
      pLevel->view_delta = hdr.view_delta;
      pLevel->view_depth = hdr.view_depth;
      pLevel->view_width = hdr.view_width;
      pLevel->view_height = hdr.view_height;
      pLevel->grid_x = hdr.grid_x;
      pLevel->grid_y = hdr.grid_y;
      pLevel->grid_z = hdr.grid_z;
      pLevel->grid_rotation = hdr.grid_rotation;
      pLevel->grid_elevation = hdr.grid_elevation;
      pLevel->grid_delta = hdr.grid_delta;
      pLevel->grid_spacing = hdr.grid_spacing;
      pLevel->grid_size = hdr.grid_size;
      pLevel->bMaintainGridDist = hdr.bMaintainGridDist;
      pLevel->bLockViewToGrid = hdr.bLockViewToGrid;
      pLevel->bSnapToGrid = hdr.bSnapToGrid;

      pLevel->nVertices = hdr.nVertices;
      pLevel->nPolygons = hdr.nPolygons;
      pLevel->nTriggers = hdr.nTriggers;
      pLevel->nMotions = hdr.nMotions;
      pLevel->nGroups = hdr.nGroups;
      pLevel->nMarks = 0;
      pLevel->nLastMark = -1;

      pLevel->flags = hdr.flags;
      pLevel->bkg_bitmap_index = hdr.bkg_bitmap_index;
      pLevel->sol_bitmap_index = hdr.sol_bitmap_index;
      pLevel->eol_bitmap_index = hdr.eol_bitmap_index;

      _fstrcpy(pLevel->level_name, hdr.level_name);
      _fstrcpy(pLevel->level_author, hdr.level_author);
      _fstrcpy(pLevel->sol_text, hdr.sol_text);
      _fstrcpy(pLevel->eol_text, hdr.eol_text);
    }
    else if (id == ID_LEVEL_DATA)
    {
      LEVEL_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (_lread(fh, ((BYTE *)&hdr)+6, 2) != 2)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (hdr.hdr_size > sizeof(hdr))
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      if (_lread(fh, ((BYTE *)&hdr)+8, (WORD)hdr.hdr_size-8) != (WORD)hdr.hdr_size-8)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      pLevel = pWndData->pLevelData + pWndData->nLevels;

      if (pWndData->nLevels == MAX_LEVELS)
      {
        MsgBox( NULL,
                MB_ICONEXCLAMATION,
                "Maximum number of levels has been reached" );

        return FALSE;
      }

      if (Frame_NewLevel(pWndData, pWndData->nLevels) == FALSE)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      ++pWndData->nLevels;

      if (hdr.nVertices > 0)
      {
        dwSize = (DWORD)sizeof(VERTEX) * (DWORD)hdr.nVertices;

        if (_hread(fh, pLevel->pVertexData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nPolygons > 0)
      {
        dwSize = (DWORD)sizeof(POLYGON) * (DWORD)hdr.nPolygons;

        if (_hread(fh, pLevel->pPolygonData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nTriggers > 0)
      {
        dwSize = (DWORD)sizeof(TRIGGER) * (DWORD)hdr.nTriggers;

        if (_hread(fh, pLevel->pTriggerData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nMotions > 0)
      {
        dwSize = (DWORD)sizeof(MOTION) * (DWORD)hdr.nMotions;

        if (_hread(fh, pLevel->pMotionData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nGroups > 0)
      {
        dwSize = (DWORD)sizeof(GROUP) * (DWORD)hdr.nGroups;

        if (_hread(fh, pLevel->pGroupData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      if (hdr.nMarks > 0)
      {
        dwSize = (DWORD)sizeof(MARK) * (DWORD)hdr.nMarks;

        if (_hread(fh, pLevel->pMarkData, (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }

      pLevel->view_home_rotation = hdr.view_home_rotation;
      pLevel->view_home_elevation = hdr.view_home_elevation;
      pLevel->view_home_x = hdr.view_home_x;
      pLevel->view_home_y = hdr.view_home_y;
      pLevel->view_home_z = hdr.view_home_z;
      pLevel->view_rotation = hdr.view_rotation;
      pLevel->view_elevation = hdr.view_elevation;
      pLevel->view_x = hdr.view_x;
      pLevel->view_y = hdr.view_y;
      pLevel->view_z = hdr.view_z;
      pLevel->view_delta = hdr.view_delta;
      pLevel->view_depth = hdr.view_depth;
      pLevel->view_width = hdr.view_width;
      pLevel->view_height = hdr.view_height;
      pLevel->grid_x = hdr.grid_x;
      pLevel->grid_y = hdr.grid_y;
      pLevel->grid_z = hdr.grid_z;
      pLevel->grid_rotation = hdr.grid_rotation;
      pLevel->grid_elevation = hdr.grid_elevation;
      pLevel->grid_delta = hdr.grid_delta;
      pLevel->grid_spacing = hdr.grid_spacing;
      pLevel->grid_size = hdr.grid_size;
      pLevel->bMaintainGridDist = hdr.bMaintainGridDist;
      pLevel->bLockViewToGrid = hdr.bLockViewToGrid;
      pLevel->bSnapToGrid = hdr.bSnapToGrid;

      pLevel->nVertices = hdr.nVertices;
      pLevel->nPolygons = hdr.nPolygons;
      pLevel->nTriggers = hdr.nTriggers;
      pLevel->nMotions = hdr.nMotions;
      pLevel->nGroups = hdr.nGroups;
      pLevel->nMarks = hdr.nMarks;
      pLevel->nLastMark = hdr.nLastMark;

      pLevel->flags = hdr.flags;
      pLevel->bkg_bitmap_index = hdr.bkg_bitmap_index;
      pLevel->sol_bitmap_index = hdr.sol_bitmap_index;
      pLevel->eol_bitmap_index = hdr.eol_bitmap_index;

      _fstrcpy(pLevel->level_name, hdr.level_name);
      _fstrcpy(pLevel->level_author, hdr.level_author);
      _fstrcpy(pLevel->sol_text, hdr.sol_text);
      _fstrcpy(pLevel->eol_text, hdr.eol_text);
    }
    else if (id == ID_ZONE_DATA)
    {
      ZONE_DATA_HDR hdr;

      _fmemset(&hdr, 0, sizeof(hdr));

      if (_lread(fh, ((BYTE *)&hdr)+6, sizeof(hdr)-6) != sizeof(hdr)-6)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      dwSize = dwTagSize - sizeof(hdr);

      if (hdr.level_index < 0 || hdr.level_index >= MAX_LEVELS)
      {
        _lclose(fh);
        SetCursor(hcurSave);
        return FALSE;
      }

      pZone = pWndData->pZoneData + hdr.level_index;

      pZone->data = (BYTE huge *)GlobalAllocPtr(GPTR, dwTagSize);

      if (pZone->data == NULL)
      {
        _lseek(fh, dwSize, SEEK_CUR);
      }
      else
      {
        hdr.id = id;
        hdr.size = (long)dwTagSize;

        pZone->size = (long)dwTagSize;

        _fmemcpy(pZone->data, &hdr, sizeof(hdr));

        if (_hread(fh, pZone->data + sizeof(hdr), (long)dwSize) != (long)dwSize)
        {
          _lclose(fh);
          SetCursor(hcurSave);
          return FALSE;
        }
      }
    }
    else
    {
      if (id > 100)
      {
        _lclose(fh);
        SetCursor(hcurSave);

        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Unknown tag %d in %s",
                id,
                (LPSTR)lpszFile );

        return FALSE;
      }

      _lseek(fh, dwTagSize - 6, SEEK_CUR);
    }
  } while(TRUE);

  //
  //  Close the file.
  //

  _lclose(fh);
  SetCursor(hcurSave);

  if (pWndData->nLevels == 0)
  {
    if (Frame_NewLevel(pWndData, 0) == FALSE)
    {
      _lclose(fh);
      SetCursor(hcurSave);
      return FALSE;
    }

    pWndData->nLevels = 1;
  }

  FilterObject(pWndData);

  return TRUE;
  
} // LoadObjectFile
