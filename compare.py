#!/usr/bin/python3
import os
import subprocess
import sys
import time

time.sleep(0)  # Sleep to ensure all books are already written

files = ['first', 'second', 'third', 'fourth', 'fifth', 'sixth', 'seventh', 'eighth',
         'ninth', 'tenth']
files = [f"resources/{name}_book.txt" for name in files]

outputs = ["00", "01", "02", "03", "04", "05", "06", "07", "08", "09"]
outputs = [f"book_{name}.txt" for name in outputs if os.path.exists(f"book_{name}.txt")]


def get_title(file_name):
    # print(f"Reading {file_name}")
    with open(file_name, 'r') as file:
        result = file.readline()
    return result


resource_titles = {get_title(key): key for key in files}
output_titles = {get_title(key): key for key in outputs}

if __name__ == "__main__":
    fail = 0
    for key in output_titles:
        try:
            output = subprocess.call(
                ["diff", resource_titles[key], output_titles[key]],
                stdout=subprocess.DEVNULL
            )
            if output != 0:
                fail += 1
                print(f"Files diff: {resource_titles[key]}, {output_titles[key]}")
        except Exception as e:
            print(f"Files diff: {resource_titles[key]}, {output_titles[key]}")
            print(e)
    if fail != 0:
        sys.exit(1)
