from copperhead import *
from pandas import *
from pandas.util.testing import rands
import random

N = 1000000
ngroups = 100


def get_test_data(ngroups=100, n=N):
    unique_groups = range(ngroups)
    arr = np.asarray(np.tile(unique_groups, n / ngroups), dtype=object)

    if len(arr) < n:
        arr = np.asarray(list(arr) + unique_groups[:n - len(arr)],
                         dtype=object)

    random.shuffle(arr)
    return arr

# aggregate multiple columns
# df = DataFrame({'key1' : get_test_data(ngroups=ngroups),
#                 'key2' : get_test_data(ngroups=ngroups),
#                 'data1' : np.random.randn(N),
#                 'data2' : np.random.randn(N)})

# df2 = DataFrame({'key1'  : get_test_data(ngroups=ngroups, n=N//10),
#                  'key2'  : get_test_data(ngroups=ngroups//2, n=N//10),
#                  'value' : np.random.randn(N // 10)})
# result = merge.merge(df, df2, on='key2')

from collections import defaultdict
import gc
import time
from pandas.util.testing import rands

@cu
def bench():
    N = 10000

    indices = np.array([rands(10) for _ in xrange(N)], dtype='O')
    indices2 = np.array([rands(10) for _ in xrange(N)], dtype='O')
    key = np.tile(indices[:8000], 10)
    key2 = np.tile(indices2[:8000], 10)

    left = DataFrame({'key': key, 'key2': key2,
                      'value': np.random.randn(80000)})
    right = DataFrame({'key': indices[2000:], 'key2': indices2[2000:],
                       'value2': np.random.randn(8000)})

    right2 = right.append(right, ignore_index=True)

    join_methods = ['inner', 'outer', 'left', 'right']
    results = DataFrame(index=join_methods, columns=[False, True])
    niter = 10
    for sort in [False, True]:
        for join_method in join_methods:
            f = lambda: merge(left, right, how=join_method, sort=sort)
            gc.disable()
            start = time.time()
            for _ in xrange(niter):
                f()
            elapsed = (time.time() - start) / niter
            print "elapsed: %s" %(elapsed)
            gc.enable()
            results[sort][join_method] = elapsed
    # results.columns = ['pandas']
    results.columns = ['dont_sort', 'sort']


if __name__ == '__main__':
    bench()
    #print 'on cpu'
    #with places.here:
        #bench()
#
    #print 'on gpu'
    #with places.gpu0:
        #bench()


