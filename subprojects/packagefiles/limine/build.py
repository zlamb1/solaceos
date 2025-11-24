#!/usr/bin/python3

import os, subprocess, shutil, sys

args = sys.argv

output_dir = None
bootstrap = None
configure = None
make = None

for arg in args[1:]:
    if arg[:2] == '--':
        pair = arg[2:].split('=', maxsplit=1)
        if len(pair) == 1:
            continue
        if pair[0] == 'output':
            output_dir = pair[1]
        if pair[0] == 'bootstrap':
            bootstrap = pair[1]
        if pair[0] == 'configure':
            configure = pair[1]
        if pair[0] == 'make':
            make = pair[1]

if output_dir == None:
    print('missing output directory', file=sys.stderr)
    sys.exit(1)

if bootstrap == None:
    print('missing bootstrap script', file=sys.stderr)
    sys.exit(1)

if make == None:
    print('missing make', file=sys.stderr)
    sys.exit(1)

if configure == None:
    print('missing configure script', file=sys.stderr)
    sys.exit(1)

os.chdir(output_dir)

bootstrap_proc = subprocess.run([ bootstrap ])

if bootstrap_proc.returncode != 0:
    sys.exit(bootstrap_proc.returncode)

configure_proc = subprocess.run([ configure, '--enable-bios' ])

if configure_proc.returncode != 0:
    sys.exit(configure_proc.returncode)

make_proc = subprocess.run([ make, '-j4' ])

if make_proc.returncode != 0:
    sys.exit(make_proc.returncode)

shutil.copy2('bin/limine', '.')
shutil.copy2('bin/limine-bios.sys', '.')