import sys
import subprocess
import glob
import os

print "Python files"
print

for fname in glob.glob("py-test-files/*.py"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "runner.py", "-py", fname])

print
print "C++ files"
print

for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "runner.py", "-cpp", fname])


