/*******************************************************************

    ved_type.h

    Global type definitions for the VEDIT project.

********************************************************************/


#ifndef _VED_TYPE_H_
#define _VED_TYPE_H_


//
//  Some useful types missing from 16-bit Windows.
//

#ifndef WIN32

typedef char            CHAR;
typedef int             INT;
typedef unsigned long   ULONG;
typedef ULONG         * PULONG;
typedef ULONG         * LPULONG;

#endif  // !WIN32


//
//  File resident structures.
//

typedef struct
{
  int      id;                          // ID_FILE_INFO
  long     size;                        // size of hdr
  char     name[32];                    // name
  char     author[32];                  // author
  char     date[32];                    // creation date
  char     version[16];                 // VEdit version used

} FILE_INFO_HDR;

typedef struct
{
  int      id;                          // ID_PALETTE_DATA
  long     size;                        // size of hdr
  BYTE     red[256];                    // red values
  BYTE     green[256];                  // green values
  BYTE     blue[256];                   // blue values

} PALETTE_DATA_HDR;

typedef struct
{
  int      id;                          // ID_BITMAP_DATA
  long     size;                        // size of hdr plus data
  int      hdr_size;                    // size of hdr
  char     bitmap_name[14];             // bitmap name (DOS name)
  int      x_len;                       // width
  int      y_len;                       // height (image data follows hdr)
  WORD     flags;                       // flags

} BITMAP_DATA_HDR;

typedef struct
{
  int      id;                          // ID_TEXTURE_DATA
  long     size;                        // size of hdr
  char     texture_name[32];            // texture name
  int      bitmap_index;                // bitmap index
  int      color_index;                 // color index for this texture
  float    pixel_size;                  // mm per bitmap pixel

} TEXTURE_DATA_HDR;

typedef struct
{
  int      id;                          // ID_SOUND_DATA
  long     size;                        // size of hdr plus data
  int      hdr_size;                    // size of hdr
  WORD     flags;                       // flags
  char     sound_name[14];              // bitmap name (DOS name)

} SOUND_DATA_HDR;

typedef struct
{
  int      id;                          // ID_ANIMATION_DATA
  long     size;                        // size of hdr
  char     animation_name[32];          // animation name
  int      num_elements;                // number of elements in animation
  int      index[MAX_ELEMENTS];         // bitmap or sound indecies
  float    pixel_size;                  // mm per bitmap pixel
  int      first_element;               // first element to show (or play)
  int      delay;                       // # timer ticks between frames

} ANIMATION_DATA_HDR;

typedef struct
{
  int      id;                          // ID_LIFEFORM_DATA
  long     size;                        // size of hdr
  char     lifeform_name[32];           // life form name
  int      lifeform_type;               // bitmap usage & life form behavior
  int      index[MAX_ELEMENTS];         // bitmap and sound indecies
  float    pixel_size;                  // mm per bitmap pixel

} LIFEFORM_DATA_HDR;

typedef struct
{
  int      id;                          // ID_ITEM_DATA
  long     size;                        // size of hdr
  char     item_name[32];               // item name
  int      item_type;                   // bitmap usage & item characteristics
  int      index[MAX_ELEMENTS];         // bitmap and sound indecies
  float    pixel_size;                  // mm per bitmap pixel

} ITEM_DATA_HDR;

typedef struct
{
  int      id;                          // ID_SPRITE_DATA
  long     size;                        // size of hdr
  WORD     flags;                       // flags
  char     sprite_name[32];             // sprite name
  int      bitmap_index;                // bitmap index (if static)
  int      animation_index;             // animation index (if animated)
  int      damage_sound_index;          // sound index (when damaged)
  int      damage_animation_index;      // animation index (when damaged)
  float    pixel_size;                  // mm per bitmap pixel

} OLD_SPRITE_DATA_HDR;

typedef struct
{
  int      id;                          // ID_SPRITE_DATA
  long     size;                        // size of hdr
  WORD     flags;                       // flags
  char     sprite_name[32];             // sprite name
  int      bitmap_index;                // bitmap index (if static)
  int      animation_index;             // animation index (if animated)
  int      damage_animation_index;      // animation index (if damaged)
  int      damage_animation_index_2;    // animation index (if damaged again)
  float    pixel_size;                  // mm per bitmap pixel

} SPRITE_DATA_HDR;

typedef struct
{
  int      id;                          // ID_NOISE_DATA
  long     size;                        // size of hdr
  WORD     flags;                       // flags
  char     noise_name[32];              // noise name
  int      sound_index;                 // sound index
  int      delay;                       // noise loop delay
  float    radius;                      // noise radius
  float    height;                      // noise height

} NOISE_DATA_HDR;

typedef struct                          // for support of old GOB files
{
  int      id;                          // ID_GROUP_DATA
  long     size;                        // size of hdr
  char     group_name[32];              // group name

} GROUP_DATA_HDR;

typedef struct                          // for support of old GOB files
{
  int      id;                          // ID_TRIGGER_DATA
  long     size;                        // size of hdr
  char     trigger_name[32];            // trigger name
  WORD     flags;                       // flags
  float    radius;                      // trigger radius
  float    height;                      // trigger height
  int      item_type;                   // required object
  int      sound_index;                 // play this sound when triggered

} TRIGGER_DATA_HDR;

typedef struct                          // for support of old GOB files
{
  int      id;                          // ID_MOTION_DATA
  long     size;                        // size of hdr
  char     motion_name[32];             // motion name
  WORD     flags;                       // flags
  float    dx;                          // motion per cycle
  float    dy;                          // motion per cycle
  float    dz;                          // motion per cycle
  int      num_steps;                   // total number of motion steps
  int      return_delay;                // return to home position delay
  int      trigger_index;               // motion is started by this trigger
  int      sound_index_start;           // play this sound at start of motion
  int      sound_index_run;             // play this sound when motion active
  int      sound_index_stop;            // play this sound at end of motion

} MOTION_DATA_HDR;

typedef struct                          // for support of old GOB files
{
  int      id;                          // ID_OBJECT_DATA
  long     size;                        // size of hdr plus object data
  int      hdr_size;                    // size of hdr
  char     object_name[32];             // object name
  char     object_author[32];           // author name
  long     object_id;                   // object id
  int      nVertices;                   // number of vertices
  int      nPolygons;                   // number of polygons

} OBJECT_DATA_HDR;


//
//  Level data structures (file resident).
//
     
typedef struct
{
  WORD     flags;
  float    x;
  float    y;
  float    z;
  int      sprite_index;                // sprite index
  int      noise_index;                 // noise index
  int      lifeform_index;              // life form index (life forms)
  int      orientation;                 // initial orientation
  int      group_index;                 // group index
  int      item_index;                  // item index
  int      trigger_index;               // trigger index
  BYTE     unused[4];                   // pad to 32 bytes

} VERTEX;
     
typedef struct
{
  WORD     flags;                       // flags & number of vertices
  int      polygon_link;                // index into polygon data
  int      texture_index;               // texture index
  float    bmx[2];                      // bitmap x pos
  float    bmy[2];                      // bitmap y pos
  int      vertices[MAX_VERTICES];      // indecies into vertex data
  int      animation_index;             // animation data index
  float    texture_dx;                  // for moving textures
  float    texture_dy;                  // for moving textures
  int      motion_index;                // for moving polygons
  int      group_index;                 // group index
  float    texture_x_offset;            // texture x offset
  float    texture_y_offset;            // texture y offset
  int      old_damage_sound_index;      // OBSOLETE!
  int      damage_animation_index;      // animation index (when damaged)
  int      old_animation_sound_index;   // OBSOLETE!
  BYTE     unused[62];                  // pad to 128 bytes
  
} POLYGON;

typedef struct
{
  char     trigger_name[32];            // trigger name
  WORD     flags;                       // flags
  float    radius;                      // trigger radius
  float    height;                      // trigger height
  int      item_type;                   // required object
  int      sound_index;

} TRIGGER;

typedef struct
{
  char     motion_name[32];             // motion name
  WORD     flags;                       // flags
  float    dx;                          // motion per cycle
  float    dy;                          // motion per cycle
  float    dz;                          // motion per cycle
  int      num_steps;                   // total number of motion steps
  int      return_delay;                // return to home position delay
  int      trigger_index;               // motion is started by this trigger
  int      sound_index_start;           // play this sound at start of motion
  int      sound_index_run;             // play this sound when motion active
  int      sound_index_stop;            // play this sound at end of motion

} MOTION;

typedef struct
{
  char     group_name[32];              // group name

} GROUP;

typedef struct
{
  char     mark_name[32];               // mark name
  long     x;                           // view x
  long     y;                           // view y
  long     z;                           // view z
  double   rotation;                    // view rotation
  double   elevation;                   // view elevation

} MARK;

typedef struct
{
  int      id;                          // ID_OLD_LEVEL_DATA
  long     size;                        // size of hdr plus object data
  int      hdr_size;                    // size of hdr
  char     level_name[32];              // level name
  char     level_author[32];            // author name
  int      nVertices;                   // number of vertices
  int      nPolygons;                   // number of polygons
  int      nTriggers;                   // number of triggers
  int      nMotions;                    // number of motions
  int      nGroups;                     // number of groups
  double   view_home_rotation;
  double   view_home_elevation;
  long     view_home_x;                 // center of projection home x pos
  long     view_home_y;                 // center of projection home y pos
  long     view_home_z;                 // center of projection home z pos
  double   view_rotation;
  double   view_elevation;
  long     view_x;                      // center of projection x position
  long     view_y;                      // center of projection y position
  long     view_z;                      // center of projection z position
  long     view_delta;
  long     view_depth;                  // distance from c.o.p. to view plane
  long     view_width;                  // view plane width
  long     view_height;                 // view plane height
  long     grid_x;                      // drawing grid origin
  long     grid_y;                      // drawing grid origin
  long     grid_z;                      // drawing grid origin
  double   grid_rotation;               // drawing grid rotation
  double   grid_elevation;              // drawing grid elevation
  long     grid_delta;
  long     grid_spacing;                // drawing grid line spacing
  long     grid_size;                   // drawing grid size
  BOOL     bMaintainGridDist;           // maintain distance to grid
  BOOL     bLockViewToGrid;             // lock view rotation to grid origin
  BOOL     bSnapToGrid;                 // snap cursor to grid lines
  WORD     flags;
  int      bkg_bitmap_index;            // background bitmap
  int      sol_bitmap_index;            // start of level bitmap
  int      eol_bitmap_index;            // end of level bitmap
  char     sol_text[MAX_LEVEL_TEXT];    // start of level text
  char     eol_text[MAX_LEVEL_TEXT];    // end of level text

} OLD_LEVEL_DATA_HDR;

typedef struct
{
  int      id;                          // ID_LEVEL_DATA
  long     size;                        // size of hdr plus object data
  int      hdr_size;                    // size of hdr
  char     level_name[32];              // level name
  char     level_author[32];            // author name
  int      nVertices;                   // number of vertices
  int      nPolygons;                   // number of polygons
  int      nTriggers;                   // number of triggers
  int      nMotions;                    // number of motions
  int      nGroups;                     // number of groups
  int      nMarks;                      // number of marks
  int      nLastMark;                   // last selected mark
  double   view_home_rotation;
  double   view_home_elevation;
  long     view_home_x;                 // center of projection home x pos
  long     view_home_y;                 // center of projection home y pos
  long     view_home_z;                 // center of projection home z pos
  double   view_rotation;
  double   view_elevation;
  long     view_x;                      // center of projection x position
  long     view_y;                      // center of projection y position
  long     view_z;                      // center of projection z position
  long     view_delta;
  long     view_depth;                  // distance from c.o.p. to view plane
  long     view_width;                  // view plane width
  long     view_height;                 // view plane height
  long     grid_x;                      // drawing grid origin
  long     grid_y;                      // drawing grid origin
  long     grid_z;                      // drawing grid origin
  double   grid_rotation;               // drawing grid rotation
  double   grid_elevation;              // drawing grid elevation
  long     grid_delta;
  long     grid_spacing;                // drawing grid line spacing
  long     grid_size;                   // drawing grid size
  BOOL     bMaintainGridDist;           // maintain distance to grid
  BOOL     bLockViewToGrid;             // lock view rotation to grid origin
  BOOL     bSnapToGrid;                 // snap cursor to grid lines
  WORD     flags;
  int      bkg_bitmap_index;            // background bitmap
  int      sol_bitmap_index;            // start of level bitmap
  int      eol_bitmap_index;            // end of level bitmap
  char     sol_text[MAX_LEVEL_TEXT];    // start of level text
  char     eol_text[MAX_LEVEL_TEXT];    // end of level text

} LEVEL_DATA_HDR;


//
//  Zone data structures. (used in ZON files)
//
//  In order to conserve memory, zone data polygons vary in size depending
//  on the number of vertices.  For the same reason, zone data animations
//  and zone data life forms vary in size depending on the number of frames.
//
//  Animated polygons or sprites are immediately followed by a zone data
//  animation structure.
//

typedef struct
{
  int                id;                // ID_ZONE_DATA_HDR
  long               size;              // size of hdr plus data
  long               zone_size;         // size of zone data alone
  int                level_index;
  char               level_name[32];
  char               level_author[32];
  int                num_vis_lists;
  float              player_x;
  float              player_y;
  float              player_z;
  float              player_orientation;
  long               build_number;
  int                bkg_bitmap_index;  // background bitmap
  int                sol_bitmap_index;  // start of level bitmap
  int                eol_bitmap_index;  // end of level bitmap
  char               sol_text[MAX_LEVEL_TEXT];
  char               eol_text[MAX_LEVEL_TEXT];

} ZONE_DATA_HDR;

typedef struct
{
  float              object_x;
  float              object_y;
  float              object_z;
  float              bitmap_x;
  float              bitmap_y;

} ZONE_DATA_VERTEX;

typedef struct                          
{
  unsigned int       type;              // ZD_POLYGONx
  unsigned int       flags;
  int                bitmap_index;
  int                motion_index;
  int                animation_index[2];
  int                current_element_index[2];
  int                current_element_delay[2];
  int                vis_list_index;
  int                mot_list_index;
  float              a;
  float              b;
  float              c;
  float              d;
  float              bitmap_dx;         // bitmap dx for moving bitmaps
  float              bitmap_dy;         // bitmap dy for moving bitmaps
  ZONE_DATA_VERTEX   vertices[MAX_CLIPPED_VERTICES];

} ZONE_DATA_POLYGON;

typedef struct                          
{
  unsigned int       type;              // ZD_SPRITE
  unsigned int       flags;
  int                bitmap_index;
  int                animation_index[3];
  int                current_element_index[3];
  int                current_element_delay[3];
  float              x;
  float              y;
  float              z;
  float              width;
  float              height;
  float              physical_width;
  float              physical_height;
  float              mm_per_pixel;

} ZONE_DATA_SPRITE;

typedef struct                          
{
  unsigned int       type;              // ZD_LIFEFORM
  unsigned int       flags;
  int                lifeform_type;
  int                lifeform_index;
  ZONE_DATA_POLYGON *list_polygon;
  float              x;
  float              y;
  float              z;
  float              width;
  float              height;
  float              physical_width;
  float              physical_height;
  float              mm_per_pixel;
  float              player_x;
  float              player_y;
  float              player_z;
  float              force_dx;
  float              force_dy;
  float              force_dz;
  int                health;
  int                wander_countdown;
  int                walking_dir;
  int                attack_timer;
  int                state;
  int                index[MAX_ELEMENTS];

} ZONE_DATA_LIFEFORM;

typedef struct                          
{
  unsigned int       type;              // ZD_ITEM
  unsigned int       flags;
  int                item_type;
  float              x;
  float              y;
  float              z;
  float              width;
  float              height;
  float              physical_width;
  float              physical_height;
  float              mm_per_pixel;
  int                state;
  int                damage;
  int                index[MAX_ELEMENTS];

} ZONE_DATA_ITEM;

typedef struct
{
  unsigned int       type;              // ZD_NOISE
  unsigned int       flags;
  float              x;
  float              y;
  float              z;
  float              radius;
  float              height;
  int                sound_index;
  int                delay;
  int                delay_remaining;

} ZONE_DATA_NOISE;

typedef struct                          
{
  unsigned int       type;              // ZD_TRIGGER
  unsigned int       flags;
  float              x;
  float              y;
  float              z;
  float              radius;
  float              height;
  int                item_type;
  int                sound_index;
  int                num_motions;
  int                motion_index[MAX_MOTIONS_PER_TRIGGER];

} ZONE_DATA_TRIGGER;

typedef struct
{
  unsigned int       type;              // ZD_MOTION
  unsigned int       flags;
  float              dx;                // motion per cycle
  float              dy;                // motion per cycle
  float              dz;                // motion per cycle
  int                num_steps;         // total number of motion steps
  int                return_delay;      // return to home position delay
  int                sound_index_start; // play this sound at start of motion
  int                sound_index_run;   // play this sound when motion active
  int                sound_index_stop;  // play this sound at end of motion

} ZONE_DATA_MOTION;


//
//  Clipboard type definitions.
//

typedef struct
{
  int      num_vertices;                // number of vertices
  int      num_polygons;                // number of polygons
  int      num_triggers;                // number of triggers
  int      num_motions;                 // number of motions
  int      num_groups;                  // number of groups
  int      num_marks;                   // number of marks
  long     grid_x;                      // drawing grid origin
  long     grid_y;                      // drawing grid origin
  long     grid_z;                      // drawing grid origin
  double   grid_rotation;               // drawing grid rotation
  double   grid_elevation;              // drawing grid elevation
  char     level_name[32];              // level name
  char     level_author[32];            // author name
  WORD     flags;                       // level flags
  int      bkg_bitmap_index;            // background bitmap
  int      sol_bitmap_index;            // start of level bitmap
  int      eol_bitmap_index;            // end of level bitmap
  char     sol_text[MAX_LEVEL_TEXT];    // start of level text
  char     eol_text[MAX_LEVEL_TEXT];    // end of level text

} CB_INFO;


//
//  Memory resident type definitions.
//
     
typedef struct
{
  double   x;
  double   y;
  double   z;
  double   bmx;
  double   bmy;

} TEXEL;
     
typedef struct
{
  long     x;
  long     y;
  long     z;
  BYTE     unused[4];                   // pad to 16 bytes

} ROUNDED_VERTEX;

typedef struct
{
  BYTE     red[256];                    // red values
  BYTE     green[256];                  // green values
  BYTE     blue[256];                   // blue values

} PALETTE_DATA;

typedef struct
{
  WORD     flags;                       // flags
  char     bitmap_name[14];             // bitmap name (DOS name)
  int      x_len;                       // width
  int      y_len;                       // height
  BYTE    *data;                        // bitmap image data

} BITMAP_DATA;

typedef struct
{
  WORD     flags;                       // flags
  char     texture_name[32];            // texture name
  int      bitmap_index;                // bitmap index
  int      color_index;                 // color index for this group
  float    pixel_size;                  // mm per bitmap pixel

} TEXTURE_DATA;

typedef struct
{
  WORD       flags;                     // flags
  char       sound_name[14];            // sound name (DOS name)
  long       length;                    // length
  BYTE huge *data;                      // sound data

} SOUND_DATA;

typedef struct
{
  char     animation_name[32];          // animation name
  int      num_elements;                // number of elements in animation
  int      index[MAX_ELEMENTS];         // bitmap or sound indecies
  float    pixel_size;                  // mm per bitmap pixel
  int      first_element;               // first element to show (or play)
  int      delay;                       // # timer ticks between frames

} ANIMATION_DATA;

typedef struct
{
  char     lifeform_name[32];           // life form name
  int      lifeform_type;               // bitmap usage & life form behavior
  int      index[MAX_ELEMENTS];         // bitmap and sound indecies
  float    pixel_size;                  // mm per bitmap pixel

} LIFEFORM_DATA;

typedef struct
{
  char     item_name[32];               // item name
  int      item_type;                   // bitmap usage & item characteristics
  int      index[MAX_ELEMENTS];         // bitmap and sound indecies
  float    pixel_size;                  // mm per bitmap pixel

} ITEM_DATA;

typedef struct
{
  WORD     flags;                       // flags
  char     sprite_name[32];             // sprite name
  int      bitmap_index;                // bitmap index (if static)
  int      animation_index;             // animation index (if animated)
  int      damage_animation_index;      // animation index (if damaged)
  int      damage_animation_index_2;    // animation index (if damaged again)
  float    pixel_size;                  // mm per bitmap pixel

} SPRITE_DATA;

typedef struct
{
  WORD     flags;                       // flags
  char     noise_name[32];              // noise name
  int      sound_index;                 // sound index
  int      delay;                       // noise loop delay
  float    radius;                      // noise radius
  float    height;                      // noise height

} NOISE_DATA;

typedef struct
{
  int           bRebuildZone;           // if TRUE, rebuild zone
  double        view_home_rotation;
  double        view_home_elevation;
  long          view_home_x;            // center of projection home x pos
  long          view_home_y;            // center of projection home y pos
  long          view_home_z;            // center of projection home z pos
  double        view_rotation;
  double        view_elevation;
  long          view_x;                 // center of projection x position
  long          view_y;                 // center of projection y position
  long          view_z;                 // center of projection z position
  long          view_delta;
  long          view_depth;             // distance from c.o.p. to view plane
  long          view_width;             // view plane width
  long          view_height;            // view plane height
  long          grid_x;                 // drawing grid origin
  long          grid_y;                 // drawing grid origin
  long          grid_z;                 // drawing grid origin
  double        grid_rotation;          // drawing grid rotation
  double        grid_elevation;         // drawing grid elevation
  long          grid_delta;
  long          grid_spacing;           // drawing grid line spacing
  long          grid_size;              // drawing grid size
  BOOL          bMaintainGridDist;      // maintain distance to grid
  BOOL          bLockViewToGrid;        // lock view rotation to grid origin
  BOOL          bSnapToGrid;            // snap cursor to grid lines
  char          level_name[32];         // level name
  char          level_author[32];       // author name
  WORD          flags;
  int           bkg_bitmap_index;       // background bitmap
  int           sol_bitmap_index;       // start of level bitmap
  int           eol_bitmap_index;       // end of level bitmap
  char          sol_text[MAX_LEVEL_TEXT];
  char          eol_text[MAX_LEVEL_TEXT];
  int           nVertices;              // number of vertices
  int           nPolygons;              // number of polygons
  int           nTriggers;              // number of triggers
  int           nMotions;               // number of motions
  int           nGroups;                // number of groups
  int           nMarks;                 // number of marks
  int           nLastMark;              // last selected mark
  VERTEX huge  *pVertexData;            // vertex data array
  POLYGON huge *pPolygonData;           // polygon data array
  TRIGGER      *pTriggerData;           // trigger data array
  MOTION       *pMotionData;            // motion data array
  GROUP        *pGroupData;             // group data array
  MARK         *pMarkData;              // mark data array

} LEVEL_DATA;

typedef struct
{
  long          size;                   // size of zone header plus data
  BYTE huge    *data;                   // zone header then data

} ZONE_DATA;


//
//  MDI child window data.
//

typedef struct _WND_DATA
{
  WORD          cid;                    // Common window data id
  BOOL          loaded;                 // TRUE if loaded with valid data
  BOOL          need_to_save;           // TRUE if the data needs to be saved
  CHAR          path[256];              // DOS data path
  CHAR          name[256];              // DOS file name
  CHAR          file[256];              // DOS data path and file name
  char          file_name[32];          // long name
  char          file_author[32];        // author
  char          file_date[32];          // creation date
  char          file_version[16];       // VEdit version used
  int           level_index;            // index of level being viewed
  long          cursor_abs_x;           // cursor's absolute x position
  long          cursor_abs_y;           // cursor's absolute y position
  long          cursor_abs_z;           // cursor's absolute z position
  long          cursor_rel_x;           // cursor's grid relative x position
  long          cursor_rel_y;           // cursor's grid relative y position
  long          cursor_rel_z;           // cursor's grid relative z position
  long          drag_start_x;           // drag start x position
  long          drag_start_y;           // drag start y position
  long          drag_start_z;           // drag start z position
  BOOL          bSelVertex;             // TRUE if wSelVertex is valid
  int           nSelVertex;             // selected vertex index
  int           nSelVertices;           // number of selected vertices
  int           nSelVert[MAX_VERTICES]; // selected vertex indecies
  BOOL          bSelPolygon;            // TRUE if wSelPolygon is valid
  int           nSelPolygon;            // selected polygon index
  BOOL          bLnkPolygon;            // TRUE if wLnkPolygon is valid
  int           nLnkPolygon;            // link-to polygon index
  RECT          sbox;                   // selection box
  int           bm_width;               // current view bitmap width
  int           bm_height;              // current view bitmap height
  HBITMAP       hbmView;                // bitmap of current view
  INT           nBitmaps;               // number of bitmaps
  INT           nTextures;              // number of textures defined
  INT           nSounds;                // number of sounds
  INT           nAnimations;            // number of animations defined
  INT           nLifeForms;             // number of life forms defined
  INT           nItems;                 // number of items defined
  INT           nSprites;               // number of sprites
  INT           nNoises;                // number of noises
  INT           nLevels;                // number of levels
  BYTE huge    *pDragData;              // vertex data before drag
  PALETTE_DATA *pPalette;               // 256 color palette
  BITMAP_DATA  *pBitmapData;            // array of bitmap data
  TEXTURE_DATA *pTextureData;           // array of texture data
  SOUND_DATA   *pSoundData;             // array of sound data
  ANIMATION_DATA *pAnimationData;       // array of animation data
  LIFEFORM_DATA *pLifeFormData;         // array of life form data
  ITEM_DATA    *pItemData;              // array of item data
  SPRITE_DATA  *pSpriteData;            // array of sprite data
  NOISE_DATA   *pNoiseData;             // array of noise data
  LEVEL_DATA   *pLevelData;             // array of level data
  ZONE_DATA    *pZoneData;              // array of zone data
  BYTE huge    *pUndoData;              // undo data (for current level)

} WND_DATA, FAR * LPWND_DATA;

#define WNDPTR(w)       ((LPWND_DATA)GetWindowLong((w),GWL_DATA_PTR))

#endif  // _VED_TYPE_H_
