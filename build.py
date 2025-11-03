#!/usr/bin/python3

import os
import subprocess
import sys

TMP = 'tmp'

# in terms of MB
SIZE = 64
OFFSET = 1
OFFSET_S = str(OFFSET)

class flag:
    def __init__(self, long_name, default_value=None):
        self.long_name = long_name
        self.value = default_value

flags = {
    'parted': flag('parted', 'parted'),
    'limine': flag('limine', 'limine'),
    'mcopy': flag('mcopy', 'mcopy'),
    'mmd': flag('mmd', 'mmd'),
    'limine-bios-sys': flag('limine-bios-sys', 'limine-bios.sys'),
    'limine-conf': flag('limine-conf', 'limine.conf'),
    'output': flag('output', 'solace.img')
}

def usage():
    print(f'{args[0]} [OPTIONS] [KERNEL]', file=sys.stderr)
    sys.exit(1)

def error(msg):
    print(f'{args[0]}: error: {msg}', file=sys.stderr)
    sys.exit(1)

if __name__ == '__main__':
    args = sys.argv
    argc = len(args)

    if argc < 1:
        sys.exit(1)

    kernel = None
    i = 1

    while i < argc:
        arg = args[i]
        if arg[0] == '-':
            if len(arg) < 3 or arg[1] != '-':
                error(f'unrecognized flag: \'{arg}\'')
            split = arg[2:].split('=', maxsplit=1)
            flag_name = split[0]
            value = None
            if len(split) == 1:
                if i + 1 >= argc:
                    error(f'expected value after flag \'{arg}\'')
                value = args[i + 1]
                i += 1
            else:
                value = split[1]
            flag = None
            for key in flags:
                _flag = flags[key]
                if _flag.long_name == flag_name:
                    flag = _flag
                    break
            if flag is None:
                error(f'unrecognized flag: \'--{flag_name}\'')
            flag.value = value
        elif kernel is None:
            kernel = arg
        else:
            print(kernel)
            usage()
        i += 1

    if kernel is None:
        usage()

    parted = flags['parted'].value
    limine = flags['limine'].value
    mcopy = flags['mcopy'].value
    mmd = flags['mmd'].value
    limine_bios_sys = flags['limine-bios-sys'].value
    limine_conf = flags['limine-conf'].value
    output = flags['output'].value
    mkfs = 'mkfs.fat'

    output_file = open(output, 'wb')
    output_file.seek(1024 * 1024 * OFFSET - 1)
    output_file.write(b'\0')
    output_file.close()

    tmp_file = open(TMP, 'wb')
    tmp_file.seek(1024 * 1024 * (SIZE - OFFSET) - 1)
    tmp_file.write(b'\0')
    tmp_file.close()

    proc = subprocess.run([mkfs, '-F', '32', TMP])
    if proc.returncode != 0:
        sys.exit(proc.returncode)

    output_file = open(output, 'r+b')
    output_file.seek(1024 * 1024 * OFFSET)
    tmp_file = open(TMP, 'rb')
    output_file.write(tmp_file.read())
    output_file.close()
    tmp_file.close()
    os.remove(TMP)

    proc = subprocess.run([parted, '-s', output, '--', 'mklabel', 'msdos', \
        'mkpart', 'primary', 'fat32', OFFSET_S + 'MB', '-1s', 'set', '1', 'boot', 'on'])
    if proc.returncode != 0:
        sys.exit(proc.returncode)

    proc = subprocess.run([limine, 'bios-install', output])
    if proc.returncode != 0:
        sys.exit(proc.returncode)

    def mcopyfile(src, dst):
        proc = subprocess.run([mcopy, '-i', output + '@@' + OFFSET_S + 'M', src, dst])
        if proc.returncode != 0:
            sys.exit(proc.returncode)

    proc = subprocess.run([mmd, '-i', output + '@@' + OFFSET_S + 'M', '::/boot'])
    if proc.returncode != 0:
        sys.exit(proc.returncode)

    mcopyfile(limine_bios_sys, '::/boot/')
    mcopyfile(limine_conf, '::/boot/')
    mcopyfile(kernel, '::/boot/')

    sys.exit(0)