#Make Arguments
CXX = g++
NAME = game
BUILD = build
SOURCE = source
INCLUDE = include library

ifeq ($(OS),Windows_NT)
CXXFLAGS = -static -mwindows
LDFLAGS = -static -mwindows ./library/PDCurses/wingui/pdcurses.a
else
LDFLAGS = -lncursesw
endif

CXXFLAGS += -Wall -Wextra -finput-charset=UTF-8 -std=c++11 $(addprefix -I,$(INCLUDE))
DEPFLAGS = -MMD -MP -MF $(BUILD)/$*.d

#Make Rules
src = $(wildcard $(SOURCE)/*.cpp) $(wildcard $(SOURCE)/*/*.cpp)
obj = $(src:$(SOURCE)/%.cpp=$(BUILD)/%.o)
dep = $(src:$(SOURCE)/%.cpp=$(BUILD)/%.d)

$(NAME): $(obj)
	$(info *** Please read README if your console appears blank ***)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: $(SOURCE)/%.cpp
$(BUILD)/%.o: $(SOURCE)/%.cpp $(BUILD)/%.d
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<

$(BUILD)/%.d: ;
.PRECIOUS: $(BUILD)/%.d

.PHONY: clean
clean:
	rm -rf $(BUILD) $(NAME) 

-include $(dep)

#Make Notes
#$@ - Rule
#$< - First Dependency
#$^ - All Dependencies
#$* - Stem (dir/a.foo.b, pattern a.%.b, stem dir/foo)