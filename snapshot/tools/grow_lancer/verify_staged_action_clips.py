"""Reopen all player assets; expose frame-sampling risks, NOT visual PASS."""
from pathlib import Path
import hashlib
import struct
import merge_player_actions as parser

root=Path(__file__).resolve().parents[2]
assets=[
    (Path(r'D:\MU FICA Season 21\Data\Player\player.bmd'),'E0F4CB5A0956192D04026135766C566BC6B9B7B559D2F196F74D24D28E2E755A'),
    (root/'Client/Data/Player/player.bmd','2964E7C41DCF686B79E53A74261192EBFC6879D36BC030C2E503EF0B8CEF38EF'),
    (root/'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/Data/Player/player.bmd','0CC3D22D5BBD426128E6BFFE9C3766585F9F28BBDA5B3DD50122EC7AD6B9CA63')]
parsed=[]
for path,pin in assets:
    assert hashlib.sha256(path.read_bytes()).hexdigest().upper()==pin
    parsed.append(parser.COMPARER.parse_zero_mesh_skeleton(parser.decode(path)))
s21,legacy,staged=parsed
assert (s21.action_count,legacy.action_count,staged.action_count)==(410,284,294)
assert staged.action_hashes[:284]==legacy.action_hashes
for source in range(185,195):
    native=source+99
    assert s21.action_hashes[source]==staged.action_hashes[native]
    print(f'PASS source{source} ->native{native}; keys={staged.key_counts[native]}; lock={staged.lock_positions[native]}')
print('PASS all284 legacy action payload hashes preserved in actual staged Player.bmd')

# A bounded counterexample under the actual unlocked PlayAnimation wrap rule.
# Does not model scheduler order, action transitions or source engine behavior.
cpp=(root/'ExMain_RISE_PC/Main5.2_RISE/ZzzBMD.cpp').read_text(encoding='latin-1')
# The native step was factored into PlayAnimationStep so QA can drive one
# explicit quantum.  Verify both the production wrapper and the helper rather
# than requiring the old inline FPS expression.
assert 'return PlayAnimationStep(AnimationFrame, PriorAnimationFrame, PriorAction,' in cpp
assert '*AnimationFrame += Speed * stepFactor;' in cpp
assert 'Key = Actions[CurrentAction].NumAnimationKeys;' in cpp
assert '*AnimationFrame = (float)(Frame % (Key)) + (*AnimationFrame - (float)Frame);' in cpp
runtime=(root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text(encoding='utf-8')
assert 'frame >= 6.8f && frame <= 7.6f' in runtime
assert staged.key_counts[288]==7 and not staged.lock_positions[288]
def f32(x):return struct.unpack('<f',struct.pack('<f',x))[0]
speed=f32(f32(.2)+f32(f32(273)*f32(.002)))
frame=f32(0)
samples=[]
while True:
    frame=f32(frame+speed)
    if frame>=7:break
    samples.append(frame)
assert not any(f32(6.8)<=x<=f32(7.6) for x in samples)
print('RISK Shining288 first unlocked traversal at speed='+str(speed)+' factor1 misses final window6.8..7.6; last pre-wrap sample='+str(samples[-1]))
print('OPEN source frame stepping and engine update/action-transition order; do not widen gates or change clip keys without evidence')
