########## DEFINITIONS ##########
FLAVORS		:= .c .C .cc .c++ .cpp .cxx

INCLUDES	:= $(INCLUDES) $(foreach n,$(SUBDIRS),-I$(n))
SOURCES		:= $(SOURCES) $(foreach n,$(SUBDIRS),$(foreach f,$(FLAVORS),$(wildcard $(n)/*$(f))))

OBJECTS		:= $(OBJECTS) $(notdir $(SOURCES))
$(foreach f,$(FLAVORS),$(eval OBJECTS := $(patsubst %$(f),%.o,$(OBJECTS))))
DEPENDENCIES := $(patsubst %.o,%.d,$(OBJECTS))

ifneq ($(patsubst lib%.a,%,$(TARGET)),$(TARGET))
TYPE := static
else ifneq ($(patsubst lib%.so,%,$(TARGET)),$(TARGET))
TYPE := shared
else ifneq ($(TARGET),)
TYPE := binary
else
TYPE := object
endif

########## OPTIONS ##########
CXX			?= g++
CXXFLAGS	?= -O0 -g -Wall
CXXFLAGS	+= $(if $(filter shared,$(TYPE)),-fpic,) 
LDFLAGS		?= -Wl,-O1

AR			?= ar
RM			?= rm -vf
INSTALL		?= install -vD

PREFIX		?= $(DESTDIR)/usr/local
BINDIR		?= $(PREFIX)/bin
LIBDIR		?= $(PREFIX)/lib

########## RULES ##########

########## BINARY ##########
ifeq (binary,$(TYPE))

all: $(TARGET)
	@echo "Compiled $(TARGET)"
	@echo

$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)"
	$(CXX) -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBRARIES)
	@echo

install: $(TARGET)
	@echo "Installing $(TARGET)"
	$(INSTALL) $(TARGET) $(BINDIR)/$(TARGET)
	@echo

uninstall:
	@echo "Uninstalling $(TARGET)"
	$(RM) $(BINDIR)/$(TARGET)
	@echo

########## STATIC ##########
else ifeq (static,$(TYPE))

all: $(TARGET)
	@echo "Created $(TARGET)"
	@echo

$(TARGET): $(OBJECTS)
	@echo "Archiving $(TARGET)"
	$(AR) rcs $(TARGET) $(OBJECTS)
	@echo

########## SHARED ##########
else ifeq (shared,$(TYPE))

all: $(TARGET)
	@echo "Created $(TARGET)"
	@echo

$(TARGET): $(OBJECTS)
	@echo "Creating $(TARGET)"
	$(CXX) -shared -o $(TARGET) $(OBJECTS) $(LDFLAGS) $(LIBRARIES)
	@echo

install: $(TARGET)
	@echo "Installing $(TARGET)"
	$(INSTALL) $(TARGET) $(LIBDIR)/$(TARGET)
	@echo

uninstall:
	@echo "Uninstalling $(TARGET)"
	$(RM) $(LIBDIR)/$(TARGET)
	@echo

########## OBJECT ##########
else

all: $(OBJECTS)
	@echo "Created $(OBJECTS)"
	@echo

endif

define RULE_O =
$(1): $(2)
	@echo "Compiling $(1)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) -MMD -o $(1) -c $(2)
	@echo
endef

# dynamically generate rules for .o files
$(foreach n,$(shell seq 1 $(words $(SOURCES))),$(eval $(call RULE_O,$(word $(n),$(OBJECTS)),$(word $(n),$(SOURCES)))))

########## DEPENDENCIES ##########
ifneq "$(MAKECMDGOALS)" "clean"
ifneq "$(MAKECMDGOALS)" "distclean"
-include $(DEPENDENCIES)
endif
endif

########## CLEAN ##########
clean:
	@echo "Cleaning files"
	$(RM) $(OBJECTS) $(DEPENDENCIES)
	@echo

distclean: clean
	@echo "Removing $(TARGET)"
	$(RM) $(TARGET)
	@echo

.PHONY: all install uninstall clean distclean
