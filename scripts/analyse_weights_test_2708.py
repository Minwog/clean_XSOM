import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import math
from os import listdir
from os.path import isfile, join
import re
import sys

def interp(weights, node1, node2, v):
    w1 = weights[node1]
    w2 = weights[node2]

    return ((w1-w2)/(node1 - node2))*(v - node1) + w1

#l1 : list d'indices entre 0 et 1, step : le pas qu'on donne
def compute_diff(l1,tw,step = 0.002):
    l1.sort()
    sum = 0
    if len(l1)>0:
        i_prev = l1[0]
        idx = math.floor(500 * i_prev)
        sublist = [tw[idx]]
        for i in l1[1:]:
            if(i - i_prev <= step):
                idx = math.floor(500 * i)
                sublist.append(tw[idx])
                i_prev = i
            else:
                sum += max(sublist) - min(sublist)
                sublist.clear()
                idx = math.floor(500 * i)
                sublist.append(tw[idx])
                i_prev = i
    return sum

def compute_diff_2(l1,tw):
    min1 = int(np.floor(500*min(l1)))
    max1 = int(np.floor(500*max(l1)))
    sum = np.abs(tw[max1] - tw[min1])
    return sum



if __name__ == "__main__":
    dir_path  = "../experiences/2SOM_retro/"
    if(sys.argv[1] == "all"):
        filesmap1 = [f for f in listdir("../experiences/2SOM_retro/") if "map1" in f]
        filesmap2 = [f for f in listdir("../experiences/2SOM_retro/") if "map2" in f]
        print(filesmap1)
        print(filesmap2)
    else:
        filesmap1 = ["map1_w_"+sys.argv[1]+".data"]
        filesmap2 = ["map2_w_"+sys.argv[1]+".data"]

    for i,file in enumerate(filesmap1):

        file1 = join(dir_path,file)
        #substr = file[-16:]
        #matches = [x for x in filesmap2 if substr in x]
        #assert(len(matches) == 1)
        file2 = join(dir_path,filesmap2[i])

        print(file1)
        print(file2)


        w1 = pd.read_csv(file1, sep = ' ',header = None)
        w2 = pd.read_csv(file2 ,sep = ' ', header = None)

        tw1 = w1[w1.columns[0:500]].values[0]
        cw1 = w1[w1.columns[500:1000]].values[0]

        tw2 = w2[w2.columns[0:500]].values[0]
        cw2 = w2[w2.columns[500:1000]].values[0]

        #plot weights



        #2D plots !

        """
        Couleurs :
        L'idée est de représenter nos 4 cartes 1D sous une forme 2D.


        TODO : enlever les couleurs et mettre des graduations.
        """
        """    for idx,val in enumerate(x):
TypeError: 'numpy.float64' object is not iterable
ngonnier@biscotti:~/Documents/Code/math_exp$



        x = np.zeros((500,2))
        x2 = tw1
        y = tw2
        y2 = np.zeros((500,2))

        print(len(x2))
        for idx,elt in enumerate(tw2):
            pos1 = int(500*cw2[idx])
            x[idx,0] = tw1[pos1]
            x[idx,1] = idx

        for idx,elt in enumerate(tw1):
            pos1 = int(500*cw1[idx])
            y2[idx,0] = tw2[pos1]
            y2[idx,1] = pos1

        colortab = list(range(0,500))

        plt.figure()
        plt.subplot(1,2,1)
        plt.scatter(x[:,0],y)
        plt.subplot(1,2,2)
        plt.scatter(x2,y2[:,0])
        """
        """
        Densités : l'idée est de séparer le premier espace en "classes", et de calculer à combien de points de y cela correspond.

        """
        """
        d_p1 = []
        d_i = []
        d_y = np.zeros((50,1))
        d_y_len = np.zeros((50,1))
        for x in np.arange(0,1,0.01):
            sublist = []
            sublist_i = []
            for idx,val in enumerate(tw1):
                if (val<=x+0.02 and val>x):
                    sublist.append(cw1[idx])
            idx = int(50*x)
            d_y[idx] = compute_diff(sublist,tw2)
            print("==================")
        print(d_p1)
        """
        d_p1 = []
        d_i = []
        d_y = []
        d_x = []
        col = []
        for i,w1 in enumerate(tw1):
            for j in range(i):
                d_x.append(np.abs(tw2[j]-w1))
                col.append(i/500.)
                sublist = [cw1[k] for k in range(j,i)]
                res = compute_diff_2(sublist,tw2)
                d_y.append(res)

        plt.figure()
        plt.title(file1)
        plt.scatter(d_x,d_y,c=col)
        pattern = '\d{4}-\d{1,2}-\d{1,2}\_\d{1,2}-\d{1,2}'
        name = re.search(pattern,file1).group(0)
        #plt.savefig(join(dir_path, "dx-dy-2D-"+ name + ".svg"))
        plt.show()

        """

        f, axes = plt.subplots(2, 2,subplot_kw=dict(polar=True))
        axes[0,0].set_title("tw1 en fonction de cw1")
        axes[0,0].plot(2*np.pi*cw1,tw1)
        axes[0,1].set_title("tw2 en fonction de cw2")
        axes[0,1].plot(2*np.pi*cw2,tw2)
        axes[1,0].set_title("tw2 en fonction de cw1")
        axes[1,0].plot(2*np.pi*cw1,tw2)
        axes[1,1].set_title("tw1 en fonction de cw2")
        axes[1,1].plot(2*np.pi*cw2,tw1)

        pattern = '\d{4}-\d{1,2}-\d{1,2}\_\d{1,2}-\d{1,2}'
        name = re.search(pattern,file1).group(0)
        plt.savefig(join(dir_path, "pol_rep_thalcort"+ name + ".svg"))
        plt.close()
        """
