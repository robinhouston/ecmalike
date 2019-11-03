# Lucas sequences

dimension = 4
parameter p q

def initial
    return 0 1 p q

def mult a11 a12 a21 a22  b11 b12 b21 b22
    c11_1 = a11 * b11
    c11_2 = a12 * b22
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
    return a12
