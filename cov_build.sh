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

cd ${WORKDIR}
export INSTALL_DIR='/usr/local'
export top_srcdir=`pwd`
export top_builddir=`pwd`

autoreconf --install
export CFLAGS="-I${INSTALL_DIR}/include/rtmessage -I${INSTALL_DIR}/include/msgpack -I${INSTALL_DIR}/include/rbus -I${INSTALL_DIR}/include -I${INSTALL_DIR}/include/cjson -I/usr/include/openssl -I/usr/iarmmgrs/rdmmgr/include/ -I/usr/iarmbus/core/include/" 
export LDFLAGS="-L/usr/local/lib -Wl,-rpath,/usr/local/lib -lparsejson -ldwnlutil -lfwutils -lsecure_wrapper -lIARMBus"
./configure --prefix=${INSTALL_DIR}
make clean
make && make install
