"""Source wiring audit only; not a native allocator/runtime test."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
native = root / 'ExMain_RISE_PC/Main5.2_RISE'
particles = (native / 'ZzzEffectParticle.cpp').read_text(encoding='utf-8')
effects = (native / 'RISE/GrowLancerEffectRuntime.cpp').read_text(encoding='utf-8')
wrapper = particles.split('int rise::growlancer::CreateMagicPinParticle(', 1)[1].split(
    'bool rise::growlancer::IsMagicPinParticleSlot', 1)[0]
assert 'if (!supported || !position || !angle || !light)\n        return -1;' in wrapper
assert 'const int slot = CreateParticleInternal(type, position, angle, light,\n        subtype, scale, owner, false);' in wrapper
assert 'if (slot >= 0 && slot < MAX_PARTICLES)\n        g_magicParticleBirthOrder.Append(static_cast<unsigned>(slot));' in wrapper
assert 'return slot;' in wrapper
assert 'CreateParticle(' not in wrapper  # legacy return0 conflates failure and slot0
query = particles.split('bool rise::growlancer::IsMagicPinParticleSlot', 1)[1].split('\n}', 1)[0]
assert 'index >= 0 && index < MAX_PARTICLES &&' in query
assert 'Particles[index].Live && g_magicParticleBirthOrder.Contains(static_cast<unsigned>(index))' in query
allocator = particles.split('static int CreateParticleInternal(', 2)[2]
reset = allocator.index('g_magicParticleBirthOrder.Remove(static_cast<unsigned>(i));')
assert allocator.index('if (!o->Live)') < reset < allocator.index('o->Live = true;')
hit = effects.split('void EmitMagicHitTick(', 1)[1].split('void CreateShiningModel', 1)[0]
assert hit.count('CreateMagicPinParticle(') == 2
assert effects.count('CreateMagicPinParticle(') == 3
assert 'CreateParticle(BITMAP_CLUD64, randomPosition, effect.Angle,\n                        light, 19, 2.0f, 0);' in effects
assert 'CreateMagicPinParticle(BITMAP_ENERGY, position, effect.Angle, light,\n            effect.SubType == 0 ? 9 : 10, 1.0f, NULL);' in effects
print('PASS source wiring: three Magic emit sites explicitly tag successful native slots')
print('PASS source guards: failure/slot0 distinction, dead-slot query and allocation reset')
print('PASS Shining shared subtype19 remains untagged; no owner substitution')
print('OPEN native allocator stress, retirement/reuse runtime and coordinated scheduling')
