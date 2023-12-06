# =============================================================================
# File:        Makefile
# Description: Makefile for the Emotion demo
# Author:      Shirobon
# Date:        2023/12/04
# =============================================================================

SRC_DIR      = src
OBJ_DIR      = obj
OUT_DIR      = out
DEBUG_DIR    = /home/pxc/development/projects/emotion/debug
ROMDISK_DIR  = romdisk
TEXTURE_DIR  = asset/texture
MODEL_DIR    = asset/model

CC = kos-cc
CFLAGS = -std=c99 -Wall -Wextra

# Source Code Dependencies
SRC  = $(wildcard $(SRC_DIR)/*.c)
DEPS = $(wildcard $(SRC_DIR)/*.h)
OBJ  = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# Texture Dependencies
TEXTURESRC = $(wildcard $(TEXTURE_DIR)/*.888)
TEXTUREOBJ = $(patsubst $(TEXTURE_DIR)/%.888, $(TEXTURE_DIR)/%.565, $(TEXTURESRC))

# Model Dependencies
MODELOBJ = $(wildcard $(MODEL_DIR)/*.obj)

# Romdisk Dependencies
ROMDISKDEPS = $(TEXTUREOBJ) $(MODELOBJ)
ROMDISKIMG  = $(OBJ_DIR)/romdisk.img
ROMDISKOBJ  = $(OBJ_DIR)/romdisk.o
# Extensions to exclude from putting in romdisk


BUILDMODE ?= DEBUG

ifeq ($(BUILDMODE),DEBUG)
	CFLAGS += -g -O0
	TARGET = $(OUT_DIR)/emotion_dbg.elf
else ifeq ($(BUILDMODE),OPTIMIZE)
	CFLAGS += -O2
	TARGET = $(OUT_DIR)/emotion_rls.elf
else
	$(error Invalid build mode: $(BUILDMODE))
endif

# Default target
all: $(TARGET)

# Create necessary directories
$(shell mkdir -p $(OUT_DIR) $(OBJ_DIR) $(ROMDISK_DIR) $(DEBUG_DIR))


# Rule to build executable
# Target depends on objects
# (compiler) (flags) -o (target) (prereqs)
$(TARGET): $(OBJ) $(ROMDISKOBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile source files
# Object files depend on source files
# (compiler) (flags) -c (prereqs) -o (outputs)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to convert RGB888 to RGB565 for putting in the romdisk
# Object files depend on source 888 textures
# (rgb convert) (prereq) (555 texture)
.SECONDARY: $($(TEXTURE_DIR)/%.565) # Prevent make from deleting "intermediate file"
$(TEXTURE_DIR)/%.565: $(TEXTURE_DIR)/%.888
	./util/rgb888_to_rgb565.py -i $< -o $(basename $<).565

# Rule to generate a romdisk image from files in romdisk/
# Romdisk image depends on asset objects existing in the romdisk directory
$(ROMDISKIMG): $(addprefix $(ROMDISK_DIR)/, $(ROMDISKDEPS))
	$(KOS_GENROMFS) -f $(ROMDISKIMG) -d $(ROMDISK_DIR)

# Rule to create romdisk object file from image
# (bin2o) (input image) (symbol name) (output)
# This needs to be KOS_CC and not CC=kos-cc, or else it fails
$(ROMDISKOBJ): $(ROMDISKIMG)
	$(KOS_BASE)/utils/bin2o/bin2o $(ROMDISKIMG) romdisk $(ROMDISKOBJ)_tmp
	$(KOS_CC) -o $(ROMDISKOBJ) -r $(ROMDISKOBJ)_tmp $(KOS_LIB_PATHS) -Wl,--whole-archive -lromdiskbase
	@rm $(ROMDISKOBJ)_tmp

# Rule to copy romdisk dependencies from asset/ to romdisk/
$(ROMDISK_DIR)/%: %
	mkdir -p $(dir $@)
	cp $< $@

# Clean all outputs
clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR) $(ROMDISK_DIR) $(DEBUG_DIR) $(TEXTURE_DIR)/*.565

# Run
TOOL = /opt/dreamcast/bin/dc-tool-ser
PORT = /dev/ttyUSB0
BAUD = 1562500
run: $(TARGET)
	sudo $(TOOL) -t $(PORT) -b $(BAUD) -c /home/pxc/development/projects/emotion/debug -x $(TARGET)
