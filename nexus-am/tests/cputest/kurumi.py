#!/usr/bin/python
'''
README:
    check the assemble result with your decoder and nexus-am's decoder
    please make sure tested programs can run succeesfully first!
    please in nexus-am/tests/cputest directory!
'''
import sys
import os
import re


ss = ""
if len(sys.argv) != 2:
    print "script need one arg!"
    print "eg: \n\tkurumi.py dummy"
    sys.exit()
else:
    ss = sys.argv[1]


with open("input", "w") as f:
    f.write("c\nq\n")


filename1 = "build/" + ss + "-x86-nemu.txt"
pat1 = re.compile(r"([0-9a-f]{6}):\s+(([0-9a-f]{2}\s+)+)")
#filename2 = ss + ".txt"
#pat2 = re.compile()
#space = re.compile(r"\s+")
exe = "make run ALL=" + ss + " < input"
print "filename1:", filename1
#print "filename2:", filename2
print "exe:", exe


def coffee(text):
    res = pat1.findall(text)
    d = {}
    for x in res:
        d[x[0]] = x[1].strip()
    return d


with open(filename1) as f:
    s1 = f.read()
#print s1
d1 = coffee(s1)

s2 = "".join(os.popen(exe).readlines())
#print s2
#print "s2: \n"
d2 = coffee(s2)

print "error export... "
for x in d2:
    if d1[x] != d2[x]:
        print "correct:", d1[x]
        print "yours:  ", d2[x]
print "end."

