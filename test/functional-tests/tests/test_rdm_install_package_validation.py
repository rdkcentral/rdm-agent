import subprocess

import pytest
from helper_functions import grep_RDMlogs


@pytest.mark.parametrize("package_value", [
    "",
    "RDK-RRD-Test:1",
    "RDK-RRD-Test:1.1.1",
    "RDK-RRD-Test:1.",
    "RDK-RRD-Test:.1",
    "RDK-RRD-Test:1.a",
    "RDK-RRD-Test:1.0:extra",
    "app:RDK-RRD-Test:1.0",
    "cert:RDK-RRD-Test:1.0",
])
def test_invalid_versioned_package_value_is_rejected(package_value):
    result = subprocess.run(
        ["./rdm", "-v", package_value],
        capture_output=True,
        text=True,
        timeout=30,
    )

    output = result.stdout + result.stderr
    if package_value:
        expected_message = f"Invalid install package value '{package_value}'"
    else:
        expected_message = "Invalid install package value: empty input"

    assert expected_message in output or expected_message in grep_RDMlogs(expected_message)
