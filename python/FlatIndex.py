#! /usr/bin/python3

import sys
#import functools
import operator

### Last dimension major
def computeMuls(dims, nbDims):
    """Compute list of the successive dimension multiplications [1, Z, Z*Y, Z*Y*X, ...]"""
    muls = [1]
    for i in range(1, nbDims+1):
        if (dims[-i] < 1):
            raise Exception("All dimensions must be positive integers.")
        muls.append(muls[i-1]*dims[-i])
    return muls

### Last dimension major
def flatIndex(index, dims):
    """Compute each column index, counting from last dimension [..., Z]"""
    nbDims = len(dims)
    indexes = []
    muls = computeMuls(dims, nbDims)
    muls.reverse()
    if (index < 0 or index >= muls[0]):
        raise Exception("Wrong flat index (out of bounds)")
    for i in range(nbDims):
        #divisor = functools.reduce(operator.mul, dims[i+1:], 1)
        divisor = muls[i+1]
        indexes.append(index//divisor)
        index = index % divisor
    return indexes

if __name__ == '__main__':
    if (len(sys.argv) < 3):
        raise Exception("At least two positive integers args: <flat index> <first dimension range> [others dimension ranges]")
    print(flatIndex(int(sys.argv[1]), [int(x) for x in sys.argv[2:]]))
