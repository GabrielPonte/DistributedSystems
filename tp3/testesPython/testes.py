import os
import subprocess
import time
import matplotlib.pyplot as plt

def plotGraph(strTitle,N,y):
    plt.title (strTitle)
    plt.plot(N, y)
    plt.xlabel("Numero de Processos")
    plt.ylabel("Tempo de execução (s)")
    plt.show()
    plt.savefig(strTitle + '.png')
    return

def test0():
    n = 2
    r = 10
    k = 2
    subprocess.run(['../cliente/cliente',str(r),str(k),str(n)])
    return 
    

def test1():
    strTitle = "Teste 1"
    r = 2
    k = 10
    y = []
    N = [2, 4, 8, 16, 32]
    for n in N:
        start_time = time.time()
        subprocess.run(['../cliente/cliente',str(r),str(k),str(n)])
        elapsed_time = time.time() - start_time
        print ("n = {}, time(s) = {}".format(n, elapsed_time))
        y.append(elapsed_time)
    plotGraph(strTitle,N,y)
    return [n,y]


def test2():
    strTitle = "Teste 2"
    r = 5
    k = 1
    y = []
    N = [2, 4, 8, 16, 32,64]
    for n in N:
        start_time = time.time()
        subprocess.run(['../cliente/cliente',str(r),str(k),str(n)])
        elapsed_time = time.time() - start_time
        print ("n = {}, time(s) = {}".format(n, elapsed_time))
        y.append(elapsed_time)
    plotGraph(strTitle,N,y)
    return [n,y]

def test3():
    strTitle = "Teste 3"
    r = 3
    k = 0
    y = []
    N = [2, 4, 8, 16, 32,64,128]
    for n in N:
        start_time = time.time()
        subprocess.run(['../cliente/cliente',str(r),str(k),str(n)])
        elapsed_time = time.time() - start_time
        print ("n = {}, time(s) = {}".format(n, elapsed_time))
        y.append(elapsed_time)
    plotGraph(strTitle,N,y)
    return [n,y]

def apresentacaoVideo1():
    strTitle = "Teste Video"
    r = 2
    k = 10
    n = 2
    start_time = time.time()
    subprocess.run(['../cliente/cliente',str(r),str(k),str(n)])
    elapsed_time = time.time() - start_time
    print ("n = {}, time(s) = {}".format(n, elapsed_time))
    return


def apresentacaoVideo2():
    strTitle = "Teste Video"
    r = 2
    k = 10
    n = 64
    start_time = time.time()
    subprocess.run(['../cliente/cliente',str(r),str(k),str(n)])
    elapsed_time = time.time() - start_time
    print ("n = {}, time(s) = {}".format(n, elapsed_time))
    return

#apresentacaoVideo1()
apresentacaoVideo2()