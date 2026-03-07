#!/bin/sh

####################################################################################
# If not stated otherwise in this file or this component's Licenses.txt file the
# following copyright and licenses apply:
#
# Copyright 2024 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
####################################################################################

export top_srcdir=`pwd`
RESULT_DIR="/tmp/l2_test_report"
mkdir -p "$RESULT_DIR"

rm -rf /media/apps
mkdir -p /media/apps
mkdir -p /etc/rdm/
mkdir -p /etc/rrd/
mkdir -p /mnt/usb/

cp ./unittest/mocks/rdm-manifest.json /etc/rdm/rdm-manifest.json
cp ./unittest/mocks/remote_debugger.json /etc/rrd/remote_debugger.json
cp /mnt/RDK-RRD-Test_1.0-signed.tar /mnt/usb/RDK-RRD-Test_1.0-pkg.tar

rbuscli  set Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CDLDM.CDLModuleUrl string https://mockxconf:50056/rdmUploadFile

echo "https://mockxconf:50056/rdmUploadFile" > /tmp/.rdm_ssr_location
echo "https://mockxconf:50056/rdmUploadFile" > /tmp/.xconfssrdownloadurl
rbuscli  set Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CDLDM.CDLModuleUrl string https://mockxconf:50056/rdmUploadFile

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_packages_install.json test/functional-tests/tests/test_rdm_packages_install.py

cat /opt/logs/rdm_status.log.0
cat /opt/logs/rdm_status.log.1

ls -l /opt/logs/

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_download_info.json test/functional-tests/tests/test_rdm_download_info.py

ls -l /media/apps/rdm/downloads/RDK-RRD-Test/

ls -l /media/apps/rdm/downloads/

cat /opt/logs/rdm_status.log.0
cat /opt/logs/rdm_status.log.1
