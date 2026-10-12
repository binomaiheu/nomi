#!/usr/bin/env python3
"""Download the third-party CC0 assets used by nomi and install them into share/.

Sources (all CC0 / public domain, see share/CREDITS.md):
  * Kenney (www.kenney.nl) : Pixel Platformer, Kenney Fonts, Digital Audio, Impact Sounds, Music Jingles
  * OpenGameArt : "Happy Adventure (Loop)" by TinyWorlds, "4 Music Box Tracks" by Aureolus_Omicron

Usage:  python tools/fetch_assets.py [--cache DIR]

Requires Pillow (pip install pillow) to cut and scale the sprites.
The results are committed to the repository, so this only has to run when changing assets.
"""

import argparse
import io
import os
import shutil
import sys
import tempfile
import urllib.request
import zipfile

from PIL import Image, ImageEnhance

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
SHARE = os.path.join(ROOT, "share")

KENNEY = "https://kenney.nl/media/pages/assets/"
DOWNLOADS = {
    "platformer": KENNEY + "pixel-platformer/33bb4921eb-1696667883/kenney_pixel-platformer.zip",
    "fonts":      KENNEY + "kenney-fonts/8d5435c213-1677661710/kenney_kenney-fonts.zip",
    "digital":    KENNEY + "digital-audio/216eac4753-1677590265/kenney_digital-audio.zip",
    "impact":     KENNEY + "impact-sounds/87b4ddecda-1677589768/kenney_impact-sounds.zip",
    "jingles":    KENNEY + "music-jingles/f37e530b9e-1677590399/kenney_music-jingles.zip",
    "musicbox":   "https://opengameart.org/sites/default/files/4_music_box_tracks_ogg.zip",
    "adventure":  "https://opengameart.org/sites/default/files/happy_adveture.mp3",
}

# sound effects : name used by the game -> (download, file inside the zip)
SOUNDS = {
    "jump":     ("digital", "Audio/phaseJump1.ogg"),
    "coin":     ("digital", "Audio/highUp.ogg"),
    "oneup":    ("digital", "Audio/powerUp7.ogg"),
    "stomp":    ("impact",  "Audio/impactSoft_medium_000.ogg"),
    "hurt":     ("digital", "Audio/lowDown.ogg"),
    "select":   ("digital", "Audio/twoTone1.ogg"),
    "start":    ("digital", "Audio/threeTone1.ogg"),
    "goal":     ("jingles", "Audio/8-Bit jingles/jingles_NES00.ogg"),
    "gameover": ("jingles", "Audio/8-Bit jingles/jingles_NES05.ogg"),
    "victory":  ("jingles", "Audio/8-Bit jingles/jingles_NES13.ogg"),
}

# Pixel Platformer tiles are 18x18, scaled x3 to sit nicely next to the 64x64 terrain tiles
KTILE = 18
SCALE = 3


def fetch(name, cache):
    url = DOWNLOADS[name]
    path = os.path.join(cache, os.path.basename(url))
    if not os.path.exists(path):
        print("downloading", url)
        req = urllib.request.Request(url, headers={"User-Agent": "nomi-asset-fetcher"})
        with urllib.request.urlopen(req) as r, open(path, "wb") as f:
            shutil.copyfileobj(r, f)
    return path


def zip_member(zf, suffix):
    """find a file in a zip by the end of its path (packs differ in their top folder)"""
    for n in zf.namelist():
        if n.replace("\\", "/").endswith(suffix):
            return n
    raise KeyError(suffix)


def kenney_tile(sheet, index):
    cols = sheet.size[0] // KTILE
    x, y = (index % cols) * KTILE, (index // cols) * KTILE
    return sheet.crop((x, y, x + KTILE, y + KTILE))


def scaled(img, factor=SCALE):
    return img.resize((img.size[0] * factor, img.size[1] * factor), Image.NEAREST)


def ensure(path):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    return path


def install_sprites(cache):
    with zipfile.ZipFile(fetch("platformer", cache)) as zf:
        sheet = Image.open(io.BytesIO(zf.read(zip_member(zf, "Tilemap/tilemap_packed.png")))).convert("RGBA")
        backs = Image.open(io.BytesIO(zf.read(zip_member(zf, "Tilemap/tilemap-backgrounds_packed.png")))).convert("RGBA")
        license_txt = zf.read(zip_member(zf, "License.txt"))

    t = lambda i: scaled(kenney_tile(sheet, i))
    size = KTILE * SCALE

    # coin : 2 frame spin (front, side)
    coin = Image.new("RGBA", (2 * size, size))
    coin.paste(t(151), (0, 0))
    coin.paste(t(152), (size, 0))
    coin.save(ensure(os.path.join(SHARE, "sprites", "coin.png")))

    # finish flag : 2 frames, each a flag tile on top of two pole tiles
    flag = Image.new("RGBA", (2 * size, 3 * size))
    for f, top in enumerate((111, 112)):
        flag.paste(t(top), (f * size, 0))
        flag.paste(t(131), (f * size, size))
        flag.paste(t(131), (f * size, 2 * size))
    flag.save(os.path.join(SHARE, "sprites", "flag.png"))

    t(44).save(os.path.join(SHARE, "sprites", "heart.png"))

    # backdrop : the left panel of the backgrounds sheet (sky, clouds, hills), scaled x4 and
    # padded at the top with sky to a multiple of 64 so Tiled can use it as a 64x64 tileset
    panel = backs.crop((0, 0, 64, 72))
    big = scaled(panel, 4)                                   # 256 x 288
    big = ImageEnhance.Color(big).enhance(2.2)               # the original is quite pale
    big = ImageEnhance.Brightness(big).enhance(0.95)
    sky = big.getpixel((2, 2))
    backdrop = Image.new("RGBA", (256, 320), sky)
    backdrop.paste(big, (0, 320 - big.size[1]), big)
    backdrop.save(ensure(os.path.join(SHARE, "tilesets", "backdrop.png")))
    print("sky colour for the maps : #%02x%02x%02x" % sky[:3])

    # entity images for the Tiled editor (an image collection tileset)
    ent = os.path.join(SHARE, "tilesets", "entities")
    os.makedirs(ent, exist_ok=True)
    t(151).save(os.path.join(ent, "coin.png"))
    flag.crop((0, 0, size, 3 * size)).save(os.path.join(ent, "goal.png"))
    chars = {"spawn": "kit_from_firefox.png", "enemy_gnu": "gnu_from_gnu.png", "enemy_tux": "tux_from_linux.png",
             "enemy_wilber": "wilber_from_gimp.png", "enemy_kisi": "kisi_from_konsolscript.png"}
    for name, sheet_file in chars.items():
        img = Image.open(os.path.join(SHARE, "pixmaps", sheet_file)).convert("RGBA").crop((0, 0, 56, 80))
        img.save(os.path.join(ent, name + ".png"))

    write_license("kenney-pixel-platformer.txt", license_txt)


def install_fonts(cache):
    with zipfile.ZipFile(fetch("fonts", cache)) as zf:
        for src, dst in (("Fonts/Kenney Future.ttf", "KenneyFuture.ttf"), ("Fonts/Kenney Pixel.ttf", "KenneyPixel.ttf")):
            with open(ensure(os.path.join(SHARE, "fonts", dst)), "wb") as f:
                f.write(zf.read(zip_member(zf, src)))
        write_license("kenney-fonts.txt", zf.read(zip_member(zf, "License.txt")))


def install_sounds(cache):
    zips = {}
    try:
        for name, (pack, member) in SOUNDS.items():
            if pack not in zips:
                zips[pack] = zipfile.ZipFile(fetch(pack, cache))
                write_license("kenney-%s.txt" % pack, zips[pack].read(zip_member(zips[pack], "License.txt")))
            with open(ensure(os.path.join(SHARE, "sounds", name + ".ogg")), "wb") as f:
                f.write(zips[pack].read(zip_member(zips[pack], member)))
    finally:
        for z in zips.values():
            z.close()


def install_music(cache):
    with zipfile.ZipFile(fetch("musicbox", cache)) as zf:
        with open(ensure(os.path.join(SHARE, "music", "title.ogg")), "wb") as f:
            f.write(zf.read(zip_member(zf, "musicbox2_cute_tune.ogg")))
    shutil.copyfile(fetch("adventure", cache), os.path.join(SHARE, "music", "level.mp3"))


def write_license(name, data):
    with open(ensure(os.path.join(SHARE, "licenses", name)), "wb") as f:
        f.write(data)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--cache", help="directory to keep the downloads in (default: a temporary directory)")
    args = ap.parse_args()

    cache = args.cache or tempfile.mkdtemp(prefix="nomi-assets-")
    os.makedirs(cache, exist_ok=True)

    install_sprites(cache)
    install_fonts(cache)
    install_sounds(cache)
    install_music(cache)
    print("assets installed in", SHARE)
    return 0


if __name__ == "__main__":
    sys.exit(main())
