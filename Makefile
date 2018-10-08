#Make arguments
CFLAGS = -Wall -Wextra -finput-charset=UTF-8 -std=gnu11
CC = gcc
NAME = game
BUILD = build
SOURCE = source
LDFLAGS = -lncursesw

#Make rules
src = $(wildcard $(SOURCE)/*.c)
obj = $(src:$(SOURCE)/%.c=$(BUILD)/%.o)
dep = $(src:$(SOURCE)/%.c=$(BUILD)/%.d)

$(NAME): $(obj)
	$(info *** Please read README if your console appears blank ***)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(dep)

$(BUILD)/%.d: $(SOURCE)/%.c | dir
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

$(BUILD)/%.o: $(SOURCE)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: dir
dir:
	@mkdir -p $(BUILD)

.PHONY: clean
clean:
	rm -rf $(obj) $(dep) $(BUILD) $(NAME) 
