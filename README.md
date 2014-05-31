[![Build Status](https://secure.travis-ci.org/onlyuser/dexvt-test.png)](http://travis-ci.org/onlyuser/dexvt-test)

NatLang
=======

Copyright (C) 2011-2013 Jerry Chen <mailto:onlyuser@gmail.com>

About
-----

A Motivating Example
--------------------

<table>
    <tr>
    <td>[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map.png)</td>
    <td>[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_reflectance_detail_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map_reflectance_detail.png)</td>
    <td>[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_refractance_detail_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map_refractance_detail.png)</td>
    </tr>
</table>

Requirements
------------

Unix tools and 3rd party components (accessible from $PATH):

    gcc (with -std=c++0x support)

Texture files not included:

    data/Colosseum/posx.png
    data/Colosseum/negx.png
    data/Colosseum/posy.png
    data/Colosseum/negy.png
    data/Colosseum/posz.png
    data/Colosseum/negz.png
    data/chesterfield_color.png
    data/chesterfield_normal.png

Make targets
------------

<table>
    <tr><th> target </th><th> action                        </th></tr>
    <tr><td> all    </td><td> make binaries                 </td></tr>
    <tr><td> test   </td><td> all + run tests               </td></tr>
    <tr><td> clean  </td><td> remove all intermediate files </td></tr>
</table>

References
----------

<dl>
    <dt>"Cube Maps: Sky Boxes and Environment Mapping"</dt>
    <dd>http://antongerdelan.net/opengl/cubemaps.html</dd>

    <dt>"Humus Cube Map Textures - Colosseum"</dt>
    <dd>http://www.humus.name/index.php?page=Textures</dd>

    <dt>"Kay's Blog Texture and game development freebies! - Well Preserved Chesterfield"</dt>
    <dd>http://kay-vriend.blogspot.tw/2012/11/well-preserved-chesterfield.html</dd>
</dl>

Keywords
--------

    OpenGL, glsl, shader, normal map, environment map, cube map, reflectance, refractance
