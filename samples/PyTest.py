# This Python file uses the following encoding: utf-8

# if__name__ == "__main__":
#     pass
def printCose(cose):
    print("This is a static string")
    print("The passed param is: ", cose)
    a = str(cose).upper()
    return a


def init():
    print("Called init")


def deinit():
    print("Called deinit")


def setParam(cose):
    print("This is a static string")
    print("The passed param is: ", cose)


def upper(cose):
    print("This is a static string")
    print("The passed param is: ", cose)
    a = str(cose).upper()
    return a


def swapp(a, b):
    return str(b) + " " + str(a)


def shiftR3(a, b, c):
    return str(c) + " " + str(a) + " " + str(b)


def sum(a,b):
    return a+b


def negate(a):
    return not a


def lengthy_func(a):
    import time
    time.sleep(a)
    return a + 42
