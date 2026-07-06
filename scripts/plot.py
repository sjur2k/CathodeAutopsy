import matplotlib.pyplot as plt
import pandas as pd
import os
plt.rcParams["mathtext.fontset"] = "cm"
plt.rcParams["font.family"] = "serif"
plt.rcParams["font.serif"] = ["CMU Serif", "DejaVu Serif"]
cwd = os.getcwd()
with open(f"{cwd}/data/grid.csv") as f:
    meta_line = f.readline().lstrip("#").strip().split(",")
rows =   int(meta_line[1])
cols =   int(meta_line[3])
res  = float(meta_line[5])

df = pd.read_csv(f"{cwd}/data/grid.csv",delimiter=",",comment="#")
x, y, z = df["x"]*res, df["y"]*res, df["z"]
fig = plt.figure(figsize=(15,8))
ax = fig.add_subplot(projection="3d")
ax.set_box_aspect((1, rows/cols, 1))
ax.plot_trisurf(x,y,z,cmap = "gray")
ax.scatter(x,y,z,s=1,marker="o",color="r")
ax.set_zlim(0,1.0)
ax.set_xlim(0,cols*res)
ax.set_ylim(0,rows*res)
ax.set_xlabel("x [m]")
ax.set_ylabel("y [m]")
ax.set_zlabel("z [m]")
ax.view_init(azim=190)
ax.set_title("Simulated example cathode erosion data",y=0.9)
plt.tight_layout()
#plt.savefig(f"{os.getcwd()}/media/Example_Cathode_Erosion.png", dpi=400)
plt.show()