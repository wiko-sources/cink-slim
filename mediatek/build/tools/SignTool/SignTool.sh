#!/bin/sh
##############################################################
# Program:
#	SIGN TOOL
#

CUR_DIR=`pwd`

if [ "$3" == "" ]; then
    echo "Usage: Sign Image .."
    echo "       ./SignTool [KEY] [CONFIG] [INPUT_IMAGE] [OUTPUT_IMAGE]"
    echo ""
    echo "Example:"
    echo "       ./SignTool.sh IMG_KEY.ini IMG_CFG.ini u-boot.bin signed-u-boot.bin"    
    echo ""
    exit 1;
fi

cd mediatek/build/tools/SignTool
make
cd $CUR_DIR

##############################################################
# Generate Signature
SignTool=${0%/*}/SignTool
./${SignTool} $1 $2 $3 SIGNATURE HEADER

##############################################################
# Attach Signature
 
if [ $? -eq 0 ] ; then
#if [ -e SIGNATURE ]; then
    echo "SIGN PASS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!."
    cp -f HEADER $4
    cat $3 >> $4
    cat SIGNATURE >> $4
    rm -f SIGNATURE
    rm -f HEADER
else
    echo "SIGN FAIL !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!."
    exit 1;
fi
