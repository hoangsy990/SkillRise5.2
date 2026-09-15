"""Check observational producer boundary, not fixed-tick or visual parity."""
from pathlib import Path
root = Path(__file__).resolve().parents[2] / 'ExMain_RISE_PC/Main5.2_RISE'
character = (root/'ZzzCharacter.cpp').read_text(encoding='latin-1')
assert '''if (!rise::growlancer::MagicPinFrameOwnsActor(*o))
    {
        AnimationCharacter(c, o, b);
        rise::growlancer::PublishCharacterAnimationObservation(*o);
    }''' in character
quantum = character.split('bool rise::growlancer::StepMagicPinActor(', 1)[1].split('void CreateWeaponBlur', 1)[0]
assert 'AnimationCharacterStep(Hero, &actor, &model, 1.0f);\n    PublishCharacterAnimationObservation(actor);' in quantum
runtime = (root/'RISE/GrowLancerEffectRuntime.cpp').read_text()
producer = runtime.split('void PublishCharacterAnimationObservation(',1)[1].split('bool ReadCharacterAnimationObservation(',1)[0]
for token in ('gAnimationActorIdentity.Matches(token)', '!actor.Live',
              '&actor != &Hero->Object', 'actor.Type != MODEL_PLAYER',
              'SceneFlag != MAIN_SCENE', 'gAnimationObservationSerial == 0xffffffffu',
              'AppendStep(actor.CurrentAction, actor.AnimationFrame)',
              'gAnimationObservation.Publish()',
              'if (actor.CurrentAction != 287) gAnimationObservationArmed = false;'):
    assert token in producer, token
for token in ('CreateEffect(', 'AnimationFrame =', 'AdvanceWholeTicks(',
              'PlayAnimation(', 'while (', 'for ('):
    assert token not in producer, token
print('PASS post-animation observational hook, exact frame/action, private owner/scene and rollover guards')
print('PASS producer performs no animation advance, effect emission, interpolation or tick catch-up')
consumer = runtime.split('bool ReadCharacterAnimationObservation(',1)[1].split('bool IsEffectType(',1)[0]
for token in ('controller.Type != kMagicPinControllerModel', 'controller.SubType != 0',
              'controller.Owner != &Hero->Object', 'gAnimationActorIdentity.Matches(token)',
              'FindAnimationSampleCursor(&controller)', 'cursor->Next(gAnimationObservation, action, frame)'):
    assert token in consumer, token
qa = (root/'RISE/GrowLancerRuntimeQA.cpp').read_text()
assert 's.observationAvailable = skill == 274 && ReadCharacterAnimationObservation(' in qa
assert 'observationAvailable=%d observedAction=%u observedFrame=%.9f' in qa
print('PASS bounded slot/actor consumer is wired to QA observation fields only')
print('OPEN coherent source tick production and runtime validation; existing emission gates unchanged')
