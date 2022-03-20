import os

root = os.path.dirname(__file__)
def PrintFiles(Folder):
    for file in os.listdir(root + "\\" + Folder):
        if os.path.isdir(file):
            PrintFiles(Folder + "\\" + os.path.basename(file))
        else:
            print((Folder + "\\" + os.path.basename(file))[1:] + ", ", end="")

PrintFiles("")