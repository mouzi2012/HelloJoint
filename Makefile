SHELL       = /bin/sh
VARIANT      = debug
ifeq "$(VARIANT)" "debug"
    GCC_DEBUG_FLAGS += -g -D_DEBUG
endif

M64 = -m32
ARCH = x86
ifeq "$(M64)" "-m64"
    ARCH=x64
endif

LD = g++ 
LIBDIR      = /usr/lib/gcc4/$(ARCH)/$(VARIANT)
INCDIR      = /usr/include

CXXFLAGS    = $(M64) $(GCC_DEBUG_FLAGS) -Wall
LDFLAGS     = $(M64) $(GCC_LINK_FLAGS)

CPPFLAGS = -I./
RPATH_DEF    =
STATIC_LINK  =
FBXSDK_LIB   = -lfbxsdk

ifeq "$(STATIC_LINK)" ""
    CXXFLAGS += -DFBXSDK_SHARED
    RPATH_DEF = -Wl,-rpath $(LIBDIR)
else
    FBXSDK_LIB = $(LIBDIR)/libfbxsdk.a
endif

INCLUDEPATH = /mbackup/mouzi/work/cxx/fbx_converter/glm
LIBS = $(FBXSDK_LIB) -lm -lrt -luuid -lstdc++ -lpthread -ldl

OBJS        = main.o AniBone.o AniVertex.o AniMaterial.o DataReader.o


firstFBX: $(OBJS)
	$(LD) $(CPPFLAGS) $(LDFLAGS) -o firstFBX $(OBJS) -L$(LIBDIR) $(LIBS) $(RPATH_DEF)


.PHONY : clean
clean :
	-rm firstFBX $(OBJS)




