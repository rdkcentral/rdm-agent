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

Feature: Fallback to tmpfs for package downloads

  Scenario: Use tmp when /media/apps does not have enough space
    Given the /media/apps directory has insufficient space for the app
    When the system attempts to download packages
    Then the system should mount a tmpfs filesystem at /media/apps
    And the packages should be downloaded to /tmp/rdm/downloads
