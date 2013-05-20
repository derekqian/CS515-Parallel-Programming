import os, sys

N = [1000,10000,100000]
T = [1,2,4,8]
for n in N:
    for t in T:
        cmd='./qsort_queue %d %d > log_%d_%d.txt' %(n,t,n,t)
        print cmd
        os.system(cmd)
