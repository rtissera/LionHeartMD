# LionheartMD

Faithful port of [Lionheart Remake](https://github.com/b3dgs/lionheart-remake) (GPL-3.0, by Pierre-Alexandre / Byron 3D Games Studio) — Java → C/SDL2 → eventual Sega Mega Drive homebrew. Original Lionheart was a 1992 Amiga platformer by Thalion.

## Layout

```
c-port/                 partial hand C/SDL2 port (phases 1+2a+2b done)
lionengine-stripped/    LionEngine fork, 8 dead subpkgs + audio modules + filters dropped
lionheart-stripped/     Lionheart fork, sc68 + lwjgl + GUI launcher + multiplayer all stripped
deliverables/           baseline + stripped jars + STRIP_REPORT.md
```

Upstream sources (`lionengine/`, `lionheart-remake/`) excluded from repo — recover with:

```bash
git clone --depth 1 https://github.com/b3dgs/lionengine
git clone --depth 1 https://github.com/b3dgs/lionheart-remake
```

## Build (stripped)

```bash
cd lionengine-stripped && mvn -B -DskipTests -Dcheckstyle.skip=true install
cd lionheart-stripped  && mvn -B -DskipTests -Dcheckstyle.skip=true -P linux -pl java/lionheart-pc -am clean package
java -jar lionheart-stripped/java/lionheart-pc/target/lionheart-pc-1.4.0-SNAPSHOT.jar
```

Requires JDK 17+, Maven 3.8+. AWT-only (no LWJGL/JNA needed).

## Build (C port)

```bash
cd c-port && make
```

Requires SDL2 + SDL2_image + SDL2_ttf.

## Strip metrics (vs upstream baseline)

| | Baseline | Stripped | Δ |
|---|---:|---:|---:|
| Java files | 1119 | 773 | **-346 (-31%)** |
| Java LOC | 151,336 | 116,415 | **-34,921 (-23%)** |
| Fat jar | 10.96 MB | 8.57 MB | **-2.39 MB (-22%)** |

What was stripped — see `deliverables/STRIP_REPORT.md`.

## License

GPL-3.0, inherited from upstream Lionheart Remake + LionEngine. Copyright headers preserved on every file derived from upstream.
