#!/usr/bin/env python3
"""Generate the nomi levels as Tiled .tmx maps (share/levels/level1.tmx, level2.tmx, ...).

The levels are described below with a small builder API (flat ground, pits, steps,
platforms, coins, enemies) so they are easy to tweak. The generated maps are ordinary Tiled
maps : open them in Tiled to edit by hand afterwards (but then don't re-run this script, or
save your edits under another name).

Every level is checked for reachability with the same jump physics as the game (src/physics.hpp):
gaps and steps on the main path must be jumpable, otherwise the script fails.

Usage:  python tools/gen_level.py
"""

import math
import os
import random
import sys

ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
LEVELS = os.path.join(ROOT, "share", "levels")

# --- physics, keep in sync with src/physics.hpp --------------------------------------------
TILE = 64
GRAVITY = 3000.0
MAX_SPEED = 420.0
JUMP_HEIGHT = 230.0
PLAYER_WIDTH = 30.0
SAFETY = 0.85            # only use 85% of the theoretical jump distance

V0 = math.sqrt(2 * GRAVITY * JUMP_HEIGHT)


def max_jump_distance(rise_px):
    """horizontal distance covered by a full jump that lands rise_px higher (negative = lower)"""
    disc = V0 * V0 - 2 * GRAVITY * rise_px
    if disc < 0:
        return -1.0
    t = (V0 + math.sqrt(disc)) / GRAVITY
    return MAX_SPEED * t


# --- tilesets -------------------------------------------------------------------------------
# terrain : share/tilesets/tileset.tsx, 6 columns of 64x64, firstgid 1
T_FIRSTGID = 1
GRASS, GRASS_L, GRASS_R = 31, 32, 33
FLOAT_L, FLOAT_M, FLOAT_R = 34, 30, 35      # floating grass platform (jagged bottom)
DIRT = 36
SLAB_L, SLAB_M1, SLAB_M2, SLAB_R = 26, 27, 28, 29   # stone slab, one-way
ISLAND = ((5, 6), (9, 10))                # 2x2 floating rock
WATER = 22
BUSH, SMALL_CACTUS, CACTUS1, CACTUS2 = 1, 4, 2, 3
FLOWERS = 15
GRASS_TUFTS = (16, 17, 18)
FENCE_L, FENCE_M, FENCE_R = 13, 12, 14
TREE_TOP, TREE_MID, TREE_TRUNK = 41, 40, 39
BIG_BUSH = 40

# backdrop : share/tilesets/backdrop.tsx, 4x5 tiles
B_FIRSTGID = 43
B_COLS, B_ROWS = 4, 5

# entities : share/tilesets/entities.tsx (image collection)
E_FIRSTGID = 63
E_SPAWN, E_COIN, E_GOAL = 0, 1, 2
E_ENEMY = {"gnu": 3, "tux": 4, "wilber": 5, "kisi": 6}
E_SIZE = {E_SPAWN: (56, 80), E_COIN: (54, 54), E_GOAL: (54, 162), 3: (56, 80), 4: (56, 80), 5: (56, 80), 6: (56, 80)}

SKY = "#c0f1ef"


class Level:
    def __init__(self, name, width, height=14, seed=1):
        self.name = name
        self.w, self.h = width, height
        self.rng = random.Random(seed)
        self.heights = [None] * width        # surface row per column, None = pit
        self.ground = self.grid()
        self.deco = self.grid()
        self.objects = []                    # (gid, x, y, w, h)
        self.platforms = []                  # (x0, x1, row) for the reachability check
        self.reserved = set()                # columns where no decoration should go
        self.deferred = []                   # entity placements resolved once the terrain is complete
        self.x = 0
        self.row = height - 3                # current surface row

    def grid(self):
        return [[0] * self.w for _ in range(self.h)]

    # --- terrain ---------------------------------------------------------------------------
    def flat(self, n, dy=0):
        """n columns of ground, dy tiles higher (or lower) than before; returns the first column"""
        self.row -= dy
        start = self.x
        for c in range(self.x, self.x + n):
            self.heights[c] = self.row
        self.x += n
        return start

    def pit(self, n):
        """a gap of n columns; returns the first column"""
        start = self.x
        self.x += n
        return start

    def ground_row(self):
        """surface row of the ground built last"""
        return self.row

    def slab(self, x0, length, row):
        """stone slab platform, one-way (jump through from below)"""
        tiles = [SLAB_L] + [SLAB_M1 if i % 2 == 0 else SLAB_M2 for i in range(length - 2)] + [SLAB_R]
        for i, t in enumerate(tiles):
            self.ground[row][x0 + i] = t
        self.platforms.append((x0, x0 + length - 1, row))
        return self

    def floating(self, x0, length, row):
        """floating grass platform, solid"""
        tiles = [FLOAT_L] + [FLOAT_M] * (length - 2) + [FLOAT_R] if length > 1 else [FLOAT_M]
        for i, t in enumerate(tiles):
            self.ground[row][x0 + i] = t
        self.platforms.append((x0, x0 + length - 1, row))
        return self

    def island(self, x0, row):
        for dy, pair in enumerate(ISLAND):
            for dx, t in enumerate(pair):
                self.ground[row + dy][x0 + dx] = t
        self.platforms.append((x0, x0 + 1, row))
        return self

    # --- entities --------------------------------------------------------------------------
    def entity(self, eid, cx, bottom):
        w, h = E_SIZE[eid]
        self.objects.append((E_FIRSTGID + eid, cx - w / 2, bottom, w, h))

    def spawn(self, col):
        self.deferred.append(lambda: self.entity(E_SPAWN, col * TILE + 32, self.surface(col) * TILE))
        self.reserved.update(range(col - 1, col + 2))
        return self

    def goal(self, col):
        w, _ = E_SIZE[E_GOAL]
        self.deferred.append(lambda: self.objects.append(
            (E_FIRSTGID + E_GOAL, col * TILE + 8, self.surface(col) * TILE, w, 162)))
        self.reserved.update(range(col - 1, col + 2))
        return self

    def enemy(self, col, kind="gnu", row=None):
        """an enemy on the ground at this column (or standing on a platform at row)"""
        self.deferred.append(lambda: self.entity(
            E_ENEMY[kind], col * TILE + 32, (row if row is not None else self.surface(col)) * TILE))
        return self

    def coin(self, col, row):
        w, h = E_SIZE[E_COIN]
        cx, cy = col * TILE + 32, row * TILE + 32
        self.objects.append((E_FIRSTGID + E_COIN, cx - w / 2, cy + h / 2, w, h))
        return self

    def coins(self, col, row, n):
        for i in range(n):
            self.coin(col + i, row)
        return self

    def coin_arc(self, col0, col1, base_row, height=2):
        """coins along a jump arc from col0 to col1"""
        n = col1 - col0 + 1
        for i in range(n):
            t = i / (n - 1) if n > 1 else 0.5
            lift = round(height * 4 * t * (1 - t))
            self.coin(col0 + i, base_row - 1 - lift)
        return self

    def surface(self, col):
        s = self.heights[col]
        if s is None:
            raise ValueError("column %d of %s is a pit" % (col, self.name))
        return s

    # --- finishing -------------------------------------------------------------------------
    def paint_terrain(self):
        for c in range(self.w):
            s = self.heights[c]
            if s is None:
                for r in range(self.h - 2, self.h):
                    self.deco[r][c] = WATER
                continue
            left = self.heights[c - 1] if c > 0 else s
            right = self.heights[c + 1] if c + 1 < self.w else s
            exposed_l = left is None or left > s
            exposed_r = right is None or right > s
            top = GRASS_L if exposed_l and not exposed_r else GRASS_R if exposed_r and not exposed_l else GRASS
            self.ground[s][c] = top
            for r in range(s + 1, self.h):
                self.ground[r][c] = DIRT

    def decorate(self):
        rng = self.rng
        c = 2
        while c < self.w - 3:
            s = self.heights[c]
            free = all(self.heights[k] == s and k not in self.reserved for k in range(c, c + 3) if k < self.w)
            if s is None or not free or s < 4:
                c += 1
                continue
            roll = rng.random()
            if roll < 0.10:                                   # tree
                self.deco[s - 1][c], self.deco[s - 2][c], self.deco[s - 3][c] = TREE_TRUNK, TREE_MID, TREE_TOP
                c += 3
            elif roll < 0.18:                                 # fence
                self.deco[s - 1][c], self.deco[s - 1][c + 1], self.deco[s - 1][c + 2] = FENCE_L, FENCE_M, FENCE_R
                c += 4
            elif roll < 0.26:
                self.deco[s - 1][c] = BUSH
                c += 2
            elif roll < 0.32:
                self.deco[s - 1][c] = FLOWERS
                c += 2
            elif roll < 0.42:
                self.deco[s - 1][c] = rng.choice(GRASS_TUFTS)
                c += 1
            elif roll < 0.46:                                 # cactus
                self.deco[s - 1][c], self.deco[s - 2][c] = CACTUS1, SMALL_CACTUS
                c += 2
            else:
                c += 1

    def backdrop(self):
        grid = self.grid()
        top = self.h - 3 - B_ROWS + 1         # bottom row of the backdrop sits at the usual ground level
        for r in range(B_ROWS):
            for c in range(self.w):
                grid[top + r][c] = B_FIRSTGID + r * B_COLS + (c % B_COLS)
        return grid

    # --- reachability ----------------------------------------------------------------------
    def check(self):
        """walk the main path left to right and check every gap and step is jumpable"""
        problems = []
        c = 0
        while c < self.w:
            if self.heights[c] is not None:
                c += 1
                continue
            start = c
            while c < self.w and self.heights[c] is None:
                c += 1
            if start == 0 or c >= self.w:
                problems.append("pit at the map edge (columns %d-%d)" % (start, c - 1))
                continue
            gap = (c - start) * TILE
            rise = (self.heights[start - 1] - self.heights[c]) * TILE
            # a platform above the pit can be used as a stepping stone
            stepping = any(p[0] <= c - 1 and p[1] >= start for p in self.platforms)
            reach = max_jump_distance(rise) * SAFETY
            if gap + PLAYER_WIDTH > reach and not stepping:
                problems.append("gap of %d tiles at column %d (rise %d) is too wide : needs %.0f px, jump reaches %.0f px"
                                % (c - start, start, rise // TILE, gap + PLAYER_WIDTH, reach))
        for c in range(1, self.w):
            a, b = self.heights[c - 1], self.heights[c]
            if a is not None and b is not None and (a - b) * TILE > JUMP_HEIGHT * 0.9:
                problems.append("step up of %d tiles at column %d is too high" % (a - b, c))
        spawns = sum(1 for o in self.objects if o[0] == E_FIRSTGID + E_SPAWN)
        goals = sum(1 for o in self.objects if o[0] == E_FIRSTGID + E_GOAL)
        if spawns != 1:
            problems.append("needs exactly one spawn, has %d" % spawns)
        if goals < 1:
            problems.append("needs a goal")
        return problems

    # --- output ----------------------------------------------------------------------------
    def write(self, filename, next_level=None):
        for place in self.deferred:
            place()
        self.paint_terrain()
        self.decorate()
        problems = self.check()
        if problems:
            raise SystemExit("level %s is not completable:\n  %s" % (self.name, "\n  ".join(problems)))

        def csv(grid):
            return ",\n".join(",".join(str(v) for v in row) for row in grid)

        props = ['  <property name="name" value="%s"/>' % self.name]
        if next_level:
            props.append('  <property name="next" value="%s"/>' % next_level)

        objs = []
        for i, (gid, x, y, w, h) in enumerate(self.objects):
            objs.append('  <object id="%d" gid="%d" x="%g" y="%g" width="%g" height="%g"/>' % (i + 1, gid, x, y, w, h))

        n_obj = len(self.objects) + 1
        origin_y = self.h * TILE - 400
        xml = f"""<?xml version="1.0" encoding="UTF-8"?>
<map version="1.10" tiledversion="1.12.2" orientation="orthogonal" renderorder="right-down" width="{self.w}" height="{self.h}" tilewidth="{TILE}" tileheight="{TILE}" infinite="0" parallaxoriginx="640" parallaxoriginy="{origin_y}" backgroundcolor="{SKY}" nextlayerid="6" nextobjectid="{n_obj}">
 <properties>
{chr(10).join(props)}
 </properties>
 <tileset firstgid="{T_FIRSTGID}" source="../tilesets/tileset.tsx"/>
 <tileset firstgid="{B_FIRSTGID}" source="../tilesets/backdrop.tsx"/>
 <tileset firstgid="{E_FIRSTGID}" source="../tilesets/entities.tsx"/>
 <layer id="1" name="Backdrop" width="{self.w}" height="{self.h}" parallaxx="0.35">
  <data encoding="csv">
{csv(self.backdrop())}
</data>
 </layer>
 <layer id="2" name="Decoration" width="{self.w}" height="{self.h}">
  <data encoding="csv">
{csv(self.deco)}
</data>
 </layer>
 <layer id="3" name="Ground" width="{self.w}" height="{self.h}">
  <data encoding="csv">
{csv(self.ground)}
</data>
 </layer>
 <objectgroup id="4" name="solids"/>
 <objectgroup id="5" name="entities">
{chr(10).join(objs)}
 </objectgroup>
</map>
"""
        path = os.path.join(LEVELS, filename)
        with open(path, "w", newline="\n") as f:
            f.write(xml)
        coins = sum(1 for o in self.objects if o[0] == E_FIRSTGID + E_COIN)
        enemies = sum(1 for o in self.objects if o[0] - E_FIRSTGID >= 3)
        print("%-16s %3dx%d tiles, %3d coins, %2d enemies -> %s" % (self.name, self.w, self.h, coins, enemies, path))


# --- the levels ----------------------------------------------------------------------------

def level1():
    """Green Hills : gentle introduction, small gaps, a few slow enemies"""
    L = Level("Green Hills", 200, seed=7)
    x = L.flat(14)
    L.spawn(x + 3)
    L.coins(x + 8, L.row - 3, 4)
    x = L.flat(6, dy=1)                    # first little step
    L.enemy(x + 3, "gnu")
    x = L.flat(6, dy=-1)
    L.coin_arc(x + 3, x + 8, L.row, 2)
    L.pit(2)                               # first pit
    x = L.flat(10)
    L.slab(x + 3, 4, L.row - 3)
    L.coins(x + 3, L.row - 4, 4)
    L.enemy(x + 8, "gnu")
    x = L.flat(4, dy=1)                    # stairs up
    x = L.flat(8, dy=1)
    L.coins(x + 2, L.row - 3, 3)
    L.enemy(x + 5, "wilber")
    L.coin_arc(x + 6, x + 11, L.row, 2)
    L.pit(3)                               # a wider pit on the high ground
    x = L.flat(6)
    x = L.flat(12, dy=-2)
    L.floating(x + 4, 4, L.row - 4)
    L.coins(x + 4, L.row - 5, 4)
    L.enemy(x + 2, "gnu")
    L.enemy(x + 9, "tux")
    L.pit(2)                               # two short pits in a row
    L.flat(3)
    L.pit(2)
    x = L.flat(10)
    L.island(x + 4, L.row - 5)
    L.coins(x + 4, L.row - 6, 2)
    L.enemy(x + 7, "wilber")
    x = L.flat(5, dy=1)                    # a hill
    x = L.flat(5, dy=1)
    L.coins(x + 1, L.row - 3, 3)
    x = L.flat(4, dy=-1)
    x = L.flat(14, dy=-1)
    L.enemy(x + 4, "gnu")
    L.enemy(x + 9, "gnu")
    L.coin_arc(x + 11, x + 16, L.row, 2)
    L.pit(3)
    x = L.flat(12)
    L.slab(x + 2, 3, L.row - 3)            # stepping platforms to bonus coins
    L.slab(x + 6, 3, L.row - 5)
    L.coins(x + 6, L.row - 6, 3)
    L.enemy(x + 9, "tux")
    x = L.flat(10, dy=2)
    L.enemy(x + 5, "wilber")
    L.coin_arc(x + 7, x + 12, L.row, 2)
    L.pit(3)
    x = L.flat(6)
    x = L.flat(12, dy=-2)
    L.floating(x + 3, 5, L.row - 4)
    L.coins(x + 3, L.row - 5, 5)
    L.enemy(x + 2, "gnu")
    L.enemy(x + 8, "tux")
    x = L.flat(L.w - L.x)                  # the final stretch
    L.coins(x + 3, L.row - 2, 5)
    L.goal(L.w - 7)
    return L


def level2():
    """Cactus Canyon : wider gaps, more and faster enemies, platforms over the pits"""
    L = Level("Cactus Canyon", 230, seed=11)
    x = L.flat(12)
    L.spawn(x + 3)
    L.enemy(x + 10, "tux")
    L.coin_arc(x + 10, x + 15, L.row, 2)
    L.pit(3)
    L.flat(6)
    L.flat(3, dy=1)
    L.flat(3, dy=1)
    x = L.flat(6, dy=1)
    L.enemy(x + 3, "kisi")
    L.coin_arc(x + 4, x + 10, L.row, 2)
    L.pit(4)                               # 4 tile gap, jumping down a step
    L.flat(5, dy=-1)
    x = L.flat(14, dy=-2)
    L.enemy(x + 3, "gnu")
    L.enemy(x + 7, "tux")
    L.enemy(x + 11, "gnu")
    L.slab(x + 4, 6, L.row - 3)
    L.coins(x + 4, L.row - 4, 6)
    L.pit(2)                               # hopping across small pillars
    x = L.flat(2)
    L.coins(x, L.row - 2, 2)
    L.pit(2)
    x = L.flat(2)
    L.coins(x, L.row - 2, 2)
    L.pit(3)
    x = L.flat(8)
    L.floating(x + 2, 3, L.row - 3)
    L.coins(x + 2, L.row - 4, 3)
    L.enemy(x + 6, "kisi")
    x = L.flat(8, dy=2)
    L.enemy(x + 4, "wilber")
    x = L.pit(5)                           # long pit with an island in the middle
    L.island(x + 2, L.row - 1)
    L.coins(x + 2, L.row - 2, 2)
    x = L.flat(10)
    L.enemy(x + 5, "tux")
    L.enemy(x + 8, "tux")
    x = L.flat(12, dy=-2)
    L.floating(x + 2, 3, L.row - 4)
    L.floating(x + 7, 3, L.row - 6)
    L.coins(x + 7, L.row - 7, 3)
    L.enemy(x + 3, "kisi")
    L.enemy(x + 9, "gnu")
    L.coin_arc(x + 9, x + 14, L.row, 2)
    L.pit(3)
    L.flat(4)
    L.flat(4, dy=2)
    x = L.flat(8, dy=-1)
    L.enemy(x + 3, "wilber")
    L.enemy(x + 6, "kisi")
    L.coin_arc(x + 6, x + 11, L.row, 2)
    L.pit(3)
    x = L.flat(12)
    L.slab(x + 1, 4, L.row - 3)
    L.slab(x + 6, 4, L.row - 5)
    L.coins(x + 6, L.row - 6, 4)
    L.enemy(x + 5, "tux")
    L.enemy(x + 9, "tux")
    L.flat(3, dy=1)
    L.pit(3)
    L.flat(3)
    L.pit(3)
    x = L.flat(8)
    L.enemy(x + 5, "kisi")
    x = L.flat(16, dy=-1)
    L.enemy(x + 3, "gnu")
    L.enemy(x + 8, "wilber")
    L.enemy(x + 12, "tux")
    L.floating(x + 4, 6, L.row - 4)
    L.coins(x + 4, L.row - 5, 6)
    L.coin_arc(x + 13, x + 18, L.row, 2)
    L.pit(3)
    x = L.flat(L.w - L.x)
    L.coins(x + 4, L.row - 2, 6)
    L.enemy(x + 12, "kisi")
    L.goal(L.w - 8)
    return L


def main():
    os.makedirs(LEVELS, exist_ok=True)
    level1().write("level1.tmx", next_level="level2.tmx")
    level2().write("level2.tmx")
    return 0


if __name__ == "__main__":
    sys.exit(main())
