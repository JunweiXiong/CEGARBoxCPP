from dataclasses import replace
import random
import os
import time
import subprocess


# formula generator
def formula_generator(depth=1, max_branches = 2,atoms_num = 5, max_clauses = 5):
    atoms = ["p"+str(i) for i in range(atoms_num)]
   
    cpl_clauses = []
    dia_clauses = []
    box_clauses = []
   
    max_literals = atoms_num
   
       
    clauses_num = random.randint(1, max_clauses)
    for i in range(clauses_num):
        literals_num = random.randint(1, max_literals)
        literals = random.sample(atoms,literals_num)
        literals = [random.choice(["~",""])+l for l in literals]
        cpl_clause = "|".join(literals)
        cpl_clause = "({})".format(cpl_clause)
        cpl_clauses.append(cpl_clause)

    clauses_num = random.randint(1, max_clauses)
    for i in range(clauses_num):
        literal_left = random.choice(["~",""])+random.choice(atoms)
        literal_right = random.choice(["~",""])+random.choice(atoms)
        box_clause = "{}=>[]{}".format(literal_left,literal_right)
        box_clause = "({})".format(box_clause)
        box_clauses.append(box_clause)

    clauses_num = random.randint(1, max_branches)
    for i in range(clauses_num):
        literal_left = random.choice(["~",""])+random.choice(atoms)
        literal_right = random.choice(["~",""])+random.choice(atoms)
        dia_clause = "{}=><>{}".format(literal_left,literal_right)
        dia_clause = "({})".format(dia_clause)
        dia_clauses.append(dia_clause)


    cpl_clauses = "&".join(cpl_clauses)
    cpl_clauses = "({})".format(cpl_clauses)
    box_clauses = "&".join(box_clauses)
    box_clauses = "({})".format(box_clauses)
    dia_clauses = "&".join(dia_clauses)
    dia_clauses = "({})".format(dia_clauses)

    formula = "{}&{}&{}".format(cpl_clauses,box_clauses,dia_clauses)
   
    if depth == 1:
        return formula
    elif depth > 1:
        return "{}&([]({}))&(<>({}))".format(formula,formula_generator(depth-1, max_branches,atoms_num),formula_generator(depth-1, max_branches,atoms_num))

file_path = "benchmarks/test/test1"

formula = formula_generator(depth=8, max_branches = 3,atoms_num = 10, max_clauses = 2)
file = open(file_path,"w")
file.write(formula)
file.close()

cpp_start = time.time()
try:
    cpp_result = subprocess.check_output(
        ["./main", "-f", file_path], timeout=10).decode("utf-8").strip()
except subprocess.TimeoutExpired:
    cpp_result = "timeout"
except Exception as e:
    cpp_result = "exception"
    print("GOT EXCEPTION", e)
cpp_end = time.time()

cpp_time = cpp_end-cpp_start
print(cpp_result)
print(cpp_time)



# while (True):

#     file_path = "benchmarks/test/test1"

#     formula = formula_generator(depth=5, max_branches = 3,atoms_num = 5, max_clauses = 2)
#     file = open(file_path,"w")
#     file.write(formula)
#     file.close()

#     cpp_start = time.time()
#     try:
#         cpp_result = subprocess.check_output(
#             ["./main", "-f", file_path], timeout=10).decode("utf-8").strip()
#     except subprocess.TimeoutExpired:
#         cpp_result = "timeout"
#     except Exception as e:
#         cpp_result = "exception"
#         print("GOT EXCEPTION", e)
#     cpp_end = time.time()

#     cpp_time = cpp_end-cpp_start

#     if cpp_result == "timeout" or cpp_result == "exception":
#         print(cpp_time,cpp_result)
#         print(formula)
#     elif cpp_time > 1:
#         print(cpp_time)
#         print(formula)

    

