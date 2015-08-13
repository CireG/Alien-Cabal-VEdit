/*******************************************************************

    vedit.h

    Master include file for the VEDIT project.

********************************************************************/


#ifndef _VEDIT_H_
#define _VEDIT_H_


//
//  System include files.
//

#include <windows.h>
#include <shellapi.h>
#define _INC_SHELLAPI
#include <windowsx.h>
#include <dlgs.h>

#ifndef RC_INVOKED

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <dos.h>
#include <ctype.h>
#include <direct.h>
#include <io.h>
#include <assert.h>
#include <commdlg.h>
#include <limits.h>
//#include "resource.h"

#endif  // RC_INVOKED


//
//  Local include files.
//

#include "ved_cons.h"
#include "ved_type.h"
#include "ved_data.h"
#include "ved_proc.h"


#endif  // _VEDIT_H_
