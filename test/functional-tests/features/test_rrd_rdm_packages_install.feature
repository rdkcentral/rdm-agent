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

Feature: RRD to RDM
  
  Scenario: Verify remote debugger process is running
    Given the remote debugger process is not running
    When I start the remote debugger process
    Then the remote debugger process should be running

  Scenario: Set tr181 cmd for Issuetype Test and verify logs
    Given the remote debugger is running
    When I trigger the event "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Feature.RDKRemoteDebugger.IssueType"
    Then the event for RRD_SET_ISSUE_EVENT should be received
    And the logs should contain "Going to RDM Request..."
    And the logs should contain "Request RDM Manager Download for a new Issue Type"
    And the logs should contain "Package TAR File Name : RDK-RRD-Test:1.0"
    And the logs should contain "Request RDM Manager Download for... RDK-RRD-Test:1.0"
     		
  Scenario: Ensure packages are successfully installed for RDK-RRD-Test
    Given the packages have been downloaded
    When the installation process completes
    Then the /media/apps directory should exist
    Then the /media/apps/rdm/downloads directory should exist
    And the directory should contain the installed package files
    And running "ls -lh /media/apps/rdm/downloads/RDK-RRD-Test" should list the package files with expected sizes

  Scenario: Verify the Issuetype is found in dynamic profile
    Given the remote debugger received the message from RBUS command
    When the remotedebugger read the json file form the dynamic path
    Then remotedebugger json read and parse should be success in dynamic path
    And the issue data node and sub-node should be found in the JSON file
  
