/*******************************************************************

    objdata.c

    Routines for managing the level data.

********************************************************************/


#include "vedit.h"


//
//  Public functions.
//

/*******************************************************************

    NAME:       CountSelectedVertices

    SYNOPSIS:   Count all selected vertices.

********************************************************************/

INT CountSelectedVertices(LEVEL_DATA *pLevel)
{
  INT i, nCount;

  nCount = 0;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (pLevel->pVertexData[i].flags & VF_SELECTED)
    {
      ++nCount;
    }
  }

  return nCount;

} // CountSelectedVertices


/*******************************************************************

    NAME:       CountSelectedPolygons

    SYNOPSIS:   Count all selected polygons.

********************************************************************/

INT CountSelectedPolygons(LEVEL_DATA *pLevel)
{
  INT i, nCount;

  nCount = 0;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (pLevel->pPolygonData[i].flags & PF_SELECTED)
    {
      ++nCount;
    }
  }

  return nCount;

} // CountSelectedPolygons


/*******************************************************************

    NAME:       CountAllSelections

    SYNOPSIS:   Count all selected objects.

********************************************************************/

INT CountAllSelections(LEVEL_DATA *pLevel)
{
  INT i, nCount;

  nCount = 0;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (pLevel->pVertexData[i].flags & VF_SELECTED)
    {
      ++nCount;
    }
  }

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (pLevel->pPolygonData[i].flags & PF_SELECTED)
    {
      ++nCount;
    }
  }

  return nCount;

} // CountAllSelections


/*******************************************************************

    NAME:       CountAllHidden

    SYNOPSIS:   Count all hidden objects.

********************************************************************/

INT CountAllHidden(LEVEL_DATA *pLevel)
{
  INT i, nCount;

  nCount = 0;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (pLevel->pVertexData[i].flags & VF_HIDDEN)
    {
      ++nCount;
    }
  }

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (pLevel->pPolygonData[i].flags & PF_HIDDEN)
    {
      ++nCount;
    }
  }

  return nCount;

} // CountAllHidden


/*******************************************************************

    NAME:       ClearSelections

    SYNOPSIS:   Clear all selection flags.

********************************************************************/

VOID ClearSelections(LEVEL_DATA *pLevel)
{
  INT i;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  //
  //  Deselect all polygons.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    (pLevel->pPolygonData + i)->flags &= ~PF_SELECTED;
  }

  //
  //  Deselect all vertices.
  //

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    (pLevel->pVertexData + i)->flags &= ~VF_SELECTED;
  }

  SetCursor(hcurSave);

  return;

} // ClearSelections


/*******************************************************************

    NAME:       HideSelections

    SYNOPSIS:   Hide the selected objects.

********************************************************************/

VOID HideSelections(LEVEL_DATA *pLevel)
{
  INT i;
  WORD *p;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  //
  //  Hide selected polygons.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    p = (WORD *)(pLevel->pPolygonData + i);

    if (*p & PF_SELECTED)
    {
      *p |= PF_HIDDEN;
      *p &= ~PF_SELECTED;
    }
  }

  //
  //  Hide selected vertices.
  //

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    p = (WORD *)(pLevel->pVertexData + i);

    if (*p & VF_SELECTED)
    {
      *p |= VF_HIDDEN;
      *p &= ~VF_SELECTED;
    }
  }

  SetCursor(hcurSave);

  return;

} // HideSelections


/*******************************************************************

    NAME:       HideGroup

    SYNOPSIS:   Hide all polygons and vertices with the group index
                given.

    ENTRY:      pLevel - The level data pointer.

                nGroup - The group index to hide.

********************************************************************/

VOID HideGroup(LEVEL_DATA *pLevel, INT nGroup)
{
  INT i;
  POLYGON huge *pPolygon;
  VERTEX huge *pVertex;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    pPolygon = pLevel->pPolygonData + i;

    if (pPolygon->group_index == nGroup)
    {
      pPolygon->flags |= PF_HIDDEN;
      pPolygon->flags &= ~PF_SELECTED;
    }
  }

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    pVertex = pLevel->pVertexData + i;

    if (pVertex->group_index == nGroup)
    {
      pVertex->flags |= VF_HIDDEN;
      pVertex->flags &= ~VF_SELECTED;
    }
  }

  return;

} // HideGroup


/*******************************************************************

    NAME:       HideAll

    SYNOPSIS:   Hide all the objects.

********************************************************************/

VOID HideAll(LEVEL_DATA *pLevel)
{
  INT i;
  WORD *p;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  //
  //  Hide polygons.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    p = (WORD *)(pLevel->pPolygonData + i);

    *p |= PF_HIDDEN;
    *p &= ~PF_SELECTED;
  }

  //
  //  Hide vertices.
  //

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    p = (WORD *)(pLevel->pVertexData + i);

    *p |= VF_HIDDEN;
    *p &= ~VF_SELECTED;
  }

  SetCursor(hcurSave);

  return;

} // HideAll


/*******************************************************************

    NAME:       ShowGroup

    SYNOPSIS:   Show (i.e. un-hide) all polygons and vertices with
                the group index given and any vertices associated
                with those polygons.

    ENTRY:      pLevel - The level data pointer.

                nGroup - The group index to hide.

********************************************************************/

VOID ShowGroup(LEVEL_DATA *pLevel, INT nGroup)
{
  INT i;
  POLYGON huge *pPolygon;
  VERTEX huge *pVertex;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    pPolygon = pLevel->pPolygonData + i;

    if (pPolygon->group_index == nGroup)
    {
      pPolygon->flags &= ~PF_HIDDEN;
    }
  }

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    pVertex = pLevel->pVertexData + i;

    if (pVertex->group_index == nGroup)
    {
      pVertex->flags &= ~VF_HIDDEN;
    }
  }

  return;

} // ShowGroup


/*******************************************************************

    NAME:       ShowAll

    SYNOPSIS:   Show all the objects (i.e. unhide everything).

********************************************************************/

VOID ShowAll(LEVEL_DATA *pLevel)
{
  INT i;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  //
  //  Unhide all polygons.
  //

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    (pLevel->pPolygonData + i)->flags &= ~PF_HIDDEN;
  }

  //
  //  Unhide all vertices.
  //

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    (pLevel->pVertexData + i)->flags &= ~VF_HIDDEN;
  }

  SetCursor(hcurSave);

  return;

} // ShowAll


/*******************************************************************

    NAME:       FindVertex

    SYNOPSIS:   Find the indicated vertex starting at the index
                given.

    ENTRY:      pLevel - The level data pointer.

                nStart - Start search at this index.

                pVertex - Pointer to the vertex to find.

    EXIT:       The index of the vertex or -1 if not found.

********************************************************************/

INT FindVertex(LEVEL_DATA *pLevel, INT nStart, VERTEX *pVertex)
{
  INT i;
  LONG lx1, ly1, lz1, lx2, ly2, lz2;
  VERTEX huge *p;

  lx1 = (long)Round(pVertex->x);
  ly1 = (long)Round(pVertex->y);
  lz1 = (long)Round(pVertex->z);

  p = pLevel->pVertexData + nStart;

  for(i = nStart; i < pLevel->nVertices; ++i)
  {
    lx2 = (long)Round(p->x);
    ly2 = (long)Round(p->y);
    lz2 = (long)Round(p->z);

    if (lx1 == lx2 && ly1 == ly2 && lz1 == lz2)
    {
      return i;
    }

    ++p;
  }

  return -1;

} // FindVertex


/*******************************************************************

    NAME:       FindPolygon

    SYNOPSIS:   Find the indicated polygon starting at the index
                given.

    ENTRY:      pLevel - The level data pointer.

                nStart - Start search at this index.

                pPolygon - Pointer to the polygon to find.

    EXIT:       The index of the polygon or -1 if not found.

********************************************************************/

INT FindPolygon(LEVEL_DATA *pLevel, INT nStart, POLYGON *pPolygon)
{
  INT i, j, k, n;
  POLYGON huge *p;

  n = (pPolygon->flags & 7) + 3;
  p = pLevel->pPolygonData + nStart;

  for(i = nStart; i < pLevel->nPolygons; ++i)
  {
    if ((INT)((p->flags & 7) + 3) == n)
    {
      for(j = 0; j < n; ++j)
      {
        for(k = 0; k < n; ++k)
        {
          if (p->vertices[j] == pPolygon->vertices[k])
          {
            break;
          }
        }

        if (k == n)
        {
          break;
        }
      }

      if (j == n)
      {
        return i;
      }
    }

    ++p;
  }

  return -1;

} // FindPolygon


/*******************************************************************

    NAME:       ReplaceVertex

    SYNOPSIS:   Replace all references to the indicated vertex with
                the new vertex given. This would typically be done
                before deleting a duplicate vertex.

    ENTRY:      pLevel - The level data pointer.

                nOld - Old vertex index

                nNew - New vertex index

    EXIT:       Number of polygons deleted due to merged vertices.

********************************************************************/

INT ReplaceVertex(LEVEL_DATA *pLevel, INT nOld, INT nNew)
{
  INT i, j, k, n, num_deleted_polygons;
  BOOL bCheckPolygon;
  POLYGON huge *p;

  if (nOld == nNew)
  {
    return 0;
  }

  p = pLevel->pPolygonData;

  i = 0;
  num_deleted_polygons = 0;
  
  while(i < pLevel->nPolygons)
  {
    bCheckPolygon = FALSE;

    n = (p->flags & 7) + 3;

    for(j = 0; j < n; ++j)
    {
      if (p->vertices[j] == nOld)
      {
        bCheckPolygon = TRUE;
        p->vertices[j] = nNew;
      }
    }

    //
    //  If any polygon vertices have been merged, delete the polygon.
    //

    if (bCheckPolygon)
    {
      j = 0;

      while(j < n - 1)
      {
        for(k = j + 1; k < n; ++k)
        {
          if (p->vertices[j] == p->vertices[k])
          {
            break;
          }
        }

        if (k < n)
        {
          DeletePolygon(pLevel, i);
          ++num_deleted_polygons;
          break;
        }

        ++j;
      }

      if (j < n - 1)
      {
        continue;
      }
    }

    ++i;
    ++p;
  }

  return num_deleted_polygons;

} // ReplaceVertex


/*******************************************************************

    NAME:       ReplacePolygon

    SYNOPSIS:   Replace all references to the indicated polygon with
                the new polygon given. This would typically be done
                before deleting a duplicate polygon.

    ENTRY:      pLevel - The level data pointer.

                nOld - Old polygon index

                nNew - New polygon index

********************************************************************/

VOID ReplacePolygon(LEVEL_DATA *pLevel, INT nOld, INT nNew)
{
  INT i;
  POLYGON huge *p;

  p = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (p->flags & PF_LINKED)
    {
      if (p->polygon_link == nOld)
      {
        p->polygon_link = nNew;
      }
    }

    ++p;
  }

  return;

} // ReplacePolygon


/*******************************************************************

    NAME:       RotatePolygon

    SYNOPSIS:   Rotate the polygon vertices.

    ENTRY:      pPolygon - The polygon pointer.

                nRotation - Rotation count (positive is clockwise)

********************************************************************/

VOID RotatePolygon(POLYGON huge *pPolygon, INT nRotation)
{
  INT i, n, new_vertices[MAX_VERTICES];

  n = (pPolygon->flags & 7) + 3;

  while(nRotation < 0)
  {
    nRotation += n;
  }

  for(i = 0; i < n; ++i)
  {
    new_vertices[i] = pPolygon->vertices[(i + nRotation) % n];
  }

  for(i = 0; i < n; ++i)
  {
    pPolygon->vertices[i] = new_vertices[i];
  }

  return;

} // RotatePolygon


/*******************************************************************

    NAME:       FlipPolygon

    SYNOPSIS:   Flip the visible side of the polygon.

    ENTRY:      pPolygon - The polygon pointer.

********************************************************************/

VOID FlipPolygon(POLYGON huge *pPolygon)
{
  INT i, n, new_vertices[MAX_VERTICES];

  n = (pPolygon->flags & 7) + 3;

  for(i = 0; i < n; ++i)
  {
    new_vertices[i] = pPolygon->vertices[n - 1 - i];
  }

  for(i = 0; i < n; ++i)
  {
    pPolygon->vertices[i] = new_vertices[i];
  }

  RotatePolygon(pPolygon, -2);

  return;

} // FlipPolygon


/*******************************************************************

    NAME:       ComputeBitmapCoordinates

    SYNOPSIS:   Given two reference points compute the bitmap
                coordinates of the given unknown. This routine
                should be used for points within the reference
                polygon.

    ENTRY:      p1 - First reference point.

                p2 - Second reference point.

                p - Point with unknown bitmap coordinates.

********************************************************************/

VOID ComputeBitmapCoordinates(TEXEL *p1, TEXEL *p2, TEXEL *p)
{
  double bdx, bdy, r, s, t, d, m, u, v, dx, dy, dz;

  dx = p2->x - p1->x;
  dy = p2->y - p1->y;
  dz = p2->z - p1->z;

  bdx = p2->bmx - p1->bmx;
  bdy = p2->bmy - p1->bmy;

  s = dx * dx + dy * dy + dz * dz;
  t = bdx * bdx + bdy * bdy;              /* d squared */
  m = t / s;
  d = sqrt(t);

  dx = p->x - p1->x;
  dy = p->y - p1->y;
  dz = p->z - p1->z;

  r = (dx * dx + dy * dy + dz * dz) * m;  /* a squared */

  dx = p->x - p2->x;
  dy = p->y - p2->y;
  dz = p->z - p2->z;

  s = (dx * dx + dy * dy + dz * dz) * m;  /* b squared */

  u = (r - s + t) / (2 * d);
  v = sqrt(r - u * u);                    /* "+" for pts inside polygon */

  p->bmx = p1->bmx + (u * bdx - v * bdy) / d;
  p->bmy = p1->bmy + (u * bdy + v * bdx) / d;

  return;

} // ComputeBitmapCoordinates


/*******************************************************************

    NAME:       ComputeBitmapCoordinates2

    SYNOPSIS:   Given two reference points compute the bitmap
                coordinates of the given unknown. This routine
                should be used for points outside the reference
                polygon.

    ENTRY:      p1 - First reference point.

                p2 - Second reference point.

                p - Point with unknown bitmap coordinates.

********************************************************************/

VOID ComputeBitmapCoordinates2(TEXEL *p1, TEXEL *p2, TEXEL *p)
{
  double bdx, bdy, r, s, t, d, m, u, v, dx, dy, dz;

  dx = p2->x - p1->x;
  dy = p2->y - p1->y;
  dz = p2->z - p1->z;

  bdx = p2->bmx - p1->bmx;
  bdy = p2->bmy - p1->bmy;

  s = dx * dx + dy * dy + dz * dz;
  t = bdx * bdx + bdy * bdy;              /* d squared */
  m = t / s;
  d = sqrt(t);

  dx = p->x - p1->x;
  dy = p->y - p1->y;
  dz = p->z - p1->z;

  r = (dx * dx + dy * dy + dz * dz) * m;  /* a squared */

  dx = p->x - p2->x;
  dy = p->y - p2->y;
  dz = p->z - p2->z;

  s = (dx * dx + dy * dy + dz * dz) * m;  /* b squared */

  u = (r - s + t) / (2 * d);
  v = -sqrt(r - u * u);                   /* "-" for pts outside polygon */

  p->bmx = p1->bmx + (u * bdx - v * bdy) / d;
  p->bmy = p1->bmy + (u * bdy + v * bdx) / d;

  return;

} // ComputeBitmapCoordinates2


/*******************************************************************

    NAME:       DeleteVertex

    SYNOPSIS:   Delete the specifed vertex.

    ENTRY:      pLevel - The level data pointer.

                nIndex - Index of the vertex to delete.

********************************************************************/

VOID DeleteVertex(LEVEL_DATA *pLevel, INT nIndex)
{
  INT i, j, n;
  POLYGON huge *p;

  if (nIndex < pLevel->nVertices - 1)
  {
    for(i = nIndex; i < pLevel->nVertices - 1; ++i)
    {
      _fmemcpy( pLevel->pVertexData + i,
                pLevel->pVertexData + i + 1,
                sizeof(VERTEX) );
    }

    //
    //  Adjust the polygon vertex offsets.
    //

    p = pLevel->pPolygonData;

    for(i = 0; i < pLevel->nPolygons; ++i)
    {
      n = (p->flags & 7) + 3;

      for(j = 0; j < n; ++j)
      {
        if (p->vertices[j] == nIndex)
        {
          MsgBox( hwndMDIClient,
                  MB_ICONEXCLAMATION,
                  "Vertex deleted while still in use! (Yes, this means something is really f'd up!)" );
        }
        else if (p->vertices[j] > nIndex)
        {
          --p->vertices[j];
        }
      }

      ++p;
    }
  }

  //
  //  Zero the empty vertex.
  //

  --pLevel->nVertices;

  _fmemset(pLevel->pVertexData + pLevel->nVertices, 0, sizeof(VERTEX));

  pLevel->bRebuildZone = TRUE;

  return;

} // DeleteVertex


/*******************************************************************

    NAME:       DeletePolygon

    SYNOPSIS:   Delete the specifed polygon.

    ENTRY:      pLevel - The level data pointer.

                nIndex - Index of the polygon to delete.

********************************************************************/

VOID DeletePolygon(LEVEL_DATA *pLevel, INT nIndex)
{
  INT i;
  POLYGON huge *p;

  if (nIndex < pLevel->nPolygons - 1)
  {
    for(i = nIndex; i < pLevel->nPolygons - 1; ++i)
    {
      _fmemcpy( pLevel->pPolygonData + i,
                pLevel->pPolygonData + i + 1,
                sizeof(POLYGON) );
    }

    //
    //  Adjust the polygon links.
    //

    p = pLevel->pPolygonData;

    for( i = 0; i < pLevel->nPolygons; ++i )
    {
      if (p->flags & PF_LINKED)
      {
        if (p->polygon_link == nIndex)
        {
          MsgBox( hwndMDIClient,
                  MB_ICONEXCLAMATION,
                  "Polygon deleted while still in use!" );
        }
        else if (p->polygon_link > nIndex)
        {
          --p->polygon_link;
        }
      }

      ++p;
    }
  }

  //
  //  Zero the empty vertex.
  //

  --pLevel->nPolygons;

  _fmemset(pLevel->pPolygonData + pLevel->nPolygons, 0, sizeof(POLYGON));

  pLevel->bRebuildZone = TRUE;

  return;

} // DeletePolygon


/*******************************************************************

    NAME:       DeleteGroup

    SYNOPSIS:   Delete the group at the indicated index.

********************************************************************/

VOID DeleteGroup(LEVEL_DATA *pLevel, INT index)
{
  INT i;
  BYTE *p;
  WORD wSize;
  POLYGON huge *pPolygon;
  VERTEX huge *pVertex;

  if (index < 0 || index > pLevel->nGroups - 1)
  {
    return;
  }

  //
  //  Update the group index of affected polygons.
  //

  pPolygon = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (pPolygon->group_index == index)
    {
      pPolygon->group_index = -1;
    }
    else if (pPolygon->group_index > index)
    {
      --pPolygon->group_index;
    }

    ++pPolygon;
  }

  //
  //  Update the group index of affected vertices.
  //

  pVertex = pLevel->pVertexData;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (pVertex->group_index == index)
    {
      pVertex->group_index = -1;
    }
    else if (pVertex->group_index > index)
    {
      --pVertex->group_index;
    }

    ++pVertex;
  }

  //
  //  Now delete the group.
  //

  p = (BYTE *)(&pLevel->pGroupData[index]);

  wSize = (pLevel->nGroups - index - 1) * sizeof(GROUP);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(GROUP), wSize);
  }

  --pLevel->nGroups;

  return;

} // DeleteGroup


/*******************************************************************

    NAME:       DeleteTrigger

    SYNOPSIS:   Delete the trigger at the indicated index.

********************************************************************/

VOID DeleteTrigger(LEVEL_DATA *pLevel, INT index)
{
  INT i;
  BYTE *p;
  WORD wSize;
  VERTEX huge *pVertex;

  if (index < 0 || index > pLevel->nTriggers - 1)
  {
    return;
  }

  //
  //  Update the trigger index of affected motions.
  //

  for(i = 0; i < pLevel->nMotions; ++i)
  {
    if (pLevel->pMotionData[i].trigger_index == index)
    {
      pLevel->pMotionData[i].trigger_index = -1;
    }
    else if (pLevel->pMotionData[i].trigger_index > index)
    {
      --pLevel->pMotionData[i].trigger_index;
    }
  }

  //
  //  Update the trigger index of affected vertices.
  //

  pVertex = pLevel->pVertexData;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (pVertex->trigger_index == index)
    {
      pVertex->trigger_index = -1;
    }
    else if (pVertex->trigger_index > index)
    {
      --pVertex->trigger_index;
    }

    ++pVertex;
  }

  //
  //  Now delete the trigger.
  //

  p = (BYTE *)(&pLevel->pTriggerData[index]);

  wSize = (WORD)(pLevel->nTriggers - index - 1) * sizeof(TRIGGER);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(TRIGGER), wSize);
  }

  --pLevel->nTriggers;

  pLevel->bRebuildZone = TRUE;

  return;

} // DeleteTrigger


/*******************************************************************

    NAME:       DeleteMotion

    SYNOPSIS:   Delete the motion at the indicated index.

********************************************************************/

VOID DeleteMotion(LEVEL_DATA *pLevel, INT index)
{
  INT i;
  BYTE *p;
  WORD wSize;
  POLYGON huge *pPolygon;

  if (index < 0 || index > pLevel->nMotions - 1)
  {
    return;
  }

  //
  //  Update the motion index of affected polygons.
  //

  pPolygon = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (pPolygon->motion_index == index)
    {
      pPolygon->motion_index = -1;
    }
    else if (pPolygon->motion_index > index)
    {
      --pPolygon->motion_index;
    }

    ++pPolygon;
  }

  //
  //  Now delete the motion.
  //

  p = (BYTE *)(&pLevel->pMotionData[index]);

  wSize = (pLevel->nMotions - index - 1) * sizeof(MOTION);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(MOTION), wSize);
  }

  --pLevel->nMotions;

  pLevel->bRebuildZone = TRUE;

  return;

} // DeleteMotion


/*******************************************************************

    NAME:       DeleteMark

    SYNOPSIS:   Delete the mark at the indicated index.

********************************************************************/

VOID DeleteMark(LEVEL_DATA *pLevel, INT index)
{
  BYTE *p;
  WORD wSize;

  if (index < 0 || index > pLevel->nMarks - 1)
  {
    return;
  }

  p = (BYTE *)(&pLevel->pMarkData[index]);

  wSize = (pLevel->nMarks - index - 1) * sizeof(MARK);

  if (wSize > 0)
  {
    _fmemmove(p, p + sizeof(MARK), wSize);
  }

  --pLevel->nMarks;

  return;

} // DeleteMark


/*******************************************************************

    NAME:       DeleteSelections

    SYNOPSIS:   Delete the selected objects.

********************************************************************/

VOID DeleteSelections(LEVEL_DATA *pLevel)
{
  INT i, j, n;
  POLYGON huge *p;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  //
  //  If any polygon uses a selected vertex then select it also.
  //

  p = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if ((p->flags & PF_SELECTED) == 0)
    {
      n = (p->flags & 7) + 3;

      for(j = 0; j < n; ++j)
      {
        if ((pLevel->pVertexData + p->vertices[j])->flags & VF_SELECTED)
        {
          p->flags |= PF_SELECTED;
          break;
        }
      }
    }

    ++p;
  }

  //
  //  Unlink any polygon that is linked to a selected polygon.
  //

  p = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (p->flags & PF_LINKED)
    {
      if ((pLevel->pPolygonData + p->polygon_link)->flags & PF_SELECTED)
      {
        p->flags &= ~PF_LINKED;
      }
    }

    ++p;
  }

  //
  //  Delete selected polygons.
  //

  for(i = pLevel->nPolygons - 1; i >= 0; --i)
  {
    if ((pLevel->pPolygonData + i)->flags & PF_SELECTED)
    {
      DeletePolygon(pLevel, i);
    }
  }

  //
  //  Delete the selected vertices.
  //

  for(i = pLevel->nVertices - 1; i >= 0; --i)
  {
    if ((pLevel->pVertexData + i)->flags & VF_SELECTED)
    {
      DeleteVertex(pLevel, i);
    }
  }

  SetCursor(hcurSave);

  return;

} // DeleteSelections


/*******************************************************************

    NAME:       NewVertex

    SYNOPSIS:   Add a new vertex to the level data.

    ENTRY:      pLevel - The level data pointer.

                x - Vertex x position.

                y - Vertex y position.

                z - Vertex z position.

    EXIT:       Returns TRUE on success or FALSE if out of space.

********************************************************************/

BOOL NewVertex(LEVEL_DATA *pLevel, LONG x, LONG y, LONG z)
{
  INT nIndex;
  VERTEX vertex;
  VERTEX huge *p;

  if (pLevel->nVertices == MAX_NUM_VERTICES)
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Out of vertex data space" );

    return FALSE;
  }

  _fmemset(&vertex, 0, sizeof(VERTEX));

  vertex.flags = 0;
  vertex.x = (float)x;
  vertex.y = (float)y;
  vertex.z = (float)z;
  vertex.group_index = -1;
  vertex.sprite_index = -1;
  vertex.noise_index = -1;
  vertex.lifeform_index = -1;
  vertex.item_index = -1;
  vertex.trigger_index = -1;

  //
  //  If we already have that vertex then just make sure it isn't hidden.
  //

  nIndex = FindVertex(pLevel, 0, &vertex);

  if (nIndex != -1)
  {
    p = pLevel->pVertexData + nIndex;
    p->flags &= ~VF_HIDDEN;
    return TRUE;
  }

  //
  //  Record the new vertex.
  //

  _fmemcpy(pLevel->pVertexData + pLevel->nVertices, &vertex, sizeof(VERTEX));

  ++pLevel->nVertices;

  pLevel->bRebuildZone = TRUE;

  return TRUE;

} // NewVertex


/*******************************************************************

    NAME:       NewPolygon

    SYNOPSIS:   Add a new polygon to the level data.

    ENTRY:      pLevel - The level data pointer.

                nVertices - Number of vertices in the polygon.

                pVertices - Vertex indecies.

    EXIT:       Returns TRUE on success or FALSE if out of space.

********************************************************************/

BOOL NewPolygon(LEVEL_DATA *pLevel, INT nVertices, INT *pVertices)
{
  INT i;
  VERTEX huge *p;
  POLYGON polygon;
  double x1, y1, z1, x2, y2, z2, dx, dy, dz, d;

  if (pLevel->nPolygons == MAX_NUM_POLYGONS)
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Out of polygon data space" );

    return FALSE;
  }

  _fmemset(&polygon, 0, sizeof(polygon));

  polygon.flags = nVertices - 3;
  polygon.group_index = -1;
  polygon.texture_index = -1;
  polygon.animation_index = -1;
  polygon.motion_index = -1;
  polygon.damage_animation_index = -1;

  for(i = 0; i < nVertices; ++i)
  {
    polygon.vertices[i] = pVertices[i];
  }

  p = pLevel->pVertexData + polygon.vertices[0];

  x1 = (double)p->x;
  y1 = (double)p->y;
  z1 = (double)p->z;

  p = pLevel->pVertexData + polygon.vertices[1];

  x2 = (double)p->x;
  y2 = (double)p->y;
  z2 = (double)p->z;

  dx = x2 - x1;
  dy = y2 - y1;
  dz = z2 - z1;

  d = sqrt(dx * dx + dy * dy + dz * dz);

  polygon.bmx[1] = (float)(d / 15.625);

  if (!IsPolygonFlat(&polygon, pLevel->pVertexData))
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Polygon is not flat" );

    return FALSE;
  }

  if (!IsPolygonConvex(&polygon, pLevel->pVertexData))
  {
    MsgBox( hwndMDIClient,
            MB_ICONEXCLAMATION,
            "Polygon is not convex" );

    return FALSE;
  }

  _fmemcpy(pLevel->pPolygonData + pLevel->nPolygons, &polygon, sizeof(POLYGON));

  ++pLevel->nPolygons;

  pLevel->bRebuildZone = TRUE;

  return TRUE;

} // NewPolygon


/*******************************************************************

    NAME:       UnlinkPolygon

    SYNOPSIS:   Unlink the polygon at the index indicated.

    ENTRY:      pLevel - The level data pointer.

                nIndex - The index of the polygon.

********************************************************************/

VOID UnlinkPolygon(WND_DATA *pWndData, LEVEL_DATA *pLevel, INT nIndex)
{
  double pixel_size, x1, y1, z1, x2, y2, z2, dx, dy, dz, d;
  POLYGON huge *p;

  p = pLevel->pPolygonData + nIndex;

  if ((p->flags & PF_LINKED) == 0)
  {
    return;
  }

  if (p->animation_index != -1 && (p->flags & (PF_ANIMATED | PF_MOTION_ANIMATED)))
  {
    pixel_size = pWndData->pAnimationData[p->animation_index].pixel_size;
  }
  else if (p->texture_index != -1)
  {
    pixel_size = pWndData->pTextureData[p->texture_index].pixel_size;
  }
  else
  {
    pixel_size = 15.625;
  }

  if (pixel_size > 0.0)
  {
    x1 = (double)pLevel->pVertexData[p->vertices[0]].x;
    y1 = (double)pLevel->pVertexData[p->vertices[0]].y;
    z1 = (double)pLevel->pVertexData[p->vertices[0]].z;

    x2 = (double)pLevel->pVertexData[p->vertices[1]].x;
    y2 = (double)pLevel->pVertexData[p->vertices[1]].y;
    z2 = (double)pLevel->pVertexData[p->vertices[1]].z;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
  
    d = sqrt(dx * dx + dy * dy + dz * dz);

    p->bmx[0] = (float)(0.0);
    p->bmy[0] = (float)(0.0);
    p->bmx[1] = (float)(d / pixel_size);
    p->bmy[1] = (float)(0.0);

    p->flags &= ~PF_LINKED;

    pLevel->bRebuildZone = TRUE;
  }

  return;

} // UnlinkPolygon


/*******************************************************************

    NAME:       ConvertToUnlinked

    SYNOPSIS:   Convert the linked polygon given into unlinked form.

    ENTRY:      pLevel - Pointer to the level data.

                nLink - Index of the linked polygon.

                pUnlinked - The buffer to hold the unlinked polygon.

********************************************************************/

VOID ConvertToUnlinked(LEVEL_DATA *pLevel, INT nLinked, POLYGON *pUnlinked)
{
  INT i, j, k, n1, n2, nUnlinked, nIndex, nMotionIndex;
  WORD wFlags;
  POLYGON huge *pLinked;
  TEXEL v1, v2, v3, v4, v5, v6;
  double a, b, r, xp, yp, zp, qx, qy, qz, rx, ry, rz;

  wFlags = (pLevel->pPolygonData + nLinked)->flags;
  nMotionIndex = (pLevel->pPolygonData + nLinked)->motion_index;

  //
  //  First, find the start of the chain.
  //

  nUnlinked = nLinked;

  while((pLevel->pPolygonData + nUnlinked)->flags & PF_LINKED)
  {
    nUnlinked = (pLevel->pPolygonData + nUnlinked)->polygon_link;
  }

  _fmemcpy(pUnlinked, pLevel->pPolygonData + nUnlinked, sizeof(POLYGON));

  //
  //  Traverse the links until the desired linked polygon has been converted.
  //

  while(nUnlinked != nLinked)
  {
    nIndex = nLinked;

    //
    //  Find the next polygon linked to the unlinked polygon.
    //

    while(nUnlinked != (pLevel->pPolygonData + nIndex)->polygon_link)
    {
      nIndex = (pLevel->pPolygonData + nIndex)->polygon_link;
    }

    pLinked = pLevel->pPolygonData + nIndex;

    //
    //  Find the common polygon edge.
    //

    n1 = (pUnlinked->flags & 7) + 3;
    n2 = (pLinked->flags & 7) + 3;

    for(i = 0; i < n1; ++i)
    {
      qx = (double)pLevel->pVertexData[pUnlinked->vertices[i]].x;
      qy = (double)pLevel->pVertexData[pUnlinked->vertices[i]].y;
      qz = (double)pLevel->pVertexData[pUnlinked->vertices[i]].z;

      k = (i + 1) % n1;

      rx = (double)pLevel->pVertexData[pUnlinked->vertices[k]].x;
      ry = (double)pLevel->pVertexData[pUnlinked->vertices[k]].y;
      rz = (double)pLevel->pVertexData[pUnlinked->vertices[k]].z;

      for(j = 0; j < n2; ++j)
      {
        xp = (double)pLevel->pVertexData[pLinked->vertices[j]].x;
        yp = (double)pLevel->pVertexData[pLinked->vertices[j]].y;
        zp = (double)pLevel->pVertexData[pLinked->vertices[j]].z;

        a = (xp - qx) * (xp - qx) + (yp - qy) * (yp - qy) + (zp - qz) * (zp - qz);
        b = (rx - qx) * (rx - qx) + (ry - qy) * (ry - qy) + (rz - qz) * (rz - qz);
        r = (xp - qx) * (rx - qx) + (yp - qy) * (ry - qy) + (zp - qz) * (rz - qz);

        if (fabs(a - r * r / b) > 1.0)
        {
          continue;   
        }

        k = (j + 1) % n2;

        xp = (double)pLevel->pVertexData[pLinked->vertices[k]].x;
        yp = (double)pLevel->pVertexData[pLinked->vertices[k]].y;
        zp = (double)pLevel->pVertexData[pLinked->vertices[k]].z;

        a = (xp - qx) * (xp - qx) + (yp - qy) * (yp - qy) + (zp - qz) * (zp - qz);
        b = (rx - qx) * (rx - qx) + (ry - qy) * (ry - qy) + (rz - qz) * (rz - qz);
        r = (xp - qx) * (rx - qx) + (yp - qy) * (ry - qy) + (zp - qz) * (rz - qz);

        if (fabs(a - r * r / b) < 1.0)
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
    //  Compute the bitmap coordinates associated with the two points (q & r) found above.
    //

    v1.x   = (double)pLevel->pVertexData[pUnlinked->vertices[0]].x;
    v1.y   = (double)pLevel->pVertexData[pUnlinked->vertices[0]].y;
    v1.z   = (double)pLevel->pVertexData[pUnlinked->vertices[0]].z;
    v1.bmx = (double)pUnlinked->bmx[0];
    v1.bmy = (double)pUnlinked->bmy[0];

    v2.x   = (double)pLevel->pVertexData[pUnlinked->vertices[1]].x;
    v2.y   = (double)pLevel->pVertexData[pUnlinked->vertices[1]].y;
    v2.z   = (double)pLevel->pVertexData[pUnlinked->vertices[1]].z;
    v2.bmx = (double)pUnlinked->bmx[1];
    v2.bmy = (double)pUnlinked->bmy[1];

    v3.x = qx;
    v3.y = qy;
    v3.z = qz;

    ComputeBitmapCoordinates(&v1, &v2, &v3);

    v4.x = rx;
    v4.y = ry;
    v4.z = rz;

    ComputeBitmapCoordinates(&v1, &v2, &v4);

    //
    //  Compute the bitmap coordinates associated with the first and second
    //  vertex of the linked polygon.
    //

    v5.x = (double)pLevel->pVertexData[pLinked->vertices[0]].x;
    v5.y = (double)pLevel->pVertexData[pLinked->vertices[0]].y;
    v5.z = (double)pLevel->pVertexData[pLinked->vertices[0]].z;

    ComputeBitmapCoordinates2(&v3, &v4, &v5);

    pUnlinked->bmx[0] = (float)v5.bmx;
    pUnlinked->bmy[0] = (float)v5.bmy;

    v6.x = (double)pLevel->pVertexData[pLinked->vertices[1]].x;
    v6.y = (double)pLevel->pVertexData[pLinked->vertices[1]].y;
    v6.z = (double)pLevel->pVertexData[pLinked->vertices[1]].z;

    ComputeBitmapCoordinates2(&v3, &v4, &v6);

    pUnlinked->bmx[1] = (float)v6.bmx;
    pUnlinked->bmy[1] = (float)v6.bmy;

    pUnlinked->flags = (pLinked->flags & ~PF_LINKED);

    for(i = 0; i < n2; ++i)
    {
      pUnlinked->vertices[i] = pLinked->vertices[i];
    }

    nUnlinked = nIndex;
  }

  pUnlinked->flags = (wFlags & ~PF_LINKED);
  pUnlinked->motion_index = nMotionIndex;

} // ConvertToUnlinked


/*******************************************************************

    NAME:       SetSelectedVertices

    SYNOPSIS:   Set all selected vertices to the indicated values.

    ENTRY:      pLevel - The level data pointer.

                sx - x value string.

                sy - y value string.

                sz - z value string.

********************************************************************/

VOID SetSelectedVertices(LEVEL_DATA *pLevel, LPSTR sx, LPSTR sy, LPSTR sz)
{
  INT i;
  double x, y, z;
  BOOL bSetX, bSetY, bSetZ;
  VERTEX huge *p;

  bSetX = FALSE;
  bSetY = FALSE;
  bSetZ = FALSE;

  if (sx)
  {
    if (*sx)
    {
      bSetX = TRUE;
      x = atof(sx);
    }
  }

  if (sy)
  {
    if (*sy)
    {
      bSetY = TRUE;
      y = atof(sy);
    }
  }

  if (sz)
  {
    if (*sz)
    {
      bSetZ = TRUE;
      z = atof(sz);
    }
  }

  p = pLevel->pVertexData;

  for(i = 0; i < pLevel->nVertices; ++i)
  {
    if (p->flags & VF_SELECTED)
    {
      if (bSetX)
      {
        p->x = (float)x;
      }

      if (bSetY)
      {
        p->y = (float)y;
      }

      if (bSetZ)
      {
        p->z = (float)z;
      }
    }

    ++p;
  }

  pLevel->bRebuildZone = TRUE;

  return;

} // SetSelectedVertices


/*******************************************************************

    NAME:       DeleteDuplicateVertices

    SYNOPSIS:   Remove duplicate vertices.

********************************************************************/

VOID DeleteDuplicateVertices(LEVEL_DATA *pLevel, BOOL bSelOnly)
{

#if 0

  INT i, j;
  DWORD dwSize;
  ROUNDED_VERTEX huge *pRoundedVertex;

  if (pLevel->nVertices > 0)
  {
    dwSize = (DWORD)pLevel->nVertices * (DWORD)sizeof(ROUNDED_VERTEX);

    pRoundedVertex = (ROUNDED_VERTEX *)GlobalAllocPtr(GPTR, dwSize);

    if (pRoundedVertex == NULL)
    {
      MsgBox( hwndMDIClient,
              MB_ICONEXCLAMATION,
              "Out of memory" );

      return;
    }

    for(i = 0; i < pLevel->nVertices; ++i)
    {
      pRoundedVertex[i].x = (long)Round(pLevel->pVertexData[i].x);
      pRoundedVertex[i].y = (long)Round(pLevel->pVertexData[i].y);
      pRoundedVertex[i].z = (long)Round(pLevel->pVertexData[i].z);
    }

    for(i = 0; i < pLevel->nVertices - 1; ++i)
    {
      for(j = i + 1; j < pLevel->nVertices; ++j)
      {
        if ( pRoundedVertex[i].x == pRoundedVertex[j].x &&
             pRoundedVertex[i].y == pRoundedVertex[j].y &&
             pRoundedVertex[i].z == pRoundedVertex[j].z )
        {
          if ((pLevel->pVertexData + j)->flags & VF_SELECTED)
          {
            (pLevel->pVertexData + i)->flags |= VF_SELECTED;
          }
          
          ReplaceVertex(pLevel, j, i);
          DeleteVertex(pLevel, j);
          pLevel->bRebuildZone = TRUE;

          while(j < pLevel->nVertices)
          {
            pRoundedVertex[j].x = pRoundedVertex[j+1].x;
            pRoundedVertex[j].y = pRoundedVertex[j+1].y;
            pRoundedVertex[j].z = pRoundedVertex[j+1].z;

            ++j;
          }

          break;
        }
      }
    }

    GlobalFreePtr(pRoundedVertex);
  }

#else 

  INT i, j, n, num_deleted_vertices, num_deleted_polygons;
  double dx, dy, dz;
  HCURSOR hcurSave;

  hcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT));

  num_deleted_vertices = 0;
  num_deleted_polygons = 0;

  if (bSelOnly)
  {
    n = pLevel->nVertices;
  }
  else
  {
    n = pLevel->nVertices - 1;
  }

  if (pLevel->nVertices > 0)
  {
    for(i = 0; i < n; ++i)
    {
      if (bSelOnly)
      {
        if (((pLevel->pVertexData + i)->flags & VF_SELECTED) == 0)
        {
          continue;
        }

        j = 0;
      }
      else
      {
        j = i + 1;
      }

      while(j < pLevel->nVertices)
      {
        if (i != j)
        {
          dx = (double)(pLevel->pVertexData + i)->x - (double)(pLevel->pVertexData + j)->x;
          dy = (double)(pLevel->pVertexData + i)->y - (double)(pLevel->pVertexData + j)->y;
          dz = (double)(pLevel->pVertexData + i)->z - (double)(pLevel->pVertexData + j)->z;

          if (dx * dx + dy * dy + dz * dz < 4.0)
          {
            if ((pLevel->pVertexData + j)->flags & VF_SELECTED)
            {
              (pLevel->pVertexData + i)->flags |= VF_SELECTED;
            }
          
            num_deleted_vertices += 1;
            num_deleted_polygons += ReplaceVertex(pLevel, j, i);
            DeleteVertex(pLevel, j);
            pLevel->bRebuildZone = TRUE;
            break;
          }
        }

        ++j;
      }
    }
  }

  if (num_deleted_vertices == 1)
  {
    MsgBox( hwndMDIClient,
            MB_ICONINFORMATION,
            "1 duplicate vertex has been deleted" );
  }
  else if (num_deleted_vertices > 1)
  {
    MsgBox( hwndMDIClient,
            MB_ICONINFORMATION,
            "%d duplicate vertices have been deleted",
            num_deleted_vertices );
  }

  if (num_deleted_polygons == 1)
  {
    MsgBox( hwndMDIClient,
            MB_ICONINFORMATION,
            "1 polygon with duplicated vertices has been deleted" );
  }
  else if (num_deleted_polygons > 1)
  {
    MsgBox( hwndMDIClient,
            MB_ICONINFORMATION,
            "%d polygons with duplicated vertices have been deleted",
            num_deleted_polygons );
  }

#endif

  SetCursor(hcurSave);

  return;

} // DeleteDuplicateVertices


/*******************************************************************

    NAME:       DeleteDuplicatePolygons

    SYNOPSIS:   Remove duplicate polygons.

********************************************************************/

VOID DeleteDuplicatePolygons(LEVEL_DATA *pLevel)
{
  INT i, j, k, n1, n2, offset, num_deleted_polygons;
  POLYGON huge *p1;
  POLYGON huge *p2;

  num_deleted_polygons = 0;

  for(i = 0; i < pLevel->nPolygons - 1; ++i)
  {
    p1 = pLevel->pPolygonData + i;
    n1 = (p1->flags & 7) + 3;

    for(j = i + 1; j < pLevel->nPolygons; ++j)
    {
      p2 = pLevel->pPolygonData + j;
      n2 = (p2->flags & 7) + 3;

      if (n1 == n2)
      {
        for(offset = 0; offset < n1; ++offset)
        {
          for(k = 0; k < n1; ++k)
          {
            if (p1->vertices[k] != p2->vertices[(k + offset) % n1])
            {
              break;					// no match
            }
          }

          if (k == n1)
          {
            break;						// found a match!
          }
        }

        if (offset < n1)
        {
          ++num_deleted_polygons;
          ReplacePolygon(pLevel, j, i);
          DeletePolygon(pLevel, j);
          pLevel->bRebuildZone = TRUE;
          break;
        }
      }
    }
  }

  if (num_deleted_polygons == 1)
  {
    MsgBox( hwndMDIClient,
            MB_ICONINFORMATION,
            "1 duplicate polygon has been deleted" );
  }
  else if (num_deleted_polygons > 1)
  {
    MsgBox( hwndMDIClient,
            MB_ICONINFORMATION,
            "%d duplicate polygons have been deleted",
            num_deleted_polygons );
  }

  return;

} // DeleteDuplicatePolygons


/*******************************************************************

    NAME:       MoveSelectedVertices

    SYNOPSIS:   Move all selected vertices by the indicated amount.

    ENTRY:      pLevel - The level data pointer.

                ldx - x displacement.

                ldy - y displacement.

                ldz - z displacement.

********************************************************************/

VOID MoveSelectedVertices(LEVEL_DATA *pLevel, LONG ldx, LONG ldy, LONG ldz)
{
  INT i;
  VERTEX huge *p;

  if (ldx != 0L || ldy != 0L || ldz != 0L)
  {
    p = pLevel->pVertexData;

    for(i = 0; i < pLevel->nVertices; ++i)
    {
      if (p->flags & VF_SELECTED)
      {
        p->x += (float)ldx;
        p->y += (float)ldy;
        p->z += (float)ldz;
      }

      ++p;
    }

    pLevel->bRebuildZone = TRUE;
  }

  return;

} // MoveSelectedVertices


/*******************************************************************

    NAME:       ExtendPolygonSelections

    SYNOPSIS:   Select all vertices of selected polygons.

    ENTRY:      pLevel - The level data pointer.

********************************************************************/

VOID ExtendPolygonSelections(LEVEL_DATA *pLevel)
{
  INT i, j, n;
  POLYGON huge *p;

  p = pLevel->pPolygonData;

  for(i = 0; i < pLevel->nPolygons; ++i)
  {
    if (p->flags & PF_SELECTED)
    {
      n = (p->flags & 7) + 3;

      for(j = 0; j < n; ++j)
      {
        (pLevel->pVertexData + p->vertices[j])->flags |= VF_SELECTED;
      }
    }

    ++p;
  }

  return;

} // ExtendPolygonSelections
