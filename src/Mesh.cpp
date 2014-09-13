#include <Mesh.h>
#include <NamedObject.h>
#include <Buffer.h>
#include <Material.h>
#include <Texture.h>
#include <Util.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <memory> // std::unique_ptr
#include <iostream>

namespace vt {

Mesh::Mesh(
        std::string name,
        size_t      num_vertex,
        size_t      num_tri)
    : NamedObject(name),
      m_num_vertex(num_vertex),
      m_num_tri(num_tri),
      m_visible(true),
      m_buffers_already_init(false),
      m_material(NULL),
      m_texture_index(-1),
      m_texture2_index(-1),
      m_normal_map_texture_index(-1),
      m_env_map_texture_index(-1),
      m_frontface_depth_overlay_texture_index(-1),
      m_reflect_to_refract_ratio(1) // 100% reflective
{
    m_vert_coords  = new GLfloat[num_vertex*3];
    m_vert_normal  = new GLfloat[num_vertex*3];
    m_vert_tangent = new GLfloat[num_vertex*3];
    m_tex_coords   = new GLfloat[num_vertex*2];
    m_tri_indices  = new GLushort[num_tri*3];
}

Mesh::~Mesh()
{
    if(m_vert_coords) {
        delete []m_vert_coords;
    }
    if(m_vert_normal) {
        delete []m_vert_normal;
    }
    if(m_vert_tangent) {
        delete []m_vert_tangent;
    }
    if(m_tex_coords) {
        delete []m_tex_coords;
    }
    if(m_tri_indices) {
        delete []m_tri_indices;
    }
}

void Mesh::set_origin(glm::vec3 origin)
{
    m_origin = origin;
    set_need_update_xform();
}

void Mesh::set_orient(glm::vec3 orient)
{
    m_orient = orient;
    set_need_update_xform();
}

glm::vec3 Mesh::get_vert_coord(int index)
{
    int offset = index*3;
    return glm::vec3(
            m_vert_coords[offset+0],
            m_vert_coords[offset+1],
            m_vert_coords[offset+2]);
}

void Mesh::set_vert_coord(int index, glm::vec3 coord)
{
    int offset = index*3;
    m_vert_coords[offset+0] = coord.x;
    m_vert_coords[offset+1] = coord.y;
    m_vert_coords[offset+2] = coord.z;
}

glm::vec3 Mesh::get_vert_normal(int index)
{
    int offset = index*3;
    return glm::vec3(
            m_vert_normal[offset+0],
            m_vert_normal[offset+1],
            m_vert_normal[offset+2]);
}

void Mesh::set_vert_normal(int index, glm::vec3 normal)
{
    int offset = index*3;
    m_vert_normal[offset+0] = normal.x;
    m_vert_normal[offset+1] = normal.y;
    m_vert_normal[offset+2] = normal.z;
}

glm::vec3 Mesh::get_vert_tangent(int index)
{
    int offset = index*3;
    return glm::vec3(
            m_vert_tangent[offset+0],
            m_vert_tangent[offset+1],
            m_vert_tangent[offset+2]);
}

void Mesh::set_vert_tangent(int index, glm::vec3 tangent)
{
    int offset = index*3;
    m_vert_tangent[offset+0] = tangent.x;
    m_vert_tangent[offset+1] = tangent.y;
    m_vert_tangent[offset+2] = tangent.z;
}

glm::vec2 Mesh::get_tex_coord(int index)
{
    int offset = index*2;
    return glm::vec2(
            m_tex_coords[offset+0],
            m_tex_coords[offset+1]);
}

void Mesh::set_tex_coord(int index, glm::vec2 coord)
{
    int offset = index*2;
    m_tex_coords[offset+0] = coord.x;
    m_tex_coords[offset+1] = coord.y;
}

glm::uvec3 Mesh::get_tri_indices(int index)
{
    int offset = index*3;
    return glm::uvec3(
            m_tri_indices[offset+0],
            m_tri_indices[offset+1],
            m_tri_indices[offset+2]);
}

void Mesh::set_tri_indices(int index, glm::uvec3 indices)
{
    int offset = index*3;
    m_tri_indices[offset+0] = indices[0];
    m_tri_indices[offset+1] = indices[1];
    m_tri_indices[offset+2] = indices[2];
}

void Mesh::init_buffers()
{
    if(m_buffers_already_init) {
        return;
    }
    m_vbo_vert_coords  = std::unique_ptr<Buffer>(new Buffer(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_num_vertex*3, m_vert_coords));
    m_vbo_vert_normal  = std::unique_ptr<Buffer>(new Buffer(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_num_vertex*3, m_vert_normal));
    m_vbo_vert_tangent = std::unique_ptr<Buffer>(new Buffer(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_num_vertex*3, m_vert_tangent));
    m_vbo_tex_coords   = std::unique_ptr<Buffer>(new Buffer(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_num_vertex*2, m_tex_coords));
    m_ibo_tri_indices  = std::unique_ptr<Buffer>(new Buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*m_num_tri*3, m_tri_indices));
    m_buffers_already_init = true;
}

void Mesh::update_buffers() const
{
    if(!m_buffers_already_init) {
        return;
    }
    m_vbo_vert_coords->update();
    m_vbo_vert_normal->update();
    m_vbo_vert_tangent->update();
    m_vbo_tex_coords->update();
    m_ibo_tri_indices->update();
}

Buffer* Mesh::get_vbo_vert_coords()
{
    init_buffers();
    return m_vbo_vert_coords.get();
}

Buffer* Mesh::get_vbo_vert_normal()
{
    init_buffers();
    return m_vbo_vert_normal.get();
}

Buffer* Mesh::get_vbo_vert_tangent()
{
    init_buffers();
    return m_vbo_vert_tangent.get();
}

Buffer* Mesh::get_vbo_tex_coords()
{
    init_buffers();
    return m_vbo_tex_coords.get();
}

Buffer* Mesh::get_ibo_tri_indices()
{
    init_buffers();
    return m_ibo_tri_indices.get();
}

void Mesh::set_material(Material* material)
{
    // NOTE: texture index for same texture varies from material to material
    if(material == m_material) {
        return;
    }
    std::string texture_name;
    if(m_material) {
        vt::Texture* texture = m_material->get_texture_by_index(m_texture_index);
        if(texture) {
            texture_name = texture->name();
        }
    }
    m_shader_context.reset();
    m_material = material;
    m_texture_index = material ? material->get_texture_index_by_name(texture_name) : -1;
}

ShaderContext* Mesh::get_shader_context()
{
    if(m_shader_context.get() || !m_material) { // FIX-ME! -- potential bug if Material not set
        return m_shader_context.get();
    }
    m_shader_context = std::unique_ptr<ShaderContext>(new ShaderContext(
            m_material,
            get_vbo_vert_coords(),
            get_vbo_vert_normal(),
            get_vbo_vert_tangent(),
            get_vbo_tex_coords(),
            get_ibo_tri_indices()));
    return m_shader_context.get();
}

ShaderContext* Mesh::get_normal_shader_context(Material* normal_material)
{
    if(m_normal_shader_context.get() || !normal_material) { // FIX-ME! -- potential bug if Material not set
        return m_normal_shader_context.get();
    }
    m_normal_shader_context = std::unique_ptr<ShaderContext>(new ShaderContext(
            normal_material,
            get_vbo_vert_coords(),
            get_vbo_vert_normal(),
            get_vbo_vert_tangent(),
            get_vbo_tex_coords(),
            get_ibo_tri_indices()));
    return m_normal_shader_context.get();
}

ShaderContext* Mesh::get_wireframe_shader_context(Material* wireframe_material)
{
    if(m_wireframe_shader_context.get() || !wireframe_material) { // FIX-ME! -- potential bug if Material not set
        return m_wireframe_shader_context.get();
    }
    m_wireframe_shader_context = std::unique_ptr<ShaderContext>(new ShaderContext(
            wireframe_material,
            get_vbo_vert_coords(),
            get_vbo_vert_normal(),
            get_vbo_vert_tangent(),
            get_vbo_tex_coords(),
            get_ibo_tri_indices()));
    return m_wireframe_shader_context.get();
}

void Mesh::update_xform()
{
    glm::mat4 translate_xform = glm::translate(glm::mat4(1), m_origin);
    glm::mat4 rotate_xform =
            glm::rotate(glm::mat4(1), static_cast<float>(ORIENT_PITCH(m_orient)*3), glm::vec3(1, 0, 0)) * // X axis
            glm::rotate(glm::mat4(1), static_cast<float>(ORIENT_YAW(m_orient)*2),   glm::vec3(0, 1, 0)) * // Y axis
            glm::rotate(glm::mat4(1), static_cast<float>(ORIENT_ROLL(m_orient)*4),  glm::vec3(0, 0, 1));  // Z axis
    glm::mat4 scale_xform = glm::scale(glm::mat4(1), m_scale);
    m_xform = translate_xform*rotate_xform*scale_xform;
}

}
