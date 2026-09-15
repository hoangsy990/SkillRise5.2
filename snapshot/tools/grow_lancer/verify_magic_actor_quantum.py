"""Native actor adapter source audit; not a live animation parity test."""
from pathlib import Path
root = Path(__file__).resolve().parents[2]
text = (root/'ExMain_RISE_PC/Main5.2_RISE/ZzzCharacter.cpp').read_text(encoding='latin-1')
body = text.split('bool rise::growlancer::StepMagicPinActor(OBJECT& actor)', 1)[1].split('\nvoid CreateWeaponBlur', 1)[0]
for guard in ('#ifdef RISE_GROW_LANCER_RUNTIME_QA', '!Hero', '&actor != &Hero->Object',
              '!actor.Live', 'actor.Type != MODEL_PLAYER', 'SceneFlag != MAIN_SCENE',
              'actor.CurrentAction != 287', '!Models', '!model.Actions',
              'model.NumActions <= actor.CurrentAction'):
    assert guard in body, guard
ordered = ['VectorCopy(actor.Position, model.BodyOrigin);',
           'model.BodyScale = actor.Scale;', 'model.CurrentAction = actor.CurrentAction;',
           'AnimationCharacterStep(Hero, &actor, &model, 1.0f);',
           'PublishCharacterAnimationObservation(actor);', 'return true;']
positions = [body.index(item) for item in ordered]
assert positions == sorted(positions)
assert '#else\n    (void)actor;\n    return false;' in body
for forbidden in ('actor.AnimationFrame =', 'SetAction(', 'FPS_ANIMATION_FACTOR =',
                  'CreateEffect(', 'StepMagicPinModels(', 'StepMagicPinParticles('):
    assert forbidden not in body, forbidden
animation = text.split('bool CharacterAnimationStep(', 1)[1].split('int GetHandOfWeapon', 1)[0]
assert 'PlaySpeed = b->Actions[b->CurrentAction].PlaySpeed;' in animation
print('PASS QA-local Magic actor gates and per-quantum shared BMD setup before native animation')
print('PASS native action-end path reused; no frame forcing, effect emission or global factor write')
print('OPEN coordinator/legacy skip, multiple-quantum runtime and visual parity')
