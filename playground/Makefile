CXX=g++
CXX_FLAGS=$(shell pkg-config --cflags opencv4)
LDLIBS=$(shell pkg-config --libs opencv4)

TARGETS=main

%: %.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@ -O3 $(LDLIBS)

all: $(TARGETS)

clean:
	rm $(TARGETS)