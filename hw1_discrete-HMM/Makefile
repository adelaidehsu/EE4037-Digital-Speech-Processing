.PHONY: all clean

CFLAGS+=
LDFLAGS+=-lm     # link to math library

TARGET=train test acc
RM=rm -f

all: $(TARGET)
#type make/make all to compile test_hmm

train: train.o
	g++ -o train train.o
train.o: train.cpp hmm.h
	g++ -c train.cpp
test: test.o
	g++ -o test test.o
test.o: test.cpp hmm.h
	g++ -c test.cpp
acc: acc.o
	g++ -o acc acc.o
acc.o: acc.cpp
	g++ -c acc.cpp

clean:
	$(RM) $(TARGET) *.o  #type make clean to remove the compiled file
	echo Clean done.