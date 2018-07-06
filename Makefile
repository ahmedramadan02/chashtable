PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include sources.mk

#Conditions
# Look for the OS
ifeq ($(OS), Windows_NT)
   	RM = del /s /F
   	MV = move /Y
else
	RM = rm -rf
	MV = mv --force
endif

# Compile Defines
#CFLAGS = -Wall -Werror -std=c99 -m32 -g3
CFLAGS = -Werror -std=c99 -g3
LDFLAGS= -L.
OBJS = $(SRCS:.c=.o)
CC = gcc
TARGET = chashtable.exe
HashtableLib = libCHashTable.a
MKDIR = mkdir

%.o: %.c
	$(CC) -c $^ $(CFLAGS) -o $@
	
.PHONY: all
all: directories $(TARGET) $(HashtableLib)

.PHONY: directories
directories: ${OUT_DIR}

${OUT_DIR}:
	${MKDIR} ${OUT_DIR}

$(LibObjFiles): $(LibSrcFiles)
	$(CC) -c $^ $(CFLAGS) -o $@

$(HashtableLib): $(LibObjFiles)
	ar rcs $@  $^

$(TARGET): $(OBJS) $(HashtableLib) $(OUT_DIR)
	$(CC) $(OBJS) $(HashtableLib) $(CFLAGS) -o $@ 
	$(MV) $(HashtableLib) $(RUNTIME_PATH)
	$(MV) $(TARGET) $(RUNTIME_PATH)
	
.PHONY: clean
clean:
	$(RM) $(OBJS) $(RUNTIME_PATH) $(LibObjFiles)
