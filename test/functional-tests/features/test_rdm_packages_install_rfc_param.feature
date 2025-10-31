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

Feature: Resolve download URL for package installation

  Scenario: Use TR-181 parameter if /tmp/.xconfssrdownloadurl file does not contain a URL
    Given the file /tmp/.xconfssrdownloadurl exists
    And the file /tmp/.xconfssrdownloadurl is empty or does not contain a valid download URL
    When the system attempts to retrieve the download URL
    Then the system should check the TR-181 parameter for the download URL
    And use the TR-181 parameter value as the download URL if it is available
