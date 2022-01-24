# SPDX-License-Identifier: ADLINKTECH 2019 All right reserved.
CC = gcc
RM = rm -rf
WARNINGS = -Wall -Werror -Wextra -Wno-implicit-fallthrough -Wno-unused-result -Wno-variadic-macros -Wno-unused-parameter -Wno-switch-default -Wno-switch-enum -Wno-packed -Wpedantic
CFLAGS = $(WARNINGS) -O2 -g -s -I./
CFLAGS += -Wno-format-truncation -Wno-format-overflow -Wno-unused-function -Wno-unused-variable -Wno-pointer-sign -Wno-maybe-uninitialized
LIBS = -ldevInfo -lcrypto -lssl -lUpdMod -lcaStore -ldl -lsqlite3 -L../../local/lib
SRCS = carota.c
OBJS = $(SRCS:.c=.o)
TARGET = carota
all: $(OBJS)
	$(CC) carota.o $(CFLAGS) $(LIBS) -o carota
	
clean:
	-${RM} *.o $(TARGET)

.PHONY: all clean

