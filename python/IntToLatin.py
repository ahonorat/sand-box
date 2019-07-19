#! /usr/bin/python3

import sys

### Starts from 1
def intToLatin(i):
    if (i <= 0):
        raise Exception("Illegal non strictly positive arg.")
    alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    result = ""
    digit = 0
    rest = i
    while True: ## alternative to do while
        digit = (rest - 1) % 26
        rest = (rest - digit) // 26
        result = alphabet[digit] + result
        if (rest == 0):
            break
    return result


if __name__ == '__main__':
    if (len(sys.argv) != 2):
        raise Exception("One positive integer arg.")
    e = sys.argv[1]
    s = intToLatin(int(e))
    print("{} --> {}".format(e,s))
