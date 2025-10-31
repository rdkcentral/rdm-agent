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

def test_RFC_Set_Handler():
    command_to_check = "tr181 -d -s -t bool -v false Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.systemd-analyze.Enable"
    result = run_shell_command(command_to_check)
    assert "Set operation success" in result, '"Set operation success" not found in the output'

def test_is_rdm_packages_install_RFC_disabled():
    command_to_check = "./rdm"
    result = run_shell_command(command_to_check)
    assert result != "", "rdm process did not start"

def test_on_demand_download_RFC_disabled():
    DL_ON_DEMAND_MSG = "Download On Demond     : 1"
    assert DL_ON_DEMAND_MSG in grep_RDMlogs(DL_ON_DEMAND_MSG)

    DL_METHOD_CTRL_MSG = "Download Method Control: RFC"
    assert DL_METHOD_CTRL_MSG in grep_RDMlogs(DL_METHOD_CTRL_MSG)

    DL_ON_DEMAND_SET_MSG = "dwld_on_demand set to yes!!! Check RFC value of the APP to be downloaded"
    assert DL_ON_DEMAND_SET_MSG in grep_RDMlogs(DL_ON_DEMAND_SET_MSG)

    RFC_ENABLE_MSG = "APP RFC is not enabled, skipping the download for:App:0=>yes=>systemd-analyze"
    assert RFC_ENABLE_MSG in grep_RDMlogs(RFC_ENABLE_MSG)

def test_RFC_Get_Handler():
    command_to_check = "tr181 -d -s -t bool -v true Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.systemd-analyze.Enable"
    result = run_shell_command(command_to_check)
    assert "Set operation success" in result, '"Set operation success" not found in the output'

def test_rdm_packages_install_RFC_enabled():
    command_to_check = "./rdm"
    result = run_shell_command(command_to_check)
    assert result != "", "rdm process did not start"

def test_on_demand_download_RFC_enabled():
    DL_ON_DEMAND_MSG = "Download On Demond     : 1"
    assert DL_ON_DEMAND_MSG in grep_RDMlogs(DL_ON_DEMAND_MSG)

    DL_METHOD_CTRL_MSG = "Download Method Control: RFC"
    assert DL_METHOD_CTRL_MSG in grep_RDMlogs(DL_METHOD_CTRL_MSG)

    DL_ON_DEMAND_SET_MSG = "dwld_on_demand set to yes!!! Check RFC value of the APP to be downloaded"
    assert DL_ON_DEMAND_SET_MSG in grep_RDMlogs(DL_ON_DEMAND_SET_MSG)

    START_DL_MSG = "Start the download of App: systemd-analyze"
    assert START_DL_MSG in grep_RDMlogs(START_DL_MSG)

    DL_MSG = "Downloading systemd-analyze..."
    assert DL_MSG in grep_RDMlogs(DL_MSG)
    
