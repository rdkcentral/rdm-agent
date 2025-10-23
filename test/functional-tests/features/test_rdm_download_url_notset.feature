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
###########################################################################

Feature: Handle missing download URL for RDM

  Scenario: RDM should exit if download URL is not found in /tmp/.xconfssrdownloadurl, /tmp/.rdm_ssr_location, or the TR-181 parameter
    Given the file /tmp/.xconfssrdownloadurl does not exist or does not contain a valid download URL
    And the TR-181 parameter for the download URL is not set or is empty
    When RDM starts the download process
    Then RDM should log an appropriate error message
    And RDM should exit immediately without attempting to download
