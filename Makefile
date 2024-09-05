CFLAGS := -g -Wall -fopenmp
LDLIST := -Wl,-lm
INCLUDE := include
SRC := src
OBJ := obj
LIB := lib
BUILD := build
MAIN := main
LIBNAMES := filecheck rltrain mlstore tttutils
CC := gcc
KERNEL := $(shell uname -s)
LIBEXT := LIBEXTSAVE
MAINEXT := MAINEXTSAVE
CONFFILENAME := vars

include $(CONFFILENAME).conf

.PHONY: setup run

$(CONFFILENAME).conf:
	touch $(CONFFILENAME).conf

setup:
	@echo "Detected $(KERNEL)";\
	LIBEXT=so;\
	LIBPATH=LD_LIBRARY_PATH;\
	MAINEXT=elf;\
	uname -s | grep _NT > /dev/null && \
		LIBEXT=dll&&\
		LIBPATH=PATH&&\
		MAINEXT=exe;\
	if [ $(KERNEL) = Darwin ]; then\
		LIBEXT=dylib;\
		LIBPATH=DYLD_LIBRARY_PATH;\
		MAINEXT=app;\
	fi&&\
	echo "$(LIBEXT)=$$LIBEXT" > $(CONFFILENAME).conf;\
	echo "$(MAINEXT)=$$MAINEXT" >> $(CONFFILENAME).conf

$(addsuffix .o, $(LIBNAMES)): %.o: $(SRC)//%.c
	@echo "Compiling $*"
	$(CC) $(CFLAGS) -c $< -o $(OBJ)/$*.o -fPIC -I$(INCLUDE)

$(MAIN).o:
	@echo "Compiling $(MAIN)"
	$(CC) $(CFLAGS) -c $(SRC)/$(MAIN).c -o $(OBJ)/$(MAIN).o -I$(INCLUDE)

$(addprefix build_, $(LIBNAMES)): build_%: %.o
	@echo "Building $*"
	$(CC) $(CFLAGS) $(OBJ)/$< -o $(LIB)/lib$*.$($(LIBEXT)) -shared -fPIC -Iinclude

build_all_libs: $(addprefix build_, $(LIBNAMES))

build_$(MAIN): $(MAIN).o build_all_libs
	@echo "Building $(MAIN)"
	$(CC) $(CFLAGS) $(OBJ)/$(MAIN).o -o $(BUILD)/$(MAIN).$($(MAINEXT)) -I$(INCLUDE) -L$(LIB) $(addsuffix .$($(LIBEXT)), $(addprefix $(LIB)/lib, $(LIBNAMES))) $(LDLIST)

run: build_$(MAIN)
	@echo "Setting path variables";\
	uname -s | grep _NT > /dev/null && \
	PATH="$$PATH;$(shell pwd)/$(LIB)" && \
	echo "$$PATH";\
	echo "Path set successfully";\
	echo "Running $(MAIN)" && \
	($(BUILD)/$(MAIN).$($(MAINEXT)) $(ARGS)) || \
	echo "Execution failed"

clean:
	@echo "Cleaning"
	rm -f $(OBJ)/*
	rm -f $(LIB)/*
	rm -f $(BUILD)/*
	rm -f $(CONFFILENAME).conf

$(V).SILENT: