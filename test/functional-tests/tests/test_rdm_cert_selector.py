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

def test_rdm_dynamic_cert_selector():
    CERT_STATUS_MSG = "RDM download certificate success. cert=/opt/certs/client.pem"
    assert CERT_STATUS_MSG in grep_RDMlogs(CERT_STATUS_MSG)

    HTTP_CODE_MSG = "curl_ret_code:0 httpCode:200"
    assert HTTP_CODE_MSG in grep_RDMlogs(HTTP_CODE_MSG)
    
