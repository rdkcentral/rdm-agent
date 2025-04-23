#
## Copyright 2023 Comcast Cable Communications Management, LLC
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
## http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.
##
## SPDX-License-Identifier: Apache-2.0
#

WORKDIR=`pwd`

## Build and install critical dependency
export ROOT=/usr
export INSTALL_DIR=${ROOT}/local
mkdir -p $INSTALL_DIR

cd $ROOT


#Build rfc
mkdir -p /opt/tempdir
cd /opt/tempdir
git clone https://github.com/rdkcentral/rfc.git
cd rfc
autoreconf -i
./configure --prefix=${INSTALL_DIR} --enable-rfctool=yes --enable-tr181set=yes
cd rfcapi
make librfcapi_la_CPPFLAGS="-I/usr/include/cjson"
make install
cd $WORKDIR
rm -rf /opt/tempdir
cd ${ROOT}
rm -rf iarmmgrs
rm -rf iarmbus
git clone https://github.com/rdkcentral/iarmmgrs.git
git clone https://github.com/rdkcentral/iarmbus.git

git clone https://github.com/rdkcentral/tr69hostif.git
cd tr69hostif
cd ./src/unittest/stubs
g++ -fPIC -shared -o libIARMBus.so iarm_stubs.cpp  -I/usr/tr69hostif/src/hostif/parodusClient/pal -I/usr/tr69hostif/src/unittest/stubs -I/usr/tr69hostif/src/hostif/parodusClient/waldb -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/tr69hostif/src/hostif/include -I/usr/tr69hostif/src/hostif/profiles/DeviceInfo -I/usr/tr69hostif/src/hostif/parodusClient/pal -fpermissive
cp libIARMBus.so /usr/local/lib
cp libIBus.h /usr/local/include
cp libIARM.h /usr/local/include

cd ${WORKDIR}
export INSTALL_DIR='/usr/local'
export top_srcdir=`pwd`
export top_builddir=`pwd`
autoreconf -i
autoupdate
./configure --prefix=${INSTALL_DIR}
make CFLAGS="-I${INSTALL_DIR}/include/rtmessage -I${INSTALL_DIR}/include/msgpack -I${INSTALL_DIR}/include/rbus -I${INSTALL_DIR}/include -I${INSTALL_DIR}/include/cjson -I/usr/include/openssl -I/usr/iarmmgrs/rdmmgr/include/ -I/usr/iarmbus/core/include/" LDFLAGS="-L/usr/local/lib -Wl,-rpath,/usr/local/lib -lparsejson -ldwnlutil -lfwutils -lsecure_wrapper -lIARMBus"
make install
