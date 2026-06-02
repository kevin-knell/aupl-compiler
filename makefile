# ============================================================================
# AUPL Build System
# Reproduces the shell script behavior exactly
# ============================================================================

CXX := g++

WARNINGS := \
	-Wall \
	-Wextra \
	-Wconversion \
	-Werror=return-type \
	-Wold-style-cast \
	-Wnon-virtual-dtor \
	-Wsuggest-override \
	-Wduplicated-cond \
	-Wlogical-op \
	-Wunused-result \
	-Wunused-value

BLACKLISTED_WARNINGS :=

COMMON_FLAGS := -std=c++23 -O3
LINK_FLAGS := -O3 -march=native -ffunction-sections -Wl,--gc-sections

LIBS := \
	-lncurses \
	-lglfw \
	-lvulkan \
	-ldl \
	-lpthread \
	-lX11 \
	-lXxf86vm \
	-lXrandr \
	-lXi \
	-lSDL2

# ============================================================================
# Source Discovery
# ============================================================================

INCLUDE_DIRS := $(shell find . -type f \( -name '*.h' -o -name '*.hpp' \) -exec dirname {} \; | sort -u)
INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))

CPP_FILES_CMP := $(shell find . -name '*.cpp' ! -name 'output.cpp')

CPP_FILES_VM := $(shell find ./vm -name '*.cpp') main.cpp

CPP_FILES_CPP_BASE := $(shell find ./vm -name '*.cpp')

PROJECT ?= terminal_rpg

GENERATED_CPP := examples/$(PROJECT)/build/cpp/output.cpp
CPP_FILES_CPP := $(CPP_FILES_CPP_BASE) $(GENERATED_CPP)

# ============================================================================
# Object Files
# ============================================================================

CMP_OBJECTS := $(patsubst %.cpp,build/cmp/%.o,$(CPP_FILES_CMP))

VM_OBJECTS := $(patsubst %.cpp,build/vm/%.o,$(CPP_FILES_VM))

CPP_OBJECTS := \
	$(foreach f,$(CPP_FILES_CPP),\
		examples/$(PROJECT)/build/cpp/$(notdir $(basename $(f))).o)

# ============================================================================
# Default Target
# ============================================================================

.PHONY: all
all: compiler

# ============================================================================
# Compiler (Parser)
# ============================================================================

.PHONY: compiler
compiler: build/cmp/aupl

build/cmp/aupl: $(CMP_OBJECTS)
	@echo -e "\e[34m[BUILD]\e[0m Linking Parser..."
	$(CXX) $(LINK_FLAGS) $^ -o $@ $(LIBS)

build/cmp/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) -c $(COMMON_FLAGS) \
		-DCOMPILER \
		$(WARNINGS) \
		$(BLACKLISTED_WARNINGS) \
		$(INCLUDE_FLAGS) \
		-o $@ $<

# ============================================================================
# VM
# ============================================================================

.PHONY: vm
vm: build/vm/aupl

build/vm/aupl: $(VM_OBJECTS)
	@echo -e "\e[34m[BUILD]\e[0m Linking VM..."
	$(CXX) $(LINK_FLAGS) $^ -o $@ $(LIBS)

build/vm/%.o: %.cpp
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	$(CXX) -c $(COMMON_FLAGS) \
		-DVM_ONLY \
		$(WARNINGS) \
		$(BLACKLISTED_WARNINGS) \
		$(INCLUDE_FLAGS) \
		-o $@ $<

# ============================================================================
# Generated Program
# ============================================================================

.PHONY: generated
generated: shaders generate_cpp examples/$(PROJECT)/build/cpp/aupl

shaders:
	@echo "compiling shaders..."
	glslc shaders/shader.vert -o shaders/shader.vert.spv
	glslc shaders/shader.frag -o shaders/shader.frag.spv

generate_cpp: build/cmp/aupl
	@mkdir -p examples/$(PROJECT)/build/cpp
	@echo -e "\e[34m[RUN]\e[0m ./build/cmp/aupl \e[35msource\e[0m"
	./build/cmp/aupl -o examples/$(PROJECT)/build/cpp/output examples/$(PROJECT)

examples/$(PROJECT)/build/cpp/aupl: $(CPP_OBJECTS)
	@echo -e "\e[34m[BUILD]\e[0m Linking C++ Generated..."
	$(CXX) $(LINK_FLAGS) $^ -o $@ $(LIBS)
	@echo -e "\e[34m[BUILD]\e[0m \e[32mDone\e[0m."

examples/$(PROJECT)/build/cpp/%.o:
	@mkdir -p examples/$(PROJECT)/build/cpp
	$(eval SRC := $(shell for f in $(CPP_FILES_CPP); do \
		if [ "$$(basename $$f .cpp)" = "$*" ]; then echo $$f; fi; \
	done))
	$(CXX) -c $(COMMON_FLAGS) \
		-DVM_ONLY \
		-Wall \
		-Wextra \
		-Werror=return-type \
		$(BLACKLISTED_WARNINGS) \
		$(INCLUDE_FLAGS) \
		-o $@ $(SRC)

# ============================================================================
# Run
# ============================================================================

.PHONY: run
run: generated
	./examples/$(PROJECT)/build/cpp/aupl

# ============================================================================
# Convenience Targets
# ============================================================================

.PHONY: build
build: compiler

.PHONY: build-all
build-all: compiler vm generated

.PHONY: clean
clean:
	rm -rf build
	rm -rf examples/$(PROJECT)/build

.PHONY: rebuild
rebuild: clean build-all