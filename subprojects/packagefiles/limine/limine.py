#!/usr/bin/python3

import os
import shutil
import subprocess
import sys

args = sys.argv

if len(args) < 5:
    sys.exit(1)

os.chdir(args[1])

proc = subprocess.run([args[2]])
if proc.returncode != 0:
    sys.exit(proc.returncode)

proc = subprocess.run([args[3], '--enable-bios'])
if proc.returncode != 0:
    sys.exit(proc.returncode)

proc = subprocess.run([args[4], '-j4'])
if proc.returncode != 0:
    sys.exit(proc.returncode)

shutil.copy2('bin/limine', '.')
shutil.copy2('bin/limine-bios.sys', '.')

sys.exit(0)