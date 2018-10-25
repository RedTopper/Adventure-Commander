#Make Arguments
CXX = g++
NAME = game
BUILD = build
SOURCE = source
INCLUDE = include
LDFLAGS = -lncursesw
CXXFLAGS = -Wall -finput-charset=UTF-8 -I$(INCLUDE)
DEPFLAGS = -MT $@ -MMD -MP -MF $(BUILD)/$*.d

#Make Rules
$(shell mkdir -p $(BUILD) >/dev/null)

src = $(wildcard $(SOURCE)/*.cpp)
obj = $(src:$(SOURCE)/%.cpp=$(BUILD)/%.o)
dep = $(src:$(SOURCE)/%.cpp=$(BUILD)/%.d)

$(NAME): $(obj)
	$(info *** Please read README if your console appears blank ***)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: $(SOURCE)/%.cpp
$(BUILD)/%.o: $(SOURCE)/%.cpp $(BUILD)/%.d
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