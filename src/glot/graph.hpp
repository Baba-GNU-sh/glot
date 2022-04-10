#pragma once

#include <glm/glm.hpp>
#include <boost/signals2.hpp>
#include "plot.hpp"
#include "shader_utils.hpp"
#include "database.hpp"
#include "timeseries.hpp"
#include "window.hpp"

struct GlyphVertex
{
    glm::vec2 vert;
    glm::vec2 tex_coords;
};

struct GlyphData
{
    GlyphVertex verts[4]; // Order: [TL, TR, BL, BR]
};

enum class LabelAlignment
{
    Left,
    Right,
    Center
};

enum class LabelAlignmentVertical
{
    Top,
    Center,
    Bottom
};

struct TimeSeriesContainer
{
    std::shared_ptr<TimeSeries> ts;
    glm::vec3 colour;
    std::string name;
    bool visible;
    float y_offset;
};

/**
 * @brief Stores and renders a graph with axes and zoom and pan mouse controls.
 */
class Graph
{
  public:
    Graph(Window &window);
    ~Graph();

    /**
     * @brief Update the size of the graph in the viewport.
     *
     * @param width The new width of the graph view in pixels.
     * @param height The new height of the graph view in pixels.
     */
    void set_size(int width, int height);
    void set_size(const glm::ivec2 &size);

    /**
     * @brief Get the size of the graph in the viewport.
     *
     * @return glm::vec2 2D vector containing the size of the graph in pixels.
     */
    glm::ivec2 size() const;

    /**
     * @brief Update the position of the graph in the viewport.
     *
     * @param x The new x position in pixels.
     * @param y The new y position in pixels.
     */
    void set_position(int x, int y);

    /**
     * @brief Get the posiiton of the graph in the viewport.
     *
     * @return glm::vec2 2D vector containing the posiiton of the graph in
     * pixels.
     */
    glm::ivec2 position() const;

    /**
     * @brief Call this to pass mouse button events through from GLFW to control
     * the graph.
     */
    void mouse_button(int button, int action, int mods);

    /**
     * @brief Call this to pass through cursor move events from GLFW to control
     * the graph.
     */
    void cursor_move(double xpos, double ypos);

    /**
     * @brief Call this to pass through mouse scroll events from GLFW to control
     * the graph.
     */
    void mouse_scroll(double xoffset, double yoffset);

    /**
     * @brief Draw the graph to the screen.
     *
     * @param plot_width
     * @param plot_colour
     * @param minmax_colour
     * @param show_plot_segments
     */
    void draw_decorations(const glm::mat3 &view_matrix) const;

    boost::signals2::signal<void(double, double)> on_drag;
    boost::signals2::signal<void(double, double)> on_zoom;

  private:
    /**
     * @brief Check if a coodinate is inside a bounding box defined by two corners.
     *
     * @param value The value to test.
     * @param tl Top-left coordinate of the bounding box.
     * @param br Bottom-right coordinate of the bounding box.
     * @return true The coordinate is within the bounding box.
     * @return false The coordinate is outside the bounding box.
     */
    bool _hittest(glm::vec2 value, glm::vec2 tl, glm::vec2 br) const;

    /**
     * @brief Converts a vector from viewport space (pixels w/ origin at TL) to
     * graph space.
     *
     * @param value The vector to convert.
     * @return glm::vec2 The resultant vector in graph space.
     */
    glm::vec2 screen2graph(const glm::mat3 &view_matrix, const glm::ivec2 &value) const;

    void _init_line_buffers();
    void _init_glyph_buffers();
    void _draw_lines(const glm::mat3 &view_matrix) const;
    void _draw_labels(const glm::mat3 &view_matrix) const;
    void _draw_label(const std::string_view text,
                     const glm::vec2 &pos,
                     float height,
                     float width,
                     LabelAlignment align,
                     LabelAlignmentVertical valign) const;
    void _draw_glyph(
        char c, const glm::vec2 &pos, float height, float width, GlyphData **buf) const;

    std::tuple<glm::vec2, glm::vec2, glm::ivec2> _tick_spacing(const glm::mat3 &view_matrix) const;

    const int GUTTER_SIZE_PX = 60;
    const int TICKLEN = 8;

    Window &m_window;
    glm::ivec2 _position;
    glm::ivec2 _size;
    glm::vec2 _cursor;

    bool _dragging;

    // glm::mat3 _view_matrix; // Transform from graph space to clip space
    // glm::mat3 _view_matrix_inv;

    // Line renderer
    GLuint _linebuf_vao;
    GLuint _linebuf_vbo;
    Program _lines_shader;

    // Glyph renderer
    GLuint _glyphbuf_vao;
    GLuint _glyphbuf_vbo;
    GLuint _glyphbuf_ebo;
    Program _glyph_shader;
    GLuint _glyph_texture;
};
