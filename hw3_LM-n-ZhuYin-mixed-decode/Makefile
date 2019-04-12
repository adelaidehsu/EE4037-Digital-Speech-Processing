# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
#SRIPATH ?= /data/DSP_HW3/103_2/srilm-1.5.10
#MACHINE_TYPE ?= i686-m64
SRIPATH ?= /Users/aliyah/desktop/srilm-1.5.10
SRILIB ?= $(SRIPATH)/lib/$(MACHINE_TYPE)
MACHINE_TYPE ?= macosx
LM ?= bigram.lm

#CXX = g++
CXX = /usr/bin/g++
CXXFLAGS = -O3 -I$(SRIPATH)/include -w --std=c++11
vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig 
SRC = mydisambig.cpp 
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
.PHONY: all clean map run

#all: $(TARGET)

#$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	#$(CXX) $(LDFLAGS) -o $@ $^

all:
	$(CXX) -I$(SRIPATH)/include $(SRC) -o $(TARGET) -L$(SRILIB) -loolm -ldstruct -lmisc
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

run:
	@echo “Bigram!”
	@for i in $(shell seq 1 10) ; do \
          echo "Running $$i.txt"; \
           ./mydisambig -text testdata/$$i.txt -map $(TO) -lm $(LM) result2/$$i.txt;\
        done;

map:
	@echo "Mapping!"
	@g++ -o mapping mapping.cpp
	@./mapping $(FROM) $(TO)
clean:
	$(RM) $(OBJ) $(TARGET)
	$(RM) mapping.o mapping $(TO)

