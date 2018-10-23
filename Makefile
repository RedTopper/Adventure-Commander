#Make Arguments
CXX = g++
NAME = game
BUILD = build
SOURCE = source
INCLUDE = include
LDFLAGS = -lncursesw
CXXFLAGS = -Wall -Wextra -finput-charset=UTF-8 -std=gnu11 -I$(INCLUDE)
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.d

#Make Rules
$(shell mkdir -p $(BUILD) >/dev/null)

src = $(wildcard $(SOURCE)/*.cpp)
obj = $(src:$(SOURCE)/%.cpp=$(BUILD)/%.o)
dep = $(src:$(SOURCE)/%.cpp=$(BUILD)/%.d)

-include $(dep)

$(NAME): $(obj)
	$(info *** Please read README if your console appears blank ***)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: $(SOURCE)/%.cpp $(BUILD)/%.d
	$(CXX) $(CFLAGS) $(DEPFLAGS) $<

.PHONY: dir
dir:
	@mkdir -p $(BUILD)

.PHONY: clean
clean:
	rm -rf $(BUILD) $(NAME) 

#Make Notes
#$@ - Rule
#$< - First Dependency
#$^ - All Dependencies
#$* - Stem (dir/a.foo.b, pattern a.%.b, stem dir/foo)