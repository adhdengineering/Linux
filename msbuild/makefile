
CXX  = g++
GCC = gcc
LINK = $(CXX)

ARCH = x86_64

ifndef VERBOSE
VERBOSE=0
endif



EXTRAFLAGS=

INCLUDE= -I. 

CPPFLAGS = 
CFLAGS = 
LFLAGS = 
ifeq ($(ARCH),x86_64)
        CPPFLAGS += -m64
	CFLAGS += -m64
	LFLAGS += -m64
else
        CPPFLAGS += -m32
	CFLAGS += -m32
	LFLAGS += -m32
endif

CPPFLAGS += -std=c++11 $(EXTRAFLAGS) -fPIC $(INCLUDE)

CFLAGS += $(EXTRAFLAGS) -fPIC

LFLAGS += $(EXTRAFLAGS) -Wl,--no-undefined

ifdef DEBUG_COMPILE
  CPPFLAGS += -g -O0
  CFLAGS += -g -O0
endif

LIBDIRS = 
          
LIBS    = 

## Source and object file list
SRC = 	CustomBuildFile.cpp \
	StringReader.cpp \
	Forker.cpp \
	WorkingDirectory.cpp \
	StringHelper.cpp \
	SolutionFile.cpp \
	ProjectConfigurationOptions.cpp \
	ProjectFileParser.cpp \
	ProjectBuilder.cpp \
	CLIOptions.cpp \
	main.cpp \
	Project.cpp \



OBJStemp = $(SRC:%.cpp=%.o)
OBJS = $(OBJStemp:%.CPP=%.O)

TRIMOBJS=$(foreach filename, $(OBJS), $(OBJ_DIR)/$(shell echo $(filename) | grep -o "[^/]*$$"))

## Rules
all: output_settings msbuild

msbuild: $(OBJS)
	$(LINK) $(EXTRAFLAGS) $(LFLAGS) $(OBJS) $(LIBDIRS) $(LIBS) -o msbuild


## Forces the compilation to the object directory
%.o: %.cpp
	@echo "Compiling $< ..."
	@if [ $(VERBOSE) -eq 1 ] ; then \
		echo $(CXX) $(CPPFLAGS) -c $< -o $@ ; \
	fi ; \
	$(CXX) $(CPPFLAGS) -c $< -o $@ 

%.O: %.CPP
	@echo "Compiling $< ..."
	@if [ $(VERBOSE) -eq 1 ] ; then \
		echo $(CXX) $(CPPFLAGS) -c $< -o $@ ; \
	fi ; \
	$(CXX) $(CPPFLAGS) -c $< -o $@ 

clean:
	@echo "Cleaning..."
	@echo "Removing $(OBJS)"
	rm -rf $(OBJS)
	@echo
	@echo "Removing msbuild"
	rm -rf msbuild

output_settings:
	@echo "**** msbuild ****"
	@echo "==== Settings ============================================"
	@echo "--- File List --------------------------------------------"
	@eval $(foreach obj,$(SRC), echo $(obj);)
	@echo "--- Include List -----------------------------------------"
	@eval $(foreach inc,$(INCLUDE), echo $(inc);)
	@echo "--- Compiler Flags ---------------------------------------"
	@echo $(CPPFLAGS)
	@echo "--- Linker Flags -----------------------------------------"
	@echo $(LFLAGS)
	@echo "--- Target Filename --------------------------------------"
	@echo $(TARGET_FN)
	@echo "--- Shared Library Name ----------------------------------"
	@echo $(LIB_NAME)
	@echo "=========================================================="
	@echo
