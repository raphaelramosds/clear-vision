CXX=g++
CXX_FLAGS=$(shell pkg-config --cflags opencv4) -g # -O3
LDLIBS=$(shell pkg-config --libs opencv4)
TARGETS=batch conf
OBJS=cvision.o

%.o: %.cpp %.hpp
	$(CXX) $(CXX_FLAGS) -c $< -o $@

%: %.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@ $(LDLIBS)

# $< eh o arquivo fonte (test.cpp)
# $@ eh o nome do arquivo alvo (test)
test: test.cpp cvision.o 
	$(CXX) $(CXX_FLAGS) $< -o $@ $(OBJS) $(LDLIBS)

all: $(TARGETS)

clean:
	rm $(TARGETS) *.o