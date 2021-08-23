from matplotlib.lines import Line2D
import matplotlib.pyplot as pyplot
import matplotlib.cm as mplcm
import matplotlib.colors as colors
import numpy as np
import os
import sys

def parse_perf_data(perf_file):
    try:
        f = open(perf_file, "r")
        func_name="no name set"
        cache_params = {"mc":-1,"kc":-1,"nc":-1}
        sq_mat_size = []
        gflops = []
        for line in f:
            if(line!=None and "bli" in line):
                func_name=line.split()[0]
            elif(line!=None and "MC" in line):
                cache_params_list = line.replace(",","").split()
                cache_params["mc"] = cache_params_list[2]
                cache_params["kc"] = cache_params_list[5]
                cache_params["nc"] = cache_params_list[8]
            elif(line!=None and "m, n, k, GFLOPS" in line):
                continue
            elif(line==None):
                continue
            else:
                new_line = line.replace(",","").split()
                sq_mat_size.append(int(new_line[2]))
                gflops.append(float(new_line[-1]))
        name = "MC" + cache_params["mc"] + "_KC" + cache_params["kc"] + "_NC" + cache_params["nc"]
        return sq_mat_size, gflops, name
    except Exception as E:
        print(E)
        print(perf_file)
        


# find the points that have the min and max gflops
def get_stats(x, y):
    max_tuple=(-1, -1e9)
    min_tuple=(-1, 1e9)
    for i, val in enumerate(x):
        size = x[i]
        gflops = y[i]
        if gflops > max_tuple[1]:
            max_tuple = (size, gflops)
        elif gflops <= min_tuple[1]:
            min_tuple = (size, gflops)
    return max_tuple, min_tuple

def main():
    
    direc = sys.argv[1]

    # assumes performance results are in the same directory as python script
    perf_path = os.getcwd() + direc
    perf_files = [f for f in os.listdir(perf_path) if os.path.isfile(os.path.join(perf_path, f))]
    print(perf_files)

    save_file_name = "cache_exper.svg"

    try:
        # remove the current script
        perf_files.remove(os.path.basename(__file__))
        # try to remove graph if it exists
        perf_files.remove(save_file_name)
        perf_files.remove("old")
    except:
        pass


    NUM_COLORS = len(perf_files)
    fig = pyplot.figure(figsize=(24, 13.5), dpi=80)
    ax = fig.add_subplot(1,1,1)
    cm = pyplot.get_cmap('gist_rainbow')
    all_markers = list(Line2D.markers.keys())
    legal_markers = [all_markers[i % 25] for i in range(NUM_COLORS)]
    cNorm  = colors.Normalize(vmin=0, vmax=NUM_COLORS-1)
    scalarMap = mplcm.ScalarMappable(norm=cNorm, cmap=cm)
    ax.set_prop_cycle(color=[scalarMap.to_rgba(i) for i in range(NUM_COLORS)], 
            marker=legal_markers)


    for f in perf_files:
        x, y, label_name = parse_perf_data(perf_path+f)
        ma, mi = get_stats(x, y)
        kc = int(label_name.split('_')[1][2:])
        #if kc < 1000:
         #   continue
        print(label_name, ma, mi)
        ax.plot(x,y,label=label_name)
    ax.legend()

    pyplot.xticks(np.arange(0, 4000, 500))
    pyplot.yticks(np.arange(0.0, 513.0, 100.0))
   
    pyplot.title(direc)
    pyplot.xlabel("mnk")
    pyplot.ylabel("GFLOPS")
    fig.savefig(save_file_name, format="svg")

main()


