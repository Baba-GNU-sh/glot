#include <glad/glad.h>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <stb_image/stb_image.h>

#include "marker.hpp"
#include "label.hpp"
#include <database/timeseries.hpp>
#include "resources.hpp"

Marker::Marker(Window &window)
    : m_window(window), m_handle("marker_center.png"), m_font("proggy_clean.png"), m_label(m_font)
{
    glGenBuffers(1, &m_line_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_line_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(glm::ivec2), nullptr, GL_STREAM_DRAW);

    glGenVertexArrays(1, &m_line_vao);
    glBindVertexArray(m_line_vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
    glEnableVertexAttribArray(0);

    std::vector<Shader> line_shaders{
        Shader(Resources::find_shader("block/vertex.glsl"), GL_VERTEX_SHADER),
        Shader(Resources::find_shader("block/fragment.glsl"), GL_FRAGMENT_SHADER)};
    m_line_shader = Program(line_shaders);

    m_handle.set_alignment(Sprite::AlignmentHorizontal::Center);
    m_handle.set_alignment(Sprite::AlignmentVertical::Top);
    m_label.set_alignment(Label::AlignmentHorizontal::Center);
    m_label.set_alignment(Label::AlignmentVertical::Top);
}

Marker::~Marker()
{
    glDeleteBuffers(1, &m_line_vertex_buffer);
    glDeleteVertexArrays(1, &m_line_vao);
}

void Marker::set_x_position(double position)
{
    m_position = position;
    update_layout();
    m_handle.set_position(m_position + glm::ivec2(0, m_height));
    m_label.set_position(m_position + glm::ivec2(0, m_height + 16));
}

void Marker::set_colour(const glm::vec3 &colour)
{
    m_colour = colour;
    m_handle.set_tint(colour);
    m_label.set_colour(colour);
}

void Marker::set_height(int height)
{
    m_height = height;
    update_layout();
    m_handle.set_position(m_position + glm::ivec2(0, m_height));
    m_label.set_position(m_position + glm::ivec2(0, m_height + 16));
}

void Marker::set_label_text(const std::string &text)
{
    m_label.set_text(text);
}

void Marker::draw(const Window &) const
{
    m_handle.draw(m_window);
    m_label.draw(m_window);

    // Align to the nearest half-pixel
    glm::vec2 position_float = m_position;
    position_float.x = round(position_float.x + 0.5) - 0.5;

    // Draw the vertical line
    const auto vp_matrix_inv = glm::mat3(m_window.viewport_transform().matrix_inverse());
    m_line_shader.use();
    int uniform_id = m_line_shader.uniform_location("view_matrix");
    glUniformMatrix3fv(uniform_id, 1, GL_FALSE, glm::value_ptr(vp_matrix_inv[0]));

    uniform_id = m_line_shader.uniform_location("colour");
    glUniform3fv(uniform_id, 1, &m_colour[0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_line_vertex_buffer);

    glm::vec2 line_verticies[2];
    line_verticies[0] = position_float;
    line_verticies[1] = position_float + glm::vec2(0, m_height);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(line_verticies), line_verticies);

    glBindVertexArray(m_line_vao);
    glDrawArrays(GL_LINES, 0, 2);
}

void Marker::update_layout()
{
}
