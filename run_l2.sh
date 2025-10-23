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

cp ./unittest/mocks/rdm-manifest.json /etc/rdm/rdm-manifest.json

echo "https://mockxconf:50056/rdmUploadFile" > /tmp/.rdm_ssr_location
echo "https://mockxconf:50056/rdmUploadFile" > /tmp/.xconfssrdownloadurl

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_packages_install.json test/functional-tests/tests/test_rdm_packages_install.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_download_info.json test/functional-tests/tests/test_rdm_download_info.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_download_url_notset.json test/functional-tests/tests/test_rdm_download_url_notset.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_packages_cleared.json test/functional-tests/tests/test_rdm_packages_cleared.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_clear_old_packages.json test/functional-tests/tests/test_rdm_clear_old_packages.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/test_rdm_packages_install_rfc_param.json test/functional-tests/tests/test_rdm_packages_install_rfc_param.py
pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_download_on_demand.json test/functional-tests/tests/test_rdm_download_on_demand.py

mkdir -p /media/apps
mount -t tmpfs -o size=60K tmpfs /media/apps

pytest --json-report --json-report-summary --json-report-file $RESULT_DIR/rdm_tmpfs_packages_install.json test/functional-tests/tests/test_rdm_packages_install_tmpfs.py

umount /media/apps

