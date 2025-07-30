OUTDIR ?= out
CROSSCC ?= ~/opt/cross/bin/x86_64-elf-gcc

WARNINGS := \
	-Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wstrict-prototypes \
	-Wmissing-prototypes -Wno-missing-braces -Wno-missing-field-initializers -Wbad-function-cast \
	-Winline -Wundef -Wunreachable-code -Wredundant-decls -Wfloat-equal -Wcast-align \
	-Wcast-qual -Wdeclaration-after-statement -Wmissing-include-dirs -Wnested-externs \
	-Wno-error=format -Wsequence-point -Wswitch -Wwrite-strings

CFLAGS := \
	-g -ffreestanding -fno-strict-aliasing \
	-mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mno-3dnow
LDFLAGS := $(CFLAGS) -nostdlib -T src/boot/i386/boot.ld

ASM_SRCFILES := src/boot/i386/boot.S
SRCFILES := src/main.c 
OBJFILES := $(patsubst %.S,$(OUTDIR)/%.o,$(ASM_SRCFILES)) $(patsubst %.c,$(OUTDIR)/%.o,$(SRCFILES))
DEPFILES := $(OBJFILES:.o=.d)

KERNIMG := $(OUTDIR)/kernel.img
SYMFILE := $(OUTDIR)/kernel.sym

.PHONY: all clean

all: $(KERNIMG)

clean:
	rm -rf $(OUTDIR)

$(KERNIMG): $(OBJFILES)
	$(CROSSCC) $(CFLAGS) $(LDFLAGS) $(OBJFILES) -o $@
	objcopy --only-keep-debug $@ $(SYMFILE)

$(OUTDIR)/%.o: %.S
	mkdir -p $(dir $@)
	$(CROSSCC) $(CFLAGS) -c -MMD $< -o $@

$(OUTDIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CROSSCC) $(WARNINGS) $(CFLAGS) -c -MMD $< -o $@

-include $(DEPFILES)