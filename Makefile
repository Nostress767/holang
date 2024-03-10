#######################
# Main configurations #
#######################

INCDIR := include
SRCDIR := src

CFLAGS   := -Werror -Wall -Wpedantic -std=c2x
CPPFLAGS := -I$(INCDIR) -finput-charset=UTF-8 -MMD
LDFLAGS  := 
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

# Compile recipe with 1° arg compiler and 2° arg flags
define create_src_obj =
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(1) $< -MF $(basename $@).d -o $@
endef

#############

# Get all source files recursively
SRC         := $(shell find $(SRCDIR)/ -type f)
ifdef DEBUG
	SRCOBJS     := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.dll,$(filter-out $(SRCDIR)/$(notdir $(basename $(EXE))).c,$(SRC))))
else
	SRCOBJS     := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SRC)))
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
dlls_only: $(SRCOBJS)

$(EXE): $(SRCOBJS) $(OBJDIR)/$(SRCDIR)/$(notdir $(basename $(EXE))).o
	@mkdir -p target/$(TARGET)
	$(CC) -o $@ $(OBJDIR)/$(SRCDIR)/$(notdir $(basename $@)).o $(LDFLAGS)
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
