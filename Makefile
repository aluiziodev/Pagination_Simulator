.DEFAULT_GOAL := all

CPPFLAGS += -Isrc
CXXFLAGS += -std=c++17 -Wall -Wextra -Wpedantic

sources := $(shell find src -type f -name '*.cpp')
objects := $(patsubst src/%.cpp,build/%.o,$(sources))
dependencies := $(objects:.o=.d)

.PHONY: all clean
all: pager

pager: $(objects)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	$(RM) pager $(objects) $(dependencies)

-include $(dependencies)
