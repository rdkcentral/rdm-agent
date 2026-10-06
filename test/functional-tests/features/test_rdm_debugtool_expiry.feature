Feature: Debug Tool Expiry

  Scenario: Schedule debug tool expiry
    Given RDM binary is available
    When rdm is executed with "-s tcpdump:12345"
    Then the command should complete successfully

  Scenario: Check debug tool expiry
    Given RDM binary is available
    When rdm is executed with "-e"
    Then the command should complete successfully
