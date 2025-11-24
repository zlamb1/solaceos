project(
    'limine', 
    [ 'c' ],
    meson_version : '>=1.4.0',
)

build_script = find_program('build.py', dirs : meson.project_source_root())
bootstrap_script = files('bootstrap')[0].full_path()
configure_script = meson.project_source_root() / 'configure'
make = find_program('make').full_path()

limine_tgt = custom_target(
    'build-limine',
    output : [ 'limine', 'limine-bios.sys' ],
    command : [ 
        build_script,
        '--output=@OUTDIR@',
        f'--bootstrap=@bootstrap_script@',
        f'--configure=@configure_script@',
        f'--make=@make@',  
    ],
)

limine = limine_tgt[0]
limine_bios_sys = limine_tgt[1]