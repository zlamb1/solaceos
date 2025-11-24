project(
    'SolaceOS',
    [ 'cpp', 'nasm' ],
    default_options : [
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

inc = include_directories('include')

srcs = files(
    f'@SRCDIR@/main.cpp'
)

linker_script = files(f'@SRCDIR@/kernel.ld')[0]

c_args = [
    '-ffreestanding',
    '-fno-exceptions',
    '-fno-rtti',
    '-fno-strict-aliasing',
    '-mcmodel=large',
    '-mno-red-zone',
    '-g',
]

link_args = [
    '-nostdlib',
    '-T', linker_script.full_path(),
]

nasm_args = [
    '-g',
    '-F', 'dwarf',
]

kernel = executable(
    'solace.elf',
    srcs,
    c_args : c_args,
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