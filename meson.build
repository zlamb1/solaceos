project(
    'SolaceOS',
    [ 'cpp', 'nasm' ],
    default_options : [
        'cpp_std=c++20',
        'warning_level=3',
        'werror=true',
    ],
    meson_version : '>=1.4.0',
    version : '0.1',
)

if not meson.is_cross_build()
    error('must cross compile (use x86_64-elf.ini)')
endif

SRCDIR = 'src'

cpp_args = [
    '-ffreestanding',
    '-fno-exceptions',
    '-fno-rtti',
    '-fno-strict-aliasing',
    '-fno-unwind-tables',
    '-mcmodel=large',
    '-mgeneral-regs-only',
    '-mno-red-zone',
    '-g',
]

linker_script = files(f'@SRCDIR@/kernel.ld')[0]

link_args = [
    '-nostdlib',
    '-T', linker_script.full_path(),
]

nasm_args = [
    '-g',
    '-F', 'dwarf',
]

inc = include_directories('include')

srcs = files(
    f'@SRCDIR@/gfx/fb.cpp',
    f'@SRCDIR@/gfx/psf.cpp',
    f'@SRCDIR@/gfx/vconsole.cpp',
    f'@SRCDIR@/abi.cpp',
    f'@SRCDIR@/io.cpp',
    f'@SRCDIR@/main.cpp',
)

ARCHDIR = f'@SRCDIR@/arch/x86'

arch_srcs = files(
    f'@ARCHDIR@/arch.cpp',
    f'@ARCHDIR@/atomic.cpp',
    f'@ARCHDIR@/irq.cpp',
)

srcs += arch_srcs

build_font = custom_target(
    input : files('assets/consolefont.psf'),
    output : 'consolefont.psf',
    command : [
        find_program('cp'),
        '@INPUT@', '@OUTPUT@',
    ],
)

font_obj = custom_target(
    input : build_font,
    output : 'consolefont.o',
    command : [
        find_program('objcopy'),
        '-O', 'elf64-x86-64',
        '-B', 'i386',
        '-I', 'binary',
        '@INPUT@', '@OUTPUT@',
    ]
)

srcs += font_obj

kernel = executable(
    'solace.elf',
    srcs,
    cpp_args : cpp_args,
    include_directories : inc,
    link_args : link_args,
    link_depends : [ linker_script ],
    nasm_args : nasm_args,
)

bios_build = find_program('build-scripts/build-bios.py', dirs : meson.project_source_root())

limine = subproject('limine')
limine_conf = files('src/limine.conf')[0]

mcopy = find_program('mcopy')
mmd = find_program('mmd')

custom_target(
    'bios-img',
    input : [  
        limine.get_variable('limine_bios_sys'), 
        limine_conf,
        kernel, 
    ],
    output : [ 'solace.img' ],
    command : [ 
        bios_build, 
        '--parted', find_program('parted'), 
        '--limine', limine.get_variable('limine'), 
        '--mcopy', mcopy, 
        '--mmd', mmd, 
        '--limine-bios-sys', limine.get_variable('limine_bios_sys'), 
        '--limine-conf', limine_conf,
        '--output', '@OUTPUT@', 
        kernel, 
    ],
)