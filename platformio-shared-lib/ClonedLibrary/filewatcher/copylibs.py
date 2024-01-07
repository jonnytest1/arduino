import json
import os
import shutil
workspaceJsonPath = "D:\\Jonathan\\visualstudio-workspaces\\smarthome.code-workspace"

workspaceJsonDirectory = os.path.dirname(workspaceJsonPath)
projectroot = os.path.realpath(os.path.join(os.path.dirname(__file__), ".."))
libsLocation = os.path.join(projectroot, "src")
libsLibLocation = os.path.join(projectroot, "lib/lib")


def copy_all():
    with open(workspaceJsonPath, 'r') as workspaceFile:
        workspaceJsonStr = workspaceFile.read()
        workspaceJson = json.loads(workspaceJsonStr)

        for folder in workspaceJson["folders"]:
            workspaceFolderPath = os.path.abspath(
                os.path.join(workspaceJsonDirectory, folder["path"]))
            platformIniPath = os.path.join(
                workspaceFolderPath, "platformio.ini")
            libDirectory = os.path.abspath(
                os.path.join(workspaceFolderPath, "src\\lib"))
            if (os.path.exists(platformIniPath) and libsLocation.lower() not in libDirectory.lower()):
                if (os.path.exists(libDirectory)):
                    try:
                        shutil.rmtree(libDirectory)
                    except:
                        pass

                copy_to_workspace(workspaceFolderPath, libDirectory)


def copy_to_workspace(workspaceFolderPath, libDirectory):
    os.mkdir(libDirectory)

    for file in os.listdir(libsLocation):
        if not file.endswith("main.cpp") and not file.endswith("creds.h"):
            fileCurrent = os.path.join(libsLocation, file)
            shutil.copy(fileCurrent, os.path.join(
                libDirectory, file))
    for file in os.listdir(libsLibLocation):
        if not file.endswith("main.cpp") and not file.endswith("creds.h"):
            fileCurrent = os.path.join(libsLibLocation, file)
            shutil.copy(fileCurrent, os.path.join(
                libDirectory, file))

    print("added for "+os.path.basename(workspaceFolderPath))
