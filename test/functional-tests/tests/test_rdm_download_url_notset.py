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

def test_is_rdm_packages_install():
    remove_file("/tmp/.xconfssrdownloadurl")
    remove_file("/tmp/.rdm_ssr_location")
    command_to_check = "./rdm -v RDK-RRD-Test:1.0, timeout=120"
    result = run_shell_command(command_to_check)
    assert result != "", "rdm process did not start"

    DL_URL_MSG = "Download URL is not available in /tmp/.xconfssrdownloadurl"
    assert DL_URL_MSG in grep_RDMlogs(DL_URL_MSG)

    RFC_PARAM_MSG = "Unable to get: Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.CDLDM.CDLModuleUrl rc =1"
    assert RFC_PARAM_MSG in grep_RDMlogs(RFC_PARAM_MSG)

    DL_URL_NOTSET_MSG = "RDM download url is not available in both /tmp/.xconfssrdownloadurl and RFC parameter. Exiting..."
    assert DL_URL_NOTSET_MSG in grep_RDMlogs(DL_URL_NOTSET_MSG)
    
