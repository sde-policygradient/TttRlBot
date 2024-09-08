CFLAGS := -g -Wall -fopenmp
LDLIST := -Wl,-lm
INCLUDE := include
PROJECTNAME := tttrlbot
SRC := src
OBJ := obj
LIB := lib
BUILD := build
CONF := conf
MAIN := main
LIBNAMES := filecheck rltrain mlstore tttutils
CC := gcc
KERNEL := $(shell uname -s)
LIBEXT := LIBEXTSAVE
MAINEXT := MAINEXTSAVE
CONFFILENAME := vars

include $(CONF)/$(CONFFILENAME).conf

.PHONY: setup run

$(CONF)/$(CONFFILENAME).conf:
	touch $(CONF)/$(CONFFILENAME).conf

setup:
	@echo "Detected $(KERNEL)";\
	mkdir $(LIB) $(OBJ) $(BUILD) $(CONF) > /dev/null 2>&1;\
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
	echo "$(LIBEXT)=$$LIBEXT" > $(CONF)/$(CONFFILENAME).conf;\
	echo "$(MAINEXT)=$$MAINEXT" >> $(CONF)/$(CONFFILENAME).conf

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
	@sudo mkdir /usr/lib/$(PROJECTNAME) 2> /dev/null;\
	sudo cp $(LIB)/* /usr/lib/$(PROJECTNAME)

build_$(MAIN): $(MAIN).o build_all_libs
	@echo "Building $(MAIN)"
	$(CC) $(CFLAGS) $(OBJ)/$(MAIN).o -o $(BUILD)/$(MAIN).$($(MAINEXT)) -L$(LIB) -I$(INCLUDE) $(addprefix $(LIB)/lib, $(addsuffix .$(LIBEXTSAVE), $(LIBNAMES))) $(LDLIST)

run: build_$(MAIN)
	echo "Running $(MAIN)" && \
	($(BUILD)/$(MAIN).$($(MAINEXT)) $(ARGS)) || \
	echo "Execution failed"

clean:
	@echo "Cleaning"
	rm -rf $(OBJ)/*
	rm -rf $(LIB)/*
	rm -rf $(BUILD)/*
	rm -rf $(CONF)/*

$(V).SILENT: