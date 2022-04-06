ifeq ($(OS),Windows_NT)
	TARGEXT := .exe
endif

TARGET   := bin/diaflow$(TARGEXT)
SRCDIR   := src
OBJDIR   := obj
SRCS     := imgui/imgui.cpp \
						imgui/imgui_draw.cpp \
						imgui/imgui_tables.cpp \
						imgui/imgui_widgets.cpp \
						imgui/backends/imgui_impl_sdl.cpp \
						imgui/backends/imgui_impl_opengl3.cpp \
						ImGui-Addons/FileBrowser/ImGuiFileBrowser.cpp \
						$(wildcard $(SRCDIR)/*.cpp)
SRCCNT   := $(words $(SRCS))
OBJS     := $(OBJDIR)/glad.o $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(patsubst imgui/%,$(OBJDIR)/%,$(patsubst ImGui-Addons/FileBrowser/%,$(OBJDIR)/%,$(SRCS:.cpp=.o))))
DEPS     := $(OBJDIR)/glad.d $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(patsubst imgui/%,$(OBJDIR)/%,$(patsubst ImGui-Addons/FileBrowser/%,$(OBJDIR)/%,$(SRCS:.cpp=.d))))
CXXFLAGS := -pedantic --std=c++17 -Wall -Wextra -Werror -O3 -Iinclude -Iimgui -Iimgui/backends -I. -Istb -IImGui-Addons/FileBrowser
LDLIBS   := -lGL -lSDL2 -lSDL2main

all: $(TARGET)

define get_index
$(sort $(eval index_count :=) \
$(foreach word,$(1), \
$(eval index_count := $(index_count) x) \
$(if $(findstring $(word),$(2)),$(words $(index_count)))))
endef

-include $(DEPS)

$(OBJDIR)/glad.o: $(SRCDIR)/glad.c
	@echo "[CC Glad] $< -> $@"
	@$(CC) -MMD --std=c99 -Iinclude -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "[CC $(call get_index,$(SRCS),$<)/$(SRCCNT)] $< -> $@"
	@$(CXX) -MMD $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: imgui/%.cpp
	@echo "[CC ImGui] $< -> $@"
	@$(CXX) -MMD --std=c++17 -c $< -o $@

$(OBJDIR)/backends/%.o: imgui/backends/%.cpp
	@echo "[CC ImGui] $< -> $@"
	@$(CXX) -MMD --std=c++17 -Iimgui `pkg-config --cflags sdl2` -c $< -o $@

$(OBJDIR)/%.o: ImGui-Addons/FileBrowser/%.cpp
	@echo "[CC ImGuiFileBrowser] $< -> $@"
	@$(CXX) -MMD --std=c++17 -Iimgui -c $< -o $@

$(TARGET): $(OBJS)
	@echo "[LD] $@"
	@$(CXX) $(LDLIBS) $(OBJS) -o $(TARGET)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET)

