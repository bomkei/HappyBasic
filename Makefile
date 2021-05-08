TARGET		= HappyBasic
EXT       =

SRCDIR		= src
INCLUDE   = src/include
OBJDIR		= build

CXXFILES	= \
  $(wildcard $(SRCDIR)/*.cpp) \
  $(wildcard $(SRCDIR)/parser/*.cpp)

OFILES		= $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(CXXFILES)))

CXXFLAGS	= -O1 -std=gnu++17 -Wno-psabi
LDFLAGS		= -Wl,--gc-sections

ifeq ($(OS), Windows_NT)
  EXT  = .exe
else
  EXT  =
endif

all: $(TARGET)

clean:
	@rm -f $(OBJDIR)/*.o
	@rm -f $(TARGET)

re: clean all

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard $(SRCDIR)/*.h) $(wildcard $(INCLUDE)/*.h)
	@echo $(notdir $<)
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@g++ $(CXXFLAGS) -c $< -o $@
  
$(OBJDIR)/%.o: $(SRCDIR)/parser/%.cpp $(wildcard $(SRCDIR)/*.h) $(wildcard $(INCLUDE)/*.h)
	@echo $(notdir $<)
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@g++ $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OFILES)
	@echo linking...
	@g++ $(LDFLAGS) $^ -o $@$(EXT)

