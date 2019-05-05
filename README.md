NoMi - a little SFML game for educating myself and hopefully someday my kids too ;-)
(c) 2019 nib

# Credits

This project is inspired by:
* The SFML Game Development book, see: https://github.com/SFML/SFML-Game-Development-Book/
* https://github.com/MORTAL2000/Simple-Super-Mario-
* https://www.youtube.com/watch?v=Aa8bXSq5LDE&list=PL21OsoBLPpMOO6zyVlxZ4S4hwkY_SLRW9&index=10
* The tmxlite SFML loader : https://github.com/fallahn/tmxlite/blob/master/SFMLExample/src/SFMLOrthogonalLayer.hpp 

# Map generation


* tiled maps : http://doc.mapeditor.org/en/stable/
* tmx format : http://doc.mapeditor.org/en/stable/reference/tmx-map-format/#


# Compilation
* set the CMAKE_TOOLCHAIN_FILE to vcpkg folder, neds boost, sfml, eigen and opengl

# Collision detection
- Vertex arrays for entities : https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php
- created tiled object layer & invisible entities from that
- detect collisions :
* https://www.metanetsoftware.com/technique/tutorialA.html
* https://www.randygaul.net/2013/03/28/custom-physics-engine-part-2-manifold-generation/
* example of tiled + 2d scroller : https://github.com/MORTAL2000/Simple-Super-Mario-
* make use of Box2d ??
