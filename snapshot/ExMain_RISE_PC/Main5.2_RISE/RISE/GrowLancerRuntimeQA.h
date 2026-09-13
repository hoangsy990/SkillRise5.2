#pragma once
class OBJECT;

namespace rise { namespace growlancer {

#ifdef RISE_GROW_LANCER_RUNTIME_QA
void RecordShiningQASample(const OBJECT& controller, float factor);
bool HandleRuntimeQAHotKey();
bool IsRuntimeQAIconPanelVisible();
#endif

}}
