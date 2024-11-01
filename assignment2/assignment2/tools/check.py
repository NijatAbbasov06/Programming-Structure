#!/usr/bin/env python3
import argparse
import os
import re
import sys
import tarfile
import shutil
import subprocess

FILES = ['sgrep.c', 'str.c', 'readme', 'EthicsOath.pdf']
PLACEHOLDER = '20009999'

def fatal(msg):
    print('[-] ERROR: {}'.format(msg))
    sys.exit(1)

def parse_args():
    p = argparse.ArgumentParser()
    p.add_argument('tar', help='a tar.gz file to check before submission')
    return p.parse_args()

def get_student_id():
    student_id = os.path.join(os.path.dirname(__file__), '../STUDENT_ID')
    if not os.path.exists(student_id):
        fatal('Cannot find STUDENT_ID')

    sid = open(student_id).read().strip()

    if sid == PLACEHOLDER:
        fatal('Please update STUDENT_ID to your student id')

    return sid

if __name__ == '__main__':
    tar = parse_args().tar

    # General check
    if not os.path.exists(tar):
        fatal('{} does not exist'.format(tar))

    sid = get_student_id()

    expected_filename = '{}_assign2.tar.gz'.format(sid)
    if os.path.basename(tar) != expected_filename:
        fatal('A file name is wrong (expected: {})'.format(expected_filename))
        sys.exit(1)

    if not tarfile.is_tarfile(tar):
        fatal('Not a tar file')
        sys.exit(1)

    # Check a tar.gz satisifes requirements
    tar = tarfile.open(tar)

    files = set(FILES)
    for f in tar:
        name = f.name
        if not name in files:
            fatal('{} is incorrectly included'.format(name))

        files.remove(name)

    if files:
        for name in files:
            fatal('{} should be included'.format(name))

    # Check whether wc209.c can be compiled with gcc209
    if not shutil.which('gcc'):
        fatal('Cannot find gcc')

    with open('.str.c', 'wb') as f:
        f.write(tar.extractfile('str.c').read())

    p = subprocess.run(['gcc', '-o', '.str', '.str.c' ])


    if p.returncode != 0:
        fatal('Cannot be compiled with gcc209')

    print('[+] Good to submit :)')
