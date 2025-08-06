OUTDIR ?= out
CROSSCC ?= ~/opt/cross/bin/x86_64-elf-gcc

WARNINGS := \
	-Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wstrict-prototypes \
	-Wmissing-prototypes -Wno-missing-braces -Wno-missing-field-initializers -Wbad-function-cast \
	-Winline -Wundef -Wunreachable-code -Wredundant-decls -Wfloat-equal -Wcast-align \
	-Wcast-qual -Wdeclaration-after-statement -Wmissing-include-dirs -Wnested-externs \
	-Wno-error=format -Wsequence-point -Wswitch -Wwrite-strings

CFLAGS   := -g -ffreestanding -fno-strict-aliasing  \
	        -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow
CFLAGS32 := $(CFLAGS) -m32 -mregparm=3
CFLAGS64 := $(CFLAGS) -mcmodel=large

LDFLAGS   := -nostdlib
LDFLAGS32 := $(CFLAGS32) $(LDFLAGS) -Wl,-melf_i386
LDFLAGS64 := $(CFLAGS64) $(LDFLAGS)

SRCDIR  := src

ARCH    := x86_64
ARCHDIR := $(SRCDIR)/arch/$(ARCH)
BOOTDIR := $(ARCHDIR)/boot
LDRDIR  := $(SRCDIR)/ldr

INCDIRS := include 

LDROUT  := $(OUTDIR)/ldr
LDRSRCS := $(BOOTDIR)/boot.S  $(BOOTDIR)/div.c $(ARCHDIR)/elf.c $(ARCHDIR)/io.c $(ARCHDIR)/paging.c \
		   $(LDRDIR)/memblock.c $(LDRDIR)/loader.c $(LDRDIR)/kprint.c $(LDRDIR)/mmap.c $(LDRDIR)/fail.c \
		   $(SRCDIR)/string.c $(SRCDIR)/vga.c $(SRCDIR)/console.c $(SRCDIR)/print.c $(SRCDIR)/sort.c \
		   $(SRCDIR)/memblock.c
LDROBJS := $(patsubst %.src,$(LDROUT)/%.o,$(addsuffix .src,$(basename $(LDRSRCS))))
LDRDEPS := $(LDROBJS:.o=.d)
LDRSCRI := $(BOOTDIR)/boot.ld

KRNSRCS := $(ARCHDIR)/start.S $(ARCHDIR)/io.c $(SRCDIR)/kmain.c $(SRCDIR)/string.c \
           $(SRCDIR)/console.c $(SRCDIR)/vga.c $(SRCDIR)/print.c $(SRCDIR)/kprint.c
KRNOBJS := $(patsubst %.src,$(OUTDIR)/%.o,$(addsuffix .src,$(basename $(KRNSRCS))))
KRNDEPS := $(KRNOBJS:.o=.d)
KRNSCRI := $(ARCHDIR)/kern.ld

LDRNM   := ldr.img
LDRIMG  := $(OUTDIR)/$(LDRNM)
KRNNM   := krn.img
KRNIMG  := $(OUTDIR)/$(KRNNM)

QMUDIR  := src/qemu
GRBCFG  := $(QMUDIR)/grub.cfg
IMGDIR  := $(OUTDIR)/img
QMUIMG  := $(OUTDIR)/qmu.img

MKIMG   := $(QMUDIR)/mkimg

.PHONY: all qemu clean

all: $(LDRIMG) $(KRNIMG)

qemu: $(QMUIMG)

clean:
	rm -rf $(OUTDIR)

$(LDRIMG): $(LDROBJS) $(LDRSCRI)
	$(CROSSCC) $(LDFLAGS32) -T $(LDRSCRI) $(LDROBJS) -o $@

$(KRNIMG): $(KRNOBJS) $(KRNSCRI)
	$(CROSSCC) $(LDFLAGS64) -T $(KRNSCRI) $(KRNOBJS) -o $@

$(QMUIMG): $(GRBCFG) $(LDRIMG) $(KRNIMG)
	$(MKIMG) $(QMUIMG) $(IMGDIR) $(GRBCFG) $(LDRIMG) $(KRNIMG) $(LDRNM) $(KRNNM)

$(LDROUT)/%.o: %.S
	mkdir -p $(dir $@)
	$(CROSSCC) $(CFLAGS32) $(addprefix -I,$(INCDIRS)) -c -MMD $< -o $@

$(LDROUT)/%.o: %.c
	mkdir -p $(dir $@)
	$(CROSSCC) $(WARNINGS) $(CFLAGS32) $(addprefix -I,$(INCDIRS)) -c -MMD $< -o $@

$(OUTDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CROSSCC) $(CFLAGS64) $(addprefix -I,$(INCDIRS)) -c -MMD $< -o $@

$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CROSSCC) $(WARNINGS) $(CFLAGS64) $(addprefix -I,$(INCDIRS)) -c -MMD $< -o $@

-include $(LDRDEPS)
-include $(KRNDEPS)