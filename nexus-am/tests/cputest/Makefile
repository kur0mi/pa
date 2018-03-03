include $(AM_HOME)/Makefile.check
.PHONY: all run clean latest $(ALL) show

ALL = $(basename $(notdir $(shell find tests/. -name "*.c")))

all: $(addprefix Makefile., $(ALL))
	@echo "" $(ALL)

$(ALL): %: Makefile.%

Makefile.%: tests/%.c latest
	@/bin/echo -e "NAME = $*\nSRCS = $<\nLIBS += klib\ninclude $${AM_HOME}/Makefile.app" > $@
	-@make -s -f $@ ARCH=$(ARCH) $(MAKECMDGOALS)
	-@rm -f Makefile.$*

run: all

clean: 
	rm -rf Makefile.* build/

latest:

show:
	@echo "target: $(ALL)"
	@echo "ARCH: $(ARCH)"
	@echo "goal: $(MAKECMDGOALS)"

