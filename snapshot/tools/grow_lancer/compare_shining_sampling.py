"""Bounded float32 sampling model, NOT captured casts or visual acceptance.

Assumes start frame0, positive constant adjusted speed, seven unlocked keys,
ordinary action completion before effect update and one controller sample per
update. No stun, interruption, target motion, pool failure or render simulation.
Run source/asset verifiers separately to validate those underlying contracts.
"""
import struct

def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]

windows = [(f32(a), f32(b)) for a, b in
           [(3.3, 3.8), (5.0, 5.6), (5.8, 6.6), (6.8, 7.6)]]

def sample(adjusted_speed, factor):
    speed = f32(f32(.2) + f32(f32(adjusted_speed) * f32(.002)))
    frame = f32(0)
    stage = 0
    emissions = []
    for step in range(10000):
        frame = f32(frame + f32(speed * f32(factor)))
        if frame >= 7:
            return emissions
        # Mirrors sequential stage-gated if statements, not independent windows.
        for index, (low, high) in enumerate(windows):
            if stage == index and low <= frame <= high:
                emissions.append((index, step + 1, frame))
                stage += 1
    raise AssertionError('Bounded replay did not end')

for adjusted in (0, 100, 200, 273):
    reference = sample(adjusted, 1)
    print(f'SIMULATED adjusted={adjusted} source25Hz stages={[x[0] for x in reference]}')
    for fps in (10, 25, 50, 60, 144):
        factor = min(f32(25 / fps), f32(2.5))
        actual = sample(adjusted, factor)
        if fps == 25:
            assert actual == reference
        print(f'  nativeFPS={fps} stages={[x[0] for x in actual]} frames={[round(x[2], 6) for x in actual]}')
assert len(sample(273, 1)) == 3
assert len(sample(273, f32(25 / 60))) == 4
print('PASS bounded differential: identical25Hz rules; 273-speed child count differs at60FPS. Not an observed cast or fix specification.')
