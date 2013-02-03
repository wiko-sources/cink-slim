#!/usr/bin/env python
'''Usages:
    %s option codebase project
    options:
    -m    merge the feature options 
    -r    refactor feature options
    -n    transform a flavor project to a base project
'''
import sys, pprint

def remove_trail(s):
    n = len(s)
    for i in range(n,0,-1):
        if s[i-1] > ' ':
            n = i; break
    return s[:n]

def parse_mk(f, m):
    f = open(f)
    while 1:
        l = f.readline()
        if l:
            a = l.split('=')
            if len(a) == 2:
                a[0] = a[0].replace(':', '')
                m[a[0].strip()] = remove_trail(a[1]).strip()
        else: break
    f.close()

a = sys.argv
l = len(a)

if l < 4 or a[1][0] != '-':
    print __doc__ % a[0]
    sys.exit()

kOp = a.pop(1)[1]
    
# list hierachy
aConfig = a[2].split('[')
if len(aConfig) == 2: aConfig[1] = a[2]
aConfig.insert(0, 'common')

m = {}
# determin platform
pConfig = '%s/mediatek/config/%s/ProjectConfig.mk'
parse_mk(pConfig % (a[1], aConfig[1]), m)
#pprint.pprint(m)

strPlatform = m.get('MTK_PLATFORM', '')
if strPlatform:
    aConfig.insert(1, strPlatform.lower())
    print 'MTK_PLATFORM', strPlatform

# merge configures
m1 = {}
if kOp != 'm':
    if kOp == 'n' and len(aConfig) == 4:
        k = -2
    else: k = -1
    for i in aConfig[:k]:
        parse_mk(pConfig % (a[1], i), m1)
    aConfig = aConfig[k:]

for i in aConfig:
    parse_mk(pConfig % (a[1], i), m)

m = m.items()
if m1:
    m1 = m1.items()
    m = list( set(m) - set(m1) )

m.sort( lambda a, b: cmp(a[0],b[0]) )

# output a configure file
if kOp == 'n': strProject = ''
else: strProject = aConfig[-1]

f = open(pConfig % (a[1], strProject), 'w')
for v in m:
    if v[0] not in ['MTK_BUILD_VERNO']:
        f.write('%s=%s\n' % v)
f.close()
