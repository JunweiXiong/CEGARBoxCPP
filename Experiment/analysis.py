import os
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt


total_CPP_time = np.array([])
total_haskell_time = np.array([])
total_haskell_optim_time = np.array([])
total_CPP_result = np.array([])
total_haskell_result = np.array([])
total_haskell_optim_result = np.array([])
total_file_path = np.array([])


for file in os.listdir(os.getcwd()+"/Experiment/data"):
    # iterate each benchmark 
    if file.endswith(".csv"):
        file = os.getcwd()+"/Experiment/data/"+file
        benchmark = os.path.basename(file)[:-4]

        df = pd.read_csv(file)
        CPP_time = df['CPP_time']
        haskell_time = df["haskell_time"]
        haskell_optim_time = df["haskell_optim_time"]
        CPP_result = df["CPP_result"]
        haskell_result = df["haskell_result"]
        haskell_optim_result = df["haskell_optim_result"]
        file_path = df["file_path"]
        # add to total 
        total_CPP_time = np.append(total_CPP_time, CPP_time)
        total_haskell_time = np.append(total_haskell_time, haskell_time)
        total_haskell_optim_time = np.append(total_haskell_optim_time, haskell_optim_time)
        total_CPP_result = np.append(total_CPP_result, CPP_result)
        total_haskell_result = np.append(total_haskell_result, haskell_result)
        total_haskell_optim_result = np.append(total_haskell_optim_result, haskell_optim_result)
        total_file_path = np.append(total_file_path, file_path)

        # find exception and timeout in each prover
        cpp_succeed_index = np.where((CPP_result == "Satisfiable") | (CPP_result == "Unsatisfiable"))[0]
        cpp_exception_index = np.where(CPP_result == "exception")[0]
        cpp_timeout_index = np.where(CPP_result == "timeout")[0]
        haskell_succeed_index = np.where((haskell_result == "Satisfiable") | (haskell_result == "Unsatisfiable"))[0]
        haskell_exception_index = np.where(haskell_result == "exception")[0]
        haskell_timeout_index = np.where(haskell_result == "timeout")[0]
        haskell_optim_succeed_index = np.where((haskell_optim_result == "Satisfiable") | (haskell_optim_result == "Unsatisfiable"))[0]
        haskell_optim_exception_index = np.where(haskell_optim_result == "exception")[0]
        haskell_optim_timeout_index = np.where(haskell_optim_result == "timeout")[0]


        # statistic result
        print(benchmark)
        print("total: ", file_path.size)
        print("CPP           ","timeout",cpp_timeout_index.size,"exception",cpp_exception_index.size)
        print("Haskell       ","timeout",haskell_timeout_index.size,"exception",haskell_exception_index.size)
        print("Haskell optim ","timeout",haskell_optim_timeout_index.size,"exception",haskell_optim_exception_index.size)
        
        print("cpp succeed: ", cpp_succeed_index.size, " -> ","haskell succeed", np.where(((CPP_result=="Satisfiable")|(CPP_result=="Unsatisfiable"))&((haskell_result=="Satisfiable")|(haskell_result=="Unsatisfiable")))[0].size,"haskell_optim succeed",np.where(((CPP_result=="Satisfiable")|(CPP_result=="Unsatisfiable"))&((haskell_optim_result=="Satisfiable")|(haskell_optim_result=="Unsatisfiable")))[0].size)
        print("cpp timeout: ", cpp_timeout_index.size, " -> ","haskell timeout", np.where((CPP_result=='timeout')&(haskell_result=='timeout'))[0].size, "haskell_optim timeout", np.where((CPP_result=='timeout')&(haskell_optim_result=='timeout'))[0].size)
        print("cpp exception: ", cpp_timeout_index.size, " -> ","haskell succeed", np.where((CPP_result=='exception')&((haskell_result=="Satisfiable")|(haskell_result=="Unsatisfiable")))[0].size, "haskell_optim succeed", np.where((CPP_result=='exception')&((haskell_optim_result=="Satisfiable")|(haskell_optim_result=="Unsatisfiable")))[0].size)
        print("______________________________________________")

        # visual result
        x = np.array([0.25,0.5,1,2,4,8,16])
        values = range(len(x))

        cppy = np.array([((CPP_time<v)&(CPP_result!="exception")).sum() for v in x])
        hy = np.array([(haskell_time<v).sum() for v in x])
        hoy = np.array([(haskell_optim_time<v).sum() for v in x])
        
        plt.figure()
        plt.plot(values,hoy,marker="o",label="Haskell Optimised")
        plt.plot(values,hy,marker="o",label="Haskell Unoptimised")
        plt.plot(values,cppy,marker="o",label="CPP")
        plt.xlabel("CPU time in seconds")
        plt.ylabel("Instances solved")
        plt.title(benchmark)
        plt.xticks(values,x)
        plt.legend()
        plt.savefig("Experiment/data/"+"K" + benchmark +".png")




