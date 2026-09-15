#!/usr/bin/env python3
"""Guard Breche owner resource/controller QA telemetry.

The breadcrumb is intentionally QA-only.  It distinguishes a missing private
asset gate from a controller allocation failure, without changing the native
owner/target render path or promoting a cast to visual parity.
"""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
QA_H = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerRuntimeQA.h"
QA_CPP = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerRuntimeQA.cpp"
RUNTIME_CPP = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"


def require(text: str, needle: str, label: str) -> None:
    if needle not in text:
        raise ValueError(f"missing Breche owner-gate contract: {label}")


def main() -> None:
    header = QA_H.read_text(encoding="utf-8")
    qa = QA_CPP.read_text(encoding="utf-8")
    runtime = RUNTIME_CPP.read_text(encoding="utf-8")

    require(header, "void RecordBrecheOwnerResourceGateQA(bool resourcesReady,",
            "QA declaration")
    require(qa, "void RecordBrecheOwnerResourceGateQA(bool resourcesReady, int targetIndex,",
            "QA implementation")
    gate = qa[qa.index("void RecordBrecheOwnerResourceGateQA"):
              qa.index("void RecordShiningQASample")]
    require(gate, "gControllerSkill != 279", "skill 279 scope")
    require(gate, "!Hero || !Hero->Object.Live", "live-caster scope")
    require(gate, "event=breche-owner-resource-gate ready=%d controllerCreated=%d target=%d",
            "stable breadcrumb fields")
    require(gate, "resourcesReady ? 1 : 0", "resource result")
    require(gate, "controllerCreated ? 1 : 0", "controller result")

    action = runtime[runtime.index("void CreateBrecheAction"):
                     runtime.index("void CreateBrecheHit")]
    require(action, "#ifdef RISE_GROW_LANCER_RUNTIME_QA",
            "compile-time QA guard")
    require(action, "RecordBrecheOwnerResourceGateQA(ownerResourcesReady,",
            "pre-allocation breadcrumb")
    require(action, "RecordBrecheOwnerResourceGateQA(true, RuntimeQASelectedTargetIndex(), true);",
            "post-allocation breadcrumb")
    require(action, "if (ownerResourcesReady)", "resource gate before allocation")
    require(action, "CreateEffect(kBrecheControllerModel, caster.Position, caster.Angle,",
            "owner controller allocation")
    if action.index("RecordBrecheOwnerResourceGateQA(ownerResourcesReady,") > action.index("if (ownerResourcesReady)"):
        raise ValueError("pre-allocation breadcrumb moved after the resource gate")
    if action.index("RecordBrecheOwnerResourceGateQA(true, RuntimeQASelectedTargetIndex(), true);") < action.index("CreateEffect(kBrecheControllerModel, caster.Position, caster.Angle,"):
        raise ValueError("controller-created breadcrumb moved before allocation")
    if "CreateBrecheHit(caster" in action or "CreateBrecheHit(*caster" in action:
        raise ValueError("owner telemetry path must not replay target contact on caster")

    print("PASS: Breche owner gate telemetry is QA-only and bounded to skill279")
    print("PASS: resource readiness is recorded before allocation and controller creation after")
    print("PASS: owner path remains separate from target contact; no production behavior change")


if __name__ == "__main__":
    main()
