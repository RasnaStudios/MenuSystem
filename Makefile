UE_VERSION = 5.4
DOTNET_VERSION = 6.0.302
BUILD_TARGET = Development
PROJECT_NAME = MenuSystem
ETRURIA_TARGET = $(PROJECT_NAME)Editor
IDE = Rider

# Check if .env file exists, and load custom variables
ifeq ($(wildcard .env),)
    $(info make - using default values.)
else
    $(info make - including custom values.)
    include .env
    export $(shell sed 's/=.*//' .env)
endif


OS := $(shell uname -s)
PROJECT_PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

# Fetch UnrealBuildTool executable
ifeq ($(OS),Darwin)
    UE_DIR ?= /Users/Shared/Epic Games
	UE_PATH := $(UE_DIR)/UE_$(UE_VERSION)/Engine
	UE_DOTNET := "$(UE_PATH)/Binaries/ThirdParty/DotNet/$(DOTNET_VERSION)/mac-arm64/dotnet"
	UE_BUILD_TOOL := "$(UE_PATH)/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll"
	TARGET_PLATFORM = Mac
	OPEN_COMMAND = open
else
    UE_DIR ?= /c/Program Files/Epic Games
	UE_PATH = $(UE_DIR)/UE_$(UE_VERSION)/Engine
	UE_DOTNET := "$(UE_PATH)/Binaries/ThirdParty/DotNet/$(DOTNET_VERSION)/windows/dotnet.exe"
	UE_BUILD_TOOL := "$(UE_PATH)/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.dll"
	TARGET_PLATFORM = Win64
	OPEN_COMMAND = start
endif

init:
	git submodule sync
	git submodule update --init --recursive

generate-files:
	$(UE_DOTNET) $(UE_BUILD_TOOL) -projectfiles -project=$(PROJECT_PATH)/$(PROJECT_NAME).uproject -game -rocket -progress
	$(UE_DOTNET) $(UE_BUILD_TOOL) -mode=GenerateProjectFiles -$(IDE) -project=$(PROJECT_PATH)/$(PROJECT_NAME).uproject $(ETRURIA_TARGET) $(TARGET_PLATFORM) $(BUILD_TARGET)

generate-clang-db:
	$(UE_DOTNET) $(UE_BUILD_TOOL) -mode=GenerateClangDatabase -project=$(PROJECT_PATH)/$(PROJECT_NAME).uproject $(ETRURIA_TARGET) $(TARGET_PLATFORM) $(BUILD_TARGET)
	cp "$(UE_PATH)/../compile_commands.json" "$(PROJECT_PATH)"
	cp "$(UE_PATH)/../compile_commands.json" "$(UE_PATH)"

generate:
	make generate-files
	make generate-clang-db

help:
	$(UE_DOTNET) $(UE_BUILD_TOOL) --help

.PHONY: build
build:
	$(UE_DOTNET) $(UE_BUILD_TOOL) -mode=Build -project=$(PROJECT_PATH)/$(PROJECT_NAME).uproject $(ETRURIA_TARGET) $(TARGET_PLATFORM) $(BUILD_TARGET)

reload:
	$(UE_DOTNET) $(UE_BUILD_TOOL) -mode=Build -ForceHotReload -project=$(PROJECT_PATH)/$(PROJECT_NAME).uproject $(ETRURIA_TARGET) $(TARGET_PLATFORM) $(BUILD_TARGET)

open-vs:
	$(OPEN_COMMAND) "$(PROJECT_NAME).code-workspace"

open-ue:
	$(OPEN_COMMAND) "$(PROJECT_NAME).uproject"

.PHONY: docs
docs:
	doxygen Doxyfile
	$(OPEN_COMMAND) Docs/html/index.html

clean:
	$(UE_DOTNET) $(UE_BUILD_TOOL) -mode=Clean -project=$(PROJECT_PATH)/$(PROJECT_NAME).uproject $(ETRURIA_TARGET) $(TARGET_PLATFORM) $(BUILD_TARGET)
	rm -r Binaries || true
	rm -r Plugins/*/Binaries || true
	rm -r Intermediate || true
	rm -r Plugins/*/Intermediate || true
	rm -r DerivedDataCache || true
	rm -r Deps || true
	rm -r Plugins/UE4CMake || true
	rm -r Plugins/UnrealFastNoise2 || true

refresh:
	make clean
	make init
	make generate
	make build
	make open-ue
