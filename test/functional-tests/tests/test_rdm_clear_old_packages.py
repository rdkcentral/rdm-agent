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
    CLEAR_PACKAGE_MSG = " Uninstall the old packages that are not listed in current manifest"
    assert CLEAR_PACKAGE_MSG in grep_RDMlogs(CLEAR_PACKAGE_MSG)

def test_packages_dir_cleared():
    """Verify /media/apps/rdm/downloads/RDK-RRD-Test/ should not exists"""
    DIR_PATH = "/media/apps/rdm/downloads/RDK-RRD-Test"
    assert not os.path.exists(DIR_PATH), f"Directory exists but should not: {DIR_PATH}"

