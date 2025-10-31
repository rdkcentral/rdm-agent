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

Feature: Conditional download of on-demand RDM packages based on RFC status

  Scenario: Verify on-demand packages are downloaded only when RFC is enabled
    Given the packages have been downloaded
    When the installation process completes
    Then the /media/apps directory should exist
    Then the /media/apps/rdm/downloads directory should exis
    And the directory should contain the installed package files
    And running "ls -lh /media/apps/rdm/downloads" should list the package files with expected sizes

  Scenario: Skip on-demand package download when RFC is disabled
    Given RFC is disabled
    When RDM checks for on-demand package downloads
    Then it should skip the download process  
