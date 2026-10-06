from helper_functions import *

def test_rdm_schedule_debugtool():
    result = run_shell_command("/l1l2/rdm-agent/rdm -s tcpdump:12345")
    assert result is not None

def test_rdm_check_debugtool_expiry():
    result = run_shell_command("/l1l2/rdm-agent/rdm -e")
    assert result is not None
