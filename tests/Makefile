#######################
# Main configurations #
#######################

INCDIR     := ../include
SRCDIR     := ../src
TESTSRCDIR := test

CFLAGS   := -fprofile-abs-path -fprofile-arcs -ftest-coverage -fanalyzer -Werror -Wall -Wpedantic -std=c2x -c -ggdb -O0
CPPFLAGS := -DUNITY_OUTPUT_COLOR -include external/unity.h -include external/unity_memory.h -I$(INCDIR) -I. -finput-charset=UTF-8 -MMD
LDFLAGS  := -lgcov

ifneq ($(OS),Windows_NT)
	CFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
endif

OBJDIR := obj

CC  := gcc
EXE := all_tests.exe

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

define create_src_obj =
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -MF $(basename $@).d -o $@
endef

#############

# Get all source files recursively
SRC         := $(subst ../,,$(shell find $(SRCDIR)/ -type f))
TESTSRC     := $(filter %c, $(shell find $(TESTSRCDIR)/ -type f)) $(subst ../,,$(filter %c, $(shell find external -type f)))
SRCOBJS     := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(filter-out src/Main.c,$(SRC)) $(TESTSRC)))
MAKEFILES   := $(addsuffix .d,$(basename $(SRCOBJS)))

###########
# Targets #
###########

.PHONY: all
# Default target
all: $(EXE)
	-@rm -f $(shell find . -type f -name "*.gcda")
	-@./$(EXE) -v

# Report code coverage
.PHONY: report
report:
	gcov -a -b -c -f -p -H -r $(patsubst %.o,%.c,$(SRCOBJS))
	@mkdir -p gcovr-report
	gcovr --root .. --filter ../include/ --filter ../src/ --html --html-details --html-theme github.dark-green --html-syntax-highlighting --output gcovr-report/coverage.html

# Compile sources
$(call get_suffix,.o  ,$(SRC)) : $(OBJDIR)/%.o : ../%.c
	$(create_src_obj)

$(call get_suffix,.o  ,$(TESTSRC)) : $(OBJDIR)/%.o : %.c
	$(create_src_obj)

## Compile executable

$(EXE): $(SRCOBJS)
	@mkdir -p obj
	$(CC) $(SRCOBJS) -o $@ $(LDFLAGS)
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
