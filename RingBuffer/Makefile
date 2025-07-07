CC 	 := g++
LDFLAGS  := -lpthread
CFLAGS := -Wall -g -std=c++11
TARGET   := mainApp

OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

$(TARGET):$(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)
	$(MAKE) clean

%.o:%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY:clean cleanall
cleanall:clean
	@rm $(TARGET)
clean:
	@rm ./*.o 
