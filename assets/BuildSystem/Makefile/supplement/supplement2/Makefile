# Default target executed when no arguments are given to make.
.PHONY: default_target

default_target: all

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =


.SUFFIXES: .hpux_make_needs_suffix_list

# The main all target
all: a b c
.PHONY: all

a:
	./a.sh

b:
	./b.sh

c:
	./c.sh

.PHONY: clean
clean:
	@echo "删除"
