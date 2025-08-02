NAME		= blog_memory

CC			= gcc
CC_FLAG		= -Werror -Wall -Wextra

BUILD_DIR	= ./build

SRC			= ./src/main.c

OUT			= $(BUILD_DIR)/$(NAME)

all: build-dir
	$(CC) $(CC_FLAG) $(MAIN_SRC) $(SRC) -o $(OUT)

build-dir:
	mkdir -p $(BUILD_DIR)
