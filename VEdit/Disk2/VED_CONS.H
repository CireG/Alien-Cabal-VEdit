/*******************************************************************

    ved_cons.h

    Global constant definitions for the VEDIT project.

********************************************************************/


#ifndef _VED_CONS_H_
#define _VED_CONS_H_

//
//  Replacements for HANDLE_MSG which assumes all message handlers return
//  a type of BOOL. 
//

#define HANDLE_MSG_BOOL(hwnd, message, fn)    \
    case (message): \
      return (BOOL)HANDLE_##message((hwnd), (wParam), (lParam), (fn))

#define HANDLE_MSG_VOID(hwnd, message, fn)    \
    case (message): \
      HANDLE_##message((hwnd), (wParam), (lParam), (fn)); \
      return TRUE


//
//  Mathematical constants
//

#define PI                      3.14159265359


//
//  Array limit definitions.
//

#define MAX_NUM_VERTICES        8192    // max number of vertices
#define MAX_NUM_POLYGONS        4096    // max number of polygons
#define MAX_MARKS               256     // max marks
#define MAX_BITMAPS             1024    // max bitmaps
#define MAX_SOUNDS              256     // max sounds
#define MAX_GROUPS              256     // max groups
#define MAX_TEXTURES            256     // max textures
#define MAX_ANIMATIONS          128     // max animation definitions
#define MAX_LIFEFORMS           128     // max life form definitions
#define MAX_ITEMS               128     // max item definitions
#define MAX_SPRITES             128     // max sprite definitions
#define MAX_NOISES              128     // max noise definitions
#define MAX_VERTICES            8       // max vertices per polygon
#define MAX_CLIPPED_VERTICES    12      // max vertices per polygon after clipping
#define MAX_FRAMES              128     // for animations
#define MAX_ELEMENTS            128     // for life forms, items
#define NUM_LIFEFORM_TYPES      4       // hardcoded life form types
#define NUM_ITEM_TYPES          21      // hardcoded item types
#define MAX_TRIGGERS            256     // max triggers
#define MAX_MOTIONS             256     // max motions
#define MAX_MOTIONS_PER_TRIGGER 128     // max motions per trigger
#define MAX_LEVELS              128     // max levels per file
#define MAX_LEVEL_TEXT         1024     // max length of level text


//
//  Level flags.
//

#define LF_BKG_BITMAP           0x0001  // background bitmap
#define LF_SOL_TEXT             0x0002  // start of level text
#define LF_SOL_BITMAP           0x0004  // start of level bitmap
#define LF_EOL_TEXT             0x0008  // end of level text
#define LF_EOL_BITMAP           0x0010  // end of level bitmap


//
//  Bitmap flags.
//

#define BF_TRANSPARENT          0x0001
#define BF_EXPANDED             0x0002  // private VSCAPE flag
#define BF_TEXTURE              0x4000  // private VSCAPE flag
#define BF_LOAD_IT              0x8000  // private VSCAPE flag


//
//  Animation flags.
//

#define AF_SOUND_RESOURCE       0x4000


//
//  Sprite flags.
//

#define SF_HANGING              0x0001
#define SF_WALK_THROUGH         0x0002
#define SF_ANIMATED             0x0004
#define SF_DAMAGE_ANIMATED      0x0008
#define SF_DAMAGE_SOUND         0x0010
#define SF_DESTRUCTABLE         0x0020
#define SF_DAMAGE_ANIMATED_2    0x0040


//
//  Vertex flags.
//

#define VF_DIFFICULTY_LEVEL_1   0x0001
#define VF_DIFFICULTY_LEVEL_2   0x0002
#define VF_DIFFICULTY_LEVEL_3   0x0004
#define VF_SELECTED             0x0008
#define VF_HIDDEN               0x0010
#define VF_SPRITE               0x0020
#define VF_LIFEFORM             0x0040
#define VF_PLAYER_START         0x1000
#define VF_NOISE                0x2000
#define VF_ITEM                 0x4000
#define VF_TRIGGER              0x8000


//
//  Polygon flags.
//

#define PF_SELECTED             0x0008
#define PF_HIDDEN               0x0010
#define PF_LINKED               0x0020
#define PF_ANIMATED             0x0040
#define PF_MOTION_ANIMATED      0x0080
#define PF_DAMAGE_ANIMATED      0x0100
#define PF_OLD_DAMAGE_SOUND     0x0200  // OBSOLETE!
#define PF_DOUBLE_SIDED         0x0400
#define PF_INVISIBLE            0x0800
#define PF_WALK_THROUGH         0x1000
#define PF_OLD_ANIMATION_SOUND  0x2000  // OBSOLETE!
#define PF_MOVING_POLYGON       0x4000
#define PF_SECRET_PLACE         0x8000


//
//  Trigger flags.
//

#define TF_USER_TRIGGER         0x0001
#define TF_PROXIMITY_TRIGGER    0x0002
#define TF_ITEM_REQUIRED        0x0004
#define TF_SOUND                0x0008
#define TF_END_LEVEL            0x0010
#define TF_END_GAME             0x0020


//
//  Motion flags.
//

#define MF_AUTO_RUN             0x0004
#define MF_AUTO_RETURN          0x0008
#define MF_TRIGGER_RETURN       0x0010
#define MF_START_SOUND          0x0020
#define MF_RUN_SOUND            0x0040
#define MF_STOP_SOUND           0x0080


//
//  Zone data flags.
//

#define FLAG_ACTIVE             0x0001  // lifeforms
#define FLAG_TURN_LEFT          0x0002  // lifeforms
#define FLAG_WANDER_MODE        0x0004  // lifeforms
#define FLAG_ATTACK_MODE        0x0008  // lifeforms
#define FLAG_LINE_OF_SIGHT      0x0010  // lifeforms

#define FLAG_DIFFICULTY_LEVEL_1 0x0020  // lifeforms, items
#define FLAG_DIFFICULTY_LEVEL_2 0x0040  // lifeforms, items
#define FLAG_DIFFICULTY_LEVEL_3 0x0080  // lifeforms, items

#define FLAG_DYING              0x0100  // lifeforms
#define FLAG_DEAD               0x0200  // lifeforms

#define FLAG_USER_TRIGGER       0x0002  // triggers
#define FLAG_PROXIMITY_TRIGGER  0x0004  // triggers
#define FLAG_ITEM_REQUIRED      0x0008  // triggers
#define FLAG_END_LEVEL          0x0010  // triggers
#define FLAG_END_GAME           0x0020  // triggers

#define FLAG_DAMAGE_ANIMATED    0x0001  // polygons, sprites
#define FLAG_DAMAGE_ANIMATED_2  0x0002  // sprites
#define FLAG_DOUBLE_SIDED       0x0002  // polygons
#define FLAG_WALK_THROUGH       0x0004  // polygons, sprites
#define FLAG_GRABBED            0x0004  // items
#define FLAG_INVISIBLE          0x0010  // polygons
#define FLAG_ANIMATED           0x0020  // polygons, sprites
#define FLAG_MOVING_BITMAP      0x0040  // polygons
#define FLAG_HANGING            0x0080  // sprites
#define FLAG_MOVING_POLYGON     0x0080  // polygons
#define FLAG_DESTRUCTABLE       0x0100  // sprites
#define FLAG_DESTROYED          0x0200  // sprites
#define FLAG_MAPPED             0x0400  // polygons
#define FLAG_SECRET             0x1000  // polygons
#define FLAG_SECRET_FOUND       0x2000  // polygons

#define FLAG_MOTION_ACTIVE      0x0001  // motions
#define FLAG_FORWARD            0x0002  // motions
#define FLAG_AUTO_RUN           0x0004  // motions
#define FLAG_AUTO_RETURN        0x0008  // motions
#define FLAG_TRIGGER_RETURN     0x0010  // motions
#define FLAG_START_SOUND        0x0020  // motions
#define FLAG_RUN_SOUND          0x0040  // motions
#define FLAG_STOP_SOUND         0x0080  // motions


//
//  File I/O constants.
//
//  Note: NEVER redefine a previously defined id code!
//        Use the next highest sequential unused id code.
//

#define ID_PALETTE_DATA         2
#define ID_OBJECT_DATA          10
#define ID_BITMAP_DATA          14
#define ID_TEXTURE_DATA         15
#define ID_ANIMATION_DATA       16
#define ID_LIFEFORM_DATA        17
#define ID_FILE_INFO            20
#define ID_VIS_DATA             22
#define ID_VIS_LIST             23
#define ID_ITEM_DATA            24
#define ID_TRIGGER_DATA         25
#define ID_MOTION_DATA          26
#define ID_GROUP_DATA           27
#define ID_SOUND_DATA           29
#define ID_OLD_SPRITE_DATA      31
#define ID_NOISE_DATA           32
#define ID_OLD_LEVEL_DATA       33
#define ID_SPRITE_DATA          38
#define ID_ZONE_DATA            39
#define ID_LEVEL_DATA           40

#define ID_SAVE_GAME_AVATAR_DATA 1000
#define ID_SAVE_GAME_ZONE_DATA   1001
#define ID_SAVE_GAME_MOTION_DATA 1002
#define ID_SAVE_GAME_TMP_SPRITES 1003
#define ID_SAVE_GAME_PROJECTILES 1004


//
//  Zone data identifier constants.
//

#define ZD_OBJ_DEF              1
#define ZD_OBJ_REF              2
#define ZD_POLYGON3             3
#define ZD_POLYGON4             4
#define ZD_POLYGON5             5
#define ZD_POLYGON6             6
#define ZD_POLYGON7             7
#define ZD_POLYGON8             8
#define ZD_SPRITE               9
#define ZD_LIFEFORM             10
#define ZD_ITEM                 11
#define ZD_TRIGGER              12
#define ZD_MOTION               13
#define ZD_NOISE                14


//
//  Get/SetWindowLong offsets.
//

#define GWL_DATA_PTR            0       // MDI child window data


//
//  Maximum printf-like output allowed by MsgBox.
//

#define MAX_PRINTF_OUTPUT       512     // characters


#ifdef WIN32

//
//  Fix the broken Win32 WM_CREATE message cracker.
//

#undef HANDLE_WM_CREATE
#define HANDLE_WM_CREATE(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (LPCREATESTRUCT)(lParam)) ? 0L : (LRESULT)-1L)

#endif  // WIN32


//
//  Some useful constants missing from 16-bit Windows.
//

#ifndef WIN32
#define MAX_PATH                260     // characters
#endif  // !WIN32


//
//  Make life with RC a little easier.
//

#ifdef RC_INVOKED
#define ID(x)           x
#else   // !RC_INVOKED
#define ID(x)           MAKEINTRESOURCE(x)
#endif  // RC_INVOKED


//
//  These are base resource values upon which the
//  various resource IDs are based.  All resources should
//  begin with one of the following prefixes:
//
//      IDI_    - Icons.
//      IDB_    - Bitmaps.
//      IDM_    - Menus.
//      IDD_    - Dialogs.
//      IDC_    - Controls.
//      IDA_    - Accelerators.
//
//  Note the the resource compiler cannot do arithmetic on some
//  of these values.  It is up to the programmer to ensure they
//  don't conflict.
//

#define IDI_BASE                        1000
#define IDB_BASE                        2000
#define IDM_BASE                        3000
#define IDD_BASE                        4000
#define IDA_BASE                        6000
#define IDS_BASE                        7000


//
//  Icon resource IDs.
//

#define IDI_FRAME                       ID(1000)        // IDI_BASE
#define IDI_OBJECT                      ID(1001)        // IDI_BASE + 1


//
//  Bitmap resource IDs.
//

#define IDB_PTR1                        2000
#define IDB_PTR2                        2001
#define IDB_SELVERT1                    2002
#define IDB_SELVERT2                    2003
#define IDB_SELPOLY1                    2004
#define IDB_SELPOLY2                    2005
#define IDB_SELGRP1                     2006
#define IDB_SELGRP2                     2007
#define IDB_NEWVERT1                    2008
#define IDB_NEWVERT2                    2009
#define IDB_NEWPOLY1                    2010
#define IDB_NEWPOLY2                    2011
#define IDB_ROTPOLY1                    2012
#define IDB_ROTPOLY2                    2013
#define IDB_FLPPOLY1                    2014
#define IDB_FLPPOLY2                    2015
#define IDB_LNKPOLY1                    2016
#define IDB_LNKPOLY2                    2017
#define IDB_UP1                         2018
#define IDB_UP2                         2019
#define IDB_DOWN1                       2020
#define IDB_DOWN2                       2021
#define IDB_LEFT1                       2022
#define IDB_LEFT2                       2023
#define IDB_RIGHT1                      2024
#define IDB_RIGHT2                      2025
#define IDB_TILTUP1                     2026
#define IDB_TILTUP2                     2027
#define IDB_TILTDN1                     2028
#define IDB_TILTDN2                     2029
#define IDB_SPINLF1                     2030
#define IDB_SPINLF2                     2031
#define IDB_SPINRT1                     2032
#define IDB_SPINRT2                     2033
#define IDB_ZIN1                        2034
#define IDB_ZIN2                        2035
#define IDB_ZOUT1                       2036
#define IDB_ZOUT2                       2037


//
//  Menu resource IDs.
//

#define IDM_FRAME_BASE                  IDM_BASE
#define IDM_FRAME                       ID(IDM_FRAME_BASE)

#define IDM_FILE_BASE                   (IDM_FRAME_BASE + 0)
#define IDM_FILE_NEW                    (IDM_FILE_BASE + 1)
#define IDM_FILE_OPEN                   (IDM_FILE_BASE + 2)
#define IDM_FILE_CLOSE                  (IDM_FILE_BASE + 3)
#define IDM_FILE_REBUILD                (IDM_FILE_BASE + 4)
#define IDM_FILE_SAVE                   (IDM_FILE_BASE + 5)
#define IDM_FILE_SAVE_AS                (IDM_FILE_BASE + 6)
#define IDM_FILE_PRINT                  (IDM_FILE_BASE + 7)
#define IDM_FILE_PRINT_SETUP            (IDM_FILE_BASE + 8)
#define IDM_FILE_EXIT                   (IDM_FILE_BASE + 9)

#define IDM_EDIT_BASE                   (IDM_FRAME_BASE + 100)
#define IDM_EDIT_UNDO                   (IDM_EDIT_BASE + 0)
#define IDM_EDIT_CUT                    (IDM_EDIT_BASE + 1)
#define IDM_EDIT_COPY                   (IDM_EDIT_BASE + 2)
#define IDM_EDIT_PASTE                  (IDM_EDIT_BASE + 3)
#define IDM_EDIT_DELETE                 (IDM_EDIT_BASE + 4)
#define IDM_EDIT_HIDE                   (IDM_EDIT_BASE + 5)
#define IDM_EDIT_HIDE_ALL               (IDM_EDIT_BASE + 6)
#define IDM_EDIT_SHOW_ALL               (IDM_EDIT_BASE + 7)
#define IDM_EDIT_CLEAR_SELECTIONS       (IDM_EDIT_BASE + 8)
#define IDM_EDIT_VERTEX_POSITION        (IDM_EDIT_BASE + 9)
#define IDM_EDIT_VERTEX_PROPERTIES      (IDM_EDIT_BASE + 10)
#define IDM_EDIT_POLYGON_PROPERTIES     (IDM_EDIT_BASE + 11)
#define IDM_EDIT_GROUP_ASSIGNMENTS      (IDM_EDIT_BASE + 12)

#define IDM_VIEW_BASE                   (IDM_FRAME_BASE + 200)
#define IDM_VIEW_HOME                   (IDM_VIEW_BASE + 0)
#define IDM_VIEW_OPTIONS                (IDM_VIEW_BASE + 1)
#define IDM_VIEW_STATISTICS             (IDM_VIEW_BASE + 2)
#define IDM_VIEW_FILE_INFO              (IDM_VIEW_BASE + 3)
#define IDM_VIEW_LEVELS                 (IDM_VIEW_BASE + 4)
#define IDM_VIEW_BITMAPS                (IDM_VIEW_BASE + 5)
#define IDM_VIEW_TEXTURES               (IDM_VIEW_BASE + 6)
#define IDM_VIEW_SOUNDS                 (IDM_VIEW_BASE + 7)
#define IDM_VIEW_ANIMATIONS             (IDM_VIEW_BASE + 8)
#define IDM_VIEW_LIFEFORMS              (IDM_VIEW_BASE + 9)
#define IDM_VIEW_ITEMS                  (IDM_VIEW_BASE + 10)
#define IDM_VIEW_SPRITES                (IDM_VIEW_BASE + 11)
#define IDM_VIEW_NOISES                 (IDM_VIEW_BASE + 12)
#define IDM_VIEW_GROUPS                 (IDM_VIEW_BASE + 13)
#define IDM_VIEW_TRIGGERS               (IDM_VIEW_BASE + 14)
#define IDM_VIEW_MOTIONS                (IDM_VIEW_BASE + 15)
#define IDM_VIEW_MARKS                  (IDM_VIEW_BASE + 16)

#define IDM_WINDOW_BASE                 (IDM_FRAME_BASE + 300)
#define IDM_WINDOW_CASCADE              (IDM_WINDOW_BASE + 0)
#define IDM_WINDOW_TILE_VERTICALLY      (IDM_WINDOW_BASE + 1)
#define IDM_WINDOW_TILE_HORIZONTALLY    (IDM_WINDOW_BASE + 2)
#define IDM_WINDOW_ARRANGE_ICONS        (IDM_WINDOW_BASE + 3)
#define IDM_WINDOW_FIRST_CHILD          (IDM_WINDOW_BASE + 4)

#define IDM_HELP_BASE                   (IDM_FRAME_BASE + 400)
#define IDM_HELP_ABOUT                  (IDM_HELP_BASE + 0)


//
//  Dialog resource IDs.
//

#define IDD_ABOUT                       ID(4000)        // IDD_BASE

#define IDD_VIEW_OPTIONS                ID(4100)        // IDD_BASE + 100
#define IDC_VIEW_X                      4101
#define IDC_VIEW_Y                      4102
#define IDC_VIEW_Z                      4103
#define IDC_VIEW_ROTATION               4104
#define IDC_VIEW_ELEVATION              4105
#define IDC_VIEW_HOME_X                 4106
#define IDC_VIEW_HOME_Y                 4107
#define IDC_VIEW_HOME_Z                 4108
#define IDC_VIEW_HOME_ROTATION          4109
#define IDC_VIEW_HOME_ELEVATION         4110
#define IDC_VIEW_DELTA                  4111
#define IDC_GRID_X                      4112
#define IDC_GRID_Y                      4113
#define IDC_GRID_Z                      4114
#define IDC_GRID_ROTATION               4115
#define IDC_GRID_ELEVATION              4116
#define IDC_GRID_DELTA                  4117
#define IDC_GRID_SIZE                   4118
#define IDC_GRID_SPACING                4119
#define IDC_MAINTAIN_GRID_DIST          4120
#define IDC_LOCK_VIEW_TO_GRID           4121
#define IDC_SNAP_TO_GRID                4122

#define IDD_VIEW_MARKS                  ID(4150)        // IDD_BASE + 150
#define IDC_MARK_LIST                   4151
#define IDC_NEW_MARK                    4152
#define IDC_DELETE_MARK                 4153
#define IDC_EDIT_MARK                   4154
#define IDC_GO_TO_MARK                  4155

#define IDD_EDIT_MARK                   ID(4200)        // IDD_BASE + 200
#define IDC_MARK_NAME                   4201
#define IDC_MARK_X                      4202
#define IDC_MARK_Y                      4203
#define IDC_MARK_Z                      4204
#define IDC_MARK_ROTATION               4205
#define IDC_MARK_ELEVATION              4206

#define IDD_FILE_INFO                   ID(4250)        // IDD_BASE + 250
#define IDC_FILE_NAME                   4251
#define IDC_FILE_AUTHOR                 4252
#define IDC_FILE_DATE                   4253
#define IDC_FILE_VERSION                4254

#define IDD_VIEW_BITMAPS                ID(4300)        // IDD_BASE + 300
#define IDC_BITMAP_LIST                 4301
#define IDC_BITMAP_TRANSPARENT          4302
#define IDC_BITMAP_PREVIEW              4303
#define IDC_BITMAP_SIZE                 4304
#define IDC_RELOAD_BITMAP               4305
#define IDC_IMPORT_BITMAP               4306
#define IDC_EXPORT_BITMAP               4307
#define IDC_DELETE_BITMAP               4308

#define IDD_VIEW_GROUPS                 ID(4350)        // IDD_BASE + 350
#define IDC_GROUP_LIST                  4351
#define IDC_NEW_GROUP                   4352
#define IDC_DELETE_GROUP                4353
#define IDC_RENAME_GROUP                4354
#define IDC_HIDE_GROUP                  4355
#define IDC_SHOW_GROUP                  4356

#define IDD_NEW_GROUP                   ID(4400)        // IDD_BASE + 400
#define IDC_NEW_GROUP_NAME              4401

#define IDD_VIEW_TEXTURES               ID(4450)        // IDD_BASE + 450
#define IDC_TEXTURE_LIST                4451
#define IDC_TEXTURE_BITMAP_LIST         4452
#define IDC_TEXTURE_BITMAP_PREVIEW      4453
#define IDC_TEXTURE_BITMAP_SIZE         4454
#define IDC_TEXTURE_PIXEL_SIZE          4455
#define IDC_NEW_TEXTURE                 4456
#define IDC_DELETE_TEXTURE              4457
#define IDC_RENAME_TEXTURE              4458

#define IDD_NEW_TEXTURE                 ID(4500)        // IDD_BASE + 500
#define IDC_NEW_TEXTURE_NAME            4501

#define IDD_POLYGON_PROPERTIES          ID(4550)        // IDD_BASE + 550
#define IDC_POLYGON_LIST_STATIC         4551
#define IDC_POLYGON_LIST                4552
#define IDC_POLYGON_DOUBLE_SIDED        4553
#define IDC_POLYGON_INVISIBLE           4554
#define IDC_POLYGON_WALK_THROUGH        4555
#define IDC_POLYGON_ANIMATED            4556
#define IDC_POLYGON_MOTION_ANIMATED     4557
#define IDC_POLYGON_DAMAGE_ANIMATED     4558
#define IDC_POLYGON_SECRET_PLACE        4559
#define IDC_POLYGON_MOVING_POLYGON      4560
#define IDC_POLYGON_TEXTURE_X_OFFSET    4561
#define IDC_POLYGON_TEXTURE_Y_OFFSET    4562
#define IDC_POLYGON_TEXTURE_DX          4563
#define IDC_POLYGON_TEXTURE_DY          4564
#define IDC_POLYGON_SHOW_TEXTURES       4565
#define IDC_POLYGON_SHOW_ANIMATIONS     4566
#define IDC_POLYGON_SHOW_MOTIONS        4567

#define IDD_VERTEX_POSITION             ID(4600)        // IDD_BASE + 600
#define IDC_VERTEX_X                    4601
#define IDC_VERTEX_Y                    4602
#define IDC_VERTEX_Z                    4603
#define IDC_VERTEX_DX                   4604
#define IDC_VERTEX_DY                   4605
#define IDC_VERTEX_DZ                   4606

#define IDD_VERTEX_PROPERTIES           ID(4650)        // IDD_BASE + 650
#define IDC_VERTEX_LIST_STATIC          4651
#define IDC_VERTEX_LIST                 4652
#define IDC_VERTEX_SPRITE               4653
#define IDC_VERTEX_LIFEFORM             4654
#define IDC_VERTEX_ITEM                 4655
#define IDC_VERTEX_NOISE                4656
#define IDC_VERTEX_TRIGGER              4657
#define IDC_VERTEX_PLAYER_START         4658
#define IDC_VERTEX_45                   4659
#define IDC_VERTEX_90                   4660
#define IDC_VERTEX_180                  4661
#define IDC_VERTEX_DIFFICULTY_LEVEL_1   4662
#define IDC_VERTEX_DIFFICULTY_LEVEL_2   4663
#define IDC_VERTEX_DIFFICULTY_LEVEL_3   4664

#define IDD_VIEW_ANIMATIONS             ID(4700)        // IDD_BASE + 700
#define IDC_ANIMATION_LIST              4701
#define IDC_ANIMATION_RESOURCE_LIST     4702
#define IDC_ANIMATION_BITMAP_PREVIEW    4703
#define IDC_ANIMATION_BITMAP_SIZE       4704
#define IDC_ANIMATION_NUM_ELEMENTS      4705
#define IDC_ANIMATION_DELAY             4706
#define IDC_ANIMATION_PIXEL_SIZE        4707
#define IDC_ANIMATION_NEW               4708
#define IDC_ANIMATION_DELETE            4709
#define IDC_ANIMATION_EDIT              4710

#define IDD_EDIT_ANIMATION              ID(4750)        // IDD_BASE + 750
#define IDC_EDIT_ANIMATION_ELEMENTS     4751
#define IDC_EDIT_ANIMATION_RESOURCES    4752
#define IDC_EDIT_ANIMATION_PREVIEW      4753
#define IDC_EDIT_ANIMATION_BITMAP_SIZE  4754
#define IDC_EDIT_ANIMATION_NUM_ELEMENTS 4755
#define IDC_EDIT_ANIMATION_DELAY        4756
#define IDC_EDIT_ANIMATION_PIXEL_SIZE   4757
#define IDC_EDIT_ANIMATION_NAME         4758
#define IDC_EDIT_ANIMATION_SHOW_BITMAPS 4759
#define IDC_EDIT_ANIMATION_SHOW_SOUNDS  4760
#define IDC_EDIT_ANIMATION_APPEND       4761
#define IDC_EDIT_ANIMATION_INSERT       4762
#define IDC_EDIT_ANIMATION_DELETE       4763

#define IDD_VIEW_LIFEFORMS              ID(4800)        // IDD_BASE + 800
#define IDC_LIFEFORM_LIST               4801
#define IDC_LIFEFORM_RESOURCES          4802
#define IDC_LIFEFORM_BITMAP_PREVIEW     4803
#define IDC_LIFEFORM_BITMAP_SIZE        4804
#define IDC_LIFEFORM_TYPE               4805
#define IDC_LIFEFORM_PIXEL_SIZE         4806
#define IDC_LIFEFORM_NEW                4807
#define IDC_LIFEFORM_DELETE             4808
#define IDC_LIFEFORM_EDIT               4809

#define IDD_EDIT_LIFEFORM               ID(4850)        // IDD_BASE + 850
#define IDC_EDIT_LIFEFORM_ELEMENTS      4851
#define IDC_EDIT_LIFEFORM_RESOURCES     4852
#define IDC_EDIT_LIFEFORM_PREVIEW       4853
#define IDC_EDIT_LIFEFORM_BITMAP_SIZE   4854
#define IDC_EDIT_LIFEFORM_TYPE          4855
#define IDC_EDIT_LIFEFORM_PIXEL_SIZE    4856
#define IDC_EDIT_LIFEFORM_NAME          4857

#define IDD_VIEW_ITEMS                  ID(4900)        // IDD_BASE + 900
#define IDC_ITEM_LIST                   4901
#define IDC_ITEM_RESOURCES              4902
#define IDC_ITEM_BITMAP_PREVIEW         4903
#define IDC_ITEM_BITMAP_SIZE            4904
#define IDC_ITEM_TYPE                   4905
#define IDC_ITEM_PIXEL_SIZE             4906
#define IDC_ITEM_NEW                    4907
#define IDC_ITEM_DELETE                 4908
#define IDC_ITEM_EDIT                   4909

#define IDD_EDIT_ITEM                   ID(4950)        // IDD_BASE + 950
#define IDC_EDIT_ITEM_ELEMENTS          4951
#define IDC_EDIT_ITEM_RESOURCES         4952
#define IDC_EDIT_ITEM_PREVIEW           4953
#define IDC_EDIT_ITEM_BITMAP_SIZE       4954
#define IDC_EDIT_ITEM_TYPE              4955
#define IDC_EDIT_ITEM_PIXEL_SIZE        4956
#define IDC_EDIT_ITEM_NAME              4957

#define IDD_VIEW_TRIGGERS               ID(5000)        // IDD_BASE + 1000
#define IDC_TRIGGER_LIST                5001
#define IDC_TRIGGER_RADIUS              5002
#define IDC_TRIGGER_HEIGHT              5003
#define IDC_TRIGGER_PROXIMITY           5004
#define IDC_TRIGGER_USER                5005
#define IDC_TRIGGER_SOUND               5006
#define IDC_TRIGGER_END_LEVEL           5007
#define IDC_TRIGGER_END_GAME            5008
#define IDC_TRIGGER_ITEM_REQUIRED       5009
#define IDC_TRIGGER_NEW                 5010
#define IDC_TRIGGER_DELETE              5011
#define IDC_TRIGGER_EDIT                5012

#define IDD_EDIT_TRIGGER                ID(5050)        // IDD_BASE + 1050
#define IDC_EDIT_TRIGGER_LIST_STATIC    5051
#define IDC_EDIT_TRIGGER_LIST           5052
#define IDC_EDIT_TRIGGER_NAME           5053
#define IDC_EDIT_TRIGGER_RADIUS         5054
#define IDC_EDIT_TRIGGER_HEIGHT         5055
#define IDC_EDIT_TRIGGER_PROXIMITY      5056
#define IDC_EDIT_TRIGGER_USER           5057
#define IDC_EDIT_TRIGGER_SOUND          5058
#define IDC_EDIT_TRIGGER_END_LEVEL      5059
#define IDC_EDIT_TRIGGER_END_GAME       5060
#define IDC_EDIT_TRIGGER_ITEM_REQUIRED  5061
#define IDC_EDIT_TRIGGER_ITEMS          5062
#define IDC_EDIT_TRIGGER_SOUNDS         5063

#define IDD_VIEW_MOTIONS                ID(5100)        // IDD_BASE + 1100
#define IDC_MOTION_LIST                 5101
#define IDC_MOTION_DX                   5102
#define IDC_MOTION_DY                   5103
#define IDC_MOTION_DZ                   5104
#define IDC_MOTION_NUM_STEPS            5105
#define IDC_MOTION_RETURN_DELAY         5106
#define IDC_MOTION_AUTO_RUN             5107
#define IDC_MOTION_AUTO_RETURN          5108
#define IDC_MOTION_TRIGGER_RETURN       5109
#define IDC_MOTION_START_SOUND          5110
#define IDC_MOTION_RUN_SOUND            5111
#define IDC_MOTION_STOP_SOUND           5112
#define IDC_MOTION_NEW                  5113
#define IDC_MOTION_DELETE               5114
#define IDC_MOTION_EDIT                 5115

#define IDD_EDIT_MOTION                 ID(5150)        // IDD_BASE + 1150
#define IDC_EDIT_MOTION_LIST_STATIC     5151
#define IDC_EDIT_MOTION_LIST            5152
#define IDC_EDIT_MOTION_NAME            5153
#define IDC_EDIT_MOTION_DX              5154
#define IDC_EDIT_MOTION_DY              5155
#define IDC_EDIT_MOTION_DZ              5156
#define IDC_EDIT_MOTION_NUM_STEPS       5157
#define IDC_EDIT_MOTION_RETURN_DELAY    5158
#define IDC_EDIT_MOTION_AUTO_RUN        5159
#define IDC_EDIT_MOTION_AUTO_RETURN     5160
#define IDC_EDIT_MOTION_TRIGGER_RETURN  5161
#define IDC_EDIT_MOTION_START_SOUND     5162
#define IDC_EDIT_MOTION_RUN_SOUND       5163
#define IDC_EDIT_MOTION_STOP_SOUND      5164
#define IDC_EDIT_MOTION_TRIGGERS        5165
#define IDC_EDIT_MOTION_SOUNDS          5166

#define IDD_EDIT_GROUP_ASSIGNMENTS      ID(5200)        // IDD_BASE + 1200
#define IDC_EDIT_GROUP_ASSIGNMENTS_LIST 5201

#define IDD_VIEW_SOUNDS                 ID(5250)        // IDD_BASE + 1250
#define IDC_SOUND_LIST                  5251
#define IDC_SOUND_LENGTH                5252
#define IDC_PLAY_SOUND                  5253
#define IDC_RELOAD_SOUND                5254
#define IDC_IMPORT_SOUND                5255
#define IDC_EXPORT_SOUND                5256
#define IDC_DELETE_SOUND                5257

#define IDD_VIEW_LEVELS                 ID(5300)        // IDD_BASE + 1300
#define IDC_LEVEL_LIST                  5301
#define IDC_NEW_LEVEL                   5302
#define IDC_IMPORT_LEVEL                5303
#define IDC_CUT_LEVEL                   5304
#define IDC_COPY_LEVEL                  5305
#define IDC_PASTE_LEVEL                 5306
#define IDC_DELETE_LEVEL                5307
#define IDC_EDIT_LEVEL                  5308

#define IDD_EDIT_LEVEL                  ID(5350)        // IDD_BASE + 1350
#define IDC_EDIT_LEVEL_TEXT_STATIC      5351
#define IDC_EDIT_LEVEL_TEXT             5352
#define IDC_EDIT_LEVEL_LIST_STATIC      5353
#define IDC_EDIT_LEVEL_LIST             5354
#define IDC_EDIT_LEVEL_BITMAP_PREVIEW   5355
#define IDC_EDIT_LEVEL_BITMAP_SIZE      5356
#define IDC_EDIT_LEVEL_SOL_TEXT         5357
#define IDC_EDIT_LEVEL_EOL_TEXT         5358
#define IDC_EDIT_LEVEL_BKG_BITMAP       5359
#define IDC_EDIT_LEVEL_SOL_BITMAP       5360
#define IDC_EDIT_LEVEL_EOL_BITMAP       5361
#define IDC_EDIT_LEVEL_NAME             5362
#define IDC_EDIT_LEVEL_SHOW_BITMAPS     5363
#define IDC_EDIT_LEVEL_SHOW_TEXT        5364

#define IDD_SELECT_LEVEL                ID(5400)        // IDD_BASE + 1400
#define IDC_SELECT_LEVEL_LIST           5401

#define IDD_VIEW_SPRITES                ID(5450)        // IDD_BASE + 1450
#define IDC_SPRITE_LIST                 5451
#define IDC_SPRITE_TYPE_STATIC          5452
#define IDC_SPRITE_TYPE_NAME            5453
#define IDC_SPRITE_BITMAP_PREVIEW       5454
#define IDC_SPRITE_BITMAP_SIZE          5455
#define IDC_SPRITE_PIXEL_SIZE           5456
#define IDC_SPRITE_HANGING              5457
#define IDC_SPRITE_WALK_THROUGH         5458
#define IDC_SPRITE_ANIMATED             5459
#define IDC_SPRITE_DAMAGE_ANIMATED      5460
#define IDC_SPRITE_DAMAGE_ANIMATED_2    5461
#define IDC_SPRITE_DESTRUCTABLE         5462
#define IDC_SPRITE_NEW                  5463
#define IDC_SPRITE_DELETE               5464
#define IDC_SPRITE_EDIT                 5465

#define IDD_EDIT_SPRITE                 ID(5500)        // IDD_BASE + 1500
#define IDC_EDIT_SPRITE_LIST_STATIC     5501
#define IDC_EDIT_SPRITE_LIST            5502
#define IDC_EDIT_SPRITE_NAME            5503
#define IDC_EDIT_SPRITE_BITMAP_PREVIEW  5504
#define IDC_EDIT_SPRITE_BITMAP_SIZE     5505
#define IDC_EDIT_SPRITE_PIXEL_SIZE      5506
#define IDC_EDIT_SPRITE_HANGING         5507
#define IDC_EDIT_SPRITE_WALK_THROUGH    5508
#define IDC_EDIT_SPRITE_ANIMATED        5509
#define IDC_EDIT_SPRITE_DAMAGE_ANIMATED 5510
#define IDC_EDIT_SPRITE_DAMAGE_ANIMATED_2 5511
#define IDC_EDIT_SPRITE_DESTRUCTABLE    5512
#define IDC_EDIT_SPRITE_SHOW_BITMAPS    5513
#define IDC_EDIT_SPRITE_SHOW_ANIMATIONS 5514

#define IDD_VIEW_NOISES                 ID(5550)        // IDD_BASE + 1550
#define IDC_NOISE_LIST                  5551
#define IDC_NOISE_RADIUS                5552
#define IDC_NOISE_HEIGHT                5553
#define IDC_NOISE_DELAY                 5554
#define IDC_NOISE_NEW                   5555
#define IDC_NOISE_DELETE                5556
#define IDC_NOISE_EDIT                  5557

#define IDD_EDIT_NOISE                  ID(5600)        // IDD_BASE + 1600
#define IDC_EDIT_NOISE_LIST             5601
#define IDC_EDIT_NOISE_NAME             5602
#define IDC_EDIT_NOISE_RADIUS           5603
#define IDC_EDIT_NOISE_HEIGHT           5604
#define IDC_EDIT_NOISE_DELAY            5605

#define IDD_VIEW_STATISTICS             ID(5650)        // IDD_BASE + 1650
#define IDC_STATISTICS_LIST             5651


//
//  Accelerator table resource IDs.
//

#define IDA_FRAME                       ID(IDA_BASE)


//
//  String table resource IDs.
//

#define IDS_VERSION                     IDS_BASE
#define IDS_NO                          IDS_BASE+1
#define IDS_YES                         IDS_BASE+2

#endif  // _VED_CONS_H_
