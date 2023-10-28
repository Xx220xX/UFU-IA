from concurrent.futures import ThreadPoolExecutor

pool = ThreadPoolExecutor(200)


def func(b):
    k = 0
    for i in range(b):
        k += i
    return k


x = [int(1e9)] * 10000

for r in pool.map(func, x):
    print(r)
