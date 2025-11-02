project(
    'solaceos',
    ['nasm', 'c'],
    meson_version : '>= 1.6.0',
    version : '0.1',
    default_options : [ 'warning_level=3', 'c_std=c11', 'werror=true' ],
)

if not meson.is_cross_build()
    error('must cross compile (use x86_64-elf.ini)')
endif

objcopy = find_program('objcopy')

limine_proj = subproject('limine')
limine = limine_proj.get_variable('limine')
limine_bios_sys = limine_proj.get_variable('limine_bios_sys')
limine_conf = files('src/limine/limine.conf')

mtools_proj = subproject('mtools')
mcopy = mtools_proj.get_variable('mcopy')
mmd = mtools_proj.get_variable('mmd')

arch_srcs = files(
    'src/arch/x86_64/fence.c'
)

srcs = files(
    'src/main.c',
    'src/mem.c',
    'src/panic.c',
    'src/print.c',
    'src/psf.c',
    'src/vcon.c',
)

srcs += arch_srcs

linker_script = files('src/k.ld')[0]

build_font_tgt = custom_target(
    input : files('assets/consolefont.psf'),
    output : 'consolefont.psf',
    command : [
        find_program('cp'),
        '@INPUT@', '@OUTPUT@',
    ],
    build_by_default : true,
)

font_tgt = custom_target(
    input : build_font_tgt[0],
    output : 'consolefont.o',
    command : [
        objcopy,
        '-O', 'elf64-x86-64',
        '-B', 'i386',
        '-I', 'binary',
        '@INPUT@', '@OUTPUT@',
    ],
    build_by_default : true,
)

srcs += font_tgt

kernel_elf = executable(
    'solace.elf',
    srcs,
    c_args : [ '-g', '-ffreestanding', '-nostdlib', '-fno-strict-aliasing', '-mno-red-zone', '-mcmodel=large' ],
    include_directories : [ 'src' ],
    link_args : [ '-nostdlib', '-T', linker_script.full_path() ],
    link_depends : [ linker_script ],
    nasm_args : [ '-g', '-F', 'dwarf' ],
)

build_script = find_program('build.py', dirs : meson.project_source_root())

boot_img = custom_target(
    input : [ kernel_elf, limine_bios_sys, limine_conf ],
    output : [ 'solace.img' ],
    command : [ build_script, find_program('parted'), limine, mcopy, mmd, limine_bios_sys, limine_conf, kernel_elf, '@OUTPUT@' ],
    build_by_default : true,
)