
import os
import subprocess
import globals

#Support function that moves the generated files to the designated output folder
def populate_output_dir(dirpath):
    #if the directories exist erase it 
    if os.path.exists(dirpath):
       try:
        subprocess.run(f"rm -rf {dirpath}", shell=True, check=True)
       except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to remove directory {dirpath}. errno: {e.returncode}", flush=True)
            exit(1) 
    #Create output folder
    try:
        subprocess.run(f"mkdir -p {dirpath}/design", shell=True, check=True)
        subprocess.run(f"mkdir -p {dirpath}/expected", shell=True, check=True)
        subprocess.run(f"mkdir -p {dirpath}/traces", shell=True, check=True)
        subprocess.run(f"mkdir -p {dirpath}/faulty_traces", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to create directory {dirpath}. errno: {e.returncode}", flush=True)
        exit(1)

    #populate every subdirectory
    try:
        subprocess.run(f"mv {globals.out_folder}*.v {dirpath}/design/", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move files to {dirpath}/design. {e}", flush=True)
        exit(1)
    try:
        subprocess.run(f"mv {globals.out_folder}specifications.txt {dirpath}/expected/", shell=True, check=True)
    except  subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move specifications.txt to {dirpath}/expected. {e}", flush=True)
        exit(1)
    try:
        subprocess.run(f"mv {globals.out_folder}golden_check_report.txt {dirpath}/expected/", shell=True, check=True)
    except  subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move golden_check_report.txt to {dirpath}/expected. {e}", flush=True)
        exit(1)
    try:
        subprocess.run(f"mv {globals.out_folder}/traces {dirpath}", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move traces to {dirpath}/traces. {e}", flush=True)
        exit(1)
    try:
        subprocess.run(f"mv {globals.out_folder}/faulty_traces {dirpath}", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move faulty_traces to {dirpath}/faulty_traces. {e}", flush=True)
        exit(1)

    #clean the output folder
    if not globals.debug:
        try:
            subprocess.run(f"rm -rf {globals.out_folder}", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to clean the output folder. {e}", flush=True)
            exit(1)

def create_outfolder():
    #check if the output folder exists and create it if needed
    if not os.path.exists(globals.out_folder):
        try:
            subprocess.run(f"mkdir -p {globals.out_folder}", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/traces/vcd", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/traces/csv", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/faulty_traces/vcd", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/faulty_traces/csv", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to create directory {globals.out_folder}. errno: {e.returncode}", flush=True)
            exit(1)
    # if the folder exists empty it
    else:
        try:
            subprocess.run(f"rm -rf {globals.out_folder}*", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to clean directory {globals.out_folder}. errno: {e.returncode}", flush=True)
            exit(1)
        try:
            subprocess.run(f"mkdir -p {globals.out_folder}/traces/vcd", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/traces/csv", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/faulty_traces/vcd", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/faulty_traces/csv", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to create directory {globals.out_folder}. errno: {e.returncode}", flush=True)
            exit(1)
    try:
        subprocess.run(f'touch {globals.out_folder}/specifications.txt', shell=True, check=True)
    except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to create specifications.txt file {globals.out_folder}. errno: {e.returncode}", flush=True)
            exit(1)

def generateCSV():
    #create the csv traces from vcd traces
    try:
        subprocess.run(f"{globals.root}/tool/build/vcd2csv --vcd-dir {globals.out_folder}faulty_traces/vcd --clk {globals.clk_name} --vcd-ss \"{globals.top_module_name}_bench::{globals.top_module_name}_\" --dump-to {globals.out_folder}faulty_traces/csv",stdout=subprocess.DEVNULL if not globals.debug else None,stderr=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to convert vcd to csv. {e}", flush=True)
        exit(1)

#Check that every golden specification holds on the golden trace and is non-vacuously triggered (its antecedent is activated at least once)
def check_golden_specifications():
    golden_check = f"{globals.root}/tool/build/golden_check"
    if not os.path.exists(golden_check):
        print(globals.CERR +"Error:" + globals.CEND + f" golden_check not found in tool/build directory, please compile it first", flush=True)
        exit(1)
    golden_vcd = f"{globals.out_folder}traces/vcd/golden.vcd"
    if not os.path.exists(golden_vcd):
        golden_vcd = f"{globals.out_folder}faulty_traces/vcd/golden.vcd"
    report = f"{globals.out_folder}golden_check_report.txt"
    try:
        subprocess.run(f"{golden_check} --spec-file {globals.out_folder}specifications.txt --vcd {golden_vcd} --clk {globals.clk_name} --vcd-ss \"{globals.top_module_name}_bench::{globals.top_module_name}_\" --dump-to {report} --psilent", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f" Some golden specifications do not hold or are never activated on the golden trace, see {report}", flush=True)
        exit(1)
