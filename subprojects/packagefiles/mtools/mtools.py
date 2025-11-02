#!/usr/bin/python3

import os
import subprocess
import sys

args = sys.argv

if len(args) < 4:
    sys.exit(1)

os.chdir(args[3])

proc = subprocess.run([args[2]])
if proc.returncode != 0:
    sys.exit(proc.returncode)

proc = subprocess.run([args[1], '-j4'])
if proc.returncode != 0:
    sys.exit(proc.returncode)

sys.exit(0)