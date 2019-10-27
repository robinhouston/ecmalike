# ECM on the Edwards curve (x^2 + y^2) * z^2 = z^4 + d * x^2 * y^2

dimension = 3
parameter x y
global d

def initial
    xx = x * x
    yy = y * y
    xxyy = xx * yy
    s = xx + yy
    t = s - 1
    d = t / xxyy

    return x y 1

def mult x1 y1 z1  x2 y2 z2
    A = x1 * z2
    B = y1 * z2
    C = z1 * x2
    D = z1 * y2
    E = A * B
    F = C * D
    G = E + F
    H = E - F

    AmC = A - C
    BmD = B - D
    AmC_BmD = AmC * BmD
    J = AmC_BmD + H

    ApD = A + D
    BpC = B + C
    ApD_BpC = ApD * BpC
    K = ApD_BpC - G

    GJ = G * J
    HK = H * K
    JK = J * K
    JKd = JK * d

    return GJ HK JKd

def result x y z
    return z
