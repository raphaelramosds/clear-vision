CXX=g++
CXX_FLAGS=$(shell pkg-config --cflags opencv4) -g # -O3
LDLIBS=$(shell pkg-config --libs opencv4)

TARGETS=batch

%: %.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@ $(LDLIBS)

all: $(TARGETS)

clean:
	rm $(TARGETS)