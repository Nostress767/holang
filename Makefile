#######################
# Main configurations #
#######################

INCDIR := include
SRCDIR := src
EXTDIR := external

CFLAGS   := -Werror -Wall -Wpedantic -std=c2x
CPPFLAGS := -I$(INCDIR) -finput-charset=UTF-8 -MMD
LDFLAGS  := 

ifeq ($(OS),Windows_NT)
	CPPFLAGS += -D_GLFW_WIN32
	LDFLAGS  += -lgdi32
else
	CPPFLAGS += -D_GLFW_WAYLAND -D_GLFW_X11 -D_POSIX_C_SOURCE=200809L
	LDFLAGS  += -lm
endif

ifdef DEBUG
	TARGET   := debug
	CPPFLAGS += -DDEBUG
	CFLAGS   += -fPIC
	CFLAGS   += -ggdb
	CFLAGS   += -Og
else
	TARGET   := release
	CFLAGS   += -c
	CFLAGS   += -O3
	ifeq ($(OS),Windows_NT)
		LDFLAGS  += -mwindows
	endif
endif

OBJDIR := obj/$(TARGET)

CC  := gcc
EXE := target/$(TARGET)/Main.exe

# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables

#######################

#############
# Functions #
#############

# Convert 1° arg suffix to 2° arg suffix
define to_suffix
	$(addsuffix $(2),$(basename $(1)))
endef

# Get 1° arg extension files from 2° arg base folder
define get_suffix
	$(call to_suffix,$(addprefix $(OBJDIR)/,$(2)),$(1))
endef

# Compile recipe with arg flags
define create_src_obj =
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(1) $< -MF $(basename $@).d -o $@
endef

#############

# Get all source files recursively
SRC    := $(shell find $(SRCDIR)/ -type f -name "*.c")
EXTSRC := $(shell find $(SRCDIR)/$(EXTDIR)/ -type f -name "*.c")
ifdef DEBUG
	EXTERNALOBJS := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(filter-out $(SRCDIR)/$(notdir $(basename $(EXE))).c,$(EXTSRC))))
	INTERNALOBJS := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.dll,$(filter-out $(SRCDIR)/$(notdir $(basename $(EXE))).c,$(filter-out $(EXTSRC),$(SRC)))))
else
	SRCOBJS      := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SRC)))
endif
MAKEFILES   := $(addsuffix .d,$(basename $(SRCOBJS)))

###########
# Targets #
###########

# Default target
all: $(EXE)

.PHONY: tests
# Test target
tests:
	make -C tests clean all

.PHONY: tests-report
tests-report:
	make -C tests report

# Compile sources
$(call get_suffix,.o  ,$(SRC)) : $(OBJDIR)/%.o : %.c
	$(call create_src_obj,-c)

$(call get_suffix,.dll,$(SRC)) : $(OBJDIR)/%.dll : %.c
	$(call create_src_obj,-shared)

## Compile executable

ifdef DEBUG
dlls_only: $(INTERNALOBJS)

$(EXE): $(EXTERNALOBJS) $(INTERNALOBJS) $(OBJDIR)/$(SRCDIR)/$(notdir $(basename $(EXE))).o
	@mkdir -p target/$(TARGET)
	$(CC) $(EXTERNALOBJS) -o $@ $(OBJDIR)/$(SRCDIR)/$(notdir $(basename $@)).o $(LDFLAGS)
else
$(EXE): $(SRCOBJS)
	@mkdir -p target/$(TARGET)
	$(CC) $(SRCOBJS) -o $@ $(LDFLAGS)
endif
##

clean:
	-rm -rf $(EXE) $(OBJDIR)

###########

########################################
# Automatic makefile generation (-MMD) #
########################################

ifneq ($(MAKECMDGOALS),clean)
$(MAKEFILES): ;
-include $(MAKEFILES)
endif

########################################
