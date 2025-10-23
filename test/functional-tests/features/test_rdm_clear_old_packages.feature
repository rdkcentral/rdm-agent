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

Feature:  Removal of obsolete RDM packages not listed in current manifest

  Scenario: Verify obsolete RDM packages are removed when not in current manifest
    Given the system has previously installed RDM packages:
      | package_name  | version |
      | RDK-RRD-Test_1.0-signed.tar  | 1.0 |
    And the current manifest lists only the valid package "SKXI11ADS_stable2_20251021095505_systemd-analyze-signed.tar" version "NA"
    When the rdm downloads package from a manifest file
    Then the package "RDK-RRD-Test_1.0-signed.tar" should be removed from the system
    And the package "SKXI11ADS_stable2_20251021095505_systemd-analyze-signed.tar" should install
    And the system log should record the removal of the obsolete package "RDK-RRD-Test_1.0-signed.tar"
