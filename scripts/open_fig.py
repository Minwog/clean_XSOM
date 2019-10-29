import pickle as pkl
import matplotlib.pyplot as plt
import sys


if __name__ == "__main__":

    if(len(sys.argv[1:]) == 1):
        figname = sys.argv[1]
        fig = pkl.load(open(figname, 'rb'))
        plt.show()
    else:
        print("script only takes one argument")
        sys.exit(0)
