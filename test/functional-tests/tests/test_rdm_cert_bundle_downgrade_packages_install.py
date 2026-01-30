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

def test_is_rdm_cert_bundle_packages_install():
    command_to_check = "./rdm -v cert-test-bundle:0.2"
    result = run_shell_command(command_to_check)
    assert result != "", "rdm process did not start"

def test_rdm_cert_download_status():
    DL_CERT_BUNDLE_MSG = "Downloading cert-test-bundle..."
    assert DL_CERT_BUNDLE_MSG in grep_RDMlogs(DL_CERT_BUNDLE_MSG)

    DL_STATUS_MSG = "RDM package download success: cert-test-bundle_0.2-signed.tar"
    assert DL_STATUS_MSG in grep_RDMlogs(DL_STATUS_MSG)

    DL_DONE_MSG = "Download completed for App cert-test-bundle with status=0"
    assert DL_DONE_MSG in grep_RDMlogs(DL_DONE_MSG)

    INST_SUCCESS_MSG = "Post Installation Successful for cert-test-bundle"
    assert INST_SUCCESS_MSG in grep_RDMlogs(INST_SUCCESS_MSG)

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
    """Verify /media/apps/rdm/downloads/cert-test-bundle/ exists and is a directory"""
    path = "/media/apps/rdm/downloads/cert-test-bundle"
    assert os.path.exists(path), f"{path} does not exist"
    assert os.path.isdir(path), f"{path} is not a directory"

def test_expected_files_present():
    path = "/media/apps/rdm/downloads/cert-test-bundle"
    expected_files = ["pkg_cpemanifest", "package.json", "pkg_padding", "cert-test-bundle_0.2.tar", "cert-test-bundle_0.2.sig"]
    actual_files = os.listdir(path)

    for f in expected_files:
        assert f in actual_files, f"Missing expected file: {f}"


