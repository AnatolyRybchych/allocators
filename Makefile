LIBS	:= 
CARGS	+= -Iinclude -ggdb -Wall -Wextra -pedantic -MMD #-Werror
OUT		:= out

SOURCES		+= main.cpp
OBJECTS		:= $(addprefix obj/, $(SOURCES:.cpp=.o))
DEPS		:= $(OBJECTS:.o=.d)

$(foreach dependency,$(DEPS),$(eval -include $(dependency)))

$(OUT): $(OBJECTS)
	@mkdir -p $(dir ./$(OUT))
	$(CXX) $(CARGS) -o ./$(OUT) $^ $(LIBS)

obj/%.o: test/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CARGS) -o $@ $<

run: $(OUT)
	./$(OUT)

gdb: $(OUT)
	gdb ./$(OUT)

clean:
	rm $(OUT)
	rm -r ./obj

.PHONY: run gdb clean