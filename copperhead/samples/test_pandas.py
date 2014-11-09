import pandas as pd
from copperhead import *

@cu
def f():
    s = pd.Series([1,3,5,np.nan,6,8])
    s = s * 2
    return s


print f()
