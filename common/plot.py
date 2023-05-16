import matplotlib.pyplot as plt
import pandas as pd
import os
import sys
import subprocess

# format should be plot.py "data.csv" expname

data = pd.read_csv("../"+sys.argv[2] + "/"+sys.argv[1])

def get_processor_name():
    command ="sysctl -n machdep.cpu.brand_string"
    return (subprocess.check_output(command, shell=True).strip()).decode("utf-8")

pname = get_processor_name()
plt.xscale("log", base=10)
plt.yscale("linear")
plt.grid('on')
# plt.xscale("linear")
plt.semilogy(data["ArraySize"], data["MetalTime"]/1000, marker='o', label="metal")
plt.semilogy(data["ArraySize"], data["BLASTime"]/1000, marker='o', label="blas")
plt.semilogy(data["ArraySize"], data["vDSPTime"]/1000, marker='o', label="vDSP")
plt.legend()
plt.title(sys.argv[2] + " (" + pname + ")")
plt.xlabel("Vec. Size")
plt.ylabel("[ms]")
plt.savefig("../"+sys.argv[2] + "/" + "plot.png", dpi=300)
plt.show()






