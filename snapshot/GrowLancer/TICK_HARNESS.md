# Private tick helper validation

Status: focused PASS, not gameplay/visual acceptance.

Client `ZzzEffect.cpp` and `Tests/GrowLancerTickTest.cpp` include the same
`RISE/GrowLancerTick.h` template. Runtime passes native UpdateEffect and
EffectDestructor; the harness passes counting callbacks and a surrogate record.
No S21 layout or address is transplanted into the helper.

MSVC x86 `/EHsc /std:c++14 /W4 /WX`, assertions enabled, test exited 0.
Output is confined to `ExMain_RISE_PC/Tests/GrowLancerBuild/GrowLancerTickTest.exe`.
Test initially needed an explicit `<initializer_list>` include; final build passed.

Covered: initial life 20/30, factors 0.25/0.5/1/2, live at life zero after
20/30 ticks, one final update before destruction, exactly one destruction,
no updates after death, retained fractional tick, and 100-tick overshoot.

Not covered: native OBJECT destructor internals, allocator slot reuse,
production update equations/coordinate transforms, floating-point wall-clock
cadence, arbitrary fractional partitions, rendering, or S21 visual parity.

Client rebuild passed with the existing cryptlib LNK4099 warnings only.
Current build hash is pinned in RUNTIME_QA.md. Goal remains IN_PROCESS.
