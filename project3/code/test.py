import os, sys, subprocess

params = sys.argv 
# print params[1]

if len(params)==2 and params[1]=='open':
    REPEATNUM = 10
    HOSTS = ['african','chatham','chinstrap','erect-crested','snares',
             'galapagos','gentoo','king','kororaa','white-flipper',
             'little','macaroni','magellanic','rockhopper','royal',
             'yellow-eyed','emperor','fiordland']

    N = [1000,5000,10000,50000,100000,500000,1000000,5000000,10000000,50000000,100000000]
    #N = [500]
    for i in range(len(N)):
        os.system('./datagen %d > input_%d'%(N[i],N[i]))

    T = [1,2,4,8]
    #T = [1]
    for i in range(len(T)):
        f = open('lamhost_%d'%T[i],'w+')
        f.write('# lamhost for %d hosts\n'%T[i])
        for j in range(T[i]):
            f.write(HOSTS[j]+'\n')
        f.close()

    SAMPLE_SORT_WITHIO=[]
    SAMPLE_SORT_WITHOUTIO=[]
    for i in range(len(T)):
        SAMPLE_SORT_WITHIO.append([])
        SAMPLE_SORT_WITHOUTIO.append([])
        for j in range(len(N)):
            SAMPLE_SORT_WITHIO[i].append(1000000)
            SAMPLE_SORT_WITHOUTIO[i].append(1000000)
    f = open('report/data.txt','w+')
    f.write('N = '+str(N)+'\n')
    f.write('T = '+str(T)+'\n')
    for t in range(0,len(T)):
        for n in range(0,len(N)):
            for i in range(0,REPEATNUM):
                process = subprocess.Popen('mpirun -hostfile lamhost_%d ./sample_sort_mpi input_%d output_%d_%d'%(T[t],N[n],T[t],N[n]),stdout=subprocess.PIPE,shell=True)
                os.system('./verify output_%d_%d'%(T[t],N[n]))
                # print process.communicate()[0]
                str1 = process.communicate()[0].split('\n')
                # print str1
                substr1 = str1[0]
                substr2 = str1[1]
                str1 = substr1.split()
                str2 = substr2.split()
                print str1[1]
                print str2[1]
                if float(str1[1])<SAMPLE_SORT_WITHOUTIO[t][n]:
                    SAMPLE_SORT_WITHOUTIO[t][n]=float(str1[1])
                if float(str2[1])<SAMPLE_SORT_WITHIO[t][n]:
                    SAMPLE_SORT_WITHIO[t][n]=float(str2[1])
    print SAMPLE_SORT_WITHOUTIO
    print SAMPLE_SORT_WITHIO
    f.write('SAMPLE_SORT_WITHOUTIO = \n')
    for i in range(len(SAMPLE_SORT_WITHOUTIO)):
        f.write('[')
        for j in range(len(SAMPLE_SORT_WITHOUTIO[0])):
            f.write('%6.3f, '%SAMPLE_SORT_WITHOUTIO[i][j])
        f.write(']\n')
    f.write('\n')
    f.write('SAMPLE_SORT_WITHIO = \n')
    for i in range(len(SAMPLE_SORT_WITHIO)):
        f.write('[')
        for j in range(len(SAMPLE_SORT_WITHIO[0])):
            f.write('%6.3f, '%SAMPLE_SORT_WITHIO[i][j])
        f.write(']\n')
    f.write('\n')
    f.close()
elif len(params)==2 and params[1]=='lam':
    REPEATNUM = 1
    HOSTS = ['african','chatham','chinstrap','erect-crested','snares',
             'galapagos','gentoo','king','kororaa','white-flipper',
             'little','macaroni','magellanic','rockhopper','royal',
             'yellow-eyed','emperor','fiordland']

    #N = [500,1000,5000,10000,50000,100000,500000,1000000]
    N = [1000,5000,10000,50000,100000,500000,1000000,5000000,10000000,50000000,100000000]
    #N = [500]
    for i in range(len(N)):
        os.system('./datagen %d > input_%d'%(N[i],N[i]))

    T = [1,2,4,8,12]
    #T = [1]
    for i in range(len(T)):
        f = open('lamhost_%d'%T[i],'w+')
        f.write('# lamhost for %d hosts\n'%T[i])
        for j in range(T[i]):
            f.write(HOSTS[j]+'\n')
        f.close()

    SAMPLE_SORT_WITHIO=[]
    SAMPLE_SORT_WITHOUTIO=[]
    for i in range(len(T)):
        SAMPLE_SORT_WITHIO.append([])
        SAMPLE_SORT_WITHOUTIO.append([])
        for j in range(len(N)):
            SAMPLE_SORT_WITHIO[i].append(1000000)
            SAMPLE_SORT_WITHOUTIO[i].append(1000000)
    f = open('report/data.txt','w+')
    f.write('N = '+str(N)+'\n')
    f.write('T = '+str(T)+'\n')
    os.system('lamhalt')
    for t in range(0,len(T)):
        os.system('lamboot lamhost_%d'%T[t])
        for n in range(0,len(N)):
            for i in range(0,REPEATNUM):
                process = subprocess.Popen('mpirun.lam N sample_sort_mpi input_%d output_%d_%d'%(N[n],T[t],N[n]),stdout=subprocess.PIPE,shell=True)
                os.system('./verify output_%d_%d'%(T[t],N[n]))
                # print process.communicate()[0]
                str1 = process.communicate()[0].split('\n')
                # print str1
                substr1 = str1[0]
                substr2 = str1[1]
                str1 = substr1.split()
                str2 = substr2.split()
                print str1[1]
                print str2[1]
                if float(str1[1])<SAMPLE_SORT_WITHOUTIO[t][n]:
                    SAMPLE_SORT_WITHOUTIO[t][n]=float(str1[1])
                if float(str2[1])<SAMPLE_SORT_WITHIO[t][n]:
                    SAMPLE_SORT_WITHIO[t][n]=float(str2[1])
                os.system('lamclean')
        os.system('lamhalt')
    print SAMPLE_SORT_WITHOUTIO
    print SAMPLE_SORT_WITHIO
    f.write('SAMPLE_SORT_WITHOUTIO = \n')
    for i in range(len(SAMPLE_SORT_WITHOUTIO)):
        f.write('[')
        for j in range(len(SAMPLE_SORT_WITHOUTIO[0])):
            f.write('%6.3f, '%SAMPLE_SORT_WITHOUTIO[i][j])
        f.write(']\n')
    f.write('\n')
    f.write('SAMPLE_SORT_WITHIO = \n')
    for i in range(len(SAMPLE_SORT_WITHIO)):
        f.write('[')
        for j in range(len(SAMPLE_SORT_WITHIO[0])):
            f.write('%6.3f, '%SAMPLE_SORT_WITHIO[i][j])
        f.write(']\n')
    f.write('\n')
    f.close()
elif len(params)==2:
    if params[1]=='debug':
        #os.putenv('OMP_NUM_THREADS','32')
        #os.system('echo python: set OMP_NUM_THREADS = $OMP_NUM_THREADS')
        #os.system('time ./qsort_omp 100')
        os.system('echo debug')
    else:
        print 'Usage: python test.py [open]/[lam]'
else:
    print 'Usage: python test.py [open]/[lam]'

f = open('report/data.txt','r+')
str1 = f.readlines()
f.close()
# print str1
withio = []
withoutio = []
for i in range(len(str1)):
    if i>2 and i<7:
        str1[i] = str1[i].replace('[',' ')
        str1[i] = str1[i].replace(']',' ')
        str1[i] = str1[i].replace(',',' ')
        str1[i] = str1[i].replace('\n',' ')
        print str1[i].split()
        withio.append(str1[i].split())
    if i>8 and i<13:
        str1[i] = str1[i].replace('[',' ')
        str1[i] = str1[i].replace(']',' ')
        str1[i] = str1[i].replace(',',' ')
        str1[i] = str1[i].replace('\n',' ')
        print str1[i].split()
        withoutio.append(str1[i].split())
print 'result:'
print withio
print withoutio

f = open('report/table.tex','w+')
for i in range(len(withio)):
    f.write('                               ')
    for j in range(len(withio[0])-1):
        f.write('& \cellcolor[gray]{0.9}{'+withio[i][j]+'} ')
    f.write('\\\\\n')

    f.write('\\raisebox{1.5ex}[0pt]{Node\#%d} '%2**i)
    for j in range(len(withoutio[0])-1):
        f.write('& \cellcolor[gray]{0.7}{'+withoutio[i][j]+'} ')
    f.write('\\\\\n')

f.close()
