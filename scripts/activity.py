import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import math
import time

def plot_and_calculate_activity(x1,x2):

    beta = 0.5
    sigma2 = 0.5

    alpha_out = 0.2

    activity = lambda val1, val2: np.exp(-(val1-val2)*(val1-val2)/(2*sigma2))
    merge = lambda val1,val2: beta * val1 + (1-beta)*val2

    at1  = [activity(x1,p) for p in tw1]
    at2 =  [activity(x2,p) for p in tw2]

    c2 = np.argmax(at1)/500
    c1 = np.argmax(at2)/500

    graph = plt.figure()

    for i in range(100):

        ac1  = [activity(c1,p) for p in cw1]
        ac2 =  [activity(c2,p) for p in cw2]

        m1 = [merge(x,c) for x,c in zip(at1,ac1)]
        m2 = [merge(x,c) for x,c in zip(at2,ac2)]

        bmu2 = np.argmax(m2)/500.
        bmu1 = np.argmax(m1)/500.

        c1 = c1 + alpha_out*(bmu2 - c1)
        c2 = c2 + alpha_out*(bmu1 - c2)


    return m1,m2,c1,c2

#Calcul des activités pour une entrée donnée



if __name__ == "__main__":

    file1 = "../experiences/2SOM_retro/map1_w_2019-10-28_10-24.data"
    file2 = "../experiences/2SOM_retro/map2_w_2019-10-28_10-24.data"


    w1 = pd.read_csv(file1, sep = ' ',header = None)
    w2 = pd.read_csv(file2 ,sep = ' ', header = None)

    tw1 = w1[w1.columns[0:500]].values[0]
    cw1 = w1[w1.columns[500:1000]].values[0]

    tw2 = w2[w2.columns[0:500]].values[0]
    cw2 = w2[w2.columns[500:1000]].values[0]

    beta = 0.5
    sigma2 = 0.5
    alpha_out = 0.2



    activity = lambda val1, val2: np.exp(-(val1-val2)*(val1-val2)/(2*sigma2))
    merge = lambda val1,val2:beta * val1 + (1-beta)*val2

    theta = np.random.rand(100,1)
    bmu1 = []
    bmu2 = []
    bmuw1 = []
    bmuw2 = []

    for t in theta:

        x1 = 0.5+0.5*np.cos(t)
        x2 = 0.5+0.5*np.sin(t)

        at1  = [activity(x1,p) for p in tw1]
        at2 =  [activity(x2,p) for p in tw2]

        ac1 = [0 for p in cw1]
        ac2 = [0 for p in cw2]

        c2 = np.argmax(at1)/500
        c1 = np.argmax(at2)/500

        for i in range(100):

            ac1  = [activity(c1,p) for p in cw1]
            ac2 =  [activity(c2,p) for p in cw2]

            m1 = [merge(x,c) for x,c in zip(at1,ac1)]
            m2 = [merge(x,c) for x,c in zip(at2,ac2)]

            b1 = np.argmax(m2)/500.
            b2 = np.argmax(m1)/500.

            c1 = c1 + alpha_out*(b2 - c1)
            c2 = c2 + alpha_out*(b1 - c2)


        bmu1.append(c2)
        bmu2.append(c1)
        bmuw1.append(tw1[int(500*c2)])
        bmuw2.append(tw2[int(500*c1)])

        """
    plt.figure()
    plt.subplot(2,1,1)
    plt.plot(np.arange(0,1,0.002), tw1,'r-')
    plt.plot(np.arange(0,1,0.002),cw1,'b-')
    #plt.scatter(d_i[2],np.zeros((len(d_i[2]),1)), s = 10, c = 'r')
    plt.subplot(2,1,2)
    plt.plot(np.arange(0,1,0.002),tw2,'r-')
    plt.plot(np.arange(0,1,0.002),cw2,'b-')

    plt.figure()
    plt.subplot(2,1,1)
    plt.axhline(y = x1)
    plt.axvline(x = c2)
    plt.plot(np.arange(0,1,0.002),m1)
    plt.subplot(2,1,2)
    plt.axhline(y = x2)
    plt.axvline(x = c1)
    plt.plot(np.arange(0,1,0.002),m2)
"""
    plt.figure()
    plt.scatter(bmuw1,bmuw2)
    plt.show()
