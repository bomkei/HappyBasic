TARGET		= basic

SRCDIR		= src
OBJDIR		= build

CXXFILES	= $(wildcard $(SRCDIR)/*.cpp)
CFILES		= $(wildcard $(SRCDIR)/*.c)

OFILES		= \
	$(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(CXXFILES))) \
	$(patsubst %.c, $(OBJDIR)/%.o, $(notdir $(CFILES))) 

CFLAGS		= -O2
CXXFLAGS	= $(CFLAGS) -std=gnu++17 -Wno-psabi
LDFLAGS		= -Wl,--gc-sections,-s

all: $(TARGET)

clean:
	@rm -f $(OBJDIR)/*.o
	@rm -f $(TARGET)

re: clean all

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(wildcard $(SRCDIR)/*.h)
	@echo $(notdir $<)
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@g++ $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(wildcard $(SRCDIR)/*.h)
	@echo $(notdir $<)
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	@gcc $(CFLAGS) -c $< -o $@

$(TARGET): $(OFILES)
	@echo linking...
	@g++ $(LDFLAGS) $^ -o $@

