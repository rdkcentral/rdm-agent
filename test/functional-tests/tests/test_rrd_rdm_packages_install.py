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

def test_clear_old_packages():
    remove_dir("/media/apps/rdm/downloads/RDK-RRD-Test")
    remove_dir("/media/apps/RDK-RRD-Test")

def test_setRRD_IssueType_RFCParam():
    command_to_check = "tr181 -d -s -t s -n rfc -v Test Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType"
    result = run_shell_command(command_to_check)
    assert "Set operation success" in result, '"Set operation success" not found in the output'
   
def test_rrd_rdm_download_status():
    RRD_ISSUE_EVENT_MSG = "Received event for RRD_SET_ISSUE_EVENT Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType"
    assert RRD_ISSUE_EVENT_MSG in grep_RRDlogs(RRD_ISSUE_EVENT_MSG)

    RDM_REQ_MSG = "Going to RDM Request..."
    assert RDM_REQ_MSG in grep_RRDlogs(RDM_REQ_MSG)

    RDM_DL_ISSUE_TYPE = "Request RDM Manager Download for a new Issue Type"
    assert RDM_DL_ISSUE_TYPE in grep_RRDlogs(RDM_DL_ISSUE_TYPE)

    PKG_NAME_MSG = "Package TAR File Name : RDK-RRD-Test:1.0"
    assert PKG_NAME_MSG in grep_RRDlogs(PKG_NAME_MSG)

    RDM_DL_REQ_PKG_MSG = "Request RDM Manager Download for... RDK-RRD-Test:1.0"
    assert RDM_DL_REQ_PKG_MSG in grep_RRDlogs(RDM_DL_REQ_PKG_MSG)


def test_with_sleep():
    time.sleep(2)  # sleep for 2 seconds
    assert True

def test_media_apps_exists():
    """Verify /media/apps exists and is a directory"""
    path = "/media/apps"
    assert os.path.exists(path), f"{path} does not exist"
    assert os.path.isdir(path), f"{path} is not a directory"

def test_rdm_downloads_exists():
    """Verify /media/apps/rdm/downloads exists and is a directory"""
    path = "/media/apps/rdm/downloads"
    assert os.path.exists(path), f"{path} does not exist"
    assert os.path.isdir(path), f"{path} is not a directory"

def test_packages_dir_exists():
    """Verify /media/apps/rdm/downloads/RDK-RRD-Test/ exists and is a directory"""
    path = "/media/apps/rdm/downloads/RDK-RRD-Test"
    assert os.path.exists(path), f"{path} does not exist"
    assert os.path.isdir(path), f"{path} is not a directory"

def test_expected_files_present():
    path = "/media/apps/rdm/downloads/RDK-RRD-Test"
    expected_files = ["pkg_cpemanifest", "package.json", "pkg_padding"]
    actual_files = os.listdir(path)

    for f in expected_files:
        assert f in actual_files, f"Missing expected file: {f}"


def test_setRRD_Issue_Type_Param():
    command_to_check = "tr181 -d -s -t s -n rfc -v Test Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType"
    result = run_shell_command(command_to_check)
    assert "Set operation success" in result, '"Set operation success" not found in the output'

def test_rrd_json_read_status():
    RRD_JSON_FILE_MSG = "Reading json config file /media/apps/RDK-RRD-Test/etc/rrd/remote_debugger.json"
    assert RRD_JSON_FILE_MSG in grep_RRDlogs(RRD_JSON_FILE_MSG)

    READ_JSON_SUCCESS_MSG = "Reading json file Success, Parsing the Content..."
    assert READ_JSON_SUCCESS_MSG in grep_RRDlogs(READ_JSON_SUCCESS_MSG)

    PARSE_JSON_SUCCESS_MSG = "Json File parse Success... /media/apps/RDK-RRD-Test/etc/rrd/remote_debugger.json"
    assert PARSE_JSON_SUCCESS_MSG in grep_RRDlogs(PARSE_JSON_SUCCESS_MSG)

    PARSE_READ_SUCCESS_MSG = "Dynamic Profile Parse And Read Success... /media/apps/RDK-RRD-Test/etc/rrd/remote_debugger.json"
    assert PARSE_READ_SUCCESS_MSG in grep_RRDlogs(PARSE_READ_SUCCESS_MSG)

    ISSUE_CATEGORY_MSG = "Reading Issue Category:Test..."
    assert ISSUE_CATEGORY_MSG in grep_RRDlogs(ISSUE_CATEGORY_MSG)

def test_check_and_stop_remotedebugger():
    kill_rrd()
