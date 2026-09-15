"""Static native game-socket boundary for the isolated Grow Lancer QA EXE."""

from pathlib import Path

ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
SOCKET = ROOT / "ExMain_RISE_PC/Main5.2_RISE/WSctlc.cpp"
LAUNCHER = ROOT / "tools/grow_lancer/start_runtime_qa_client.ps1"


def main() -> None:
    source = SOCKET.read_text(encoding="utf-8-sig")
    start = source.index("int CWsctlc::Connect(")
    guard_start = source.index("#ifdef RISE_GROW_LANCER_RUNTIME_QA", start)
    guard_end = source.index("#endif", guard_start)
    guard = source[guard_start:guard_end]
    assert "strcmp(ip_addr, \"127.0.0.1\") != 0" in guard
    assert "port != 44412 && port != 55910" in guard
    assert 'GetEnvironmentVariableA("RISE_GL_LOCAL_SERVER_QA"' in guard
    assert "return FALSE;" in guard
    assert guard_end < source.index("gethostbyname(ip_addr)", start)
    assert guard_end < source.index("nResult = connect(m_socket", start)
    # The normal binary retains its original loopback rejection; this new
    # fail-closed nonlocal condition exists only in the QA compilation arm.
    assert "#else\n\t\treturn (FALSE);\n#endif" in source[guard_end:]
    launcher = LAUNCHER.read_text(encoding="utf-8-sig")
    assert "if ($Launch -and !$LocalServer)" in launcher
    assert "refusing default/VPS endpoint" in launcher
    assert "$env:RISE_GL_LOCAL_SERVER_QA = if ($LocalServer) { '1' } else { '0' }" in launcher
    assert "$arguments = if ($LocalServer) { '/u127.0.0.1 /p44412' } else { '' }" in launcher
    print("PASS: QA gameplay socket refuses non-127.0.0.1/non-44412/55910 before DNS/connect")
    print("PASS: launcher requires verified local stack; ordinary SS6 source branch unchanged")
    print("Scope: static/build game-socket boundary only; local login/gameplay not proved")


if __name__ == "__main__":
    main()
