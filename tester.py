import sys
import subprocess
import glob
import os

nodeToCount = "-For"
args = "void"

def testDir(lang, directory):
	for fname in glob.glob(directory):
		print
		print "RUNNING RUNNER ON ", fname
		subprocess.call(["python", "commonastLocal.py", lang, nodeToCount, args, fname])



print "Python files"
print

testDir("-py", "/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/py-test-files/*.py")

print
print "C++ files"
print

testDir("-cpp", "/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/*.cpp")

testDir("-cpp", "/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw1/*.cpp")

for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw2/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])

for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw3/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])

for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw4/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])


for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw5/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])

for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw6/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])


for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw7/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])

for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw8/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])

for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw9/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])

for fname in glob.glob("/mnt/c/Users/Elizabeth/Documents/spring2017/commonASTs/cpp-test-files/dataStructures/hw10/*.cpp"):
	print
	print "RUNNING RUNNER ON ", fname
	subprocess.call(["python", "commonastLocal.py", "-cpp", nodeToCount, args, fname])

