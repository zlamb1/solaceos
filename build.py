#!/usr/bin/python3

import os
import subprocess
import sys

args = sys.argv

if len(args) < 9:
    sys.exit(1)

# in terms of MB
size = 64
offset = 1
offset_s = str(offset)

parted = args[1]
limine = args[2]
mcopy = args[3]
mmd = args[4]
limine_bios_sys = args[5]
limine_conf = args[6]
kernel_elf = args[7]
output_img = args[8]

tmp_img = 'tmp.img'

output_file = open(output_img, 'wb')
output_file.seek(1024 * 1024 * offset)
output_file.write(b'\0')
output_file.close()

tmp_file = open(tmp_img, 'wb')
tmp_file.seek(1024 * 1024 * (size - offset))
tmp_file.write(b'\0')
tmp_file.close()

proc = subprocess.run(['mkfs.fat', '-F', '32', tmp_img])
if proc.returncode != 0:
    sys.exit(proc.returncode)

output_file = open(output_img, 'r+b')
output_file.seek(1024 * 1024 * offset)
tmp_file = open(tmp_img, 'rb')
output_file.write(tmp_file.read())
output_file.close()
tmp_file.close()
os.remove(tmp_img)

proc = subprocess.run([parted, '-s', output_img, '--', 'mklabel', 'msdos', \
    'mkpart', 'primary', 'fat32', offset_s + 'MB', '-1s', 'set', '1', 'boot', 'on'])
if proc.returncode != 0:
    sys.exit(proc.returncode)

proc = subprocess.run([limine, 'bios-install', output_img])
if proc.returncode != 0:
    sys.exit(proc.returncode)

def mcopyfile(src, dst):
    proc = subprocess.run([mcopy, '-i', output_img + '@@' + offset_s + 'M', src, dst])
    if proc.returncode != 0:
        sys.exit(proc.returncode)

proc = subprocess.run([mmd, '-i', output_img + '@@' + offset_s + 'M', '::/boot'])
if proc.returncode != 0:
    sys.exit(proc.returncode)

mcopyfile(limine_bios_sys, '::/boot/')
mcopyfile(limine_conf, '::/boot/')
mcopyfile(kernel_elf, '::/boot/')

sys.exit(0)