import os
import time
import numpy as np
import pandas as pd
import subprocess
from matplotlib import pyplot as plt


def find_files():
    haskell_files = {}
    cpp_files = {}
    for root, subdirs, files in os.walk(os.getcwd()):
        parts = root.split("/")
        if "benchmarks" not in parts or parts[-1] == "benchmarks":
            continue
        benchmark_group = parts[parts.index("benchmarks")+1]
        if benchmark_group not in haskell_files:
            haskell_files[benchmark_group] = []
            cpp_files[benchmark_group] = []
        for file in files:
            if file.endswith(".hf"):
                haskell_files[benchmark_group].append(root+"/"+file)
            elif file.endswith(".cf"):
                cpp_files[benchmark_group].append(root+"/"+file)
    for key in haskell_files:
        haskell_files[key].sort()
        cpp_files[key].sort()
    return haskell_files, cpp_files


TIMEOUT = 40


def compare_times(haskell_file, cpp_file):
    base_path = os.getcwd()[:-11]

    haskell_start = time.time()
    try:
        haskell_result = subprocess.check_output(
            [os.path.expanduser('~')+"/.cabal/bin/CEGARBox", haskell_file], timeout=TIMEOUT).decode("utf-8").strip()
    except subprocess.TimeoutExpired:
        haskell_result = "timeout"
    except Exception as e:
        haskell_result = "exception"
        print("GOT EXCEPTION", e)
    haskell_end = time.time()

    haskell_optim_start = time.time()
    try:
        haskell_optim_result = subprocess.check_output(
            [os.path.expanduser('~')+"/.cabal/bin/CEGARBoxOptim", haskell_file], timeout=TIMEOUT).decode("utf-8").strip()
    except subprocess.TimeoutExpired:
        haskell_optim_result = "timeout"
    except Exception as e:
        haskell_optim_result = "exception"
        print("GOT EXCEPTION", e)
    haskell_optim_end = time.time()

    cpp_start = time.time()
    try:
        cpp_result = subprocess.check_output(
            ["./main", "-f", cpp_file], timeout=TIMEOUT).decode("utf-8").strip()
    except subprocess.TimeoutExpired:
        cpp_result = "timeout"
    except Exception as e:
        cpp_result = "exception"
        print("GOT EXCEPTION", e)

    cpp_end = time.time()

    # if cpp_result != haskell_result and cpp_result != "timeout" and haskell_result != "timeout" and cpp_result != "exception" and haskell_result != "exception":
    #     print("Inconsistant result on", cpp_file, "cpp got", cpp_result,
    #           "while haskell got", haskell_result)

    cpp_time = cpp_end - cpp_start
    haskell_time = haskell_end - haskell_start
    haskell_optim_time = haskell_optim_end - haskell_optim_start

    return cpp_time, haskell_time, haskell_optim_time, cpp_result, haskell_result, haskell_optim_result
  

def run_experiment(haskell_files, cpp_files):
    print("Starting experiment")
    satisfiable = 0
    unsatisfiable = 0

    cpp_times = {}
    haskell_times = {}
    haskell_optim_times = {}
    cpp_results = {}
    haskell_results = {}
    haskell_optim_results = {}
    file_path = {}
    try:
        for group in haskell_files:
            cpp_times[group] = []
            haskell_times[group] = []
            haskell_optim_times[group] = []
            cpp_results[group] = []
            haskell_results[group] = []
            haskell_optim_results[group] = []
            file_path[group] = []
            for file1, file2 in zip(haskell_files[group], cpp_files[group]):
                print(file1, file2)
                cpp_time, haskell_time, haskell_optim_time, cpp_result, haskell_result, haskell_optim_result = compare_times(
                    file1, file2)
                cpp_times[group].append(cpp_time)
                haskell_times[group].append(haskell_time)
                haskell_optim_times[group].append(haskell_optim_time)
                cpp_results[group].append(cpp_result)
                haskell_results[group].append(haskell_result)
                haskell_optim_results[group].append(haskell_optim_result)
                file_path[group].append(file1+"  "+file2)
                
                keys = ["CPP","Haskell","Haskell_optim"]
                results = [cpp_result,haskell_result,haskell_optim_result]
                times = [cpp_time,haskell_time,haskell_optim_time]
                sorted_index = sorted(range(len(times)), key=lambda k: times[k])

                print(keys[sorted_index[0]],results[sorted_index[0]]," ",keys[sorted_index[1]],results[sorted_index[1]]," ",keys[sorted_index[2]],results[sorted_index[2]])
                print(keys[sorted_index[0]],times[sorted_index[0]]," ",keys[sorted_index[1]],times[sorted_index[1]]," ",keys[sorted_index[2]],times[sorted_index[2]])
        
                results = set(
                    [cpp_result, haskell_result, haskell_optim_result])
                if "Satisfiable" in results and "Unsatisfiable" not in results:
                    satisfiable += 1
                elif "Unsatisfiable" in results and "Satisfiable" not in results:
                    unsatisfiable += 1
           
    except KeyboardInterrupt:
        pass
    print("CPP:", cpp_times)
    print("Unhas:", haskell_times)
    print("OpHas:", haskell_optim_times)

    print("SAT", satisfiable, "UNSAT", unsatisfiable)
    return haskell_optim_times, haskell_times, cpp_times, haskell_optim_results, haskell_results, cpp_results, file_path


if __name__ == "__main__":

    haskell_files, cpp_files = find_files()

    # test them seperately 
#    keys = list(haskell_files.keys())
#    print(keys)
#    for k in keys:
#        if k != "3CNF":
#            haskell_files.pop(k)
#            cpp_files.pop(k)

    haskell_optim_times, haskell_times, cpp_times, haskell_optim_results, haskell_results, cpp_results, file_path = run_experiment(haskell_files, cpp_files)

    # store the results
    for group in file_path:
        data = {"CPP_time":cpp_times[group],
            "haskell_time":haskell_times[group],
            "haskell_optim_time":haskell_optim_times[group],
            "CPP_result":cpp_results[group],
            "haskell_result":haskell_results[group],
            "haskell_optim_result":haskell_optim_results[group],
            "file_path":  file_path[group]}
        df = pd.DataFrame(data)
        df.to_csv("Experiment/data/"+group+'.csv')


    haskell_optim_all = []
    haskell_all = []
    cpp_all = []
    for group in cpp_times:
        haskell_optim_all.extend(haskell_optim_times[group])
        haskell_all.extend(haskell_times[group])
        cpp_all.extend(cpp_times[group])
        print(group)
        print(haskell_optim_times[group])
        print(haskell_times[group])
        print(cpp_times[group])

        h_o_a = np.cumsum(np.sort(
            np.array(list(filter(lambda x: x < TIMEOUT, haskell_optim_times[group])))))
        h_u_a = np.cumsum(
            np.sort(np.array(list(filter(lambda x: x < TIMEOUT, haskell_times[group])))))
        c_a = np.cumsum(
            np.sort(np.array(list(filter(lambda x: x < TIMEOUT, cpp_times[group])))))
        plt.figure()
        plt.plot(h_o_a, np.arange(h_o_a.size), label="Haskell Optimised")
        plt.plot(h_u_a, np.arange(h_u_a.size), label="Haskell Unoptimised")
        plt.plot(c_a, np.arange(c_a.size), label="C++")
        plt.xlabel("Time (seconds)")
        plt.legend()
        plt.title(group + " Benchmarks (K)")
        plt.ylabel("Problems Solved")
        plt.savefig("Experiment/data/"+"K" + group + ".png")
    print("Total")
    print(haskell_optim_all)
    print(haskell_all)
    print(cpp_all)
    plt.figure()
    plt.plot(np.cumsum(np.sort(np.array(haskell_optim_all))),
             np.arange(len(haskell_optim_all)), label="Haskell Optimised")
    plt.plot(np.cumsum(np.sort(np.array(haskell_all))),
             np.arange(len(haskell_all)), label="Haskell Unoptimised")
    plt.xlabel("Time (seconds)")
    plt.plot(np.cumsum(np.sort(np.array(cpp_all))),
             np.arange(len(cpp_all)), label="C++")
    plt.legend()
    plt.title("MQBF Benchmarks")
    plt.ylabel("Problems Solved")
    plt.savefig("Experiment/data/"+"K" + "overall.png")
