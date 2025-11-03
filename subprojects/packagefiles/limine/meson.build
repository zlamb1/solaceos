project(
    'limine', 
    'c', 
    meson_version : '>= 1.6.0',
)

make = find_program('make')

build_script = find_program('limine.py', dirs : meson.project_source_root())
bootstrap_script = files('bootstrap')[0]

limine_tgt = custom_target(
    'build-limine',
    output : [ 'limine', 'limine-bios.sys' ],
    command : [ build_script, '@OUTDIR@', bootstrap_script.full_path(), meson.project_source_root() / 'configure', make ],
)

limine = limine_tgt[0]
limine_bios_sys = limine_tgt[1]