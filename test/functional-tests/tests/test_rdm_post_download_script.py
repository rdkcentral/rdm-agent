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
    command_to_check = "./rdm -v RDK-RRD-Test:1.0"
    result = run_shell_command(command_to_check)
    assert result != "", "rdm process did not start"

def test_rdm_post_services_status():
    RUN_SCRIPT_RDM_DL_MSG = "Running Scripts after RDM Download"
    assert RUN_SCRIPT_RDM_DL_MSG in grep_RDMlogs(RUN_SCRIPT_RDM_DL_MSG)

    POST_DL_SCRIPT_MSG = "RDM - Executing post-download scripts /media/apps/RDK-RRD-Test/etc/rdm/post-services//post_cadl.sh"
    assert POST_DL_SCRIPT_MSG in grep_RDMlogs(POST_DL_SCRIPT_MSG)

