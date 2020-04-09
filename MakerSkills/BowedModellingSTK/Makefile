# path macros
INC_PATH := include
BIN_PATH := bin
OBJ_PATH := obj
LIB_PATH := lib
SRC_PATH := src

DBG_PATH := debug


# tool macros
CC := clang++# FILL: the compiler
CCFLAG := -Wall  $(addprefix -I,$(INC_PATH))
LDFLAGS := -L/usr/lib -std=c++17 -lm -lpthread -llo
RTFLAGS :=
DBGFLAG := -g
CCOBJFLAG := -c $(CCFLAG)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	LDFLAGS += -lasound
	RTFLAGS += -D__LINUX_ALSA__
endif
ifeq ($(UNAME_S),Darwin)
	LDFLAGS += -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
	RTFLAGS += -D__MACOSX_CORE__
endif

# compile macros
TARGET_NAME := BowedModel # FILL: target name
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)
MAIN_SRC := main.cpp # FILL: src file contains `main()`

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
LIB := $(foreach x, $(LIB_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))
OBJ += $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(LIB)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG)
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(DISTCLEAN_LIST)



# default rule
default: all

# non-phony targets
$(TARGET): $(OBJ) $(MAIN_SRC)
	$(CC) $(CCFLAG) -o $@ $? $(LDFLAGS) $(RTFLAGS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CCOBJFLAG) -o $@ $<

$(OBJ_PATH)/%.o: $(LIB_PATH)/%.c*
	$(CC) $(CCOBJFLAG) $(RTFLAGS) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(CCOBJFLAG) $(DBGFLAG) -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) $(CCFLAG) $(DBGFLAG) $? -o $@



# phony rules
.PHONY: all
all: $(TARGET)

.PHONY: debug
debug: $(TARGET_DEBUG)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)
