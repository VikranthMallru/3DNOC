CPP := g++
OUTPUT := ../build
INPUT := .

VERBOSE := 0
VERBOSE_NOLOG := 0
DEBUG := 0
RELEASE := 0
VERBOSE_LOG := 0
TEST_GEN := 0

CPPFLAGS := -I. -I./types -I./packet -I./config -I./port -I./structures -I./storage -I./controller -I./logger -I./integration -I./test -I./compression -I./multithreading -I./prefetcher -I./protocol -I./stats -I./topology -I./routing -DENABLE_PTHREADS=ON
LDFLAGS := -L. -lm 

ifeq	 ($(VERBOSE), 1)
	CFLAGS   := -w -Wall -O3 -D VERBOSE_LOG
	VERBOSE_LOG := 1
	TARGET := $(OUTPUT)/ucc_verbose
endif

ifeq	 ($(VERBOSE_NOLOG), 1)
	CFLAGS   := -w -Wall -O3 -g
	TARGET := $(OUTPUT)/ucc_verbose
endif

ifeq	 ($(DEBUG), 1)
	CFLAGS   := -g -w -Wall -D VERBOSE_LOG
	VERBOSE_LOG := 1
	TARGET := $(OUTPUT)/ucc_debug
endif

ifeq	 ($(RELEASE), 1)
	CFLAGS   := -w -Wall -O3
	TARGET := $(OUTPUT)/ucc_release
endif


SRC := $(wildcard compression/*.cpp config/*.cpp controller/*.cpp integration/*.cpp logger/*.cpp multithreading/*.cpp packet/*.cpp port/*.cpp prefetcher/*.cpp protocol/*.cpp routing/*.cpp stats/*.cpp storage/*.cpp structures/*.cpp test/*.cpp topology/*.cpp types/*.cpp)
OBJ := $(SRC:%.cpp=$(OUTPUT)/%.o)
all: $(TARGET)
	
$(TARGET): $(OBJ)
	$(CPP) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS)  $(OBJ) -o $(TARGET)

$(OUTPUT)/%.o: %.cpp | $(OUTPUT)
	@mkdir -p "$(@D)"
	$(CPP) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(OUTPUT):
	rm -rf $(OUTPUT)
	mkdir -p $(OUTPUT)

clean:
	rm -rf $(OUTPUT)
	mkdir -p $(OUTPUT)
