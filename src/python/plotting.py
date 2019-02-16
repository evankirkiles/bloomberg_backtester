import argparse
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser()
parser.add_argument('-f', '--filepath', action='store', dest='filepath', help='Path of the file to read in and plot from.')
args = parser.parse_args()

with open(args.filepath, 'r') as f:
    array = [[float(x) for x in line.split()] for line in f]
    a = range(len(array))
    plt.plot(a, array)
    plt.show()


