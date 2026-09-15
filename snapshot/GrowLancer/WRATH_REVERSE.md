# Wrath (skill 278) Season 21 client reverse

### Current QA-only projection safety and stage — 2026-09-15

The ground probe now checks native `PerspectiveX/Y` and camera-space Z before
calling `ZzzOpenglUtil::TransformPosition`, whose screen-coordinate math
divides by those values. Source precondition order and bounds verifier PASS;
the actual Wrath terrain/model blend and renderer remain unchanged. Current
private staged QA Engine SHA-256 is `6DE3A1FECF7A868F428A962851084470FA6F0687527CB575210A5F75740218BF`
with exact prior `2E69CECC...` rollback and full private Data verifier PASS.
The current candidate adds only a QA-hidden merged-player Spin pose fixture; the Wrath
probe path is unchanged. The `FB4C8E42...` candidate described below is a
historical checkpoint.
There is still **no** new owner Wrath F7 gameplay ground-pixel row; black-square
attribution and visual acceptance remain OPEN.

### Native ground before/after pixel attribution probe — 2026-09-15

Stock `RenderTerrainAlphaBitmap` does not draw a single fake square: it binds
the registered bitmap and emits immediate `GL_TRIANGLE_FAN` terrain tiles at
`BackTerrainHeight+5`. The private Wrath call selects `EnableAlphaBlend`
(`GL_ONE/GL_ONE`) before and restores it after. Historical selected-target
PID28160 reached 32 ground draws with blend1/src1/dst1 and bound GL texture
3207, but did not record the expected `brocken` GL handle or a frame delta.
Those older rows plus dark JPG corners cannot identify the owner's black
square by themselves.

QA-only `RISE_GL_WRATH_PIXEL_QA=1` now samples up to four actual native
ground draws after `EnableAlphaBlend` and around only
`RenderTerrainAlphaBitmap`: one native ground-center projection
(`RequestTerrainHeight+5`, `TransformPosition`) and four +/-32 viewport points,
RGBA before/after pixels, a non-nested `GL_SAMPLES_PASSED` query, read/draw
buffer and blend factors. `RecordWrathGroundQA` additionally compares the
actual post-draw bound GL handle with `Bitmaps.FindTexture(kWrathBrokenBitmap)`.
All samples buffer until F11/controller flush, with no file I/O in render;
the launcher defaults the opt-in OFF and restores its parent environment.
`verify_wrath_ground_pixel_probe.py` PASS pins four-draw/five-point bounds,
source draw order and material check. Isolated QA/nonQA Win32 link PASS,
private staged QA Engine FB4C8E42... with 4C5A21C1... rollback/full Data
verifier PASS. Hidden material PID12060 remains GPU/release PASS, but no
owner Wrath F7 has yet populated these pixel rows. A few sampled points do
not prove the full square or visual parity; owner black-square gate OPEN.

### Native private model shader upload — 2026-09-15

Stock `OGL330MODEL::ConvertOldMeshToVaoMesh(modelId)` returns for
`modelId >= MAX_MODELS`; every registered visible Grow Lancer model is in
that tail. Previous isolated hidden Wrath PID5924 loaded exact BMD/bitmap
materials and GPU pixels but had `shaderMesh=0`. Package `EnsureModel` now
calls the existing native `BMD::LoadMeshToVAO` and `UploadAllToGPU` for only
its registered visible BMDs when the shader/WGL context exists, including
models opened earlier. The shared SS6 converter and loader remain unchanged.

QA/nonQA x86 builds passed. Hash-pinned isolated QA Engine B47BF165... was
staged with a C699C5CD... rollback and full Data verification. Hidden PID4008
exit0 logged both Wrath BMD materials, matching CPU/GPU texture pixels,
nonzero VAO/VBO/IBO/index counts, GL error0 and stable model/bitmap release.
`verify_private_model_shader_upload.py` pins package scope. This proves mesh
upload, **not** an ingame draw, the owner-reported black-square fix, shader
batching, persistent buff timing or S21 matching-frame visual parity.

The subsequent QA-only `RISE_GL_PRIVATE_MODELS_QA` hidden PID3248 ran all 18
registered visible BMDs (10119..10136), including both Wrath models, and
logged native texture/geometry GPU upload, GL error0 and stable release for
18/18. `verify_private_model_gpu_log.py --pid 3248` PASS. The owner's Wrath
black-square observation remains OPEN until controlled gameplay pixels and
actual layer blend/bound texture are measured.

### Black-square asset/material attribution corrected — 2026-09-15

`verify_wrath_asset_edge_alpha.py` pins each original/staged Wrath JPG/OZT
hash and follows the **actual** native loader offsets: OZJ JPEG starts
at byte24, OZT BGRA pixel stream at byte22 (`OpenTga`), not byte24.
`brocken.OZJ` is 256x256 RGB with black corners and all 4-pixel outer-border
channels <=4; the source-proven ground draw uses GL_ONE/GL_ONE, under which
true black RGB cannot darken the framebuffer. `alpha_light.OZT` outer alpha
max44/255 and `dust01.OZT` max16/255, both below the native 0.25 alpha-test
threshold. `!Combo3` and `alpha_line` have some brighter edge pixels, so the
audit does **not** claim every Wrath layer has a uniformly black border.
These are asset/source observations, not measured Wrath gameplay GL state,
shader batching or owner pixels; the reported large black square remains OPEN.

The pinned S21 `wratha01.bmd` v0F and isolated RISE v0C decoded payloads
match byte-for-byte and contain exactly `alpha_line.jpg`. The corresponding
`wratha02.bmd` pair contains exactly `dust01.tga`, **not**
`alpha_line.tga`; the old two-material attribution below was incorrect.
`verify_wrath_model_material_identity.py` PASS. The source tree has
`dust01.OZT` but no `alpha_line.OZT`, and native `EnsureModel` resolves each
mesh material in its model directory via existing `BMD::Open2`/bitmap loader.
This rules out an imagined missing `alpha_line.OZT` as a reason to stage or
alter assets; actual model material GPU binding and black-square pixels
still need isolated QA.

### Native 5.2 additive-state leak corrected — 2026-09-15

The private `kWrathBrokenBitmap` terrain renderer already selected the
source-proven ONE/ONE blend mode before its JPG ground draw, but returned
without calling native `DisableAlphaBlend`. `ZzzEffect.cpp::RenderEffects`
does not reset blend at pass end, so later native effect records could inherit
Wrath's additive state. The scoped adapter now restores state **after** the
terrain call and bounded QA readback, leaving Wrath's actual ground draw and
source blend factors unchanged. `verify_wrath_ground_blend_restore.py` and the
repaired extracted `test_wrath_ground_sampler.py` PASS; isolated QA/non-QA
Win32 clients build/link PASS. The private QA binary SHA256 13F48071... is
hash-staged with 6E2BAFC1... rollback and complete Data verifier PASS.
This proves source/native state hygiene only. The owner's reported black square,
animation/scale and matching S21 pixels remain unverified and are **not** PASS.

Actual sampler regression: test_wrath_ground_sampler.py compiles the extracted
RecordWrathGroundQA body with controlled GL functions. It proves exact captured
values (including deliberately incorrect blend factors),32-sample/128-read bound,
and no GL calls for non-Wrath/non-ground inputs. C++14 W4/WX PASS. This is not an
actual OpenGL context test; current owner black-square report remains unresolved.

### Ground-state QA instrumentation — 2026-09-13

Existing ground adapter already calls EnableAlphaBlend (ONE/ONE) before terrain
draw, matching pinned15A0B63. No new texture/blend change was justified by source.
QA now records at most32 reached ground draws after the terrain call: actual
blend enable/src/dst and bound texture plus scale/life/RGB. It only arms during
Wrath F7, buffers without file I/O in render and flushes through the existing
controller flush. Readbacks are diagnostics and may affect timing in that bounded
window. No current GL capture/pixel acceptance exists; black-square report remains
OPEN. Isolated build PASS, candidateF06B928F; original/staged clients unchanged.

Status: base cast controller and its complete subtype-0 child continuation are
statically recovered. Server defense-cost buff semantics and runtime QA remain
open.

## Identity/dispatch

### Native player render caller connected — 2026-09-13

Source13E7C9F loads OBJECT Type into switch local-9AC; compare0xA94 at
13E7DF2 selects13E7E03, the player branch containing13EB9DA. This agrees with
the previously recovered player animation/model identity. It is not enclosed
by the earlier back-item Bind block. Native ZzzCharacter.cpp::RenderCharacter
now calls SubmitWrathPersistentVisuals in case MODEL_PLAYER, before the existing
Attack/HelpNpc buff visuals, after action-specific visuals. Only native424 OR425
membership samples timeGetTime and invokes the submission; ordinary SS6 actors
do not consume an additional clock sample, RNG draw or pool allocation here.
The callee retains its teleport-alpha, membership, owner and resource guards.

Isolated Win32 build and source switch/gate verifier PASS. This connects the
previously uncalled function; it does not enable the disabled high-ID transport.
Known differences: exact ordering relative to other concurrent buff visuals,
source shared-clock sampling point vs per-character sample, frame frequency,
quality allocation throttling, bone equivalence and owner QA remain unverified.
No production change, client QA deployment, restart or visual PASS claimed.

### Outer teleport-alpha predicate mapped — 2026-09-13

Source OBJECT+3A is mapped to native Teleport by its writer174398E..17439E2:
state1 subtracts .1 from OBJECT+DC, and below .1 changes state to2. The writer
was decoded continuously from enclosing prologue173EF78; selected instructions
and float are pinned in verify_wrath_persistent_sprites.py. Native
ZzzInterface.cpp has the identical TELEPORT_BEGIN/Alpha/TELEPORT transition;
_define.h defines those states1/2. This is behavioral evidence, not offset copying.
The renderer13DBA07..13DBA27 skips ONLY state2 with Alpha strictly below
float1e-6 (1B509C0), not below .1. Equality/unordered falls through the JBE.
SubmitWrathPersistentVisuals now uses native Teleport/Alpha for this isolated
gate. Build Win32 and source verifier PASS. No global RenderCharacter change.
Caller insertion, surrounding branch eligibility and runtime cadence remain open;
the persistent submit function still has no caller and network readiness is false.

### Persistent submission membership gate compiled — 2026-09-13

Pinned13EB9DA..13EBA12 checks424 first, then425; either enters the same branch,
neither jumps13ECB52 past every persistent layer. SubmitWrathPersistentVisuals
now performs the same native membership OR before asset acquisition, random
sampling or pool allocation. This stops new emission on removal without deleting
already allocated children or inventing a duration. Both buffs do not double emit.
Isolated Win32 build and extended persistent-sprite source verifier PASS.
The function remains uncalled: native render-stage insertion is pending the
outer visibility predicate mapping (source OBJECT+3A==2 and Alpha threshold).
Existing source buff27 early exit corresponds numerically to native
eBuff_CrywolfNPCHide, but full source status identity still needs confirmation.
No QA deployment or authoritative buff activation was performed.

### Type/owner cleanup dispatch verified — 2026-09-13

`verify_wrath_cleanup.py` hash-pins the main dump, fully decodes selector
10EF919..10EFB4D (excluding its trailing jump table), and evaluates the reached
comparison path for424/425: both return zero. Wrapper14B65D8 therefore selects
14B66DB. This scans live records for exact type, optional subtype (-1 wildcard),
and exact owner; matching records go through14B6820. Importantly, secondary
pool10EFF55 is searched ONLY if the first pool found no match, not unconditionally.
Its type/subtype/owner filters are likewise pinned. Destruction invokes optional
13163FB and then common reset1315E97; the complete reset body is not yet audited.

This is not an owner-wide removal contract. Known cast children5EB/5EC/81E3
do not equal424/425, and the bitmap particle children use a separate pool.
Do not map these calls to deleting all native Wrath visuals. Actual allocation
of effects typed424/425 remains unproven; no claim that such records never exist.
Native activation remains disabled. Next integration work should trace the
persistent renderer's membership gate independently of this narrow cleanup.

### Gameplay membership scale refresh integrated — 2026-09-13

Anchored full source141043E decode proves world9DFB65 branch at141058F /
141059A reaches141062F early exit. Other path calls scene getter9B4504 and
source state5 exits through14106E0 before membership override. Symbolic scene
mapping beyond the native gameplay adapter remains OPEN; do not equate numeric
S21 state5 with native MAIN_SCENE5. Native CHARACTER_SCENE is4.

Native SetCharacterScale now reapplies1.1 when either424/425 exists, ONLY in
native MAIN_SCENE and outside native ChaosCastle, after baseline calculation.
Existing Change early return and selection-scene scale behavior remain untouched.
No buff means exact native baseline; two Wrath memberships never stack scale.
This is the explicit gameplay scope of the adapter, not proof of all scene parity.

Focused tests cover both memberships, absence, double presence and excluded stage.
Initial client compile exposed the legacy g_isCharacterBuff macro's unparenthesized
pointer argument; fixed using a local OBJECT pointer rather than changing the
global macro. Isolated Global Release Win32 rebuild PASS. No QA deployment or
network activation. World97, source scene names, cleanup and persistent rendering
remain open; clear-policy is still not inferred from unused metadata fields.

### Physical scale event hooks compiled — 2026-09-13

InsertBuffPhysicalEffect now intercepts exact424/425 and assigns1.1 except
native InChaosCastle; ClearBuffPhysicalEffect assigns absolute0.9 without map
guard. Both guard null owner for this isolated branch. Pure scale tests cover
nonstandard baseline, repeat application, world exception, removal/reapplication.
Existing SS6 switch cases are unchanged. Source scale verifier and isolated
Global Release Win32 build PASS. Feature readiness remainsfalse.

This closes only physical scale assignment, NOT full lifecycle. Native
InChaosCastle maps source18..23/53 but S21 world97 remains unmapped. Source
class-refresh early-exit correspondence must be resolved before placing its
membership override in native SetCharacterScale. Cleanup14B65D8 is still separate.

Cleanup evidence expanded:14B65D8 selects manager10EFF55 or fallback14B66DB.
Fallback scans records, checks live,type,optional subtype(-1 wildcard),owner,
then calls14B6820. If no match, it calls10EFF55. Do NOT call native DeleteEffect
with numeric buff424/425: these are source lookup keys, not registered private
native effect IDs. Need map actual matching effect records before cleanup port.

### Native display metadata completed; clear-policy remains separate — 2026-09-13

Record destructor9605E2 calls94B987 onrecord+34; it reaches94FF34, using
begin/end/capacity pointers at0/4/8 and dividing allocation extent by24.
Destructor loop946212 advances24 bytes. This supports bounded iteration of
the description string container, independently of assuming it is a linked list.
Full snapshot424/425 each contains one UTF8 description:
"Tăng sức tấn công và sức chiến đấu. Phòng thủ giảm."
Inspector now checks bounds, stride, string termination and exact expected text.

GetBuffinfo recognizes only424/425 and supplies the verified names, description
list/string, effectType123, native normal-buff class and item255/255. UTF8 text
is encoded via universal escapes in the private metadata header, fitting native
50-byte name and100-byte description buffers. Focused constexpr bounds/invalid-ID
tests and full isolated Win32 build PASS. No coefficients inferred from tooltip.

Source column7=1 remains an unmapped policy field; native s_ClearType retains
default0. Source column6=0 matches default notice0, but semantics remain a
separate policy audit. No global clear behavior is invented, and transport
featureReady staysfalse. Display metadata is usable; full buff lifecycle is NOT
complete merely because its tooltip and classification are now represented.

### Native buff atlas load/render integrated — 2026-09-13

stage_wrath_buff_atlas.py converts the pinned decoded DDS to native OZT at
Client/Data/RISE/GrowLancer/Wrath/WrathBuffAtlas.OZT. It preserves all RGBA
pixels with bottom-up BGRA serialization expected by CGlobalBitmap::OpenTga:
dimensions at16/18, bit depth32 at20, pixel data at22. Inverse row/channel
transform compares byte-identically with decoded DDS pixels. Output SHA256
B4CDD279EB66D2178C570C88149B3529E0E596998B7FB00F16A1839F27B44E9B.
Script refuses different existing output and pins source hash/worktree path.

Private native ID32980 was unused in inspected source and now names this atlas.
EnsureWrathBuffAtlas uses the existing bitmap loader, validates filename,
1024x256 dimensions and four components; it never replaces an unrelated slot.
UI RenderBuffIcon intercepts only424/425 and uses exact source rect UVs; loading
failure does not fall through to invalid SS6 atlas math. UnloadImages releases
only the matching private resource through native DeleteBitmap.

UI display size intentionally follows existing RISE slot width/height divided
by1.8, not S21's26x36 screen dimensions. This is native UI layout adaptation,
not a visual parity claim. GL_LINEAR/GL_CLAMP follows native UI sampling;
actual S21 filtering/edge sampling parity remains runtime-unverified.
Isolated Win32 build and scoped git diff --check PASS. No QA deployment or buff
activation: metadata descriptions/scale/lifecycle still incomplete.

### Exact Wrath atlas coordinates verified — 2026-09-13

Custom tag field order cross-checked against primary JPEXS parser source:
https://raw.githubusercontent.com/jindrapetrik/jpexs-decompiler/master/libsrc/ffdec_lib/src/com/jpexs/decompiler/flash/tags/gfx/DefineSubImage.java
and DefineExternalImage2.java in the same directory. No implementation copied.
Tag1008 is six UI16 fields: character,image,x1,y1,x2,y2. Tag1009 supplies
imageId0,target1024x256,fileImageData_I7.dds. Exact exports/crops:
- icon_424 -> symbol73 -> image0 -> (220,120)-(240,148), 20x28.
- icon_425 -> symbol72 -> image0 -> (242,120)-(262,148), 20x28.
These are source bitmap rectangles, not the script's26x36 display dimensions.
Inspector --image now asserts export, image and rectangle links explicitly.

Original ImageData_I7.ozd SHA256
E11700D7CEB453B84FA4A7106BEB346272DC8560A141B00CABF01D0CE4F0C55A
copied to isolated atlas_input and converted with pinned GFxDec_v4.exe
(`GFxDec_v4.exe atlas_input`). Decoded DDS SHA256
D00E868BE46E73623C80AFCB0661181816D661AD0D1586F5EB6E00198348BAF8.
Pillow parsed/loaded DDS1024x256 RGBA, matching target dimensions: no resize
needed to interpret these rectangles. This is decode validation, not visual QA.

Native GrowLancerWrathBuffMetadata now contains exact rectangle accessor and
atlas dimensions. Invalid/full-width alias IDs leave output unchanged. Focused
x86 constexpr tests and isolated Win32 build PASS. Renderer not wired and texture
not staged to QA; continue with native bitmap registration/format conversion.

### ImageData exports found: Wrath icons are not loose files — 2026-09-13

Read-only original ImageData.ozg SHA256
12468D64D767009F7D35E0A597FE1954F24E0447952EC4E3B10F0BB6AD14BFCA
copied into isolated buff_gfx_decode_20260913/image_input and decoded by
the previously pinned converter (`GFxDec_v4.exe image_input`). Output
Decrypted/image_input/ImageData.gfx SHA256
C524FF93EEA8CC47BFB8B0765AD28182F746998A4B09D1EF6E7BA1D7ED064433.

Exact ExportAssets records: symbol72="icon_425", symbol73="icon_424".
Their custom tag1008 payloads are respectively
48000000f200780006019400 (offset109F) and
49000000dc007800f0009400 (offset10C0). Custom tag1009 at11E contains
ImageData_I7.dds in payload
000009000e00000400010769636f6e5f393910496d616765446174615f49372e64647300.
Need verify custom tag schema before interpreting those fields as atlas crops;
the matching export names alone do not prove image dimensions/UVs.

ImageData root action333C..33F1 defines getBitmap(exportName), directly calls
flash.display.BitmapData.loadBitmap(exportName), returns bitmap or undefined
after a trace. This is the matching named-provider candidate. Movie/loading
relationship to BuffList's inherited lookup still needs explicit confirmation.

Also inspected base.ozg: source2487C880E7FEDD37D972ABB2A39BF999E32C03693B2B2F9F6C287789B73FB1CA,
decoded632A88AE5034DE482EFF9968645B14C2F8A25F9DD23C9C1783CA321DB01FAF28.
Its getBitmap prefixes "image" to its argument, so do not conflate this helper
with ImageData's direct export-name lookup. Both new decoded files are isolated.
GFx/AVM1 inspectors accept --base and --image with separate exact hash guards.
No icon staged to native client; asset and loading chain still being verified.

### AVM1 buff icon dataflow recovered — 2026-09-13

Offline inspector inspect_wrath_buff_avm1.py decodes the hash-pinned
DoInitAction payloads for sprite76 (Frame.BuffListMC.BuffIcon) and117
(asData.ImageLoader). It resolves constant pools, typed Push values,
function parameters/register flags, nested bodies and relative branches.
Adobe SWF v10 action encoding reference:
https://www.flashrealtime.com/content/dam/Adobe/en/devnet/swf/pdf/swf_file_format_spec_v10.pdf
This is a disassembler, not a complete AVM1 interpreter; unknown opcodes retain
their numeric opcode and cannot establish semantics by themselves.

BuffIcon.setData bodyEAFD..ECE6 reads this._data.BuffType, converts to Number,
casts this._icon to MovieClip and calls gSystem.Image.DrawIcon_Buff(_icon, ID).
ImageLoader DrawIcon_Buff body2CC3F..2CC6A has parameters targetMC register4,
buffIndex register3; constructs "icon_" + buffIndex, calls this.getBitmap,
then returns this.AttachImg(targetMC, bitmap). No effectType123 substitution
occurs in these script bodies. Hence Wrath requests icon_424 / icon_425.

AttachImg rejects undefined bitmap with false; otherwise clears/reuses or
creates mcDrawImg and calls attachBitmap(img,1,"auto",true). Caller sets icon
width26,height36 and visible to returned success; failure exposes debug text.
The two requested loose .ozd files were NOT found at the expected GFx paths,
nor matching icon_424/425 filenames in Data/Interface enumeration. Resolution
inside getBitmap/provider or packed resources remains OPEN; do not substitute
icon123 or declare that missing loose files prove no original icon exists.

Initial decoder lacked Push double type6 and stdout failed on Korean constants;
both corrected (SWF word-swapped double, escaped unsupported console text).
Both complete selected payload decodes now PASS. No native/QA changes.

### BuffList GFx decoded in isolated sandbox — 2026-09-13

Existing GFxDec_v4.exe SHA256
07CF357CDF14D8B8598D7F99403FCBABC1F0B1DC5C49BF60879C78E67AFDEFDD
was copied with three original asset copies into
artifacts/grow_lancer/buff_gfx_decode_20260913. Command `GFxDec_v4.exe .`
from that sandbox produced Decrypted/*.gfx/*.dds. A prior /? invocation
reported Input error and produced no successful decode; it was not the conversion.
All writes isolated; original asset files untouched.

SHA256 provenance (source -> decoded):
- BuffList.ozg 9D31A17AFA5513CE06F9F7AA35FAE0622D4E522542146FD4B0D441EF526FE547
  -> BuffList.gfx 934F8FA696D2B9BEADA80CF1D8F7BDAD337C42D8FA5DA4A1364BEFE0A8595FE1
- BuffList_I12.ozd E81C28BF7DCADC6F485541964CE665996EFD8990E8606EFC4781A58838754C6C
  -> DDS 3C352CCA0A3CB1AE68030E6FBE2EFC27B4D327616631F02A65495509103E8C16
- BuffList_I14.ozd A1702A117EA7F39C1164B7810EAAB500A3F9E88EE09419CAF702D38847876305
  -> DDS 92ED543635A7644AADEC8631BB1A18E44DC4575390ADB6FA03E242253EC10FD7

CFX v10 expands to exactly193713 bytes including header. Bounded tag parser
checks zlib EOF/length, nested tag extents and End tags. Relevant DoInitAction
payloads: sprite76 atE66D size2056 includes DrawIcon_Buff/BuffType;
sprite117 at2C797 size3090 includes DrawIcon_Buff/icon_. These are concrete
AVM1 decode entrypoints, not yet proof of buff-to-icon mapping. DDS dimensions
from headers: I12=256x64 and I14=128x32; no claim these contain Wrath icons.
Tool: tools/grow_lancer/inspect_wrath_buff_gfx.py. No native integration this step.

### Display name recovered; native icon path incompatible — 2026-09-13

Offline bounded UTF8 name reads confirm424="Thịnh Nộ" and425="Tăng Cường
Thịnh Nộ". Record+10 holds the string; existing pinned8E7C00 selects heap
storage when capacity+14>=16, otherwise inline. Inspector checks capacity,
length, termination and expected text and cross-snapshot accessor equality.
Description container at+34 still requires a verified iteration contract.

Native NewUIBuffWindow::RenderBuffIcon uses buff ID directly, not metadata
effectType123. With width20,height28, IDs424/425 enter third atlas, column3/4,
row26, v=728/256=2.84375. Therefore native legacy atlas math is NOT a usable
Wrath icon mapping. Do not reinterpret effectType123 as atlas index without
S21 consumer evidence or reuse the cast-skill icon as a guessed buff icon.

Read-only candidate assets found: Data/Interface/GFx/BuffList.ozg (54964 bytes),
BuffList_I12.ozd (16546), BuffList_I14.ozd (4258). These are leads only;
actual symbol/shape/image-to-buff mapping has not yet been extracted. Original
files unchanged, no staging or runtime readiness enabled.

### Classifier meaning and native exact-ID recognition — 2026-09-13

Complete C888AB..C88999 consumer decode calls113B5F8, then tests result==1
for the literal isDebuff and result==2 for isCashBuff. Offline row1 (Attack)
has classifier0; row56 (Freeze) has1; Wrath424/425 both have0. The inspector
now checks these comparison/label pins and equality across both snapshots.
This supports mapping Wrath classification to native eBuffClass_Buff (0),
not the native invalid sentinel eBuffClass_Count (2). S21 cash class2 must
NOT be generically cast to that native enum.

Native IsBuffClass now recognizes only exact signed IDs424/425 through the
Wind Soul-independent GrowLancerWrathBuffMetadata helper. Other IDs retain
the original lookup/count behavior. No enum count, disk layout, buff slots or
legacy packet widths changed. GetBuffinfo/icon/description registration remains
incomplete; this isolated classification hook alone is NOT feature readiness.
Tests exhaust -1..65535 plus wide signed alias cases; focused x86 compile and
isolated Global Release Win32 build PASS. No deployment or runtime acceptance.

### Full-memory snapshot closes missing classifier rows — 2026-09-13

The earlier missing-heap result applies to process_23824 module dumps only.
The isolated full dump process21072, SHA256
374d1ecb06719389416938e0be27a32596e91e0a5b5d5a7c6f8df690c326efed,
DOES contain BuffEffect map at70A22F8+38: head1058E540, count439.
Offline lookup finds424 at record1636373C and425 at1636332C.
Both records start [ID,123,255,255]; classifier field+28 is0, column6 is0,
column7 is1. These are actual client rows, not borrowed server records.
Inspector verifies hash and cross-snapshot code equality for lookup/classifier,
loader and iterator paths before following the tree. Iterator113E27B returns
node+10; lookup adds4, hence inline record=node+14 (hex offsets).
Tool: tools/grow_lancer/inspect_wrath_buff_snapshot.py.

This supersedes the metadata-values OPEN statements below for these two rows.
S21 enum semantics/notice/clear meaning must still be checked at consumers before
mapping into native BuffInfo; native eBuff_Count is210 and rejects424/425 today.
Native BuffInfo and disk _BUFFINFO already use short IDs: no file-layout widening
is needed merely to represent424/425. This does not enable status or visual QA.

### Metadata loader located — 2026-09-13

Registration follow-through: loader113B27E is virtual slot+8 of vtable1B92A3C.
Constructor11CFB8D installs that vtable at11CFBC2, initializes map+38 and
fallback+40, and sets minimum column count+88=10 at11CFBE5. Script-group
setup11C1664 contains11C1792 BuffEffect UTF16 name, owner+850 and call
11331CA at11C17A3. This is the named provider connection, not a guessed path.
Provider11331CA reaches jump11331FE -> C014583 -> E12FF78. Bytes after the
first jump are not its executed linear continuation; do not treat their
spurious ret instructions as function boundaries or recovered parser code.
Protected provider semantics and actual424/425 records remain OPEN. Existing
module dump cannot supply missing map heap just because constructor is known.

Actual classifier map at70A22F8+38 in the pinned process snapshot contains
head10432178 and count439. No enumerated process_23824 binary file range
covers that heap address. Do not read unrelated bytes or assume the missing
node contents: concrete metadata values424/425 remain unverified.

Alternative read-only path found: row loader113B27E. It parses columns0..3
as integers into a local record at-5C, column4 as string into-4C, column5
as integer into-34 (record+28), columns6/7 into-30/-2C, column8 description,
and column9 into-1C. It inserts through113D48F onthis+38 at113B486.
Error branch113B4B7 references UTF16 BuffEffect.txt File Loading Error!
at1B8B680. This establishes where classifier field+28 originates (column5),
not the value for424/425 or the external script/package location.

Native w_BuffScriptLoader uses Data/RISE/Config/BuffEffect.bmd and its own
BuffInfo schema. Do not copy S21 struct offsets into it. Next locate the S21
script provider feeding113B27E; direct filename enumeration found no standalone
BuffEffect file in the original client tree, so packed resources remain a lead.

### Logical activation detour resolved — 2026-09-13

Do not linearly decode through1289327: patched instruction1289322 jumps to
IGC.dll10886690. Same-process dump10860000.IGC.dll SHA256
C6002FA6B6FF2BC7D41031ECF838AA00697AC24E7806EAC45188AB528A25D3C8
contains this trampoline and mapped sections (RVA equals raw offset here).
Its108866D0 callback subtracts0x71 and accepts unsigned result<=0x5B;
424/425 fail the range and returnAL0 at10886727. Trampoline then restores
the displaced assignment and returns through pointer109AF178=128932B.
Main switch subtracts0x1D; byte table1289C23 maps both424/425 to27; target
table1289BB3 maps27 to epilogue1289B9C. Thus this **logical activation
callback has no extra424/425 body**, including the actual installed detour.
Not a claim that Wrath has no other effects/server behavior. No hook copied.
Verifier: tools/grow_lancer/verify_wrath_logical_detour.py pins both dumps.

Classifier113B5F8 returns field+28 from lookup113B50E. Lookup uses map at
this+38 and fallback record this+40 for missing key or ID>=461; next identify
the record loader/content before naming that field's enum or allowing native
classification. Avoid assuming metadata from the server BuffEffect table is
the same client record layout.

### Local-owner status input lead — 2026-09-13

Activation continuation verified:12FD717..12FD881 calls1289DD1 physical,
1284D00 exception(false),130C63E membership insert and12892D3 logical.
Physical1289DD1..128D07E routes424/425 through128A2B4/128A2C4 to128C0B5:
if9DFB65(world) is false, assigns1.1f at128C0D3, then in either case calls
14B65D8(incoming buffID, incoming OBJECT,-1). No local root creation occurs
in that selected branch. Do not add a duplicate Wrath cast controller here.

World predicate9DFB65..9DFB87 tests18..23 inclusive,53 or97. Native
MapManager::InChaosCastle covers18..23 and53 but does not list97. This
explains the source guard structurally; no blind numeric world transplant or
claim of full equivalence. Removal physical scale0.9 has no matching local
world guard. The asymmetric add/remove behavior must be retained where mapped.

Anchored1293B12 reads unsigned WORD at input+10 and accepts1..460
(reject zero or>=1CD). Byte+8 zero selects addition: forwards ID, local
hero OBJECT, DWORD input+C and WORD input+22 to12FD717 at1293BAC.
Nonzero selects removal1305838 at1293C52 with the same WORD ID and local
hero OBJECT. This path can represent424/425 without truncation. Opcode,
packet length validation, units of the additional fields and remote viewport
path remain OPEN; do not transplant these offsets into native5.2 packets.
Activation helper12FD717 begins with the same113B5F8 classification lookup
used by removal, rejects-1, and has a world/remote-owner gate. Continue from
12FD7B2 before claiming complete physical/map/logical activation semantics.

### Repeat receipt refresh versus membership — 2026-09-13

ManagerA0D270 compares find/end using inequalityA0E297. Existing key takes
A0EEB6 before sampling a fresh clock and inserting new record. The removal
helper obtains a key range throughA0DC06 and passes it toA0E47D/A0E4F4;
the selected range loop calls node removalA0E56A. Pair constructorA0D8F9
copies the key and four DWORD record fields. This refreshes countdown input,
unlike character membership130C63E which skips an existing key entirely.

Added caller-owned BuffCountdown::Receive/Remaining and tests proving repeated
receipt replaces start/duration, does not add duration, and accepts zero.
No additional membership map or packet layout introduced. Source verifier and
focused x86 test PASS. Direct rel32 scan forA0D5EA found no candidates; this
does not prove absence of indirect/virtual/inlined UI usage. UI consumer remains
unverified, and must not delay the already-established countdown arithmetic.

### Manager countdown unit recovered — 2026-09-13

CompleteA0D270..A0D3E6 samples IAT1B4C9A8=winmm!timeGetTime, divides
unsigned DWORD by1000, then builds record(startSeconds,buffID,arg2,arg3).
GetterA0D5EA..A0D66A reads this record: elapsed=nowMilliseconds/1000-start,
unsigned subtraction; if duration<=elapsed returns0, else duration-elapsed.
Duration is record+8, from manager arg2, hence receive DWORD+0Ch. The getter
does not erase the record, clear membership or remove visual effects.
Final arg3/WORD+22h is not the duration used in this countdown.

Added native GrowLancerBuffCountdown.h arithmetic helper and constexpr tests
covering quantized second boundaries, zero/exact/over expiry, unsigned backward
clock and source DWORD wrap behavior. Helper is not wired to activation yet.
Do not silently replace division-before-subtraction with millisecond-delta math;
do not use countdown0 as authoritative visual removal. Focused Win32 compile
PASS after explicitly supplying MSVC/UCRT include paths (initial bare cl lacked
cstdint include path). Pinned source timer/import verifier PASS. Source server
formula generating duration and actual client lifecycle wiring remain OPEN.

### Status opcode and pre-membership manager — 2026-09-13

Anchored ProtocolCore12FE380 bounds opcode toFD and jumps through130505C.
Entry2D ->130231B forwards the buffer directly to1293B12 at130231E.
Complete1293B12..1293D37 reads WORD buff+10h, accepts1..460; byte+8 zero
adds, nonzero removes. Addition forwards DWORD+0Ch and WORD+22h to manager
A0D270(this1E04FF4) BEFORE12FD717. Removal callsA0D3E7 on that same manager
BEFORE1305838. This is a concrete additional lifecycle path, not covered by
the logical activation callback's no-op proof. Continue these manager methods.

Minimum observed buffer extent is36 bytes; this is not an asserted wire size.
Upstream header/framing and length checks remain unverified. No native2D
layout is changed merely because both versions use the same opcode. Pinned
by verify_wrath_status_dispatch.py. Original S21 and production remain read-only.

### Logical removal reaches shared timer cleanup — 2026-09-13

Revalidated installed logical activation detour:424/425 select the epilogue,
not a timer-registration body. New complete128558E..12859EB removal decode
shows local-owner424/425 take default12859CF, then1311235. Non-local/null
owners exit before this tail.1311235 derives key via1311D60, looks up a timer
record and, only if found, calls IAT1B4C7D4=user32!KillTimer before erasing it.
1311D60 returns0 for invalid classifier; otherwise metadata record field+4
plus1005. These selected paths are pinned in verify_wrath_logical_detour.py.

Presence of generic cleanup does not prove a Wrath timer was ever registered.
Do not invent duration or claim timer expiry is responsible for membership
removal. Wrath metadata and actual network removal remain separate gates.

### Generic comparison branch relevance narrowed — 2026-09-13

CompleteA0ABA4..A0ACB7 initializer clears four lists, populates only:
this+0={1,400,346}, this+8={2,401,347}, this+10={142,402,348,427};
default this+18 remains empty in this initializer. CompleteA0B614..A0B6A1
dispatches exactly those IDs to the three lists and everything else to+18.
Neither424 nor425 occurs in these initialized groups. A0ACB8 also starts with
the same explicit-ID dispatch; no Wrath formula was established there.
The list-clear helper resets sentinel links and size to0. Selected initializer,
selector and clear pins added to verify_wrath_membership.py.

Consequently do not hold Wrath visual activation pending reverse of this
generic comparison/request subsystem: it is not a demonstrated Wrath consumer.
This is scoped to observed initialization/selection, not a whole-program claim
that no later writer can alter lists. Return to actual Wrath metadata/packet
delivery and logical timer registration, retaining earlier payload evidence.

### Payload accessors and request-state consumer — 2026-09-13

Full accessor decodes:130CAF9 reads payload+4 (missing=0),130CB64 writes+4
only if present;130CC12 reads+8 (missing=-1),130CBBB writes+8 only if present.
Reply-shaped handler1293A5C..1293B11 derives actor key from input bytes4/5,
looks up character, tests WORD buff ID at+6 for existing membership, then
writes DWORD input+8 into payload+4 and sets payload+8=1. Missing actor/buff
exits. It neither inserts a buff nor starts its visual chain.

Anchored consumerA0B4F4..A0B613 first selects an active buff from a generated
list. When payload+8=-1 it callsA0BF35 with actor key and buff, sets+8=0,
returns0. State0 returns0 without another request. Nonzero state reads+4 and
compares it against A0ACB8 output. Thus+8 has an observed request/wait/received
role here, not a countdown. A0BF35 begins a C1/C7 packet with subcommand7;
complete serialization/receive dispatcher and comparison units remain OPEN.
These generic consumers are not yet proven to select424/425; do not assume
they are Wrath duration logic. Selected pins extend verify_wrath_membership.py.

### Membership storage and repeated insertion — 2026-09-13

Full130C63E..130C6CC insertion compares lookup with end using equality130E115.
Existing entry returns immediately: no counter increment, payload replacement
or refresh at this layer. Absent entry allocates12 bytes with fields1,
incoming argument2 and-1, then inserts the pointer. Removal130C769..130C7EC
checks nonempty and lookup!=end, deletes12-byte payload and erases entry;
missing entry exits without release. Equality/inequality helpers decoded as
well. `verify_wrath_membership.py` pins these selected branches.

Activation12FD7DE forwards its fourth argument to membership payload+4;
12FD864 separately forwards its third argument to logical effects. These are
distinct values, not a single proven duration. Do not label payload+4 as time
or reset it on repeated insertion without tracing consumers.

Native `w_Buff.cpp::Buff::RegisterBuff` already has insert-if-absent behavior,
and `UnRegisterBuff` erases only a found entry. Thus existing native membership
can represent the presence semantics; no duplicate per-owner aura membership
container is justified. Native DWORD map value is not the source12-byte
payload, whose additional fields still need consumer tracing. Native buff
timer control is a separate layer and is not proof of S21 duration semantics.

### Exception gate and classifier boundary — 2026-09-13

Complete contiguous decode1284D00..1285279 closes the selected Wrath
CheckExceptionBuff path. Removal tests only IDs11/12 before default5094
returns AL=1. Addition compares IDs11..17, then default526B returns AL=1.
Both424/425 therefore permit membership insertion/removal here, without
executing the special-case calls. This does not bypass upstream classification.
Pinned by `verify_wrath_buff_exception.py`, including every comparison and
conditional branch in the selected dispatch regions.

Classifier113B5F8 returns record+28. Record constructor113DF42 initializes
this field to-1; activation12FD750 rejects-1. Native BuffScriptLoader returns
eBuffClass_Count (2) for missing/out-of-range metadata instead. These are not
interchangeable numeric enums. Matching the default exception behavior alone
does not authorize unknown Wrath metadata in the native classifier.

Read-only recheck of server BuffEffectManager.xml also finds older Wrath218
and223..225 alongside424/425. Older descriptions contain numerical bonuses;
424/425 descriptions do not. Do not borrow older bonuses or alias those IDs
to fill missing current Wrath duration/stat rules.

### Persistent orchestration implementation — 2026-09-13

Native `SubmitWrathPersistentVisuals` now sequences existing lightmark, terrain,
scatter, flare and mono helpers. `GrowLancerWrathEmission.h` preserves allocator
interleaving: first X/Y, shared scale, first texture, allocation; second X/Y,
second texture, allocation; flares; one mono branch draw; four allocations.
The focused constexpr test simulates RNG consumption inside allocation and
covers all three branch residues. It does not prove the source quality wrapper
or live RNG parity. Native isolated x86 build passes; function remains uncalled
until authoritative buff membership/lifecycle and render invocation are mapped.
Return true represents prerequisite acceptance, not pool allocation success.

### Character enlargement verified — 2026-09-13

Removal follow-through: exact1305838..13058C3 calls physical cleanup1285A3A,
then exception check1284D00, membership removal130C769 and logical128558E.
Physical dispatcher1285A3A..1286487 routes424/425 at1285D99/1285DA6 to
12861EE. It assigns **0.9f** (float1B4EDA4) to incoming OBJECT+A0 before
calling14B65D8 with(type=incoming buff ID,owner=incoming object,subtype=-1).
Thus this source removal branch does NOT restore a saved pre-buff scale or
multiply by1/1.1. The general scale function may recompute class scale later;
order of those later callers remains open. Do not remap effect type424/425
directly into native SS6 effect IDs without verifying deletion semantics.

Native WSclient.cpp::UnRegisterBuff has the same high-level physical ->
exception -> map removal -> logical order, but g_IsBuffClass rejects unknown
states first. Existing BYTE packets also cannot carry424/425. This is a proven
native integration boundary, not authorization to bypass classification or
fabricate an active buff during F7 preview.

Owner's enlargement observation has a matching binary branch. Function
`0x141043E..0x14108C1`, decoded contiguously from its prologue through ret,
tests membership424 at1410879/1410887, then425 at1410893/14108A1.
Either true reaches14108AD: load float1.1 from1B58D70 and **assign** it to
character+32F4, i.e. embedded OBJECT+3254 plus scale+A0. This is an absolute
scale override, not multiplication by1.1 and not accumulation per cast.

Do not apply unconditionally: entry byte+E nonzero returns; the world-helper
branch via9DFB65 and manager-result5 branch also return before the buff tail.
Other branches establish class-dependent base scale before the override.
Native ZzzCharacter.cpp::SetCharacterScale has a comparable Change guard and
class-scale selection, but lacks Wrath membership override. Native selection
scene policy differs; S21 early exits must not be mapped to guessed enums.
Buff ingestion424/425, when scale is refreshed on activation/removal, and
restoring native baseline remain required before claiming lifecycle parity.

Verifier: tools/grow_lancer/verify_wrath_character_scale.py. This pins the
selected branch and scalar, not full caller CFG or rendered-size acceptance.
Direct-call search reports13 candidates including1410412 and1410C08; callers
need anchored decoding. Additional buff-driven visual branch13EB9DA checks
424/425 and emits resource8078 through172760A; its complete child chain is
newly identified and still OPEN, not covered by the cast controller alone.

### Supplied server formula evidence (not yet application semantics)

`D:\GameServer S21\Data\FormulaData.xml`, SHA-256
`9A513DD4375116CFD0FA599114B2103B4D21B194B775AE376E0C6289BA36E6FF`:
Character/44 (combat power increase) is `(%d+%d)*0+10`, parameters STR/AGI;
Character/46 (attack increase) is `(%d+%d)/3`, STR/AGI;
Character/47 (defense decrease) is `%d*(((%d+%d)*0+0.05)*-1)`, DEF/STR/AGI.
These simplify mathematically to 10, (STR+AGI)/3 and -0.05*DEF, respectively.
Application units for combat power, order of defense modifiers, argument
selection and integer rounding still require call-site evidence. Do not label
10 as ten percent without the consumer or truncate each stat before division.
Formula IDs repeat in other XML sections with different expressions; the
Character section is essential, not an optional qualifier.

The supplied Skills/SkillList.xml hash matches the accepted Desktop/Skills
profile (`3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0`).
Base278/buff424 and master895/buff425 both have Duration=0. This does not prove
zero-second expiry or infinite/toggle lifetime: the server consumer is not
recovered. Visual life70/controller and repeating aura life30 cannot supply
that missing gameplay duration. No default 60-second timer is introduced.
Verifier: tools/grow_lancer/verify_wrath_server_formula.py.

- Base skill 278 is Grow Lancer `Wrath`: level 66, mana 40, AG 30, strength
  200, dexterity 200 and buff index 424.
- Local handler `0x10E470E` sends the packet, selects action 192 with argument
  321 and plays sound 1118.
- Received handler `0x12CB6AC` mirrors action 192/321 for a remote caster,
  creates invisible controller `0x5EA subtype 0` owned by the caster, and marks
  the character active.
- Sound 1118 registration: `0x18E2F08` -> `Data\Sound\Wrath.wav`.

## Controller 0x5EA

- Controller Timer0/private whole ticks added after verifying common tail
  1579222->15945F1: life70..0 allows71 update visits unless owner safety
  guard terminates. Removed superseded unreachable fractional particle bodies.
  Child owners verified at1578D1C/1578ED3:5EC and5EB use controller.Owner
  (caster), unlike MagicPin children; existing mapping remains unchanged.
  Ground81E3 call15791D5..1579222 uses null owner and controllerLight at
  15791F3, now passed explicitly instead of a temporary white vector. Its
  constructor may override light, so this call-argument fix alone is not
  evidence of visible change. No action-change guard was invented: source
  reads owner frame immediately at15789DD. Native null/Live safety remains.
  Isolated ClCompile/project3cpp22headers PASS; not linked/staged yet.
- Constructor `0x1480E08`: lifetime 70, internal stage counter 0, scale/raw A0
  1.0.
- Renderer `0xA4941B`: exact no-draw stub.
- Update `0x15789DD` reads the owner animation frame and advances four one-shot
  stage windows:
  - frame `[3.0,3.5]`: local offset `(0,-70,250)`, particle `0x81E4`
    (`!Combo3.jpg` dispatcher, subtype 0, null owner).
  - frame `[3.5,4.2]`: local offset `(0,-80,30)`, effect `0x5EC subtype 0`.
  - frame `[5.0,5.5]`: local offset `(0,-30,-40)`, effect `0x5EB subtype 0`.
  - frame `[5.6,6.5]`: local offset `(0,-80,220)`, particle `0x81E6`
    (`alpha_light.tga`), followed by effect `0x81E3` (`brocken.jpg`) at local
    offset `(0,-80,5)`.
  Every local offset is angle-transformed and added to the controller position.

## Bitmap children 0x81E4, 0x81E6 and 0x81E3

- Particle cadence correction:81E4 subtype0 and81E6 subtype0/1 now use
  private whole-particle ticks before generic SS6 lifetime pre-decrement.
  1709744/17097C4 reach172343C, so lifetimes10/20 produce10/20 updates
  and no life0 update. Alpha-light starts.5, crosses negative on update17,
  clamps at18; clamp is on entry, not immediately after subtraction.
  Combo scale crossing likewise clamps on next entry and leaves RGB unchanged
  in the clamp branch. Per-slot remainder resets on ALL particle allocations.
  Shared GrowLancerWrathParticle.h and12-case x86W4WX regression PASS,
  with explicit crossing test. Subtypes outside audited selection unchanged.
  ClCompile PASS; not linked/staged. Real pool/GPU sampling remains open.
- `0x81E4` constructor `0x168ED1D`, subtype 0: lifetime 10, scale 2.0,
  alpha 1.0 and light `(1,1,0.62)`. Update dispatch `0x17092F8`, subtype-0
  block `0x17095FD` adds `sin(30 radians)*0.18` to scale and
  multiplies each light channel by 0.85 per tick. Particle dispatch
  `0x1621347..0x16213A7` falls through to `0x1640DD2`, which renders its
  own texture/light/rotation through the sprite helper. The earlier Combo4
  claim incorrectly followed the separate OBJECT renderer; it is withdrawn.
  Helper `0x96A03C` passes 30 unchanged to CRT sin, whose fallback at
  `0x1A36D8F` executes FSIN: radians, not degrees.
- `0x81E6` constructor `0x168EFDD`, subtype 0: lifetime 20, scale 3.0,
  alpha 0.5 and light `(1,3,10)`. Update `0x1709749` subtracts alpha 0.03 per
  tick until zero. Renderer `0x162D2E2` submits a native sprite with each
  light channel multiplied by alpha.
- `0x81E3` is an effect-pool object, not a particle or generic sprite carrier.
  Constructor `0x1480FD7`: lifetime/max lifetime 30, scale 4.0, alpha 1.0,
  white light. Update `0x1579AB4`: below remaining lifetime 11, alpha loses
  0.11 per tick; every tick mutates each stored light channel by multiplying
  it by alpha. Renderer `0x15A0B63` uses the terrain-alpha bitmap path with
  position, scale, stored light, rotation 0, alpha argument 1 and height 5.
- The base controller calls all three with null owner. The native SS6 adapters
  retain their real particle/effect pools and their recovered destruction via
  lifetime; none of these nodes now use the Grow Lancer sprite carrier.

## Child 0x5EB — wratha02

- Model registration `0x18D2D20` -> `Data\Effect\wratha02.bmd`.
- Constructor `0x1480E3A`: lifetime 20, scale/raw A0 0.6, alpha 0.6.
- Update `0x1579227`: alpha `-0.065` per tick; local movement `(0,0,-65)` is
  angle-transformed and added to world position.
  No lower alpha clamp occurs before the jump at `0x1579399` to the shared
  tail; expiry is checked before normal lifetime decrement, including life 0.
- Renderer `0xA49422`: RGB is `(0.37,0.57,1) * alpha`, independent of
  incoming object light. Float VAs `0x1B502A8`/`0x1B502CC` contain
  `A470BD3E`/`85EB113F`. Native BMD mesh 0, flags `0x42`, blend-mesh index
  0 (push at `0xA49527`), blend light alpha, literal UV `(0,0)`, texture -1.
- Decoded model identity: `wratha02.SMD`, 1 mesh, 2 bones, 1 action. Its sole
  embedded material string is `dust01.tga` (original/staged payload verified);
  `alpha_line.jpg` belongs to the separate `wratha01.bmd` model.

## Child 0x5EC — wratha01 and persistent continuation

- Shared aura scalar helper now used by runtime and x86 regression:24 total
  Wrath cases PASS. Both aura subtypes execute31 updates (11 rising/20 falling),
  one life1 continuation eligibility. Final alpha is-.097/-.74 without clamp;
  U=.496/.52,V=.043/-.17,subtype1 scale=.7051;subtype0 scale=.95-.15*sin30.
  Test does NOT query real buff maps or execute CreateEffect/pool/rendering.
  Isolated Win32 link PASS, SHA323C7F8FC757D7D99F4BC8BCC7D6D8CDEA3E549DDF5E5CDEA3DDDD818F659B9F.
  Not staged yet; previous7E619364 remains isolated client executable.
- Aura0/1 now initializes Timer0 and uses private whole-effect ticks:
  1579645/15798E2->1579AAF->15945F1 includes31 visits30..0.
  Continuation gate is exact life==1 at157959D and1579844, replacing
  the fractional adapter's <=1 gate. Native buff424/425 conditional remains,
  no fabricated permanent buff. Actual server-owned buff activation still open.
  Stage predicates shared between runtime and regression:16 controller/particle
  cases plus boundaries PASS, including frame3.5 taking stage0 then1 in one
  visit and71 controller visits. ClCompile PASS; aura scalar endpoint tests
  and actual buff recurrence not covered by those16 cases yet.
- Model registration `0x18D2CEB` -> `Data\Effect\wratha01.bmd`.
- Constructor `0x1480E78`:
  - subtype 0: lifetime/max 30, scale/raw A0 0.95, alpha 0, raw B0 -0.7.
  - subtype 1: lifetime/max 30, scale/raw A0 0.7, alpha 0, raw AC 0.25,
    raw B0 -0.7.
  - subtype 2 exists for upgraded skill paths and is not used by base 278.
- Update `0x157939E` rotates angle Z by 1.5 per tick. The exact split is
  `maxLife/2 + 4`, therefore remaining life 19 for a 30-tick object. Subtype 0
  uses alpha `+0.073/-0.045`, scale deltas
  `-sin(30 radians)*0.012/-sin(30 radians)*0.0009`, U `+0.016/+0.016` and V
  `+0.053/+0.008`. Subtype 1 uses alpha `+0.06/-0.07`, scale
  `+0.0001/+0.0002`, U `+0.01/+0.008` and V `+0.03/+0.01`. These constants
  were read directly from their referenced float VAs in the pinned dump.
  When subtype 0 reaches remaining life 1 it creates
  subtype 1 at the same position. When subtype 1 reaches remaining life 1, it
  queries the owner buff list for effect 424 or 425 and recreates subtype 1
  only while either buff remains active. This is the authoritative persistent
  visual ownership/cleanup contract; it must not be replaced by a fixed timer.
- Renderer `0xA49554`:
  - subtype 0 draws native mesh 0, flags `0x42`, using white * alpha.
  - subtype 1 replaces model light with the effect's per-channel light times alpha,
    then draws mesh 0 with flags `0x42`.
  - both base subtypes submit blend-mesh index 0 (pushes at `0xA49676`
    and `0xA4978B`), not SS6's provisional -2 default.
  - subtype 2 copies the effect light and draws mesh 0 in native mode 2.
- The subtype-0 to subtype-1 continuation passes light `(0.65,0.75,1.0)`;
  the values are the three immediate float loads before the `CreateEffect`
  call at `0x157963D`.
- Decoded model identity: `wratha01.SMD`, 1 mesh, 1 bone, 1 action; embedded
  texture `alpha_line.jpg`.

## Assets and open acceptance work

### Persistent buff visual follow-through — 2026-09-13

Tail follow-through13EC3F8..13ECB52: two scattered particles at13EC59D /
13EC6B2 use8081+rand()%3, subtype0, shared scale(rand()%3+3)*.1,
RGB(.6,.75,1), null owner. Independent XY offsets rand()%220-110 are
added to incoming OBJECT position, Z unchanged; angle uses OBJECT+164.
Bone helper calls13EC6D1/13EC6F0 select29/38 into locals-58/-19C.
Two7EF7 sprites13EC76E/13EC7B6 use these positions, size4,
RGB(.22,.2,1), rotation0, character OBJECT owner.

The following rand()%3 selects ONE four-particle branch, not all12 sites:

| Remainder | First pair (bone29, bone38) | Second pair (bone29, bone38) |
| --- | --- | --- |
| 0 | 8086 subtype13, 8086 subtype14 | 8089 subtype13, 8089 subtype14 |
| 1 | 8087 subtype14, 8087 subtype15 | 8089 subtype13, 8089 subtype14 |
| 2 | 8086 subtype13, 8086 subtype14 | 8089 subtype13, 8089 subtype14 |

All selected particles use scale.75, RGB(.4,.6,1), character OBJECT owner,
incoming OBJECT angle and trailing0. Branches converge13ECB52 (next status).
Verifier verify_wrath_persistent_tail.py pins continuous decode from13DB9D7,
random branch destinations, all12 texture/subtype/position/owner call setups.
It does NOT verify constructor/update/render/destruction or outer cadence.
Registry leads:8081..8083 lighting_mega01..03.jpg;7EF7 flare01.jpg;
8086 firehik_mono01.jpg;8087 firehik_mono02.jpg;8089 firehik_mono03.jpg.
Their asset hashes/format/load still need verification before staging.
Total local submissions per reached visit:13sprites,4terrain,6particles;
this is not runtime cadence, particle population, or visual parity proof.

Particle wrapper1724176..1724226 has an additional emission filter:
if DE7FE7(global1DD85DC) is true, sample rand()%max(globalA484644,1).
Lookup10EFDFC(texture,subtype), this1FB0CA0, forces remainder0 on nonzero
return; otherwise nonzero remainder skips allocation. False outer predicate
allocates unconditionally. Both paths forward all8 arguments to16412E5,
pool thisA490B9C. Thus6 submissions do NOT guarantee6 allocated particles.
Predicate/global semantics and per-resource lookup membership remain OPEN.
Existing GrowLancerFireParticle.h covers Breche8084/0,806E/4,8085/0 only;
it is not evidence for Wrath8086/8087/8089 subtype13..15 behavior.

Constructor dispatch16419CF subtracts8012 then uses16A9789/16A96A1:
8081/8082/8083 ->16663CE;8086 ->16673C6;8087 ->164B3FF;
8089 ->166C453. For8086, subtype table16A9C9D maps BOTH13/14 to16685F0.
Full selected block16685F0..16687F4 assigns independent random values:
counter+48=R%5+27; scale+18=(R%72+52)*.01*suppliedScale;
rotation+60=R%360; parameter+6C=(R%14+20)*.1.
It zeroes+44, copies incoming RGB to+7C, zeroes live RGB+38 AND vector+88,
then jumps166A5C6. Therefore the Breche constructor is not interchangeable:
scale offset52 rather than72, parameter20..33 rather than64..87, and
additional zeroed vector. Meaning of+88 in selected update still OPEN.
The focused tail verifier now pins texture tables and selected8086 constructor
parameters. 8087/8089 selected subtype branches and all update/render paths
still need follow-through; no native constructor was changed on partial evidence.

Follow-through:8087 subtype14/15 compares164CADF/164CAF0 converge164CAFA.
Selected block to164CC7B sets counter=R%12+8, vector+70=0,
scale=(R%30+20)*.01 (REPLACES supplied scale, no multiplier), rotation=R%360,
parameter+6C=(R%15+15)*.01, vector+88=0; tail164D1E4->16A9524.
8089 subtype13/14 compares166D4DF/166D4F0 converge166D4FA:
counter=15, rotation=R%360, alpha+44=1, copies RGB to+7C;
keeps generic incoming scale/light/position copy. Tail166F197->16A9524.

Correction to earlier next-action text:1620984 is RENDER, not update.
Actual update16AA2ED table17236F5/1723611 maps8086->16DA1D5,
8087->16B8BDD,8089->16DF6D7.8086 subtype table1723C01 maps13/14->16DC17E.
Full selected block16DC17E..16DC6D0 requires nonnull owner and96139E(owner)
(getter reads byte0); failure clears Live/counter then exits to16DE8BD.
With valid owner: if counter<15 alpha-=.2, else if alpha<1 add(R%2+2)*.1,
else alpha=1. Alpha<.1 clears Live but processing continues. Light=savedRGB*alpha.
If scale>0 subtract(R%3+2)*.01 else clear Live (pre-subtraction test).
Offset vector+88.z += parameter+6C. Lookup owner's model via968B22(owner+5C)
then EF2A81; recalculate bone29 for subtype13 / bone38 for subtype14 via132EC63.
Position = current bone world position + accumulated vector+88; rotation+=3.
Thus this particle follows moving bones and drifts upward, not free-world motion.
Model lookup semantics, remaining8087/8089 updates, renderer and shared decrement/
release remain OPEN; this is not a full child lifecycle or native QA claim.

Update follow-through8087 subtype14/15:16BB079..16BB2E7.
Scale -= (R%20+10)*.001. Stack scalar-6864 -= .05 (NOT a write to
particle Alpha+44; shared-tail use still to trace). Re-read owner model and
bone29 for14 / bone38 for15. Offset+88.z += parameter+6C*10;
Position=bone world position+offset. Exits16BBB1A->172343C.
There is no local owner null/live guard or alpha/scale death branch in this
selected block; do not assume the safety gate seen in8086 exists here.

8089 subtype13/14 update16E18F7..16E1B91: recalculate bone29/38 from owner,
set Position directly to bone (no accumulated drift), Scale-=.05,
Alpha-=float32(0.0666666030883789), Light=savedRGB*Alpha.
No local random call, rotation change, clamp or death test. Exits16E37C5.
No local owner null/live guard. Outer/common owner validation and cleanup
must be audited before safely mapping this to SS6. Selected branch verifiers
PASS; no runtime implementation or full lifecycle claim at this checkpoint.

Common-tail audit:16AA6DB..16AA6DE resets scalar-6864 to0 for each
eligible particle. Selected8087 changes it to-.05, then jumps16BBB1A->172343C.
That tail never reads-6864 or writes Alpha: it decrements counter+48,
adds global1DD801C to+AC, advances iterator, releases temporary handle,
then loops16AA608. Therefore the selected stack subtraction has no persistent
alpha effect.8089 exits16E37C5->172343C as well.
Predispatch16AA657..16AA66E removes counter<=0 or !Live before update;
Live cleared inside a body is removed on a later visit, not immediately.
Owner-presence block16AA743 copies owner position/terrain height but does not
validate owner Live. Exact erase/helper ownership internals still OPEN.

Native scalar/bone-position adapter GrowLancerWrathPersistentTick.h now implements
the three selected update bodies without importing layouts or addresses.
Caller supplies current bone world position and isolated accumulated offset.
Lifetime decrement stays outside these functions. Mono02/03 caller ownership
guard remains a required integration task; adapter is NOT connected to engine yet.
GrowLancerWrathPersistentTickTest.cpp compile-time fixtures /std:c++14 /W4 /WX
PASS: alpha overshoot, pre-subtraction death, moving-bone drift, dead-owner exit,
no invented Mono02 alpha decrement, Mono03 exact fade/no premature clamp.
This is focused scalar behavior evidence, not engine build or visual acceptance.

Renderer follow-through:8086 routes162112C->1635D2A; subtype13/14 table
16410E8 both select1635D58, draw1635DF4.8087 routes162113C->1627A2A;
14/15 skip subtype21 special case to1627B36, draw1627BD2.
8089 is NOT8085's renderer branch:16210DC->1624C92, selected13/14 skip
21/24/26/27/28 special cases to1624DF8, draw1624E94.
All three selected draws call18E8B0C with original texture+10, Position+20,
bitmap dimensions*Scale, storedRGB+38, Rotation+60, UV(0,0,1,1), axis4.
No extra Alpha multiplication in these selected blocks. Shared blend1620B3A:
metadata byte+214==3 calls18E7137, otherwise18E709C(0). Actual loaded
S21 metadata remains to verify; RGB JPEG alone is not runtime blend proof.

Read-only mono asset verifier pins source D:/MU FICA Season 21/Data/Effect:

| Resource | File | Bytes | SHA256 |
| --- | --- | --- | --- |
| 8086 | firehik_mono01.OZJ | 16340 | c3e18c474ba5ae9f06b9e7159afc0dab34869b78eab425cb5538b94fe0e7138f |
| 8087 | firehik_mono02.OZJ | 17160 | ea67813627c32a9f9eae948ffff2ee47a0fd44a49426cce2879fcef4a6b5f7d6 |
| 8089 | firehik_mono03.OZJ | 16049 | cf6ba21108507e377c40145912ec3db7fbbd19031ac8aeaf129f5a6773fff315 |

All payloads decode at offset24 as64x64 RGB JPEG. Prefix also beginsFFD8,
so searching only first JPEG signature is insufficient. Native GlobalBitmap.cpp
OZJ loader skips24. Source assets unchanged; not yet staged/engine-load-proven.

Successful-loader metadata contract now confirmed (not a live snapshot):
mono registrations18BD7EC/18BD80C/18BD84C pass texture8086/8087/8089,
verified literal Effect/firehik_mono01/02/03.jpg paths, LINEAR2601,
CLAMP2900 and trailing1,0 to18F8FA2. JPEG branchCC4793 callsCC52F7;
CC562D sets metadata+214=3. Renderer1620B47 selects additive18E7137
for component3. Bitmap lookup1620AE3 usesCC4B68 managerBEACFC8,
returning cached/map metadata or error fallback1FAA0D0 when missing.
Thus additive is established for successful mono JPEG loads; missing loads
must fail closed, not be treated as proof of component3 at runtime.
Native EnsureBitmaps globally usesCLAMP_TO_EDGE. Follow the isolated
EnsureBrecheBitmaps pattern with private Wrath registration and CLAMP;
do not change shared texture behavior. No registration code changed yet.

Scatter8081..8083 subtype0 lifecycle: constructor table16A9C75[0] selects
16663FC..1666533, counter5, rotationR%360, parameter+6C=0, Alpha=1,
savedRGB=inputRGB and liveRGB=0; supplied Scale/Position retained.
Update table1723BD9[0] selects16D8F22..16D9080: Alpha-=.15;
Alpha<.1 clears Live, then Light=savedRGB*Alpha, no motion/scale/rotation write.
Counter decrements in common tail; constructor5 means fifth update leaves
alpha about.25 and counter0, next manager visit removes it. Do not extend
lifetime until fade reaches0. Common velocity initialized0; emitter ownerNULL.
Renderer163111D subtype0 skips7/8/9 to1631343; draw16313DF is storedRGB
billboard, original texture, rotation, bitmap dimensions*Scale, fullUV,axis4.
New UpdateWrathScatterTick scalar adapter and focused constexpr tests PASS;
not engine-wired. Scatter asset hash/registration remains next.

Sprite follow-through: shared1727079 renderer envelope1727082..1727119
adds.1/caps1 when Visible; subtracts.1/floors.2 otherwise, per render visit.
Native special case formerly CircleShiny only now includes private Wrath
lightmarks/flare01, leaving ordinary SS6 sprites unchanged. Native render
manager clears Live after pass0/2 and retains pass1, corresponding to source
1726F87 behavior. Visibility producer, pass scheduling and full owner lifetime
still require runtime audit; no persistent effect-style lifetime is invented.
Direct-call leads for enclosing13DB9D7:9A7720,1253C01,13F3E9C,
13F3EAE,18F495D. Inspected13F3E64..13F3ECF: caller checks object Live
and byte+B visibility before calls; two branches pass third argument0/1
based on comparison of local-C to globals1DD8134/1DD8130. Object/class
iteration role and full frame scheduling not established by these call sites.

Caller now anchored13F3BA4..13F3EF7, fully decoded. Iterates400 slots,
gets character through9610CB/9A45D3 then OBJECT=character+3254. It applies
team/cloaking-shaped filtering before Live/Visible checks, compares iteration
index against globals1DD8134/1DD8130 and passes selection0/1 to13DB9D7.
Native RenderCharactersClient has corresponding character iteration,
cloaking/team filters, live/visible and SelectedCharacter/SelectedNpc handling.
This establishes the character-render-stage correspondence, not frame frequency.
Direct calls to this loop:C6965A,18F4F4E,18F682C remain pass-context leads.
Primary13DB9D7 entry also has early exits for OBJECT+3A/+DC predicate,
buff27 and missing/empty model. Model lookup from OBJECT+5C through968B22/
EF2A81 stores-97C, the same model pointer used by Wrath bone helper calls.
Do not bypass these gates just because native live/visible conditions match.

Scene ordering18F60EF..18F6D93 linearly decoded: character render18F682C,
sprite visibility pass18F693B, sprite draw18F6A80(pass0), particle draw18F6A8C(pass0).
Later optional path calls visibility18F6B48, sprites18F6B81/particles18F6B8E
with local-78 pass argument, without another character-render call in this function.
Native main-scene ZzzScene.cpp has corresponding RenderCharactersClient ->
CheckSprites -> RenderSprites/Particles then optional water pass without character
re-emission. Do not run the buff emitter again merely because sprite pass repeats.
Visibility producer1726FCF..1727014 scans1000 sprites and sets byte+B=1 for
Live entries, matching native CheckSprites. This resolves the ordinary visibility
producer, not all special render paths.18F4D5E also has character/check/sprite/
particle ordering; C6962F is another caller requiring context if enabled outside gameplay.

Fourth terrain pulse recovered18F930D..18F93DE. Let speed=arg8,
phase=argC, T=global70B53D8. Following float operations and truncation:
r = int((phase*0.01745329238474369f)*1000.f/speed+T)
    % int(6283.185546875f/speed);
a = float(r)*.001f*speed;
v = float(cos(double(a))); if(a < 3.1415927410125732f) v=-v;
return(v+1.f)*.5f. The cosine wrapper A01178 calls1A29A90, whose fallback
1A36C9F contains FCOS at1A36CBD. Preserve signed remainder and negative
speed: Wrath uses pulse(-8,0) for RGB(.25,.65,.85), and pulse(4,0)*2+.9
for the fourth ground layer's size. Do not replace with a generic sine wave.

Native CustomItemRenderMesh.cpp::Render22 is NOT equivalent: a1/100 work
factor, timeGetTime clock, approximate degree/pi constants, cosf, and special
zero-angle behavior(.5 vs source0). Keep that unrelated helper unchanged.
Native clock mapping and numerical regression still required before runtime use.

Native Wrath-only pulse adapter now in GrowLancerWrathGroundPulse.h, specialized
to verified phase0 and speeds-8/4. It preserves signed integer remainder,
float multiply order and float rounding around caller-provided double cosine.
Compile-time tests pass for truncated periods785/1570, fractional/negative
clock truncation and the sign branch below/above pi. Tests inject constant
cosine to isolate arithmetic; they do NOT claim libm equivalence or establish
native clock units. Not wired to terrain until those contracts are resolved.

Clock source verified against existing Breche reverse and re-read writer13268D4:
IAT1B4C9A8=timeGetTime, unsignedDWORD corrected to double using table1CA4320,
then CVTSD2SS writes70B53D8 at13268FB. Native WorldTime is double timer-relative
milliseconds (ZzzAI.cpp/CTimer::GetTimeElapsed), not the same epoch. Wrath must
own an explicit timeGetTime float sample; do not replace or retime WorldTime.
Pulse adapter now emulates CVTTSS2SI integer-indefinite for out-of-range/NaN,
avoiding C++ conversion UB at long uptime. Tests cover2^31/2^32 and last valid
positive float. Sampling location/cadence and actual cosine comparison remain open.

Terrain continuation13EBFDE..13EC3F6: four additive1765DF1 calls use8078,
7FDD,7FDD,8147. First is size8, RGB(.15,.1,1), rotation0; second size2.8,
RGB(.5,.5,.5), rotation global70B53D8*.05; third size4.5,sameRGB,
opposite rotation. All use incoming second object's XY, alpha1,height5,
terrain flag1,flip0. Fourth8147 uses helper18F930D to modulate size/light;
that helper's arithmetic/libm equivalence is not yet fully mapped.
Registry identifies7FDD flareBlue.jpg and8147 Shockwave2.jpg; asset hash/load
and registration reachability still need the same provenance gate as8078.

Bone helper132EC63..132EE64 was fully inspected: validates bone index/object,
uses object-specific matrix if available or global fallback, obtains matrix
translation for null relative position, multiplies by model+74 and adds
object position. Native BMD::TransformByObjectBone has the corresponding
BodyScale/matrix/Position operation. Preserve this native API; do not apply
object scale again. Object matrix getters13165F9/1316442 and caller model
selection/bone compatibility remain to pin before calling mapping complete.

First sprite batch decoded continuously from enclosing prologue13DB9D7 to
13EBFDE, with known buff gate13EB9DA. Eleven8078 submissions follow bone
helper132EC63 arguments4,11,34,25,35,26,10,4,17,18,20 in that order.
They use RGB(.23,.2,1), shared jitter(rand()%15)/37+.4, and added base sizes
.2 for first six,.5 for next two,.8 for last three. Last bone20 position
adds world(0,0,20) after helper return. Bone helper semantics/skeleton mapping
remain to verify; these are numeric arguments, not guessed anatomical names.

Branch continues after those11 sprites: selected calls include terrain1765DF1
at13EC0E7/13EC1C1/13EC260/13EC3EA, particle1724176 at13EC59D/13EC6B2
and later, plus7EF7 sprite submissions13EC76E/13EC7B6. Complete continuation
control flow and their arguments are OPEN. Do not stop after11 lightmarks or
claim these call counts prove visual completeness/cadence.
Verifier: tools/grow_lancer/verify_wrath_persistent_sprites.py covers only
first11 submission identities/bone arguments and selected scalar constants.

Resource8078 is **Effect/lightmarks.jpg**, not Breche's lightmarks_red.jpg.
Registry entry18BD68C is independently supported by decoded registration:
18BD69A pushes8078;18BD69F pushes1BB8F34 (NUL string Effect\\lightmarks.jpg);
18BD6A4 calls18F8FA2. Actual source Data/Effect/lightmarks.OZJ is17877 bytes,
SHA256 DC4E83DDB76554298ED9619FEFB02DE7E0D3A210DB8424B97CB1244E62B4305F.
Read-only hash checked; not staged and format/load not yet verified.

Buff branch calls172760A, decoded completely172760A..172767B: two manager
gates can return0; otherwise forwards all nine args to1726C50 with sprite-pool
this=A4C1130. This supports sprite-pool provenance, not permission to omit
the manager gates or replace lightmarks with a similarly named texture.
Full13EB9DA enclosing function and all emissions/bones remain OPEN.

Scale caller follow-through: exact140FD3F..141043D (equipment-shaped updates)
and1410942..1410C0F (body-part updates) call141043E at1410412/1410C08.
Both are fully linearly decoded and pinned by the scale verifier. These are
not yet evidence that buff removal invokes either caller. Candidate1294E5A
encloses12962B6 scale call; next trace its actual dispatch/packet role.

Renderer follow-up A49554..A497AD confirms aura0/1 mesh0 flags42,
blendmesh0,blendlightAlpha and animated UV. Subtype1 assigns BodyLight from
effectRGB*Alpha, not old model light*effectRGB*Alpha; native mapping matches.
verify_wrath_aura_renderer.py PASS checks selected call/assignment arguments.
All current Wrath corrections linked and staged with SHA256
323C7F8FC757D7D99F4BC8BCC7D6D8CDEA3E549DDF5E5CDEA3DDDD818F659B9F.
Previous7E619364 retained in isolated before-wrath GUID backup. Stage verifier
PASS2658base/79overlay/3rootData,Player hash/CRC. Not visual/buff runtime PASS.

The isolated staging verifier pins both BMD containers, `alpha_line.OZJ`,
`dust01.OZT`, `!Combo3.OZJ`,
`alpha_light.OZT`, `brocken.OZJ` and `Wrath.wav`.
No production or S21 source path is modified.

Remaining: prove server defense-cost damage bonus and buff duration/refresh;
complete the action/skeleton runtime comparison; build and compare runtime
cleanup after buff removal. Private RISE runtime IDs are assigned in
`GrowLancerResources.h`; direct S21 addresses/layouts are not transplanted.

### Owner visual note recorded — 2026-09-14

The owner reports that after the buff is applied to the player there are two
small fire-like marks followed by a blue lightning effect.  In the recovered
chain those player-attached marks are the two `7EF7/flare01` sprites at bones
29/38, followed by the blue `8086/8087/8089` mono-particle branch on the same
bones.  The preceding `8081..8083` pair is a separate null-owner scatter
emission around the player position, not the attached fire-dot pair.  This is
not Breche or Clash.
The observation is retained as a runtime lead only; exact cadence, branch
residue and owner-pixel parity remain governed by the dump evidence and still
need direct buff-active QA.

### Post-buff visual sequence contract — 2026-09-14

`tools/grow_lancer/verify_wrath_buff_visual_sequence.py` now locks the
source-to-native order behind the 424/425 membership gate: two S21 scatter
particles (`lighting_mega01..03`) are submitted first, then the two bone
flares (`flare01` at attachments 29/38), followed by the four blue mono
particles (`firehik_mono01..03`) attached to those same bones.  The verifier
also checks the private registrations and the native adapter callbacks, so a
future Breche/Clash render change cannot silently remove the player buff
visual chain.  This is static/source evidence only; buff-active owner pixels,
frame cadence, blend state and removal cleanup remain open for runtime QA.

`tools/grow_lancer/verify_wrath_flare_native_adapter.py` additionally pins the
two fire-dot sprites to the native `CreateSprite`/`RenderSprites` path: private
`flare01.jpg` is loaded as a 64x64 RGB resource, subtype 0 selects the native
additive blend, and the private visibility envelope is retained.  This closes
the source-level black-square/blend routing ambiguity; it does not promote
live pixels or cadence to PASS.

The isolated RuntimeQA client now exposes `F12` as a reversible fixture for
the recovered base Wrath buff gate 424. Turning it on arms the bounded owner
telemetry; turning it off flushes the sequence before unregistering. It only
calls the native local `BuffMap` register/unregister path, emits diagnostic
log lines, and never sends a packet or changes the production build. This makes
the post-buff owner layer testable without entering S21 Siege; the resulting
screen pixels and cadence still require owner review.

### Native mono adapter follow-through — 2026-09-14

`tools/grow_lancer/verify_wrath_mono_native_adapter.py` now checks the complete
RISE source route for all selected S21 mono resources: allocation preserves the
13/14 and 14/15 attachment subtype pairs and the live owner pointer;
`MoveParticles` validates the owner, resolves bone29/38, and dispatches the
separate 8086/8087/8089 update bodies; `RenderParticles` sends all three through
the native full-UV `RenderSprite` path; and owner retirement unlinks all three
variants without changing the ordinary particle pool policy.  The verifier
passes.  This closes the source adapter routing gap, but not live buff cadence,
GPU pixels, blend observation or removal stress.
