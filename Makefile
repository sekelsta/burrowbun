# what folders
SRCDIR = src/
OBJDIR = obj/
BINDIR = ./

DEPDIR := .d

all:
	python3 pymake.py

lint:
	python3 pymake.py lint

# To remove generated files
# This purposely does not remove the binary output
clean: 
	rm -rf $(DEPDIR) $(OBJDIR)

.PHONY: all clean lint
