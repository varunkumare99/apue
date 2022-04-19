CC = cc
# Makefile settings - Can be customized.
APPNAME = ls
EXT = .c
SRCDIR = src
OBJDIR = obj
DEPDIR = dep

SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEP = $(OBJ:$(OBJDIR)/%.o=$(DEPDIR)/%.d)

# UNIX-based OS variables & settings
RM = rm -f
DELOBJ = $(OBJ)

#all:
#	@echo "varun"
#	@echo "src=$(SRC)"
#	@echo "obj values = $(OBJ)"
#	@echo "pwd = $(PWD)" 
#	@echo "CFLAGS = $(CFLAGS)" 

all: $(APPNAME)
# Builds the app
$(APPNAME): $(OBJ)
	$(CC) -o $@ $^ 

# Creates the dependecy rules
%.d: $(SRCDIR)/%$(EXT)
	@$(CPP) $(CFLAGS) $< -MMD -MP -MT $(@:%.d=$(OBJDIR)/%.o) >$@

# Includes all .h files
-include $(DEP)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
	@mkdir -p $(@D)
	$(CC) -o $@ -c $<

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project
.PHONY: clean
clean:
	$(RM) $(DELOBJ) $(APPNAME)

# Cleans only all files with the extension .d
.PHONY: cleandep
cleandep:
	$(RM) $(DEP)
