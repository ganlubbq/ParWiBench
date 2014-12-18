include ../../common/makefile.config

SRC = ScrambMain.cpp  Scrambler.cpp
SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp

SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I. -I$(LIB_DIR) -I$(SRC_DIR)

TARGETS = ScrDescr.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DScramb
endif

ScrDescr.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
