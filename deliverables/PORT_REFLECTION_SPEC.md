# C Port Reflection Spec

Catalog of every Java reflection site in stripped LionEngine + Lionheart and the explicit dispatch each needs in the C/SDL2 port. Closed-world dispatch — all class names come from shipped XMLs, never user input.

## Verdict

Reflection sites: **13 distinct call points** in 11 files. Total dispatch table size in C: **~150 entries**. Java tree currently uses reflection at runtime; C port replaces every site with a `const struct` table.

## Site catalog

### 1. `lionengine-core/UtilReflection.java` (~365 LOC)
Generic reflective ctor + interface walker.
**C replacement**: delete entirely. All callers get explicit dispatch (see sites 3, 5, 6, 7, 12).

### 2. `lionengine-core/XmlReader.java:517,522` — `loader.loadClass()`
Generic `getImplementation()` for arbitrary XML node values.
**C replacement**: audit shows zero runtime callers in shipping game. Delete or no-op.

### 3. `lionengine-game/feature/Factory.java:282,291,293,319` — entity dispatch
Reads `<lionengine:class>` from featurable XML.
**Closed values (3 entries)**:
| XML string | C ctor |
|---|---|
| `com.b3dgs.lionengine.game.feature.FeaturableModel` | `featurable_model_new` |
| `com.b3dgs.lionengine.helper.EntityHelper` | `entity_helper_new` |

(Setup variants resolved separately, see site 4.)

### 4. `lionengine-game/feature/Setup.java:134` — setup config class
Reads `<lionengine:setup>`.
**Closed values (2 entries)**:
| XML string | C ctor |
|---|---|
| `com.b3dgs.lionengine.game.feature.rasterable.SetupSurfaceRastered` | `setup_surface_rastered_new` |
| `com.b3dgs.lionheart.object.SetupEntity` | `setup_entity_new` |

### 5. `lionengine-game/feature/FeaturableConfig.java:222` — feature dispatch
Reads `<lionengine:feature>` strings (cached `Map<String,Class>`).
**Closed values: ~80 feature classes** (one entry per feature class shipped). Generate at port-time by scanning all `*.xml` in `assets/src/main/resources/com/b3dgs/lionheart/` for `<lionengine:feature>` values.
Generation command:
```bash
grep -rh "<lionengine:feature>" /home/romain/LionheartMD/lionheart-stripped/assets/src/main/resources/ \
  | sed 's/.*>\(.*\)<.*/\1/' | sort -u > deliverables/feature_classes.txt
```

### 6. `lionengine-game/feature/state/StateHandler.java:169,267,271,276` — state factory + dynamic ctor
- Line 169: `<lionengine:state initial="...">` → init state class. Resolves string→Class.
- Lines 267-276: walks state class ctor signatures via `UtilReflection.getCompatibleConstructor` to inject the right feature dependency.
**Closed values**: 62 State subclasses in `lionheart-game/src/main/java/com/b3dgs/lionheart/object/state/**/*.java`.
Enumeration command:
```bash
find /home/romain/LionheartMD/lionheart-stripped/java/lionheart-game/src/main/java/com/b3dgs/lionheart/object/state -name 'State*.java' | xargs -I{} basename {} .java | sort -u
```
**C dispatch**: `state_id_t` enum (62 entries) + `const struct { state_id_t id; state_factory_fn ctor; feature_id_t needs; } state_table[STATE_COUNT]`. Initial-state lookup needs `string→state_id_t` map (only 7 distinct names appear in XML — much smaller).

### 7. `lionengine-game/feature/Services.java:123,133` — service auto-instantiation
`UtilReflection.create(Class<?>, ...)` for registered services.
**C replacement**: every caller passes a static `.class` literal. Inline to direct `new ServiceX(services)` at registration site. No table needed.

### 8. `lionengine-helper/DeviceControllerConfig.java:191,197` — input config
`loader.loadClass()` for input mapping enum + DevicePush class.
**Closed values (2-3 entries)**:
| XML string | C type |
|---|---|
| `com.b3dgs.lionengine.awt.Keyboard` | `INPUT_KEYBOARD` |
| `com.b3dgs.lionengine.awt.Mouse` | `INPUT_MOUSE` |
| (gamepad — already stripped) | `INPUT_GAMEPAD` |

### 9. `lionengine-game/feature/Features.java:55,60,173,182` — `@FeatureInterface` walker
**Load-bearing**: walks parent interfaces and registers the feature under each `@FeatureInterface`-annotated parent type. Enables `featurable.getFeature(SomeInterface.class)` polymorphism.
**C replacement**: each feature class declares its keys explicitly via `const feature_id_t feature_keys[]` field. `getFeature(id)` is array-indexed. ~80 feature classes × 1-3 keys each = ~150 array entries.
This is the biggest single port-time data table.

### 10. `lionengine-game/feature/HandlablesImpl.java:65,91` — type→feature map
Walks feature's interface tree via `UtilReflection.getInterfaces()` to register under each interface key.
**C replacement**: same `feature_keys[]` data as site 9. One generation pass populates both.

### 11. `lionheart-game/object/EntityModel.java:95,110` (and `EntityHelper.java:67,75`) — animation name from State class
`state.getSimpleName().substring(PREFIX).toLowerCase()`. Maps `StateWalk` → `"walk"`.
**C replacement**: build-time codegen of `const char* state_animation_names[STATE_COUNT]`. Trivial.

### 12. `lionengine-core/graphic/engine/UtilSequence.java:55,92` — sequence transition
`UtilReflection.create(nextSequence, ...)` for screen transitions.
**Closed values (~10 entries)**: every `Sequence` subclass in `lionheart-game/.../{Loading,menu/Menu,menu/Continue,Scene,ScenePicture,intro/{Intro,Part1,Part2,Part3,Part4},extro/{Extro,Part1,Part2,Part3,Part4,Part5,Credits}}`.
**C dispatch**: `sequence_id_t` enum + factory function pointer per sequence.

### 13. `*.toString()` `getClass().getSimpleName()` (~10 sites, Geom/Force/Damages/etc.)
Pure debug strings.
**C replacement**: hardcode literal in each `toString()`-equivalent (or drop entirely on MD — no debug strings in ROM).

### Non-reflection (false positives)
- `Engine.getName()`, `Animation.getName()`, `CollisionCategory.getName()` — instance `getName()` methods on game objects; not reflection.
- `featurable.getFeature(SomeClass.class)` — uses Class as type token in `Map<Class,Feature>`; ports cleanly to enum-keyed array.

## Total dispatch table sizes for C port

| Table | Entries | Source |
|---|---:|---|
| Entity class factory | 2-3 | site 3 |
| Setup factory | 2 | site 4 |
| Feature class factory | ~80 | site 5 (auto-gen from XML scan) |
| State class factory | 62 | site 6 |
| State init-name → state_id | 7 | site 6 |
| State animation name (codegen) | 62 | site 11 |
| Feature interface keys (codegen) | ~150 | sites 9 + 10 |
| Input device factory | 2-3 | site 8 |
| Sequence factory | ~10 | site 12 |
| **Total** | **~380 entries** | |

Fits comfortably in MD ROM. Generate via build-time scripts scanning XMLs + Java sources.

## Generation pipeline (build-time codegen)

```bash
# Run at C build time, output: gen/dispatch_tables.h + gen/dispatch_tables.c
python3 c-port/scripts/gen_dispatch.py \
  --java-root  /home/romain/LionheartMD/lionheart-stripped/java \
  --xml-root   /home/romain/LionheartMD/lionheart-stripped/assets/src/main/resources \
  --out        c-port/gen/
```

Script enumerates:
1. `find State*.java` → state enum + factory table
2. `find Sequence*.java` (Loading, Menu, Scene, etc.) → sequence enum + factory table
3. `grep <lionengine:feature>` → feature class registry
4. `grep <lionengine:class>` → entity factory dispatch
5. State `getSimpleName()` derivations → animation name array

Each Java refactor (Java A1+A2+A3) above can be skipped if C port relies on the codegen tables instead. Java keeps using reflection until it dies.

## Java-side strip recommendation

**Skip phase A in Java.** The Java tree currently works. The audit-recommended dispatch tables are deliverable as C-port codegen scripts that read the existing Java sources + XMLs, so we don't need to refactor Java to enable the C port.

Phase A in Java would deliver:
- ~150 dispatch entries in static initializers
- Comparable runtime semantics
- Same effort as the C codegen scripts

**Better plan**: write the codegen scripts at C-port time. They scan the same data the audit found. Output the C tables directly. Java stays unchanged.

The reflection audit's actual deliverable is THIS document — the catalog of what dispatch tables need exist in C, sized concretely so MD ROM budget can be planned.

## Files to retain in Java tree

All current. Reflection-using files (UtilReflection.java, Factory.java, FeaturableConfig.java, StateHandler.java, Setup.java, DeviceControllerConfig.java, UtilSequence.java) stay. They're the runtime that proves the dispatch works; the C port replaces them with codegen output.
