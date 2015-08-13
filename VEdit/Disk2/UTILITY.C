/*******************************************************************

    utility.c

    This file contains routines of general utility.

********************************************************************/


#include "vedit.h"


/*******************************************************************

    NAME:       GenWindowTitle

    SYNOPSIS:   Generate an appropriate title for the window.

    ENTRY:      pWndData - Pointer to the window data.
                
                szTitle - The title buffer.
                
                cbTitle - The length of the title buffer.

    RETURNS:    The pointer to the string buffer.

********************************************************************/

LPSTR GenWindowTitle(WND_DATA *pWndData, LPSTR szTitle, INT cbTitle)
{
  wsprintf(szTitle, "%s", pWndData->name);
  
  return szTitle;

} // GenWindowTitle


/*******************************************************************

    NAME:       LoadStr

    SYNOPSIS:   Load a string resource (with error checking).

    ENTRY:      idResource - The resource ID.
                
                lpszBuffer - The string buffer.
                
                cbBuffer - The length of the buffer.

    RETURNS:    The number of bytes copied or zero if not found.

********************************************************************/

INT LoadStr(UINT idResource, LPSTR lpszBuffer, INT cbBuffer)
{
  INT nResult;
  
  nResult = LoadString(hInst, idResource, lpszBuffer, cbBuffer);
  
  if (nResult == 0)
  {
    MsgBox( hwndMDIClient,
            MB_ICONSTOP | MB_OK,
            "LoadString failed (%d)",
            idResource );

    *lpszBuffer = '\0';
  }

  return nResult;

} // LoadStr


/*******************************************************************

    NAME:       Round

    SYNOPSIS:   Round a floating point number to closest integer.

    ENTRY:      num - The number to be rounded.

    EXIT:       Returns the rounded number.

********************************************************************/

double Round(double num)
{
  double rounded_num;

  if (num < 0.0)
  {
    rounded_num = (double)((long)(num - 0.5));
  }
  else
  {
    rounded_num = (double)((long)(num + 0.5));
  }

  return rounded_num;
  
} // Round


/*******************************************************************

    NAME:       DoubleToString

    SYNOPSIS:   Convert the double to a string.

    ENTRY:      szBuf - Buffer to receive the string.
                
                num - The number to convert.
                
                precision - How many digits past the decimal point.
                
    RETURNS:    The szBuf pointer.

********************************************************************/

LPSTR DoubleToString(LPSTR szBuf, double num, INT precision)
{
  INT i;
  LONG lTmp, lMult;
  CHAR szFormat[20];
  
  lMult = 1L;

  if (precision > 9)
  {
    precision = 9;
  }
      
  for(i = 0; i < precision; ++i)
  {
    lMult *= 10L;
  }

  if (num > 0.0)
  {
    lTmp = (LONG)(num * (double)lMult + 0.5);
  }
  else
  {
    lTmp = (LONG)(num * (double)lMult - 0.5);
  }

  if (precision > 0)
  {
    if (lTmp < 0L)
    {
      wsprintf(szFormat, "-%%ld.%%0%dld", precision);
      wsprintf(szBuf, szFormat, -lTmp / lMult, -lTmp % lMult);
    }
    else
    {
      wsprintf(szFormat, "%%ld.%%0%dld", precision);
      wsprintf(szBuf, szFormat, lTmp / lMult, lTmp % lMult);
    }
  }
  else
  {
    wsprintf(szBuf, "%ld", lTmp);
  }

  return szBuf;

} // DoubleToString


/*******************************************************************

    NAME:       isalnum_str

    SYNOPSIS:   Return TRUE if the string is alpha-numeric.

    ENTRY:      string - The string to test.

    RETURNS:    TRUE if the string is alpha-numeric.
                FALSE if not alpha-numeric.

********************************************************************/

BOOL isalnum_str(char *str)
{
  char *p;

  p = str;

  if (!(*p))
  {
    return FALSE;                       // empty string is not alpha-numeric
  }
  
  while(*p && isalnum(*p))
  {
    ++p;
  }
  
  if (*p)
  {
    return FALSE;                       // string is not alpha-numeric
  }
  
  return TRUE;                          // string is alpha-numeric
  
} // isalnum_str


/*******************************************************************

    NAME:       StripTrailingSpaces

    SYNOPSIS:   Strip trailing spaces (or tabs) from the end of the
                string given.

    ENTRY:      string - The string.

    RETURNS:    none

********************************************************************/

VOID StripTrailingSpaces(char *str)
{
  char *p;
  int len;

  len = _fstrlen(str);
  p = str + len;

  while(len > 0)
  {
    --len;
    --p;

    if (*p == ' ' || *p == 9)
    {
      *p = '\0';
    }
    else
    {
      break;
    }
  }

  return;

} // StripTrailingSpaces


/*******************************************************************

    NAME:       StripExtension

    SYNOPSIS:   Strip the extension from the DOS file name given.

    ENTRY:      dos_name - The DOS name.

    RETURNS:    none

********************************************************************/

VOID StripExtension(char *dos_name)
{
  char drive[3], dir[64], fname[9], ext[5];
  
  _splitpath(dos_name, drive, dir, fname, ext);

  if (ext[0])
  {
    *(_fstrrchr(dos_name, '.' )) = '\0';
  }
  
  return;

} // StripExtension


/*******************************************************************

    NAME:       CreatePath

    SYNOPSIS:   Create the path indicated. The path must be
                terminated in a backslash.

    ENTRY:      path - The DOS path.

    RETURNS:    TRUE if all goes well.
                FALSE if unable to create directory.

********************************************************************/

BOOL CreatePath(char *path)
{
  char next_path[128], *ptr;
  struct find_t fs;

  _fstrcpy(next_path,path);

  ptr = _fstrrchr(next_path,':');

  if (ptr)
  {
    ++ptr;
  }
  else
  {
    ptr = next_path;
  }

  if (*ptr == '\0')
  {
    return TRUE;                        // all done
  }
  
  ptr = _fstrchr(++ptr,'\\');

  while(ptr)
  {
    *ptr = '\0';

    if (_dos_findfirst(next_path, _A_SUBDIR, &fs))
    {
      if (_mkdir(next_path)) 
      {
        return FALSE;
      }
    }

    _fstrcpy(next_path, path);

    ptr = _fstrchr(++ptr, '\\');
  }

  return TRUE;
  
} // CreatePath


/*******************************************************************

    NAME:       MsgBox

    SYNOPSIS:   A printf-like interface to MessageBox.

    ENTRY:      hwndParent - The "owning" parent window.

                fuType - A set of MB_* flags.

                pszFormat - A printf-like format string.

                ... - Other printf-like arguments as needed.

********************************************************************/

INT MsgBox(HWND hwndParent, UINT fuType, LPSTR pszFormat, ...)
{
  INT nResult;
  CHAR szOutput[MAX_PRINTF_OUTPUT];
  va_list ArgList;

  va_start(ArgList, pszFormat);
  wvsprintf(szOutput, pszFormat, ArgList);
  va_end(ArgList);

  nResult = MessageBox(hwndParent, szOutput, pszAppLongName, fuType);
  
  return nResult;

} // MsgBox


/*******************************************************************

    NAME:       ExtPrintf

    SYNOPSIS:   A printf-like interface to TextOut.

    ENTRY:      hdc - Display context for the text.

                x - X coordinate in logical units.

                y - Y coordiante in logical units.

                pszFormat - A printf-like format string.

                ... - Other printf-like arguments as needed.

********************************************************************/

VOID ExtPrintf(HDC hdc, INT x, INT y, LPSTR pszFormat, ...)
{
  INT cbOutput;
  CHAR szOutput[MAX_PRINTF_OUTPUT];
  va_list ArgList;

  va_start(ArgList, pszFormat);
  cbOutput = wvsprintf(szOutput, pszFormat, ArgList);
  va_end(ArgList);

  TextOut(hdc, x, y, szOutput, cbOutput);
  
} // ExtPrintf


/*******************************************************************

    NAME:       CopyFile

    SYNOPSIS:   Copy a file from source to destination.

    ENTRY:      szReadFile - Source file specification.
                
                szWriteFile - Destination file specification.

    RETURNS:    TRUE on success, FALSE on error.

********************************************************************/

BOOL CopyFile(LPSTR szReadFile, LPSTR szWriteFile)
{
  BOOL bSuccess;
  WORD wBytes;
  LPSTR pBuf;
  HFILE hReadFile, hWriteFile;

  pBuf = (LPSTR)GlobalAllocPtr(GPTR, 16384);
  
  if (pBuf == NULL)
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Out of memory" );

    return FALSE;
  }

  hReadFile = _lopen(szReadFile, READ);

  if (hReadFile == HFILE_ERROR)
  {
    GlobalFreePtr(pBuf);
    
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Unable to open %s",
            szReadFile );

    return FALSE;
  }

  hWriteFile = _lcreat(szWriteFile, 0);

  if (hWriteFile == HFILE_ERROR)
  {
    _lclose(hReadFile);

    GlobalFreePtr(pBuf);
    
    if (access(szWriteFile, 0) == 0)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to overwrite %s",
              szWriteFile );
    }
    else
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Unable to create %s",
              szWriteFile );
    }

    return FALSE;
  }

  bSuccess = TRUE;
  
  do
  {
    wBytes = _lread(hReadFile, pBuf, 16384);

    if (wBytes == HFILE_ERROR)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Error reading %s",
              szReadFile );

      bSuccess = FALSE;
      break;
    }

    if (wBytes > 0)
    {
      if (_lwrite(hWriteFile, pBuf, wBytes) != wBytes)
      {
        MsgBox( hwndMDIClient,
                MB_ICONEXCLAMATION,
                "Error writing %s",
                szWriteFile );

        bSuccess = FALSE;
        break;
      }
    }

  } while(wBytes > 0);
  
  _lclose(hWriteFile);
  _lclose(hReadFile);

  GlobalFreePtr(pBuf);

  if (!bSuccess)
  {
    remove(szWriteFile);
  }
  
  return bSuccess;
  
} // CopyFile


/*******************************************************************

    NAME:       FilePrintf

    SYNOPSIS:   A fprintf-like interface for file output.

    ENTRY:      hf - The file handle (from _lopen or _lcreat).

                pszFormat - A printf-like format string.

                ... - Other printf-like arguments as needed.

********************************************************************/

INT FilePrintf(HFILE hf, LPSTR pszFormat, ...)
{
  INT cbOutput;
  CHAR szOutput[MAX_PRINTF_OUTPUT];
  va_list ArgList;

  va_start(ArgList, pszFormat);
  cbOutput = wvsprintf(szOutput, pszFormat, ArgList);
  va_end(ArgList);

  cbOutput = _lwrite(hf, szOutput, cbOutput);

  return cbOutput;
  
} // FilePrintf


/*******************************************************************

    NAME:       GetFirstBitmapIndex

    SYNOPSIS:   Get first bitmap index of an animation.

    ENTRY:      pAnimation - The animation pointer. 

    RETURNS:    The bitmap index or -1.

********************************************************************/

INT GetFirstBitmapIndex(ANIMATION_DATA *pAnimation)
{
  INT i, n, nBmpIndex;

  nBmpIndex = -1;

  n = pAnimation->num_elements;

  for(i = 0; i < n; ++i)
  {
    if (pAnimation->index[i] != -1)
    {
      if ((pAnimation->index[i] & AF_SOUND_RESOURCE) == 0)
      {
        nBmpIndex = pAnimation->index[i];
        break;
      }
    }
  }

  return nBmpIndex;

} // GetFirstBitmapIndex


/*******************************************************************

    NAME:       GetNumLifeFormElements

    SYNOPSIS:   Get the total number of elements used by the
                indicated life form type.

    ENTRY:      nLifeFormType - The life form type.

    RETURNS:    The number of elements used for this life form.

********************************************************************/

INT GetNumLifeFormElements(INT nLifeFormType)
{
  INT nElements;

  nElements = 0;

  if (nLifeFormType == 0)
  {
    nElements = 21;                     // insect
  }
  else if (nLifeFormType == 1)
  {
    nElements = 72;                     // robot
  }
  else if (nLifeFormType == 2)
  {
    nElements = 47;                     // agent
  }
  else if (nLifeFormType == 3)
  {
    nElements = 46;                     // alien
  }
  
  return nElements;

} // GetNumLifeFormElements


/*******************************************************************

    NAME:       GetNumLifeFormBitmaps

    SYNOPSIS:   Get the total number of bitmaps used by the
                indicated life form type.

    ENTRY:      nLifeFormType - The life form type.

    RETURNS:    The number of bitmaps used for this life form.

********************************************************************/

INT GetNumLifeFormBitmaps(INT nLifeFormType)
{
  INT nBitmaps;

  nBitmaps = 0;

  if (nLifeFormType == 0)
  {
    nBitmaps = 19;                      // insect
  }
  else if (nLifeFormType == 1)
  {
    nBitmaps = 64;                      // robot
  }
  else if (nLifeFormType == 2)
  {
    nBitmaps = 41;                      // agent
  }
  else if (nLifeFormType == 3)
  {
    nBitmaps = 42;                      // alien
  }
  
  return nBitmaps;

} // GetNumLifeFormBitmaps


/*******************************************************************

    NAME:       GetNumLifeFormSounds

    SYNOPSIS:   Get the total number of sounds used by the
                indicated life form type.

    ENTRY:      nLifeFormType - The life form type.

    RETURNS:    The number of sounds used for this life form.

********************************************************************/

INT GetNumLifeFormSounds(INT nLifeFormType)
{
  INT nSounds;

  nSounds = 0;

  if (nLifeFormType == 0)
  {
    nSounds = 2;                        // insect
  }
  else if (nLifeFormType == 1)
  {
    nSounds = 8;                        // robot
  }
  else if (nLifeFormType == 2)
  {
    nSounds = 6;                        // agent
  }
  else if (nLifeFormType == 3)
  {
    nSounds = 4;                        // alien
  }

  return nSounds;

} // GetNumLifeFormSounds


/*******************************************************************

    NAME:       GetLifeFormTypeString

    SYNOPSIS:   Get the string for the indicated life form type.

    ENTRY:      nLifeFormType - The life form type.

    RETURNS:    The string.

********************************************************************/

LPSTR GetLifeFormTypeString(INT nLifeFormType)
{
  LPSTR lpsz;

  lpsz = "Unknown";

  if (nLifeFormType == 0)
  {
    lpsz = "Insect";
  }
  else if (nLifeFormType == 1)
  {
    lpsz = "Robot";
  }
  else if (nLifeFormType == 2)
  {
    lpsz = "Agent";
  }
  else if (nLifeFormType == 3)
  {
    lpsz = "Alien";
  }
  
  return lpsz;

} // GetLifeFormTypeString


/*******************************************************************

    NAME:       GetLifeFormElementString

    SYNOPSIS:   Get the string for the indicated life form type and
                element.

    ENTRY:      nLifeFormType - The life form type.

                nElement - The element number.

    RETURNS:    The string.

********************************************************************/

LPSTR GetLifeFormElementString(INT nLifeFormType, INT nElement)
{
  LPSTR lpsz;

  static CHAR *szInsect[] =
  {
    "Flight 1",
    "Flight 2",
    "Flight 3",
    "Flight 4",
    "Flight 5",
    "Flight 6",
    "Attack 1",
    "Attack 2",
    "Attack 3",
    "Attack 4",
    "Death 1",
    "Death 2",
    "Death 3",
    "Death 4",
    "Death 5",
    "Death 6",
    "Death 7",
    "Death 8",
    "Death 9",
    "Flight sound",
    "Death sound"
  };

  static CHAR *szRobot[] =
  {
    "Standing 0",
    "Standing 45",
    "Standing 90",
    "Standing 135",
    "Standing 180",
    "Standing 225",
    "Standing 270",
    "Standing 315",
    "Left foot 0",
    "Left foot 45",
    "Left foot 90",
    "Left foot 135",
    "Left foot 180",
    "Left foot 225",
    "Left foot 270",
    "Left foot 315",
    "Right foot 0",
    "Right foot 45",
    "Right foot 90",
    "Right foot 135",
    "Right foot 180",
    "Right foot 225",
    "Right foot 270",
    "Right foot 315",
    "Attack 1",
    "Attack 2",
    "Attack 3",
    "Attack 4",
    "Attack 5",
    "Attack 6",
    "Attack 7",
    "Attack 8",
    "Normal death 1",
    "Normal death 2",
    "Normal death 3",
    "Normal death 4",
    "Normal death 5",
    "Normal death 6",
    "Normal death 7",
    "Normal death 8",
    "Normal death 9",
    "Normal death 10",
    "Normal death 11",
    "Normal death 12",
    "Normal death 13",
    "Normal death 14",
    "Normal death 15",
    "Normal death 16",
    "Normal death 17",
    "Normal death 18",
    "Normal death 19",
    "Normal death 20",
    "Flying plasma 1",
    "Flying plasma 2",
    "Flying plasma 3",
    "Plasma impact 1",
    "Plasma impact 2",
    "Plasma impact 3",
    "Exploding death 1",
    "Exploding death 2",
    "Exploding death 3",
    "Exploding death 4",
    "Exploding death 5",
    "Exploding death 6",
    "Robot walk sound",
    "Plasma flying sound",
    "Plasma explode sound",
    "Robot slash sound",
    "Player struck sound",
    "Robot ricochet sound",
    "Normal death sound",
    "Exploding death sound"
  };

  static CHAR *szAgent[] =
  {
    "Standing 0",
    "Standing 45",
    "Standing 90",
    "Standing 135",
    "Standing 180",
    "Standing 225",
    "Standing 270",
    "Standing 315",
    "Left foot 0",
    "Left foot 45",
    "Left foot 90",
    "Left foot 135",
    "Left foot 180",
    "Left foot 225",
    "Left foot 270",
    "Left foot 315",
    "Right foot 0",
    "Right foot 45",
    "Right foot 90",
    "Right foot 135",
    "Right foot 180",
    "Right foot 225",
    "Right foot 270",
    "Right foot 315",
    "Attack 1",
    "Attack 2",
    "Attack 3",
    "Normal death 1",
    "Normal death 2",
    "Normal death 3",
    "Normal death 4",
    "Normal death 5",
    "Normal death 6",
    "Normal death 7",
    "Normal death 8",
    "Explosion death 1",
    "Explosion death 2",
    "Explosion death 3",
    "Explosion death 4",
    "Explosion death 5",
    "Explosion death 6",
    "Player seen sound 1",
    "Player seen sound 2",
    "Player seen sound 3",
    "Gunshot sound",
    "Normal death sound",
    "Explosion death sound"
  };

  static CHAR *szAlien[] =
  {
    "Standing 0",
    "Standing 45",
    "Standing 90",
    "Standing 135",
    "Standing 180",
    "Standing 225",
    "Standing 270",
    "Standing 315",
    "Left foot 0",
    "Left foot 45",
    "Left foot 90",
    "Left foot 135",
    "Left foot 180",
    "Left foot 225",
    "Left foot 270",
    "Left foot 315",
    "Right foot 0",
    "Right foot 45",
    "Right foot 90",
    "Right foot 135",
    "Right foot 180",
    "Right foot 225",
    "Right foot 270",
    "Right foot 315",
    "Physical attack 1",
    "Physical attack 2",
    "Physical attack 3",
    "Physical attack 4",
    "Physical attack 5",
    "Physical attack 6",
    "Physical attack 7",
    "Physical attack 8",
    "Death 1",
    "Death 2",
    "Death 3",
    "Death 4",
    "Death 5",
    "Death 6",
    "Death 7",
    "Death 8",
    "Mental attack 1",
    "Mental attack 2",
    "Physical attack sound",
    "Player struck sound",
    "Death sound",
    "Mental attack sound"
  };

  lpsz = "Unknown";

  if (nElement >= 0 && nElement < GetNumLifeFormElements(nLifeFormType))
  {
    if (nLifeFormType == 0)
    {
      lpsz = szInsect[nElement];
    }
    else if (nLifeFormType == 1)
    {
      lpsz = szRobot[nElement];
    }
    else if (nLifeFormType == 2)
    {
      lpsz = szAgent[nElement];
    }
    else if (nLifeFormType == 3)
    {
      lpsz = szAlien[nElement];
    }
  }
  
  return lpsz;

} // GetLifeFormElementString


/*******************************************************************

    NAME:       GetNumItemElements

    SYNOPSIS:   Get the total number of elements used by the
                indicated item type.

    ENTRY:      nItemType - The item type.

    RETURNS:    The number of elements used for this item.

********************************************************************/

INT GetNumItemElements(INT nItemType)
{
  INT nElements;

  static INT nElementCount[] =
  {
     3,  3,  3,  3,  3,
    12, 12, 12,  2,  2,
     2,  2, 18, 23,  2,
     2, 19, 19, 19,  9,
     8
  };

  if (nItemType < sizeof(nElementCount) / sizeof(nElementCount[0]))
  {
    nElements = nElementCount[nItemType];
  }
  else
  {
    nElements = 0;
  }
  
  return nElements;

} // GetNumItemElements


/*******************************************************************

    NAME:       GetNumItemBitmaps

    SYNOPSIS:   Get the total number of bitmaps used by the
                indicated item type.

    ENTRY:      nItemType - The item type.

    RETURNS:    The number of bitmaps used for this item.

********************************************************************/

INT GetNumItemBitmaps(INT nItemType)
{
  INT nBitmaps;

  static INT nBitmapCount[] =
  {
     2,  2,  2,  2,  2,
    10, 10, 10,  1,  1,
     1,  1, 16, 20,  1,
     1, 17, 17, 17,  8,
     7
  };

  if (nItemType < sizeof(nBitmapCount) / sizeof(nBitmapCount[0]))
  {
    nBitmaps = nBitmapCount[nItemType];
  }
  else
  {
    nBitmaps = 0;
  }
  
  return nBitmaps;

} // GetNumItemBitmaps


/*******************************************************************

    NAME:       GetNumItemSounds

    SYNOPSIS:   Get the total number of sounds used by the
                indicated item type.

    ENTRY:      nItemType - The item type.

    RETURNS:    The number of sounds used for this item.

********************************************************************/

INT GetNumItemSounds(INT nItemType)
{
  INT nSounds;

  static INT nSoundCount[] =
  {
    1, 1, 1, 1, 1,
    2, 2, 2, 1, 1,
    1, 1, 2, 3, 1,
    1, 2, 2, 2, 1,
    1
  };

  if (nItemType < sizeof(nSoundCount) / sizeof(nSoundCount[0]))
  {
    nSounds = nSoundCount[nItemType];
  }
  else
  {
    nSounds = 0;
  }
  
  return nSounds;

} // GetNumItemSounds


/*******************************************************************

    NAME:       GetItemTypeString

    SYNOPSIS:   Get the string for the indicated item type.

    ENTRY:      nItemType - The item type.

    RETURNS:    The string.

********************************************************************/

LPSTR GetItemTypeString(INT nItemType)
{
  LPSTR lpsz;

  static CHAR *szItemTypeStrings[] =
  {
    "Key Card 1",
    "Key Card 2",
    "Key Card 3",
    "Key Card 4",
    "ID Card",
    "Pistol",
    "Shotgun",
    "Machine Gun",
    "Box of Bullets",
    "Box of Shells",
    "Health Kit",
    "Armor",
    "Capsule",
    "Bazooka",
    "Box of Rockets",
    "Rocket",
    "Grenade",
    "Pack of grenades",
    "Box of grenades",
    "Barrel",
    "Power Cell"
  };

  lpsz = "Unknown";

  if (nItemType >= 0)
  {
    if (nItemType < sizeof(szItemTypeStrings) / sizeof(szItemTypeStrings[0]))
    {
      lpsz = szItemTypeStrings[nItemType];
    }
  }
  
  return lpsz;

} // GetItemTypeString


/*******************************************************************

    NAME:       GetItemElementString

    SYNOPSIS:   Get the string for the indicated item type and
                element.

    ENTRY:      nItemType - The item type.

                nElement - The element number.

    RETURNS:    The string.

********************************************************************/

LPSTR GetItemElementString(INT nItemType, INT nElement)
{
  LPSTR lpsz;

  static CHAR *szKeyCardStrings[] =
  {
    "Sprite",
    "Icon",
    "Grabbed"
  };

  static CHAR *szWeaponStrings[] =
  {
    "Sprite",
    "Idle",
    "Firing 1",
    "Firing 2",
    "Solid Impact 1",
    "Solid Impact 2",
    "Solid Impact 3",
    "Flesh Impact 1",
    "Flesh Impact 2",
    "Flesh Impact 3",
    "Firing",
    "Grabbed"
  };

  static CHAR *szBazookaStrings[] =
  {
    "Sprite",
    "Idle",
    "Firing 1",
    "Firing 2",
    "Smoke Trail 1",
    "Smoke Trail 2",
    "Smoke Trail 3",
    "Smoke Trail 4",
    "Smoke Trail 5",
    "Smoke Trail 6",
    "Smoke Trail 7",
    "Explosion 1",
    "Explosion 2",
    "Explosion 3",
    "Explosion 4",
    "Explosion 5",
    "Explosion 6",
    "Explosion 7",
    "Explosion 8",
    "Explosion 9",
    "Firing",
    "Explosion",
    "Grabbed"
  };

  static CHAR *szCapsuleStrings[] =
  {
    "Normal",
    "Bubbles 1",
    "Bubbles 2",
    "Bubbles 3",
    "Capsule Break 1",
    "Capsule Break 2",
    "Capsule Break 3",
    "Capsule Break 4",
    "Capsule Break 5",
    "Capsule Break 6",
    "Capsule Break 7",
    "Capsule Break 8",
    "Capsule Broken",
    "Head Shot 1",
    "Head Shot 2",
    "Dead Alien",
    "Ricochet",
    "Capsule Breaks"
  };

  static CHAR *szGrenadeStrings[] =
  {
    "Sprite",
    "Idle",
    "Spinning 1",
    "Spinning 2",
    "Spinning 3",
    "Spinning 4",
    "Spinning 5",
    "Spinning 6",
    "Explosion 1",
    "Explosion 2",
    "Explosion 3",
    "Explosion 4",
    "Explosion 5",
    "Explosion 6",
    "Explosion 7",
    "Explosion 8",
    "Explosion 9",
    "Explosion",
    "Grabbed"
  };

  static CHAR *szBarrelStrings[] =
  {
    "Sprite",
    "Explosion 1",
    "Explosion 2",
    "Explosion 3",
    "Explosion 4",
    "Explosion 5",
    "Explosion 6",
    "Explosion 7",
    "Explosion"
  };

  static CHAR *szPowerCellStrings[] =
  {
    "Sprite",
    "Explosion 1",
    "Explosion 2",
    "Explosion 3",
    "Explosion 4",
    "Explosion 5",
    "Exploded Cell",
    "Explosion"
  };

  static CHAR *szOtherStrings[] =
  {
    "Sprite",
    "Grabbed"
  };

  lpsz = "Unknown";

  if (nElement >= 0)
  {
    if (nItemType >= 0 && nItemType <= 4)
    {
      if (nElement < sizeof(szKeyCardStrings) / sizeof(szKeyCardStrings[0]))
      {
        lpsz = szKeyCardStrings[nElement];
      }
    }
    else if (nItemType >= 5 && nItemType <= 7)
    {
      if (nElement < sizeof(szWeaponStrings) / sizeof(szWeaponStrings[0]))
      {
        lpsz = szWeaponStrings[nElement];
      }
    }
    else if (nItemType == 12)
    {
      if (nElement < sizeof(szCapsuleStrings) / sizeof(szCapsuleStrings[0]))
      {
        lpsz = szCapsuleStrings[nElement];
      }
    }
    else if (nItemType == 13)
    {
      if (nElement < sizeof(szBazookaStrings) / sizeof(szBazookaStrings[0]))
      {
        lpsz = szBazookaStrings[nElement];
      }
    }
    else if (nItemType >= 16 && nItemType <= 18)
    {
      if (nElement < sizeof(szGrenadeStrings) / sizeof(szGrenadeStrings[0]))
      {
        lpsz = szGrenadeStrings[nElement];
      }
    }
    else if (nItemType == 19)
    {
      if (nElement < sizeof(szBarrelStrings) / sizeof(szBarrelStrings[0]))
      {
        lpsz = szBarrelStrings[nElement];
      }
    }
    else if (nItemType == 20)
    {
      if (nElement < sizeof(szPowerCellStrings) / sizeof(szPowerCellStrings[0]))
      {
        lpsz = szPowerCellStrings[nElement];
      }
    }
    else
    {
      if (nElement < sizeof(szOtherStrings) / sizeof(szOtherStrings[0]))
      {
        lpsz = szOtherStrings[nElement];
      }
    }
  }
  
  return lpsz;

} // GetItemElementString


/*******************************************************************

    NAME:       PointInPolygon

    SYNOPSIS:   Check if the point is in the polygon. It is assumed
                that the point given is in the plane of the polygon.

    ENTRY:      p - Pointer to the polygon.

                v - Pointer to the vertex array.

                x - The x coordinate of the point.

                y - The y coordinate of the point.

                z - The z coordinate of the point.

    EXIT:       Return TRUE if the point is in the polygon.

********************************************************************/

BOOL PointInPolygon(POLYGON *p, VERTEX huge *v, double x, double y, double z)
{
  INT i, j, k, n;
  double xv, yv, zv, xc, yc, zc, fc;
  double a1, a2, a3, b1, b2, b3, a_dot_a, a_dot_b;
  double xp[10], yp[10], zp[10], a[10], b[10], c[10], d[10], f[10];

  //
  //  Collect all the vertices.
  //

  n = (p->flags & 7) + 3;

  for(i = 0; i < n; ++i)
  {
    xp[i] = (double)v[p->vertices[i]].x;
    yp[i] = (double)v[p->vertices[i]].y;
    zp[i] = (double)v[p->vertices[i]].z;
  }

  //
  //  Compute the center of the polygon.
  //

  xc = 0.0;
  yc = 0.0;
  zc = 0.0;

  for(i = 0; i < n; ++i)
  {
    xc += xp[i];
    yc += yp[i];
    zc += zp[i];
  }

  xc /= (double)i;
  yc /= (double)i;
  zc /= (double)i;

  //
  //  Compute the coefficients representing the bounding planes.
  //

  for(i = 0; i < n; ++i)
  {
    if (i == 0)
    {
      j = n - 1;
    }
    else
    {
      j = i - 1;
    }

    if (i == n - 1)
    {
      k = 0;
    }
    else
    {
      k = i + 1;
    }

    xv = xp[i];
    yv = yp[i];
    zv = zp[i];

    a1 = xp[k] - xv;
    a2 = yp[k] - yv;
    a3 = zp[k] - zv;

    b1 = xp[j] - xv;
    b2 = yp[j] - yv;
    b3 = zp[j] - zv;

    a_dot_a = a1 * a1 + a2 * a2 + a3 * a3;
    a_dot_b = a1 * b1 + a2 * b2 + a3 * b3;

    a[i] = a_dot_b * a1 - a_dot_a * b1;
    b[i] = a_dot_b * a2 - a_dot_a * b2;
    c[i] = a_dot_b * a3 - a_dot_a * b3;
    d[i] = -(a[i] * xv + b[i] * yv + c[i] * zv);
    f[i] = a[i] * xc + b[i] * yc + c[i] * zc + d[i];

    /* test... fc should be very close to zero in both cases
    fc = a[i] * x[i] + b[i] * y[i] + c[i] * z[i] + d[i];
    fc = a[i] * x[k] + b[i] * y[k] + c[i] * z[k] + d[i];
    */
  }

  for(i = 0; i < n; ++i)
  {
    fc = a[i] * x + b[i] * y + c[i] * z + d[i];

    if (fc * f[i] < 0.0)
    {
      return FALSE;
    }
  }

  return TRUE;

} // PointInPolygon


/*******************************************************************

    NAME:       IsPolygonFlat

    SYNOPSIS:   Check if the polygon is flat.

    ENTRY:      p - Pointer to the polygon.

                v - Pointer to the vertex array.

    EXIT:       Return TRUE if the polygon is flat.

********************************************************************/

BOOL IsPolygonFlat(POLYGON *p, VERTEX huge *v)
{
  INT i, n;
  double xp[10], yp[10], zp[10], a, b, c, d, f;

  //
  //  Collect all the vertices.
  //

  n = (p->flags & 7) + 3;
  
  if (n == 3)
  {
    return TRUE;
  }

  for(i = 0; i < n; ++i)
  {
    xp[i] = (double)v[p->vertices[i]].x;
    yp[i] = (double)v[p->vertices[i]].y;
    zp[i] = (double)v[p->vertices[i]].z;
  }

  //
  //  Use the first 3 vertices to generate the polygon plane equation coefficients.
  //

  a =  yp[0] * (zp[2] - zp[1]) - zp[0] * (yp[2] - yp[1]) + yp[2] * zp[1] - yp[1] * zp[2];
  b = -xp[0] * (zp[2] - zp[1]) + zp[0] * (xp[2] - xp[1]) - xp[2] * zp[1] + xp[1] * zp[2];
  c =  xp[0] * (yp[2] - yp[1]) - yp[0] * (xp[2] - xp[1]) + xp[2] * yp[1] - xp[1] * yp[2];
  d = sqrt(a * a + b * b + c * c);

  a = a / d;
  b = b / d;
  c = c / d;
  d = -(a * xp[0] + b * yp[0] + c * zp[0]);

  //
  //  Check the distance of the remaining vertices from the calculated plane.
  //

  for(i = 3; i < n; ++i)
  {
    f = a * xp[i] + b * yp[i] + c * zp[i] + d;
    
    if (fabs(f) > 1.5)
    {
      return FALSE;
    }
  }

  return TRUE;

} // IsPolygonFlat


/*******************************************************************

    NAME:       IsPolygonConvex

    SYNOPSIS:   Check if the polygon is convex.

    ENTRY:      p - Pointer to the polygon.

                v - Pointer to the vertex array.

    EXIT:       Return TRUE if the polygon is convex.

********************************************************************/

BOOL IsPolygonConvex(POLYGON *p, VERTEX huge *v)
{
  INT i, j, k, n;
  double xv, yv, zv, a1, a2, a3, b1, b2, b3, a_dot_a, a_dot_b;
  double xp[10], yp[10], zp[10], a, b, c, d, f;

  //
  //  Collect all the vertices.
  //

  n = (p->flags & 7) + 3;

  if (n == 3)
  {
    return TRUE;
  }

  for(i = 0; i < n; ++i)
  {
    xp[i] = (double)v[p->vertices[i]].x;
    yp[i] = (double)v[p->vertices[i]].y;
    zp[i] = (double)v[p->vertices[i]].z;
  }

  //
  //  Compute the coefficients representing the bounding planes.
  //

  for(i = 0; i < n; ++i)
  {
    if (i == 0)
    {
      j = n - 1;
    }
    else
    {
      j = i - 1;
    }

    if (i == n - 1)
    {
      k = 0;
    }
    else
    {
      k = i + 1;
    }

    xv = xp[i];
    yv = yp[i];
    zv = zp[i];

    a1 = xp[k] - xv;
    a2 = yp[k] - yv;
    a3 = zp[k] - zv;

    b1 = xp[j] - xv;
    b2 = yp[j] - yv;
    b3 = zp[j] - zv;

    a_dot_a = a1 * a1 + a2 * a2 + a3 * a3;
    a_dot_b = a1 * b1 + a2 * b2 + a3 * b3;

    a = a_dot_b * a1 - a_dot_a * b1;
    b = a_dot_b * a2 - a_dot_a * b2;
    c = a_dot_b * a3 - a_dot_a * b3;
    d = -(a * xv + b * yv + c * zv);

    /* test... f should be very close to zero in both cases
    f = a * xp[i] + b * yp[i] + c * zp[i] + d;
    f = a * xp[k] + b * yp[k] + c * zp[k] + d;
    */

    for(j = 2; j < n; ++j)
    {
      k = (j + i) % n;

      f = a * xp[k] + b * yp[k] + c * zp[k] + d;

      if (f > 0.0)
      {
        return FALSE;
      }
    }
  }

  return TRUE;

} // IsPolygonConvex
