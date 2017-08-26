[![Build Status](https://secure.travis-ci.org/onlyuser/dexvt-test.png)](http://travis-ci.org/onlyuser/dexvt-test)

dexvt-test
==========

Copyright (C) 2011-2014 Jerry Chen <mailto:onlyuser@gmail.com>

About
-----

dexvt-test is a c++/glsl/glm-based 3d engine.
It features environment-mapped reflection/refraction/double refraction (with Fresnel effect, chromatic dispersion, and Beer's law), screen space ambient occlusion, bump mapping, Phong shading, Bloom filter, 3ds mesh import, hierarchy linkages, [NDOF 3D Inverse Kinematics solver (CCD)](https://github.com/onlyuser/dexvt-test/blob/master/src/XformObject.cpp#L211), and platonic primitives generation.
Platonic primitives supported include sphere, cube, cylinder, cone, grid, tetrahedron, and round brilliant diamond.
Many features still experimental.

Screenshots
-----------

[//]: # "[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_thumb.gif?attredirects=0)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test.gif?attredirects=0)"
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
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_nine_shape_bloom_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_nine_shape_bloom.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_cube_bloom_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_cube_bloom.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_ssao_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_ssao.png)
[![Screenshot](https://sites.google.com/site/onlyuser/projects/graphics/thumbs/dexvt-test_ssao_angle_thumb.png)](https://sites.google.com/site/onlyuser/projects/graphics/images/dexvt-test_ssao_angle.png)

Requirements
------------

Unix tools and 3rd party components (accessible from $PATH):

    gcc mesa-common-dev freeglut3-dev libglew-dev libglm-dev libpng-dev curl imagemagick

Resource files not included:

<table>
    <tr>
        <th>resource files</th>
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
    <tr>
        <td>data/star_wars/TI_Low0.3ds</td>
        <td>3D Studio mesh</td>
    </tr>
</table>

Make targets
------------

<table>
    <tr><th> target          </th><th> action                        </th></tr>
    <tr><td> all             </td><td> make binaries                 </td></tr>
    <tr><td> test            </td><td> all + run tests               </td></tr>
    <tr><td> clean           </td><td> remove all intermediate files </td></tr>
    <tr><td> resources       </td><td> download resource files       </td></tr>
    <tr><td> clean_resources </td><td> remove resource files         </td></tr>
</table>

Controls
--------

Keyboard:

<table>
    <tr><th> key         </th><th> purpose                 </th></tr>
    <tr><td> b           </td><td> toggle bounding-box     </td></tr>
    <tr><td> f           </td><td> toggle frame rate       </td></tr>
    <tr><td> g           </td><td> toggle guide wires      </td></tr>
    <tr><td> h           </td><td> toggle HUD              </td></tr>
    <tr><td> l           </td><td> toggle lights           </td></tr>
    <tr><td> n           </td><td> toggle normals          </td></tr>
    <tr><td> p           </td><td> toggle ortho-projection </td></tr>
    <tr><td> t           </td><td> toggle texture          </td></tr>
    <tr><td> w           </td><td> toggle wireframe        </td></tr>
    <tr><td> x           </td><td> toggle axis             </td></tr>
    <tr><td> z           </td><td> toggle labels           </td></tr>
    <tr><td> up/down     </td><td> pitch                   </td></tr>
    <tr><td> left/right  </td><td> yaw                     </td></tr>
    <tr><td> pg-up/pg-dn </td><td> roll                    </td></tr>
    <tr><td> home        </td><td> toggle target           </td></tr>
    <tr><td> esc         </td><td> exit                    </td></tr>
</table>

Mouse:

<table>
    <tr><th> mouse-button + drag </th><th> purpose </th></tr>
    <tr><td> left                </td><td> orbit   </td></tr>
    <tr><td> right               </td><td> zoom    </td></tr>
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
    <dt>"Philip Rideout's OpenGL Bloom Tutorial"</dt>
    <dd>http://prideout.net/archive/bloom/</dd>
    <dt>"An investigation of fast real-time GPU-based image blur algorithms"</dt>
    <dd>https://software.intel.com/en-us/blogs/2014/07/15/an-investigation-of-fast-real-time-gpu-based-image-blur-algorithms</dd>
    <dt>"john-chapman-graphics SSAO Tutorial"</dt>
    <dd>http://john-chapman-graphics.blogspot.tw/2013/01/ssao-tutorial.html</dd>
    <dt>"Know your SSAO artifacts"</dt>
    <dd>http://mtnphil.wordpress.com/2013/06/26/know-your-ssao-artifacts/</dd>
    <dt>"songho.ca OpenGL Projection Matrix"</dt>
    <dd>http://www.songho.ca/opengl/gl_projectionmatrix.html</dd>
    <dt>"3D MeshWorks"</dt>
    <dd>http://www.jrbassett.com</dd>
    <dt>"Missing gtc/constants.hpp #12"</dt>
    <dd>https://github.com/g-truc/glm/issues/12</dd>
    <dt>"Kinematics (Advanced Methods in Computer Graphics) Part 4"</dt>
    <dd>http://what-when-how.com/advanced-methods-in-computer-graphics/kinematics-advanced-methods-in-computer-graphics-part-4/</dd>
    <dt>"alfanick's inverse-kinematics repo (ccd.cpp)"</dt>
    <dd>https://github.com/alfanick/inverse-kinematics/blob/master/ccd.cpp</dd>
    <dt>"Best Inverse kinematics algorithm with constraints on joint angles"</dt>
    <dd>https://stackoverflow.com/questions/21373012/best-inverse-kinematics-algorithm-with-constraints-on-joint-angles</dd>
</dl>

Keywords
--------

    OpenGL, glsl shader, glm, environment map, cube map, normal map, bitangent, tbn matrix, ssao, screen space ambient occlusion, backface refraction, double refraction, second refraction, backface sampling, Fresnel effect, chromatic dispersion, chromatic aberration, Bloom filter, Beer's law, Snell's law, Newton's method, 3ds file import, round brilliant diamond rendering, gemstone rendering, hierarchy linkages, NDOF 3D Inverse Kinematics solver (Cyclic Coordinate Descent)
