OUTDIR ?= out
CROSSCC ?= ~/opt/cross/bin/x86_64-elf-gcc

WARNINGS := \
	-Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wstrict-prototypes \
	-Wmissing-prototypes -Wno-missing-braces -Wno-missing-field-initializers -Wbad-function-cast \
	-Winline -Wundef -Wunreachable-code -Wredundant-decls -Wfloat-equal -Wcast-align \
	-Wcast-qual -Wdeclaration-after-statement -Wmissing-include-dirs -Wnested-externs \
	-Wno-error=format -Wsequence-point -Wswitch -Wwrite-strings

CFLAGS := \
	-g -ffreestanding -fno-strict-aliasing -mcmodel=large \
	-mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow
LDFLAGS := $(CFLAGS) -nostdlib

ARCH     := x86_64
ARCHSRC  := src/arch/$(ARCH)
BOOTSRC  := $(ARCHSRC)/boot
ARCHSRCS := $(BOOTSRC)/boot.S $(ARCHSRC)/page.c
LDSCRIPT := $(BOOTSRC)/boot.ld

INCLUDES := include 
SRCFILES := $(ARCHSRCS) src/main.c
OBJFILES := $(patsubst %.src,$(OUTDIR)/%.o,$(addsuffix .src,$(basename $(SRCFILES))))
DEPFILES := $(OBJFILES:.o=.d)

KERNIMG := $(OUTDIR)/kernel.img
SYMFILE := $(OUTDIR)/kernel.sym

.PHONY: all clean

all: $(KERNIMG) $(SYMFILE)

clean:
	rm -rf $(OUTDIR)

$(SYMFILE): $(KERNIMG)
	objcopy --only-keep-debug $< $@

$(KERNIMG): $(OBJFILES) $(LDSCRIPT)
	$(CROSSCC) $(CFLAGS) $(LDFLAGS) -T $(LDSCRIPT) $(OBJFILES) -o $@

$(OUTDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CROSSCC) $(CFLAGS) $(addprefix -I,$(INCLUDES)) -c -MMD $< -o $@

$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CROSSCC) $(WARNINGS) $(CFLAGS) $(addprefix -I,$(INCLUDES)) -c -MMD $< -o $@

-include $(DEPFILES)