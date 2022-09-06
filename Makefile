# This makefile was originally generated by C/C++ Project Generator Extension for VSCode made by danielpinto8zz6
# It was then modified by me (Patryk Pochmara) to suit my personal needs for this project

NAME = Rubik

# general settings
CXX := g++

CFLAGS	:= -std=c++17 -Wall -Wextra -Wpedantic
DEBUG_CFLAGS	:= -D_DEBUG -g -Og
RELEASE_CFLAGS	:= -O2

LFLAGS	:=
STATIC_LFLAGS	:= -static-libgcc -static-libstdc++ -static

LPATHS	:=
DEBUG_LPATHS	:=
RELEASE_LPATHS	:=

# linking mode
LMODE	?= dynamic
ifeq ($(LMODE),static)
$(warning ---------------------------------- WARNING! ----------------------------------)
$(warning If linker errors appear when static linking, 'make clean' first and then retry)
$(warning ---------------------------------- WARNING! ----------------------------------)
LFLAGS	+= $(STATIC_LFLAGS)
else
ifneq ($(LMODE),dynamic)
$(error Incorrect linking mode - it must be either 'static' or 'dynamic')
endif
endif

# raylib
RELEASE_CFLAGS	+= -mwindows
ifeq ($(LMODE),static)
DEBUG_LPATHS	+= -lraylib-s-d
RELEASE_LPATHS	+= -lraylib-s
else
DEBUG_LPATHS	+= -lraylib-d
RELEASE_LPATHS	+= -lraylib
endif
RAYLIB_EXT_LPATHS	:= -lopengl32 -lgdi32 -lwinmm
DEBUG_LPATHS	+= $(RAYLIB_EXT_LPATHS)
RELEASE_LPATHS	+= $(RAYLIB_EXT_LPATHS)

# directories
DEBUG_DIR	:= debug
RELEASE_DIR	:= release
OBJECTS_SUBDIR	:= objects

SRC_DIR	:= src
INC_DIR	:= include
LIB_DIR	:= lib

# shell setup
FIXPATH = $(subst /,\,$1)
RM	:= del /q /f
RM_ERR_SILENCER	:= 2>NUL

# preparing arguments
INCLUDES	:= $(patsubst %,-I%, $(INC_DIR:%/=%))
LIBS	:= $(patsubst %,-L%, $(LIB_DIR:%/=%))

# finding files
MAIN	:= $(NAME).exe
SOURCES	:= $(wildcard $(patsubst %,%/*.cpp, $(SRC_DIR)))
DEBUG_OBJECTS	:= $(patsubst $(SRC_DIR)%,$(DEBUG_DIR)/$(OBJECTS_SUBDIR)%, $(SOURCES:.cpp=.o))
RELEASE_OBJECTS	:= $(patsubst $(SRC_DIR)%,$(RELEASE_DIR)/$(OBJECTS_SUBDIR)%, $(SOURCES:.cpp=.o))

# targets
.PHONY: debug release clean cleanup

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: LFLAGS += $(DEBUG_LFLAGS)
debug: LPATHS += $(DEBUG_LPATHS)
debug: $(DEBUG_OBJECTS)
	@echo Linking $(MAIN)...
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $(DEBUG_DIR)/$(MAIN) $(DEBUG_OBJECTS) $(LFLAGS) $(LPATHS) $(LIBS)
	@echo Debug build sucessful!

release: CFLAGS += $(RELEASE_CFLAGS)
release: LFLAGS += $(RELEASE_LFLAGS)
release: LPATHS += $(RELEASE_LPATHS)
release: $(RELEASE_OBJECTS)
	@echo Linking $(NAME) executable...
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $(RELEASE_DIR)/$(MAIN) $(RELEASE_OBJECTS) $(LFLAGS) $(LPATHS) $(LIBS)
	@echo Release build sucessful!

$(DEBUG_DIR)/$(OBJECTS_SUBDIR)/%.o : $(SRC_DIR)/%.cpp
	@echo Compiling $(<F)...
	@$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(RELEASE_DIR)/$(OBJECTS_SUBDIR)/%.o : $(SRC_DIR)/%.cpp
	@echo Compiling $(<F)...
	@$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean: cleanup
	@echo Deleting executables...
	@$(RM) $(call FIXPATH,$(DEBUG_DIR)/$(MAIN)) $(call FIXPATH,$(RELEASE_DIR)/$(MAIN)) $(RM_ERR_SILENCER)
	@echo Executables deleted.

cleanup:
	@echo Deleting object files...
	@$(RM) $(call FIXPATH,$(DEBUG_OBJECTS)) $(call FIXPATH,$(RELEASE_OBJECTS)) $(RM_ERR_SILENCER)
	@echo Object files deleted.
