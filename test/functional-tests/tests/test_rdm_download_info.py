##########################################################################
# If not stated otherwise in this file or this component's LICENSE
# file the following copyright and licenses apply:
#
# Copyright 2018 RDK Management
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
##########################################################################

from time import sleep
from helper_functions import *

def test_rdm_download_info_file_exists():
    FILE_PATH = "/opt/persistent/rdmDownloadInfo.txt"
    assert os.path.exists(FILE_PATH), f"File does not exist: {FILE_PATH}"

    APP_NAME_MSG = "RDK-RRD-Test"
    assert APP_NAME_MSG in grep_RDMDlInfologs(APP_NAME_MSG)

    PKG_NAME_MSG = "RDK-RRD-Test_1.0-signed.tar"
    assert PKG_NAME_MSG in grep_RDMDlInfologs(PKG_NAME_MSG)

    APP_DL_LOCATION_MSG = "/media/apps/RDK-RRD-Test/RDK-RRD-Test"
    assert APP_DL_LOCATION_MSG in grep_RDMDlInfologs(APP_DL_LOCATION_MSG)

    STATUS_MSG = "SUCCESS"
    assert STATUS_MSG in grep_RDMDlInfologs(STATUS_MSG)
    
