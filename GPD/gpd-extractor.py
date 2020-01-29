import os
import glob
import errno
import sys
from unidecode import unidecode
import shutil

def main():
    gpd_sdk_path="/mnt/c/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7/protocol/zigbee/app/gpd"
    destinationPath = os.getcwd() + '/headlessGPD'

    if not os.path.exists(gpd_sdk_path):
        print('GPD Source folder does not exist : ' + gpd_sdk_path)
        sys.exit()
    
    try:
        os.mkdir(destinationPath)
    except OSError as e:
        if e.errno != errno.EEXIST:
            print(e)
            raise

    directoriesOfInterest = ["components", "gpd-gcc-ld-files", "plugin/apps", "sample-app/green-power-device"]
    filesOfInterest = [ ["gpd-components-common.h", "gpd-nv.c", "gpd-nwk.c", "gpd-rail.c", "gpd-security.c", "gpd-send-receive.c"],
                        ["gpd-gcc-cfg-series1.ld"],
                        ["gpd-apps-commission.c", "gpd-apps-crypto-test.c", "gpd-apps-main.c", "gpd-apps-node.c", "gpd-apps-utility.c"],
                        ["gpd-callbacks.c", "gpd-mbedtls-config.h", "rail_config.c", "rail_config.h"] ]

    dirIndex = 0 #TODO check if this is not available elsewhere
    for scannedDirectory in directoriesOfInterest:
        currentWorkingDir = gpd_sdk_path + '/' + scannedDirectory
        destinationDir = destinationPath + '/' + scannedDirectory

        print("Scanning Files in " + currentWorkingDir)
        print("Creating destination folder...")
        try:
            os.makedirs(destinationDir)
        except OSError as e:
            if e.errno != errno.EEXIST:
                print(e)
                raise

        for fileOfInterest in filesOfInterest[dirIndex]:
            filePath = currentWorkingDir + '/' + fileOfInterest
            print("Working on file " + filePath)

            try:
                os.path.exists(filePath)
                os.path.isfile(filePath)
            except OSError as e:
                print(e)
                raise

            print("Copying file to destination directory")
            shutil.copy(filePath, destinationDir)

        dirIndex += 1
            

if __name__== "__main__":
    try:
        main()
    except Exception as e:
        print("An exception occurred, leaving")
        print(e)