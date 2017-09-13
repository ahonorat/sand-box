#! /usr/bin/python3


def collatzGen(mul, div, plus, maxIter, a):
    count = 1
    while a != 1 and count < maxIter:
        if a % div != 0:
            a = a*mul + plus
        else:
            a = a/div
        count+=1
    if count == maxIter:
        print("{} max iteration reached ({},{},{})".
                  format(x, mul, div, plus))

def fillDoubles(listToFill, nb, maxi):
    a = nb
    while a < maxi:
        if a not in listToFill:
            listToFill.append(a)
        else:
            print("{} already in the list !!".format(a))
        a = 2*a
        
def collatzRange(maxi):
    l = []
    fillDoubles(l,1,maxi)
    for i in range(3, maxi+1):
        if i not in l:
            a = i
            newNbs = []
            reducedNewNbs = []
            steps = []
            while a not in l:
                newNbs.append(a)
                reducedNewNbs.append(a)
                if a % 2 != 0:
                    a = a*3+1
                    steps.append("m")
                else:
                    a = a//2
                    reducedNewNbs.pop()
                    steps.append("d")
            print("{} discovered the following numbers and terminated at number {}.".
                  format(i,a))
            print("\t"+str(newNbs))
            print("\tIt nessecitated the following steps:")
            print("\t"+str(steps))
            for x in reducedNewNbs:
                fillDoubles(l, x, maxi)
            
        
if __name__ == '__main__':
    nbMaxIter = 1000
    nbMaxRange = 100
#    for x in range(1,100):
#        collatz(3,2,1,nbMaxIter,x)
    collatzRange(nbMaxRange)
