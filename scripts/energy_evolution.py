import matplotlib.pyplot as plt
import numpy as np
import random
import sys
from os import listdir
from os.path import isfile, join
import pandas as pd

def phi(w,k,H,inputs):
    s = sum([H[k][j]*sum_diff(j,w[k],inputs) for j in range(0,size)])
    return (1./(size))*s

def sum_diff(j, wk,inputs):
    l = [(x - wk)*(x-wk) for x in inputs[j]]
    s = sum(l)
    return s

def find_argmin(input,w):
    return np.argmin([(input-elt)*(input-elt) for elt in w])


def build_bmu_dict(inputs,w):
    dico = {}
    for k in range(len(w)):
        dico[k] = []
    for elt in inputs:
        k = find_argmin(elt,w)
        dico[k].append(elt)
    return dico

if __name__ == "__main__":

    size = 500
    sigma = 0.05

    dir_path  = "../experiences/2SOM_retro/"

    filesmap1 = "map1_w_"+sys.argv[1]+".data"
    filesmap2 = "map2_w_"+sys.argv[1]+".data"

    file1 = join(dir_path,filesmap1)
    file2 = join(dir_path,filesmap2)

    w1 = pd.read_csv(file1, sep = ' ',header = None)
    w2 = pd.read_csv(file2 ,sep = ' ', header = None)


    s1,s2 = w1.shape


    tw1 = w1[w1.columns[0:500]].values
    cw1 = w1[w1.columns[500:1000]].values
    tw2 = w2[w2.columns[0:500]].values
    cw2 = w2[w2.columns[500:1000]].values



    np.reshape(tw1,(500,s1))
    np.reshape(cw1,(500,s1))
    np.reshape(cw2,(500,s1))
    np.reshape(tw2,(500,s1))

    print(tw1.shape)



    inputs_theta = np.random.rand(1000)

    inputs_x = [0.5 + 0.5 * np.cos(2*np.pi*t) for t in inputs_theta]
    inputs_y = [0.5 + 0.5 * np.sin(2*np.pi*t) for t in inputs_theta]

    ex = []
    ey = []
    """
    fig = plt.figure()
    ax1,ax2 = fig.subplots(1,2)
    ax1.legend(["external weights, context weights"])
    for n in range(s1):
        ax1.clear()
        ax2.clear()
        ax1.set_title("map1, iteration : "+str(n*50))
        ax2.set_title("map2")
        print(n)
        ax1.plot(np.linspace(0,1,500),tw1[n,:],'-r')
        ax1.plot(np.linspace(0,1,500),cw1[n,:],'-g')
        ax2.plot(np.linspace(0,1,500),tw2[n,:],'-r')
        ax2.plot(np.linspace(0,1,500),cw2[n,:],'-g')
        plt.draw()
        plt.savefig("../tmp" + "/file%03d.png" % n)
        plt.pause(0.01)
    """

    Hx = [[np.exp(-(i/500. - j/500.)**2/(2*sigma**2)) for i in range(500)] for j in range(500) ]
    Hy = [[np.exp(-(i/500. - j/500.)**2/(2*sigma**2)) for i in range(500)] for j in range(500) ]
    for n in range(s1):
    ## Calcul des clusters
    #Premiere option : évaluation des clusters indépendamment sur chaque carte avec la distance

        inp_bmu_x = build_bmu_dict(inputs_x,tw1[n,:])
        inp_bmu_y = build_bmu_dict(inputs_y, tw2[n,:])

        #deuxième option : on définit un cluster par l'ensemble des xi pour lequel le noeud wi gagne dans la carte , en prenant en compte l'architecture et la mise a jour
        #=> necessaire de récupérer les bmus obtenus pour chaque entrée lors d'une phase de test.



        #calcul des energies



        #Hx = [[np.exp(-(cw1[n,i] - cw1[n,j])**2/(2*sigma**2)) for i in range(500)] for j in range(500) ]
        #Hy = [[np.exp(-(cw2[n,i] - cw2[n,j])**2/(2*sigma**2)) for i in range(500)] for j in range(500) ]

        phi_wx = [phi(tw1[n,:],k,Hx,inp_bmu_x) for k in range(size)]
        phi_wy = [phi(tw2[n,:],k,Hy,inp_bmu_y) for k in range(size)]

        ex.append(1./size * sum(phi_wx))
        ey.append(1./size * sum(phi_wy))



    plt.figure()
    plt.plot(range(s1),ex)
    plt.plot(range(s1),ey)
    plt.show()
