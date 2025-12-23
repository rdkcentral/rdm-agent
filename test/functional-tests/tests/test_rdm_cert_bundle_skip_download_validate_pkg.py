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
    command_to_check = "./rdm -v cert-test-bundle:0.7"
    result = run_shell_command(command_to_check)
    assert result != "", "rdm process did not start"

def test_skip_download_and_validate_package_name():
    SKIP_DL_VALIDATE_PKG_MSG = "cert-test-bundle already downloaded on secondary storage. So skip the download and validate the package."
    assert SKIP_DL_VALIDATE_PKG_MSG in grep_RDMlogs(SKIP_DL_VALIDATE_PKG_MSG)
    
    VALIDATE_PKG_MSG = "Validate the Package"
    assert VALIDATE_PKG_MSG in grep_RDMlogs(VALIDATE_PKG_MSG)

