import json
import os
import shutil
from time import sleep
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
                with open(platformIniPath, 'r') as platformIniFile:
                    platformIniStr = platformIniFile.read()
                    if ";skiplibs" in platformIniStr or "; skiplibs" in platformIniStr:
                        print("skiplib in "+os.path.basename(workspaceFolderPath))
                        continue
                    if (os.path.exists(libDirectory)):
                        try:
                            shutil.rmtree(libDirectory)
                        except:
                            pass

                    copy_to_workspace(workspaceFolderPath,
                                      libDirectory, platformIniStr)


def copy_to_workspace(workspaceFolderPath, libDirectory, platformIniStr):
    try:
        os.mkdir(libDirectory)
    except:
        pass

    whitelisted = ";only " in platformIniStr

    def is_whitelisted(file: str):
        if (not whitelisted):
            return True
        basefile = file.split(".")[0]
        if (";only "+basefile in platformIniStr):
            return True
        return False

    for file in os.listdir(libsLocation):

        if (not is_whitelisted(file)):
            continue
        if not file.endswith("main.cpp") and not file.endswith("creds.h"):
            fileCurrent = os.path.join(libsLocation, file)
            try:
                shutil.copy(fileCurrent, os.path.join(
                    libDirectory, file))
            except PermissionError as e:
                print(e)
                sleep(0.5)
                copy_to_workspace(workspaceFolderPath,
                                  libDirectory, platformIniStr)
                return

    for file in os.listdir(libsLibLocation):
        if (not is_whitelisted(file)):
            continue
        if not file.endswith("main.cpp") and not file.endswith("creds.h"):
            fileCurrent = os.path.join(libsLibLocation, file)
            shutil.copy(fileCurrent, os.path.join(
                libDirectory, file))

    print("added for "+os.path.basename(workspaceFolderPath))
