import os

root = os.path.dirname(__file__)
NumFiles = int(0)

def PrintFiles(Folder):
    global NumFiles
    for file in os.listdir(root + "\\" + Folder):
        if os.path.isdir(Folder + "\\" + file):
            PrintFiles(Folder + "\\" + os.path.basename(file))
        else:
            print((Folder + "\\" + os.path.basename(file)) + ", ", end="")
            NumFiles = NumFiles + 1

def PrintFilesEntry():
    global NumFiles
    for file in os.listdir(root):
        if os.path.isdir(file):
            PrintFiles(os.path.basename(file))
        else:
            print((os.path.basename(file)) + ", ", end="")
            NumFiles = NumFiles + 1

PrintFilesEntry()
print()
print(NumFiles)