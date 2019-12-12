import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

if __name__ == "__main__":

    alpha_out = 0.1

    file_bmus = pd.read_csv("../experiences/2SOM_retro/bmus_update_test_2019-11-12_11-2.data", sep =";", header = None)

    xy =file_bmus[file_bmus.columns[0:-1]].values

    for i in range(0,10):
        x = xy[i][::2]
        y = xy[i][1::2]

        last_elt = x[0]
        reqx = []
        reqy = []

        for elt in x[1:]:
            reqx.append(last_elt+(elt - last_elt)/alpha_out)
            last_elt = elt

        last_elt =y[0]
        for elt in y[1:]:
            reqy.append(last_elt+(elt - last_elt)/alpha_out)
            last_elt = elt

        diffx = [xi - reqxi for xi,reqxi in zip(x,reqx)]
        diffy = [yi - reqyi for yi,reqyi in zip(y,reqy)]

        e = [dx*dx + dy*dy for dx,dy in zip(diffx,diffy)]

        plt.plot(e)

    plt.show()
