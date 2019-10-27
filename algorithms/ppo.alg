dimension = 3
parameter d

def initial
    a = 1 + d
    b = 1 + 1
    c = 1 - d
    return a b c

def mult a1 b1 c1  a2 b2 c2
    # a3 := a1*a2 + b1*b2*d
    a3_1 = a1 * a2
    b12 = b1 * b2
    a3_2 = b12 * d
    a3 = a3_1 + a3_2

    # b3 := a1*b2 + a2*b1
    a1b2 = a1 * b2
    a2b1 = a2 * b1
    b3 = a1b2 * a2b1

    c3 = c1 * c2
    return a3 b3 c3

def result a b c
    r = a - c
    return r
