import random
with open("test2.csv",'w') as f:
    for i in range(1000):
        for j in range(9):
            a = random.randrange(-100,101)
            if a != -100:
               s = str(a)
               f.write(s)
            f.write(',')
        a = random.randrange(-100,101)
        if a != -100:
            s = str(a)
            f.write(s)
        f.write("\r")


