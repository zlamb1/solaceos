CROSS_CC := ~/opt/cross/bin/x86_64-elf-gcc
CROSS_OC := ~/opt/cross/bin/x86_64-elf-objcopy

WARNINGS := -Wall -Wextra -Werror -Wshadow -Wpointer-arith -Wstrict-prototypes \
			-Wmissing-prototypes -Wno-missing-braces -Wno-missing-field-initializers \
			-Wbad-function-cast -Winline -Wundef -Wunreachable-code -Wredundant-decls \
			-Wfloat-equal -Wcast-align -Wcast-qual -Wmissing-include-dirs -Wnested-externs \
			-Wno-error=format -Wsequence-point -Wswitch -Wwrite-strings
CFLAGS   := $(WARNINGS) -g -ffreestanding -fno-strict-aliasing \
			-mno-red-zone -mno-mmx -mno-sse2 -mno-3dnow -mcmodel=large
LDFLAGS  := -nostdlib

SRCDIR   := src
ASTDIR   := assets
LIMDIR   := limine
OUTDIR   := build
OSRCDIR  := $(OUTDIR)/$(SRCDIR)
OASTDIR  := $(OUTDIR)/$(ASTDIR)
PRTDIR   := $(OUTDIR)/boot
MNTDIR   := $(OUTDIR)/mnt

LIMBIN   := $(LIMDIR)/bin
LIMSYS   := $(LIMBIN)/limine-bios.sys
LIMCFG   := $(SRCDIR)/limine/limine.conf
LIMEXE   := $(LIMBIN)/limine

FNTS     := $(ASTDIR)/consolefont.psf
SRCS     := mem.c psf.c vcon.c print.c main.c
LNK      := $(SRCDIR)/k.ld
USRCS    := $(patsubst %.S,%.src,$(patsubst %.c,%.src,$(SRCS)))
OBJS     := $(patsubst %.src,$(OUTDIR)/$(SRCDIR)/%.o,$(USRCS))
OFNTS    := $(patsubst %.psf,$(OUTDIR)/%.o,$(FNTS))
DEPS     := $(OBJS:.o=.d)

KRNIMG   := $(OUTDIR)/kernel.img
OUTIMG   := $(OUTDIR)/bootdisk.img

ABSPATH  := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

.PHONY: all limine limine-clean clean

all: $(OUTIMG)

$(OUTIMG): $(LIMSYS) $(KRNIMG) | $(OUTDIR)
	dd if=/dev/zero of=$@ count=1 bs=64MB
	mkdir -p $(PRTDIR) $(MNTDIR)
	cp $(LIMSYS) $(PRTDIR)/limine-bios.sys
	cp $(LIMCFG) $(PRTDIR)/limine.conf
	cp $(KRNIMG) $(PRTDIR)/kernel.img
	parted -s $@ -- mklabel msdos mkpart primary fat32 1MB -1s set 1 boot on
# TODO: find a loopback dev
	sudo losetup -P /dev/loop0 $@; sudo mkfs.msdos -F 32 /dev/loop0p1; \
	sudo mount /dev/loop0p1 $(MNTDIR); sudo cp -r $(PRTDIR) $(MNTDIR)/boot; \
	sudo umount $(MNTDIR); sudo losetup -d /dev/loop0;
	rmdir $(MNTDIR)
	$(LIMEXE) bios-install $@

$(KRNIMG): $(OFNTS) $(OBJS) $(LNK) | $(OUTDIR)
	$(CROSS_CC) -T $(LNK) $(LDFLAGS) $(OFNTS) $(OBJS) -o $@

$(OUTDIR)/%.o: %.psf | $(OASTDIR)
# NOTE: cd into directory to prevent objcopy from prepending path to binary symbols
	cd $(dir $<); $(CROSS_OC) -O elf64-x86-64 -B i386 -I binary -L -- $(notdir $<) $(ABSPATH)/$@

$(OUTDIR)/%.o: %.c | $(OSRCDIR)
	$(CROSS_CC) -c -MMD -Isrc $(CFLAGS) $< -o $@

$(OUTDIR)/%.o: %.S | $(OSRCDIR)
	$(CROSS_CC) -c -MMD $(CFLAGS) $< -o $@

$(LIMSYS): $(LIMEXE)

$(LIMEXE): | $(LIMDIR)
	cd $(LIMDIR); ./bootstrap; ./configure --enable-bios; make -j4

$(OASTDIR): | $(OUTDIR)
	mkdir $@

$(OSRCDIR): | $(OUTDIR)
	mkdir $@

$(OUTDIR):
	mkdir $@

limine-clean:
	git submodule deinit --all -f
	git submodule update --init

clean:
	rm -rf $(OUTDIR)

-include $(DEPS)