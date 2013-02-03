function Usage
{
    cat<<HELP
Usage: mediatek/build/android/tools/`basename $0` project
Description:
    project
        project represent the correct project name 
Example:
   mediatek/build/android/tools/`basename $0` oppo
   mediatek/build/android/tools/`basename $0` generic   
Attention:
    for emulator,please use generic for the project argument
HELP
}

while getopts "h" OPT;do
case $OPT in
h) Usage;exit 1 ;;
?) Usage;exit 1 ;;
esac
done
shift `expr $OPTIND - 1`
if [ $# != 1 ];then
    Usage
    exit 1
fi
prjConfig="./mediatek/config/$1/ProjectConfig.mk"
if [ ! -e "${prjConfig}" ]; then
    echo "Error!the argument $1 is not a correct project name!"
    echo "please enter -h for help infomation!"
    exit 1
fi

mkdir -p vendor/mediatek/$1/artifacts/obj/hardware/libhardware_legacy/gps
cp out/target/product/$1/obj/SHARED_LIBRARIES/libhardware_legacy_intermediates/gps/*.o vendor/mediatek/$1/artifacts/obj/hardware/libhardware_legacy/gps/
cp mediatek/build/android/tools/mkfiles/hardware_gps.mk vendor/mediatek/$1/artifacts/obj/hardware/libhardware_legacy/gps/Android.mk
mkdir -p vendor/mediatek/$1/artifacts/obj/hardware/libhardware_legacy/wifi
cp out/target/product/$1/obj/SHARED_LIBRARIES/libhardware_legacy_intermediates/wifi/*.o vendor/mediatek/$1/artifacts/obj/hardware/libhardware_legacy/wifi/
cp mediatek/build/android/tools/mkfiles/hardware_wifi.mk vendor/mediatek/$1/artifacts/obj/hardware/libhardware_legacy/wifi/Android.mk
mkdir -p vendor/mediatek/$1/artifacts/obj/$(MTK_PATH_SOURCE)/hardware/ril/mtk-ril
cp mediatek/build/android/tools/mkfiles/mtk-ril.mk vendor/mediatek/$1/artifacts/obj/$(MTK_PATH_SOURCE)/hardware/ril/mtk-ril/Android.mk
mkdir -p vendor/mediatek/$1/artifacts/obj/external/opencore
cp mediatek/build/android/tools/mkfiles/opencore.mk vendor/mediatek/$1/artifacts/obj/external/opencore/Android.mk
mkdir -p vendor/mediatek/$1/artifacts/out/target/common/obj/JAVA_LIBRARIES
cp -rf out/target/common/obj/JAVA_LIBRARIES/static_gemini_intermediates/ vendor/mediatek/$1/artifacts/out/target/common/obj/JAVA_LIBRARIES/
# release BT database
# Todo: 
# Use MTK_BT_SUPPORT switch to wrap here
mkdir -p vendor/mediatek/$1/artifacts/BTDataBase
cp mediatek/source/external/bluetooth/blueangel/_bt_scripts/database_win32/BTCatacherDB vendor/mediatek/$1/artifacts/BTDataBase/.

# prevent dive into deeper layer
echo "# empty "> vendor/mediatek/$1/artifacts/Android.mk

