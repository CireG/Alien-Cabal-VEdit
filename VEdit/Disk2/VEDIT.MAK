# Microsoft Visual C++ generated build script - Do not modify

PROJ = VEDIT
DEBUG = 0
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\VEDIT\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = DIALOGS.C   
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /Gy /W3 /Zi /AL /Od /D "_DEBUG" /FR /GA /Fd"VEDIT.PDB"
CFLAGS_R_WEXE = /nologo /G3 /W3 /AL /O2 /D "NDEBUG" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /NOD /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE /CO  
LFLAGS_R_WEXE = /NOLOGO /NOD /PACKC:61440 /ALIGN:16 /ONERROR:NOEXE  
LIBS_D_WEXE = oldnames libw llibcew commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WEXE = oldnames libw llibcew commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo 
RESFLAGS = /nologo /k
RUNFLAGS = 
DEFFILE = VEDIT.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = DIALOGS.SBR \
		FRAME.SBR \
		GLOBALS.SBR \
		INIT.SBR \
		UTILITY.SBR \
		VEDIT.SBR \
		WND.SBR \
		OBJWND.SBR \
		READ.SBR \
		WRITE.SBR \
		OPTDLG.SBR \
		BMPDLG.SBR \
		GRPDLG.SBR \
		PLYDLG.SBR \
		OBJDATA.SBR \
		VRTXDLG.SBR \
		ANIMDLG.SBR \
		LFRMDLG.SBR \
		FILEDLG.SBR \
		ITEMDLG.SBR \
		TRIGDLG.SBR \
		MDEFDLG.SBR \
		TXTRDLG.SBR \
		SNDDLG.SBR \
		LVLDLG.SBR \
		SPRTDLG.SBR \
		NOISEDLG.SBR \
		MRKDLG.SBR \
		STATSDLG.SBR


DIALOGS_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


FRAME_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


GLOBALS_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


INIT_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


UTILITY_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


VEDIT_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


WND_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


VEDIT_RCDEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h \
	c:\vedit\frame.ico \
	c:\vedit\object.ico \
	c:\vedit\up1.bmp \
	c:\vedit\up2.bmp \
	c:\vedit\down1.bmp \
	c:\vedit\down2.bmp \
	c:\vedit\right1.bmp \
	c:\vedit\left1.bmp \
	c:\vedit\left2.bmp \
	c:\vedit\right2.bmp \
	c:\vedit\tiltdn1.bmp \
	c:\vedit\tiltdn2.bmp \
	c:\vedit\tiltup1.bmp \
	c:\vedit\tiltup2.bmp \
	c:\vedit\spinrt1.bmp \
	c:\vedit\spinrt2.bmp \
	c:\vedit\spinlf1.bmp \
	c:\vedit\zin1.bmp \
	c:\vedit\zout1.bmp \
	c:\vedit\zin2.bmp \
	c:\vedit\zout2.bmp \
	c:\vedit\spinlf2.bmp \
	c:\vedit\ptr1.bmp \
	c:\vedit\ptr2.bmp \
	c:\vedit\selvert1.bmp \
	c:\vedit\selvert2.bmp \
	c:\vedit\selpoly1.bmp \
	c:\vedit\selpoly2.bmp \
	c:\vedit\selgrp1.bmp \
	c:\vedit\selgrp2.bmp \
	c:\vedit\newvert1.bmp \
	c:\vedit\newvert2.bmp \
	c:\vedit\newpoly1.bmp \
	c:\vedit\newpoly2.bmp \
	c:\vedit\rotpoly1.bmp \
	c:\vedit\rotpoly2.bmp \
	c:\vedit\flppoly1.bmp \
	c:\vedit\flppoly2.bmp \
	c:\vedit\lnkpoly1.bmp \
	c:\vedit\lnkpoly2.bmp


OBJWND_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


READ_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


WRITE_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


OPTDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


BMPDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


GRPDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


PLYDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


OBJDATA_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


VRTXDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


ANIMDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


LFRMDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


FILEDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


ITEMDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


TRIGDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


MDEFDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


TXTRDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


SNDDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


LVLDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


SPRTDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


NOISEDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


MRKDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


STATSDLG_DEP = c:\vedit\vedit.h \
	c:\vedit\ved_cons.h \
	c:\vedit\ved_type.h \
	c:\vedit\ved_data.h \
	c:\vedit\ved_proc.h


all:	$(PROJ).EXE $(PROJ).BSC

DIALOGS.OBJ:	DIALOGS.C $(DIALOGS_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c DIALOGS.C

FRAME.OBJ:	FRAME.C $(FRAME_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FRAME.C

GLOBALS.OBJ:	GLOBALS.C $(GLOBALS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c GLOBALS.C

INIT.OBJ:	INIT.C $(INIT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c INIT.C

UTILITY.OBJ:	UTILITY.C $(UTILITY_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c UTILITY.C

VEDIT.OBJ:	VEDIT.C $(VEDIT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c VEDIT.C

WND.OBJ:	WND.C $(WND_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c WND.C

VEDIT.RES:	VEDIT.RC $(VEDIT_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r VEDIT.RC

OBJWND.OBJ:	OBJWND.C $(OBJWND_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c OBJWND.C

READ.OBJ:	READ.C $(READ_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c READ.C

WRITE.OBJ:	WRITE.C $(WRITE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c WRITE.C

OPTDLG.OBJ:	OPTDLG.C $(OPTDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c OPTDLG.C

BMPDLG.OBJ:	BMPDLG.C $(BMPDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c BMPDLG.C

GRPDLG.OBJ:	GRPDLG.C $(GRPDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c GRPDLG.C

PLYDLG.OBJ:	PLYDLG.C $(PLYDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c PLYDLG.C

OBJDATA.OBJ:	OBJDATA.C $(OBJDATA_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c OBJDATA.C

VRTXDLG.OBJ:	VRTXDLG.C $(VRTXDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c VRTXDLG.C

ANIMDLG.OBJ:	ANIMDLG.C $(ANIMDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ANIMDLG.C

LFRMDLG.OBJ:	LFRMDLG.C $(LFRMDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LFRMDLG.C

FILEDLG.OBJ:	FILEDLG.C $(FILEDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c FILEDLG.C

ITEMDLG.OBJ:	ITEMDLG.C $(ITEMDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ITEMDLG.C

TRIGDLG.OBJ:	TRIGDLG.C $(TRIGDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c TRIGDLG.C

MDEFDLG.OBJ:	MDEFDLG.C $(MDEFDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MDEFDLG.C

TXTRDLG.OBJ:	TXTRDLG.C $(TXTRDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c TXTRDLG.C

SNDDLG.OBJ:	SNDDLG.C $(SNDDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SNDDLG.C

LVLDLG.OBJ:	LVLDLG.C $(LVLDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c LVLDLG.C

SPRTDLG.OBJ:	SPRTDLG.C $(SPRTDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SPRTDLG.C

NOISEDLG.OBJ:	NOISEDLG.C $(NOISEDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c NOISEDLG.C

MRKDLG.OBJ:	MRKDLG.C $(MRKDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MRKDLG.C

STATSDLG.OBJ:	STATSDLG.C $(STATSDLG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c STATSDLG.C


$(PROJ).EXE::	VEDIT.RES

$(PROJ).EXE::	DIALOGS.OBJ FRAME.OBJ GLOBALS.OBJ INIT.OBJ UTILITY.OBJ VEDIT.OBJ WND.OBJ \
	OBJWND.OBJ READ.OBJ WRITE.OBJ OPTDLG.OBJ BMPDLG.OBJ GRPDLG.OBJ PLYDLG.OBJ OBJDATA.OBJ \
	VRTXDLG.OBJ ANIMDLG.OBJ LFRMDLG.OBJ FILEDLG.OBJ ITEMDLG.OBJ TRIGDLG.OBJ MDEFDLG.OBJ \
	TXTRDLG.OBJ SNDDLG.OBJ LVLDLG.OBJ SPRTDLG.OBJ NOISEDLG.OBJ MRKDLG.OBJ STATSDLG.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
DIALOGS.OBJ +
FRAME.OBJ +
GLOBALS.OBJ +
INIT.OBJ +
UTILITY.OBJ +
VEDIT.OBJ +
WND.OBJ +
OBJWND.OBJ +
READ.OBJ +
WRITE.OBJ +
OPTDLG.OBJ +
BMPDLG.OBJ +
GRPDLG.OBJ +
PLYDLG.OBJ +
OBJDATA.OBJ +
VRTXDLG.OBJ +
ANIMDLG.OBJ +
LFRMDLG.OBJ +
FILEDLG.OBJ +
ITEMDLG.OBJ +
TRIGDLG.OBJ +
MDEFDLG.OBJ +
TXTRDLG.OBJ +
SNDDLG.OBJ +
LVLDLG.OBJ +
SPRTDLG.OBJ +
NOISEDLG.OBJ +
MRKDLG.OBJ +
STATSDLG.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
c:\msvc\lib\+
c:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) VEDIT.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	VEDIT.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) VEDIT.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
