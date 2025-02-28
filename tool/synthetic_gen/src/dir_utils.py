
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
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to remove directory {dirpath}. errno: {e.returncode}")
            exit(1) 
    #Create output folder
    try:
        subprocess.run(f"mkdir -p {dirpath}/design", shell=True, check=True)
        subprocess.run(f"mkdir -p {dirpath}/expected", shell=True, check=True)
        subprocess.run(f"mkdir -p {dirpath}/traces", shell=True, check=True)
        subprocess.run(f"mkdir -p {dirpath}/faulty_traces", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to create directory {dirpath}. errno: {e.returncode}")
        exit(1)

    #populate every subdirectory
    try:
        subprocess.run(f"mv {globals.out_folder}*.v {dirpath}/design/", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move files to {dirpath}/design. {e}")
        exit(1)
    try:
        subprocess.run(f"mv {globals.out_folder}specifications.txt {dirpath}/expected/", shell=True, check=True)
    except  subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move specifications.txt to {dirpath}/expected. {e}")
        exit(1)
    try:
        subprocess.run(f"mv {globals.out_folder}/traces {dirpath}", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move traces to {dirpath}/traces. {e}")
        exit(1)
    try:
        subprocess.run(f"mv {globals.out_folder}/faulty_traces {dirpath}", shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to move faulty_traces to {dirpath}/faulty_traces. {e}")
        exit(1)

    #clean the output folder
    if not globals.debug:
        try:
            subprocess.run(f"rm -rf {globals.out_folder}", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to clean the output folder. {e}")
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
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to create directory {globals.out_folder}. errno: {e.returncode}")
            exit(1)
    # if the folder exists empty it
    else:
        try:
            subprocess.run(f"rm -rf {globals.out_folder}*", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to clean directory {globals.out_folder}. errno: {e.returncode}")
            exit(1)
        try:
            subprocess.run(f"mkdir -p {globals.out_folder}/traces/vcd", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/traces/csv", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/faulty_traces/vcd", shell=True, check=True)
            subprocess.run(f"mkdir -p {globals.out_folder}/faulty_traces/csv", shell=True, check=True)
        except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to create directory {globals.out_folder}. errno: {e.returncode}")
            exit(1)
    try:
        subprocess.run(f'touch {globals.out_folder}/specifications.txt', shell=True, check=True)
    except subprocess.CalledProcessError as e:
            print(globals.CERR +"Error:" + globals.CEND + f"Failed to create specifications.txt file {globals.out_folder}. errno: {e.returncode}")
            exit(1)

def generateCSV():
    global clk_name
    #create the csv traces from vcd traces
    try:
        subprocess.run(f"{globals.root}/tool/build/vcd2csv --vcd-dir {globals.out_folder}faulty_traces/vcd --clk {globals.clk_name} --vcd-ss \"{globals.top_module_name}_bench::{globals.top_module_name}_\" --dump-to {globals.out_folder}faulty_traces/csv",stdout=subprocess.DEVNULL if not globals.debug else None,stderr=subprocess.DEVNULL if not globals.debug else None, shell=True, check=True)
    except subprocess.CalledProcessError as e:
        print(globals.CERR +"Error:" + globals.CEND + f"Failed to convert vcd to csv. {e}")
        exit(1)
