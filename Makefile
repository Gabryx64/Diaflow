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
						implot/implot.cpp \
						implot/implot_items.cpp \
						ImGui-Addons/FileBrowser/ImGuiFileBrowser.cpp \
						$(wildcard $(SRCDIR)/*.cpp)
OBJS     := $(OBJDIR)/glad.o $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(patsubst imgui/%,$(OBJDIR)/%,$(patsubst ImGui-Addons/FileBrowser/%,$(OBJDIR)/%,$(patsubst implot/%,$(OBJDIR)/%,$(SRCS:.cpp=.o)))))
DEPS     := $(OBJDIR)/glad.d $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(patsubst imgui/%,$(OBJDIR)/%,$(patsubst ImGui-Addons/FileBrowser/%,$(OBJDIR)/%,$(patsubst implot/%,$(OBJDIR)/%,$(SRCS:.cpp=.d)))))
CXXFLAGS := -pedantic --std=c++17 -Wall -Wextra -Werror -O3 -Iinclude -Iimgui -Iimgui/backends -I. -Istb -IImGui-Addons/FileBrowser -Iimplot -g
LDLIBS   := -lGL -lSDL2 -lSDL2main -ltinyxml2

all: $(TARGET)

-include $(DEPS)

$(OBJDIR)/glad.o: $(SRCDIR)/glad.c
	@echo "[CC  Glad] $< -> $@"
	@$(CC) -MMD --std=c99 -Iinclude -c $< -o $@

$(OBJDIR)/%.o: imgui/%.cpp
	@echo "[CXX ImGui] $< -> $@"
	@$(CXX) -MMD --std=c++17 -c $< -o $@

$(OBJDIR)/backends/%.o: imgui/backends/%.cpp
	@echo "[CXX ImGui] $< -> $@"
	@$(CXX) -MMD --std=c++17 -Iimgui `pkg-config --cflags sdl2` -c $< -o $@

$(OBJDIR)/%.o: ImGui-Addons/FileBrowser/%.cpp
	@echo "[CXX ImGuiFileBrowser] $< -> $@"
	@$(CXX) -MMD --std=c++17 -Iimgui -c $< -o $@

$(OBJDIR)/%.o: implot/%.cpp
	@echo "[CXX ImPlot] $< -> $@"
	@$(CXX) -MMD --std=c++17 -Iimgui -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "[CXX Diaflow] $< -> $@"
	@$(CXX) -MMD $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@echo "[LD] $@"
	@$(CXX) $(LDLIBS) $(OBJS) -o $(TARGET)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DEPS) $(TARGET)

