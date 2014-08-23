[![Build Status](https://secure.travis-ci.org/onlyuser/dexvt-test.png)](http://travis-ci.org/onlyuser/dexvt-test)

dexvt-test
==========

Copyright (C) 2011-2013 Jerry Chen <mailto:onlyuser@gmail.com>

About
-----

dexvt-test is a modernized version of my DexVT 3d engine.
It features glsl environment-mapped reflection/refraction/double refraction (with Fresnel effect, chromatic dispersion, and Beer's law), bump mapping, Phong shading, and platonic primitives generation.
Platonic primitives supported include sphere, cube, cylinder, cone, grid, tetrahedron, and round brilliant diamond.
Many features still experimental.

Screenshots
-----------

[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_reflectance_detail_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map_reflectance_detail.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_refractance_detail_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map_refractance_detail.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_fresnel_effect_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map_fresnel_effect.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_chromatic_dispersion_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map_chromatic_dispersion.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_env_map_wireframe_normals_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_env_map_wireframe_normals.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_cube_double_refraction_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_cube_double_refraction.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_diamond_refraction_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_diamond_refraction.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_diamond_specular_reflection_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_diamond_specular_reflection.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_nine_shape_ground_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_nine_shape_ground.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_nine_shape_horizon_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_nine_shape_horizon.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_nine_shape_sky_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_nine_shape_sky.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_sphere_backface_reflection_halo_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_sphere_backface_reflection_halo.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_sphere_specular_reflection_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_sphere_specular_reflection.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_ripple_refraction_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_ripple_refraction.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_ripple_wireframe_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_ripple_wireframe.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_ripple_wireframe_normals_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_ripple_wireframe_normals.png)

Requirements
------------

Unix tools and 3rd party components (accessible from $PATH):

    gcc (with -std=c++0x support)

Texture files not included:

<table>
    <tr>
        <th>texture files</th>
        <th>purpose</th>
    </tr>
    <tr>
        <td>
            data/SaintPetersSquare2/posx.png<br>
            data/SaintPetersSquare2/negx.png<br>
            data/SaintPetersSquare2/posy.png<br>
            data/SaintPetersSquare2/negy.png<br>
            data/SaintPetersSquare2/posz.png<br>
            data/SaintPetersSquare2/negz.png
        </td>
        <td>Cube map texture (6 faces)</td>
    </tr>
    <tr>
        <td>data/chesterfield_color.png</td>
        <td>Color map texture</td>
    </tr>
    <tr>
        <td>data/chesterfield_normal.png</td>
        <td>Normal map texture</td>
    </tr>
</table>

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
    <dt>"Setting up an OpenGL development environment in Ubuntu Linux"</dt>
    <dd>http://www.codeproject.com/Articles/182109/Setting-up-an-OpenGL-development-environment-in-Ub</dd>

    <dt>"OpenGL Programming/Intermediate/Textures - A simple libpng example"</dt>
    <dd>http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures#A_simple_libpng_example</dd>

    <dt>"Cube Maps: Sky Boxes and Environment Mapping"</dt>
    <dd>http://antongerdelan.net/opengl/cubemaps.html</dd>

    <dt>"3D C/C++ tutorials - OpenGL - GLSL cube mapping"</dt>
    <dd>http://www.belanecbn.sk/3dtutorials/index.php?id=24</dd>

    <dt>"Skyboxes using glsl Version 330"</dt>
    <dd>http://gamedev.stackexchange.com/questions/60313/skyboxes-using-glsl-version-330</dd>

    <dt>"Rioki's Corner - GLSL Skybox"</dt>
    <dd>http://www.rioki.org/2013/03/07/glsl-skybox.html</dd>

    <dt>"How would you implement chromatic aberration?"</dt>
    <dd>http://gamedev.stackexchange.com/questions/58408/how-would-you-implement-chromatic-aberration</dd>

    <dt>"How do you calculate the angle between two normals in glsl?"</dt>
    <dd>http://stackoverflow.com/questions/338762/how-do-you-calculate-the-angle-between-two-normals-in-glsl</dd>

    <dt>"Chapter 7. Environment Mapping Techniques"</dt>
    <dd>http://http.developer.nvidia.com/CgTutorial/cg_tutorial_chapter07.html</dd>

    <dt>"Humus Cube Map Textures - Colosseum"</dt>
    <dd>http://www.humus.name/index.php?page=Textures</dd>

    <dt>"Kay's Blog Texture and game development freebies! - Well Preserved Chesterfield"</dt>
    <dd>http://kay-vriend.blogspot.tw/2012/11/well-preserved-chesterfield.html</dd>
</dl>

Keywords
--------

    OpenGL, glsl shader, glm, environment map, cube map, normal map, bitangent, tbn matrix, backface refraction, double refraction, second refraction, backface sampling, Fresnel effect, chromatic dispersion, chromatic aberration, Beer's law, Snell's law, Newton's method, round brilliant diamond rendering, gemstone rendering
