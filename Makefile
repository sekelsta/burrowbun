CXX = g++
# For debugging use -Og -g
# For going fast use -O3
CXXFLAGS = -std=c++14 -Wall -D_GLIBCXX_DEBUG -D_LIBCXX_DEBUG_PEDANTIC -Og -g
#CXXFLAGS = -std=c++14 -Wall -O3
INCLUDE_FLAGS = -I/usr/include/libnoise/ -L/usr/lib/ 
LINKER_FLAGS = -lnoise -lpthread

#Stuff copied from a different makefile
LD_FLAGS = -lrt

CUDA_PATH       ?= /usr/local/cuda
CUDA_INC_PATH   ?= $(CUDA_PATH)/include
CUDA_BIN_PATH   ?= $(CUDA_PATH)/bin
CUDA_LIB_PATH   ?= $(CUDA_PATH)/lib

# CUDA code generation flags
GENCODE_FLAGS   := -gencode arch=compute_30,code=sm_30 \
        -gencode arch=compute_35,code=sm_35 \
        -gencode arch=compute_50,code=sm_50 \
        -gencode arch=compute_52,code=sm_52 \
        -gencode arch=compute_60,code=sm_60 \
        -gencode arch=compute_61,code=sm_61 \
        -gencode arch=compute_61,code=compute_61
        
# Common binaries
NVCC            ?= $(CUDA_BIN_PATH)/nvcc

# OS-specific build flags
ifeq ($(shell uname),Darwin)
	LDFLAGS       := -Xlinker -rpath $(CUDA_LIB_PATH) -L$(CUDA_LIB_PATH) -lcudart -lcufft
	CCFLAGS   	  := -arch $(OS_ARCH)
else
	ifeq ($(OS_SIZE),32)
		LDFLAGS   := -L$(CUDA_LIB_PATH) -lcudart
		CCFLAGS   := -m32
	else
		CUDA_LIB_PATH := $(CUDA_LIB_PATH)64
		LDFLAGS       := -L$(CUDA_LIB_PATH) -lcudart
		CCFLAGS       := -m64
	endif
endif

# OS-architecture specific flags
ifeq ($(OS_SIZE),32)
	NVCCFLAGS := -m32
else
	NVCCFLAGS := -m64
endif

FLAGS = -g -Wall -D_REENTRANT -std=c++0x -pthread
INCLUDE = -I$(CUDA_INC_PATH)
LIBS = -L$(CUDA_LIB_PATH) -lcudart -lcufft -lsndfile

# what folders
SRCDIR = src/
OBJDIR = obj/
BINDIR = ./

# File names
SOURCEFILES := $(notdir $(wildcard $(SRCDIR)*.cc))
SOURCES = $(addprefix $(SRCDIR), $(SOURCEFILES))
OBJECTS = $(addprefix $(OBJDIR), $(SOURCEFILES:.cc=.o))
EXEC = burrowbun

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDE_FLAGS) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

all: $(EXEC)

$(EXEC): $(OBJECTS) cuda.o mapgen.cu.o noisegen.cu.o noise_helpers.cu.o
	$(CXX) $^ -I$(CUDA_INC_PATH) $(INCLUDE_FLAGS) $(LIBS) $(LINKER_FLAGS) -o $(BINDIR)$@


$(OBJDIR)%.o: $(SRCDIR)%.cc
$(OBJDIR)%.o: $(SRCDIR)%.cc $(DEPDIR)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

# Compile CUDA Source Files
%.cu.o: src/%.cu
	$(NVCC) $(NVCC_FLAGS) $(NVCC_GENCODES) -dc -o $@ $(NVCC_INCLUDE) $<

cuda.o: mapgen.cu.o noisegen.cu.o noise_helpers.cu.o
	$(NVCC) $(NVCC_FLAGS) $(NVCC_GENCODES) -dlink -o $@ $(NVCC_INCLUDE) $^

# To remove generated files
# This purposely does not remove the binary output
clean: 
	rm -f $(OBJDIR)*.o $(SRCDIR)*.gch

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

.PHONY: all clean

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCEFILES))))
