"""Focused source-order guard for the native Grow Lancer local-only socket path."""

from pathlib import Path

ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
SOURCE = ROOT / "ExMain_RISE_PC/Main5.2_RISE/WSctlc.cpp"


def main() -> None:
    source = SOURCE.read_text(encoding="utf-8-sig")
    start = source.index("if (addr.sin_addr.S_un.S_un_b.s_b1 == 127")
    end = source.index("nResult = connect(m_socket", start)
    block = source[start:end]
    assert "#ifdef RISE_GROW_LANCER_RUNTIME_QA" in block
    assert 'GetEnvironmentVariableA("RISE_GL_LOCAL_SERVER_QA"' in block
    assert "port != 44412 && port != 55910" in block
    assert "#else\n\t\treturn (FALSE);\n#endif" in block
    assert block.index("#ifdef RISE_GROW_LANCER_RUNTIME_QA") < block.index("#else")
    assert "nResult = connect(m_socket" in source[end:]
    print("PASS: only opt-in local Grow Lancer QA permits 127.0.0.1 on CS/GS ports")
    print("PASS: ordinary native client still rejects loopback; connect() remains native")


if __name__ == "__main__":
    main()
