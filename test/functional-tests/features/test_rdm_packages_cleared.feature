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

Feature: Cleanup of downloaded RDM packages on failure

  Scenario: Cleanup on failure during RDM package download or extracting or installing
    Given the RDM package "RDK-RRD-Test_1.0-signed.tar" is available for installation
    When the RDM starts downloading The Package package
    And the "downloading or extracting or installing" step fails due to "File not Found or Failed to extract the package"
    And all partially downloaded, extracted, or installed files should be cleaned up
    And an error log should record the failure and cleanup actions
    Then the app download status should be updated in the file /opt/persistent/rdm_download_info.txt

