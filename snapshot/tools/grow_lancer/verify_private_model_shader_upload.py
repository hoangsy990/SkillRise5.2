"""Pin the native, package-scoped VAO path for registered S21 models."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RESOURCE = ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.cpp"
STOCK = ROOT / "ExMain_RISE_PC/Main5.2_RISE/New_ModelBMD.cpp"
QA = ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp"


def main() -> None:
    resource = RESOURCE.read_text(encoding="utf-8")
    stock = STOCK.read_text(encoding="utf-8")
    qa = QA.read_text(encoding="utf-8")
    ensure = resource.split("bool EnsureModel(int modelId)", 1)[1].split(
        "bool IsVisibleModel(int modelId)", 1
    )[0]
    assert "!Models || !IsVisibleModel(modelId)" in ensure
    assert ensure.count("OGL330::IsShader() && wglGetCurrentContext()") == 2
    assert ensure.count("model.LoadMeshToVAO();") == 2
    assert ensure.count("model.UploadAllToGPU();") == 2
    assert "OGL330MODEL::ConvertOldMeshToVaoMesh(modelId);" not in ensure
    assert "modelId >= MAX_MODELS" in stock.split(
        "void OGL330MODEL::ConvertOldMeshToVaoMesh", 1
    )[1].split("void OGL330MODEL::UseShader", 1)[0]
    assert "RISE_GL_WRATH_MODEL_QA" in qa
    assert "shaderUploaded = mesh.VAO != 0 && mesh.VBO != 0" in qa
    assert "event=wrath-native-model-release stable=%d" in qa
    print("PASS: private visible models use native BMD VAO/GPU upload; SS6 converter unchanged")


if __name__ == "__main__":
    main()
