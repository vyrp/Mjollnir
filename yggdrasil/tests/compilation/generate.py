#!/usr/bin/python

amount = 1000
with open('src/templates.cpp', 'w') as f:
    f.write('#include <map>\n')
    f.write('using namespace std;\n')
    f.write('typedef map<int, int> M1;\n')
    for i in xrange(1, amount):
        f.write('typedef map<M{0}, M{0}> M{1};\n'.format(i, i+1))
    f.write('int main(){ M' + str(amount) + ' tmp; }\n')
