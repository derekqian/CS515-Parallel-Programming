import os, sys, subprocess


params = sys.argv # print params[1]

if len(params)==1:
    REPEATNUM = 10

    f = open('report/data.txt','w+')
    f.write('PRIME:\n')
    N = [10,100,1000,10000,100000,1000000,10000000,100000000]
    f.write('N = '+str(N)+'\n')
    T = [1,2,4,8,16,32,64,128]
    f.write('T = '+str(T)+'\n')
    PRIME_SEQ=[]
    for i in range(len(N)):
        PRIME_SEQ.append(1000000)
    PRIME_OMP = []
    for i in range(len(T)):
        PRIME_OMP.append([])
        for j in range(len(N)):
            PRIME_OMP[i].append(1000000)
    for n in range(0,len(N)):
        for i in range(0,REPEATNUM):
            # Note: time output its message into stderr instead of stdout
            #(myin,myout,myerr) = os.popen3('time ./qsort_queue 100 32')
            process = subprocess.Popen('time ./prime_seq %d'%N[n],stderr=subprocess.PIPE,shell=True)
            str1 = process.communicate()[1].split('\n')
            for substr in str1:
                if 'real' in substr:
                    break
            str1 = substr.split()
            print str1[1]
            if float(str1[1])<PRIME_SEQ[n]:
                PRIME_SEQ[n]=float(str1[1])
        for t in range(0,len(T)):
            os.putenv('OMP_NUM_THREADS','%d'%T[t])
            os.system('echo python: set OMP_NUM_THREADS = $OMP_NUM_THREADS')
            for i in range(0,REPEATNUM):
                process = subprocess.Popen('time ./prime_omp %d'%N[n],stderr=subprocess.PIPE,shell=True)
                str1 = process.communicate()[1].split('\n')
                for substr1 in str1:
                    if 'real' in substr1:
                        break
                str1 = substr1.split()
                print str1[1]
                if float(str1[1])<PRIME_OMP[t][n]:
                    PRIME_OMP[t][n]=float(str1[1])
    print PRIME_SEQ
    f.write('PRIME_SEQ = \n')
    f.write('[')
    for i in range(len(PRIME_SEQ)):
        f.write('%3.1f, '%PRIME_SEQ[i])
    f.write(']\n')
    print PRIME_OMP
    f.write('PRIME_OMP = \n')
    for i in range(len(PRIME_OMP)):
        f.write('[')
        for j in range(len(PRIME_OMP[0])):
            f.write('%3.1f, '%PRIME_OMP[i][j])
        f.write(']\n')
    f.write('\n')

    f.write('QSORT:\n')
    N = [10,100,1000,10000,100000,1000000,10000000]
    f.write('N = '+str(N)+'\n')
    T = [1,2,4,8,16,32,64,128]
    f.write('T = '+str(T)+'\n')
    QSORT_SEQ=[]
    for i in range(len(N)):
        QSORT_SEQ.append(1000000)
    QSORT_OMP = []
    for i in range(len(T)):
        QSORT_OMP.append([])
        for j in range(len(N)):
            QSORT_OMP[i].append(1000000)
    QSORT_QUEUE = []
    for i in range(len(T)):
        QSORT_QUEUE.append([])
        for j in range(len(N)):
            QSORT_QUEUE[i].append(1000000)
    for n in range(0,len(N)):
        for i in range(0,REPEATNUM):
            process = subprocess.Popen('time ./qsort_seq %d'%N[n],stderr=subprocess.PIPE,shell=True)
            str1 = process.communicate()[1].split('\n')
            for substr1 in str1:
                if 'real' in substr1:
                    break
            str1 = substr1.split()
            print str1[1]
            if float(str1[1])<QSORT_SEQ[n]:
                QSORT_SEQ[n]=float(str1[1])
        for t in range(0,len(T)):
            os.putenv('OMP_NUM_THREADS','%d'%T[t])
            os.system('echo python: set OMP_NUM_THREADS = $OMP_NUM_THREADS')
            for i in range(0,REPEATNUM):
                process = subprocess.Popen('time ./qsort_omp %d'%N[n],stderr=subprocess.PIPE,shell=True)
                str1 = process.communicate()[1].split('\n')
                for substr1 in str1:
                    if 'real' in substr1:
                        break
                str1 = substr1.split()
                print str1[1]
                if float(str1[1])<QSORT_OMP[t][n]:
                    QSORT_OMP[t][n]=float(str1[1])
        for t in range(0,len(T)):
            for i in range(0,REPEATNUM):
                process = subprocess.Popen('time ./qsort_queue %d %d'%(N[n],T[t]),stderr=subprocess.PIPE,shell=True)
                str1 = process.communicate()[1].split('\n')
                for substr1 in str1:
                    if 'real' in substr1:
                        break
                str1 = substr1.split()
                print str1[1]
                if float(str1[1])<QSORT_QUEUE[t][n]:
                    QSORT_QUEUE[t][n]=float(str1[1])
    print QSORT_SEQ
    f.write('QSORT_SEQ = \n')
    f.write('[')
    for i in range(len(QSORT_SEQ)):
        f.write('%3.1f, '%QSORT_SEQ[i])
    f.write(']\n')
    print QSORT_OMP
    f.write('QSORT_OMP = \n')
    for i in range(len(QSORT_OMP)):
        f.write('[')
        for j in range(len(QSORT_OMP[0])):
            f.write('%3.1f, '%QSORT_OMP[i][j])
        f.write(']\n')
    print QSORT_QUEUE
    f.write('QSORT_QUEUE = \n')
    for i in range(len(QSORT_QUEUE)):
        f.write('[')
        for j in range(len(QSORT_QUEUE[0])):
            f.write('%3.1f, '%QSORT_QUEUE[i][j])
        f.write(']\n')
    f.close()
elif len(params)==2:
    if params[1]=='debug':
        #os.system('time ./prime_seq 100')
        #os.putenv('OMP_NUM_THREADS','4')
        #os.system('echo python: set OMP_NUM_THREADS = $OMP_NUM_THREADS')
        #os.system('time ./prime_omp 1000')
        #os.putenv('OMP_NUM_THREADS','8')
        #os.system('echo python: set OMP_NUM_THREADS = $OMP_NUM_THREADS')
        #os.system('time ./prime_omp 1000')
        #os.system('time ./qsort_seq 100')
        #os.putenv('OMP_NUM_THREADS','32')
        #os.system('echo python: set OMP_NUM_THREADS = $OMP_NUM_THREADS')
        #os.system('time ./qsort_omp 100')
        os.system('time ./qsort_queue 100 32')
    else:
        print 'Usage: python test.py [debug]'
else:
    print 'Usage: python test.py [debug]'


