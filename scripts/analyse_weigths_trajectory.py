import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys
from os import listdir
from os.path import isfile, join

if __name__ == "__main__":

    dir_path  = "../experiences/2SOM_retro/"

    filesmap1 = "map1_w_"+sys.argv[1]+".data"
    filesmap2 = "map2_w_"+sys.argv[1]+".data"

    file1 = join(dir_path,filesmap1)
    file2 = join(dir_path,filesmap2)

    print(file1)
    print(file2)


    w1 = pd.read_csv(file1, sep = ' ',header = None)
    w2 = pd.read_csv(file2 ,sep = ' ', header = None)

    print(w1.shape)
    print(w2.shape)

    s1,s2 = w1.shape


    tw1 = w1[w1.columns[0:500]].values
    cw1 = w1[w1.columns[500:1000]].values
    tw2 = w2[w2.columns[0:500]].values
    cw2 = w2[w2.columns[500:1000]].values



    np.reshape(tw1,(500,s1))
    np.reshape(cw1,(500,s1))
    np.reshape(cw2,(500,s1))
    np.reshape(tw2,(500,s1))
    positions = [10,50,100,150,200,250,300,350,400]
    fig = plt.figure()



    ax1 = fig.add_subplot(2,2,1)
    for p in positions:
        ax1.plot(tw1[:,p])
    ax1.set_title("External map x weights evolution")
    ax1.set_xticks([25,50,75,100,125,150])
    ax1.set_xticklabels([50*i for i in [25,50,75,100,125,150]])

    ax2 = fig.add_subplot(2,2,2)
    for p in positions:
        ax2.plot(cw1[:,p])
    ax2.set_title("Contextual map x weights evolution")
    ax2.set_xticks([25,50,75,100,125,150])
    ax2.set_xticklabels([50*i for i in [25,50,75,100,125,150]])

    ax3 = fig.add_subplot(2,2,3)
    for p in positions:
        ax3.plot(tw2[:,p])
    ax3.set_title("External map y weights evolution")
    ax3.set_xticks([25,50,75,100,125,150])
    ax3.set_xticklabels([50*i for i in [25,50,75,100,125,150]])

    ax4 = fig.add_subplot(2,2,4)
    for p in positions:
        ax4.plot(cw2[:,p])
    ax4.set_title("Contextual map y weights evolution")
    ax4.set_xticks([25,50,75,100,125,150])
    ax4.set_xticklabels([50*i for i in [25,50,75,100,125,150]])
    plt.show()
