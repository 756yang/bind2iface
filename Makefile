##############################################################################
##
##  API Extention to Bind interface for winsock.
##
##  Copyright (c) Yanglan Network.  All rights reserved.
##

!include .\common.mak

LIBS=$(LIBS) kernel32.lib iphlpapi.lib ws2_32.lib

##############################################################################

all: dirs \
    $(BIND)\bind2iface$(DETOURS_BITS).dll \
    $(BIND)\test.exe \
    \
!IF $(DETOURS_SOURCE_BROWSING)==1
    $(OBJD)\bind2iface$(DETOURS_BITS).bsc \
    $(OBJD)\test.bsc \
!ENDIF
    option

##############################################################################

dirs:
    @if not exist $(BIND) mkdir $(BIND) && echo.   Created $(BIND)
    @if not exist $(OBJD) mkdir $(OBJD) && echo.   Created $(OBJD)

$(OBJD)\bind2iface.obj : bind2iface.cpp

$(OBJD)\bind2iface.res : bind2iface.rc

$(BIND)\bind2iface$(DETOURS_BITS).dll $(BIND)\bind2iface$(DETOURS_BITS).lib: \
        $(OBJD)\bind2iface.obj $(OBJD)\bind2iface.res $(DEPS)
    cl /LD $(CFLAGS) /Fe$(@R).dll /Fd$(@R).pdb \
        $(OBJD)\bind2iface.obj $(OBJD)\bind2iface.res \
        /link $(LINKFLAGS) /subsystem:console \
        /export:DetourFinishHelperProcess,@1,NONAME \
        /export:bifbind \
        /export:bifconnect \
        /export:bifsendto \
        /export:bifWSAConnect \
        /export:bifWSASendTo \
!IF 0==1
        /export:bifWSAConnectByList \
!ENDIF
        /export:bifWSAConnectByNameA \
        /export:bifWSAConnectByNameW \
        $(LIBS)

$(OBJD)\bind2iface$(DETOURS_BITS).bsc : $(OBJD)\bind2iface.obj
    bscmake /v /n /o $@ $(OBJD)\bind2iface.sbr

$(OBJD)\test.obj : test.cpp

$(BIND)\test.exe : $(OBJD)\test.obj $(DEPS)
    cl $(CFLAGS) /Fe$@ /Fd$(@R).pdb $(OBJD)\test.obj \
        /link $(LINKFLAGS) $(LIBS) \
        /subsystem:console

$(OBJD)\test.bsc : $(OBJD)\test.obj
    bscmake /v /n /o $@ $(OBJD)\test.sbr

##############################################################################

clean:
    -del *~ 2>nul
    -del $(BIND)\bind2iface*.* 2>nul
    -del $(BIND)\test.* 2>nul
    -rmdir /q /s $(OBJD) 2>nul

realclean: clean
    -rmdir /q /s $(OBJDS) 2>nul

############################################### Install non-bit-size binaries.

!IF "$(DETOURS_OPTION_PROCESSOR)" != ""

$(OPTD)\bind2iface$(DETOURS_OPTION_BITS).dll:
$(OPTD)\bind2iface$(DETOURS_OPTION_BITS).pdb:

$(BIND)\bind2iface$(DETOURS_OPTION_BITS).dll : $(OPTD)\bind2iface$(DETOURS_OPTION_BITS).dll
    @if exist $? copy /y $? $(BIND) >nul && echo $@ copied from $(DETOURS_OPTION_PROCESSOR).
$(BIND)\bind2iface$(DETOURS_OPTION_BITS).pdb : $(OPTD)\bind2iface$(DETOURS_OPTION_BITS).pdb
    @if exist $? copy /y $? $(BIND) >nul && echo $@ copied from $(DETOURS_OPTION_PROCESSOR).

option: \
    $(BIND)\bind2iface$(DETOURS_OPTION_BITS).dll \
    $(BIND)\bind2iface$(DETOURS_OPTION_BITS).pdb \

!ELSE

option:

!ENDIF

##############################################################################

test: all
    set SOCK_BIND_IFACE_INDEX=-1
    set SOCK_BIND_IFACE_INDEX_V6=-1
    @echo -------- Should not load bind2iface$(DETOURS_BITS).dll --------------------------------------
    $(BIND)\test.exe www.baidu.com 80
    @echo.
    @echo -------- Should load bind2iface$(DETOURS_BITS).dll dynamically using withdll.exe----------
    $(LIBD)\withdll.exe -d:$(BIND)\bind2iface$(DETOURS_BITS).dll $(BIND)\test.exe www.baidu.com 80
    @echo.

debug: all
    set SOCK_BIND_IFACE_INDEX=-1
    set SOCK_BIND_IFACE_INDEX_V6=-1
    windbg -o $(LIBD)\withdll.exe -d:$(BIND)\bind2iface$(DETOURS_BITS).dll $(BIND)\test.exe www.baidu.com 80


################################################################# End of File.
