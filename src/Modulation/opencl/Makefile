include ../../../common/makefile.config

SRC = $(SRC_DIR)/main_test.cpp Modulation.cpp test.cpp ../util.cpp

SRC += $(LIB_DIR)/gauss.cpp $(LIB_DIR)/GeneralFunc.cpp $(LIB_DIR)/opencl/clutil.cpp $(LIB_DIR)/check/check.cpp $(LIB_DIR)/timer/meas.cpp
SRC += $(SRC_DIR)/lte_phy.cpp

INC_FLAGS += -I.. -I$(SRC_DIR) -I$(LIB_DIR)

LINKFLAGS += -lOpenCL

TARGETS = ModDemod.out

OBJS := $(patsubst %.cpp, %.o, $(SRC))

ifeq ($(TX),1)
	CXXFLAGS += -DMod
endif

ModDemod.out: $(OBJS)
	$(CXX) $^ $(LINKFLAGS) -o $@

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(TARGETS) $(OBJS)
