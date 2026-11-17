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
from collections import deque
from helper_functions import *

def test_is_rdm_packages_install():
    result = subprocess.run(
        ["./rdm", "-v", "RDK-RRD-Test:1.0"],
        capture_output=True,
        text=True,
    )
    package_path = "/media/apps/rdm/downloads/RDK-RRD-Test"
    if result.returncode != 0 or not os.path.isdir(package_path):
        try:
            with open(LOG_FILE, encoding="utf-8", errors="replace") as log_file:
                log_tail = "".join(deque(log_file, maxlen=80))
        except OSError as error:
            log_tail = f"Unable to read {LOG_FILE}: {error}"
        raise AssertionError(
            f"RDM installation failed: exit code {result.returncode}, "
            f"package directory present: {os.path.isdir(package_path)}\n"
            f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}\n"
            f"Last 80 lines of {LOG_FILE}:\n{log_tail}"
        )

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

        
