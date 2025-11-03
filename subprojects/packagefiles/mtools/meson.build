project(
    'mtools', 
    'c', 
    meson_version : '>= 1.6.0',
    version : '4.0.49',
)

make = find_program('make')

build_script = find_program('mtools.py', dirs : meson.project_source_root())
configure_script = files('configure')[0]

build_dir = meson.project_build_root()

mtools_tgt = custom_target(
    'build-mtools',
    output : [ 'mcopy', 'mmd' ],
    command : [ build_script, make, configure_script.full_path(), '@OUTDIR@' ],
)

mcopy = mtools_tgt[0]
mmd = mtools_tgt[1]