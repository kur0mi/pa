NAME = nemu
INC_DIR += ./include
BUILD_DIR ?= ./build
OBJ_DIR ?= $(BUILD_DIR)/obj
BINARY ?= $(BUILD_DIR)/$(NAME)

include Makefile.git

.DEFAULT_GOAL = app

# Compilation flags
CC = gcc
LD = gcc
INCLUDES  = $(addprefix -I, $(INC_DIR))
ifeq ($(debug), y)
$(call clean)
CFLAGS   = -O2 -MMD -Wall -ggdb -D MY_DEBUG $(INCLUDES)
else ifeq ($(debug), n)
CFLAGS   = -O2 -MMD -Wall -Werror $(INCLUDES)
else
CFLAGS   = -O2 -MMD -Wall -Werror -ggdb $(INCLUDES)
endif

debug:
	@echo $(CFLAGS)

# Files to be compiled
SRCS = $(shell find src/ -name "*.c")
OBJS = $(SRCS:src/%.c=$(OBJ_DIR)/%.o)

# Compilation patterns
$(OBJ_DIR)/%.o: src/%.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

# Depencies
-include $(OBJS:.o=.d)

# Some convinient rules

.PHONY: app run commit push gdb clean
app: $(BINARY)

ARGS ?= -l $(BUILD_DIR)/nemu-log.txt

# Command to execute NEMU
NEMU_EXEC := $(BINARY) $(ARGS)

$(BINARY): $(OBJS)
	$(call git_commit, "compile")
	@echo + LD $@
	@$(LD) -O2 -o $@ $^ -lSDL2 -lreadline

run: $(BINARY)
	$(call git_commit, "run")
	$(NEMU_EXEC)

message ?= update
commit: $(BINARY)
	git add .. -A
	git commit -m "$(message)"

push:
	git push kur0mi master:master

gdb: $(BINARY)
	$(call git_commit, "gdb")
	gdb -s $(BINARY) --args $(NEMU_EXEC)

clean: 
	$(call git_commit, "clean")
	rm -rf $(BUILD_DIR)
