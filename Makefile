# tool macros
CC ?= gcc
CCFLAGS := -lSDL -lm -pthread -std=gnu89
DBGFLAGS := -g
CCOBJFLAGS := $(CCFLAGS) -c

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := src
DBG_PATH := debug

# compile macros
# targets: client server
TARGET_1_NAME := client
TARGET_2_NAME := server
TARGET_1 := $(BIN_PATH)/$(TARGET_1_NAME)
TARGET_2 := $(BIN_PATH)/$(TARGET_2_NAME)
TARGET_1_DEBUG := $(DBG_PATH)/$(BIN_PATH)/$(TARGET_1_NAME)
TARGET_2_DEBUG := $(DBG_PATH)/$(BIN_PATH)/$(TARGET_2_NAME)

# src files & obj files
SHARED_SRC := $(foreach file, $(SRC_PATH), $(wildcard $(addprefix $(file)/*,.c*)))
SRC_1 := $(foreach file, $(SRC_PATH)/$(TARGET_1_NAME), $(wildcard $(addprefix $(file)/*,.c*)))
SRC_2 := $(foreach file, $(SRC_PATH)/$(TARGET_2_NAME), $(wildcard $(addprefix $(file)/*,.c*)))

SHARED_OBJ :=  $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SHARED_SRC)))))
OBJ_1 :=  $(addprefix $(OBJ_PATH)/$(TARGET_1_NAME)/, $(addsuffix .o, $(notdir $(basename $(SRC_1)))))
OBJ_2 :=  $(addprefix $(OBJ_PATH)/$(TARGET_2_NAME)/, $(addsuffix .o, $(notdir $(basename $(SRC_2)))))

SHARED_OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SHARED_SRC)))))
OBJ_1_DEBUG := $(addprefix $(DBG_PATH)/$(TARGET_1_NAME)/, $(addsuffix .o, $(notdir $(basename $(SRC_1)))))
OBJ_2_DEBUG := $(addprefix $(DBG_PATH)/$(TARGET_2_NAME)/, $(addsuffix .o, $(notdir $(basename $(SRC_2)))))

# clean files list
DISTCLEAN_LIST := $(SHARED_OBJ) \
				  $(OBJ_1) \
				  $(OBJ_2) \
                  $(SHARED_OBJ_DEBUG) \
                  $(OBJ_1_DEBUG) \
				  $(OBJ_2_DEBUG)
CLEAN_LIST := $(TARGET_1) \
			  $(TARGET_2) \
			  $(TARGET_1_DEBUG) \
			  $(TARGET_2_DEBUG) \
			  $(DISTCLEAN_LIST)

# default rule
default: makedir all

# non-phony targets
$(TARGET_1): $(OBJ_1) $(SHARED_OBJ)
	$(CC) -o $@ $(OBJ_1) $(SHARED_OBJ) $(CCFLAGS)

$(TARGET_2): $(OBJ_2) $(SHARED_OBJ)
	$(CC) -o $@ $(OBJ_2) $(SHARED_OBJ) $(CCFLAGS) 

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) -o $@ $< $(CCOBJFLAGS) 

$(OBJ_PATH)/$(TARGET_1_NAME)/%.o: $(SRC_PATH)/$(TARGET_1_NAME)/%.c*
	$(CC) -o $@ $< $(CCOBJFLAGS) 

$(OBJ_PATH)/$(TARGET_2_NAME)/%.o: $(SRC_PATH)/$(TARGET_2_NAME)/%.c*
	$(CC) -o $@ $< $(CCOBJFLAGS) 

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(DBGFLAGS) -o $@ $< $(CCOBJFLAGS) 

$(DBG_PATH)/$(TARGET_1_NAME)/%.o: $(SRC_PATH)/$(TARGET_1_NAME)/%.c*
	$(CC) $(DBGFLAGS) -o $@ $< $(CCOBJFLAGS) 

$(DBG_PATH)/$(TARGET_2_NAME)/%.o: $(SRC_PATH)/$(TARGET_2_NAME)/%.c*
	$(CC) $(DBGFLAGS) -o $@ $< $(CCOBJFLAGS) 

$(TARGET_1_DEBUG): $(OBJ_1_DEBUG) $(SHARED_OBJ_DEBUG)
	$(CC) $(DBGFLAGS) $(OBJ_1_DEBUG) $(SHARED_OBJ_DEBUG) -o $@ $(CCFLAGS) 

$(TARGET_2_DEBUG): $(OBJ_2_DEBUG) $(SHARED_OBJ_DEBUG)
	$(CC) $(DBGFLAGS) $(OBJ_2_DEBUG) $(SHARED_OBJ_DEBUG) -o $@ $(CCFLAGS) 

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH)
	@mkdir -p $(OBJ_PATH)/$(TARGET_1_NAME) $(OBJ_PATH)/$(TARGET_2_NAME)
	@mkdir -p $(DBG_PATH)/$(BIN_PATH) $(DBG_PATH)/$(TARGET_1_NAME) $(DBG_PATH)/$(TARGET_2_NAME)

.PHONY: all
all: $(TARGET_1) $(TARGET_2)

.PHONY: $(TARGET_1_NAME)
$(TARGET_1_NAME): $(TARGET_1)

.PHONY: $(TARGET_2_NAME)
$(TARGET_2_NAME): $(TARGET_2)

.PHONY: debug
debug: $(TARGET_1_DEBUG) $(TARGET_2_DEBUG)

.PHONY: debug_$(TARGET_1_NAME)
debug_$(TARGET_1_NAME): $(TARGET_1_DEBUG)

.PHONY: debug_$(TARGET_2_NAME)
debug_$(TARGET_2_NAME): $(TARGET_2_DEBUG)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)