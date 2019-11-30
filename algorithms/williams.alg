# Williams’ version of the p+1 algorithm

dimension = 4
parameter A

def initial
    minus_one = 0 - 1
    return 0 1 minus_one A

def mult a11 a12 a21 a22  b11 b12 b21 b22
    c11_1 = a11 * b11
    c11_2 = a12 * b21
    c11 = c11_1 + c11_2

    c12_1 = a11 * b12
    c12_2 = a12 * b22
    c12 = c12_1 + c12_2

    c21_1 = a21 * b11
    c21_2 = a22 * b21
    c21 = c21_1 + c21_2

    c22_1 = a21 * b12
    c22_2 = a22 * b22
    c22 = c22_1 + c22_2

    return c11 c12 c21 c22

def result a11 a12 a21 a22
    a11_times_2 = a11 + a11
    A_times_a12 = A * a12
    r = a11_times_2 + A_times_a12
    r' = r - 1
    r'' = r' - 1
    return r''
