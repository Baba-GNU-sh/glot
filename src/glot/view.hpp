#pragma once

// TODO: Remove this dependency
#include <cstddef>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "window.hpp"
#include <algorithm>
#include <vector>
#include "graph_utils.hpp"

struct HitBox
{
    glm::dvec2 tl;
    glm::dvec2 br;
};

struct View
{
    virtual ~View() = default;
    virtual void draw(const Window &window)
    {
        std::for_each(
            m_views.begin(), m_views.end(), [&window](View *view) { view->draw(window); });
    }

    virtual void on_scroll(Window &window, double xoffset, double yoffset)
    {
        std::for_each(m_views.begin(), m_views.end(), [&window, xoffset, yoffset](auto *view) {
            if (GraphUtils::hit_test(
                    window.cursor(), view->position(), view->position() + view->size()))
            {
                view->on_scroll(window, xoffset, yoffset);
            }
        });
    }

    virtual void on_mouse_button(const glm::dvec2 &cursor_pos, int button, int action, int mods)
    {
        // This logic gets a bit tricky
        // If a view is clicked, then the cursor moved outside of the view before the view is
        // released, we still want to send the release event to the view. How do we handle multiple
        // simultaneous button presses?
        if (action == GLFW_PRESS)
        {
            // Search views backward, which is the opposite of the render order
            for (auto iter = m_views.rbegin(); iter != m_views.rend(); iter++)
            {
                auto *view = *iter;
                const auto hitbox = view->get_hitbox();
                if (GraphUtils::hit_test(cursor_pos, hitbox.tl, hitbox.br))
                {
                    view->on_mouse_button(cursor_pos, button, action, mods);
                    m_sticky_view = view;
                    break;
                }
            }
        }
        else if (action == GLFW_RELEASE)
        {
            if (m_sticky_view)
            {
                m_sticky_view->on_mouse_button(cursor_pos, button, action, mods);
                m_sticky_view = nullptr;
            }
            else
            {
                std::for_each(m_views.begin(),
                              m_views.end(),
                              [&cursor_pos, button, action, mods](auto *view) {
                                  if (GraphUtils::hit_test(cursor_pos,
                                                           view->position(),
                                                           view->position() + view->size()))
                                  {
                                      view->on_mouse_button(cursor_pos, button, action, mods);
                                  }
                              });
            }
        }
    }

    virtual void on_cursor_move(Window &window, double xpos, double ypos)
    {
        std::for_each(m_views.begin(), m_views.end(), [&window, xpos, ypos](auto *view) {
            view->on_cursor_move(window, xpos, ypos);
        });
    }

    virtual void on_key(Window &window, int key, int scancode, int action, int mods)
    {
        std::for_each(
            m_views.begin(), m_views.end(), [&window, key, scancode, action, mods](auto *view) {
                view->on_key(window, key, scancode, action, mods);
            });
    }

    virtual void on_resize(int width, int height)
    {
        std::for_each(m_views.begin(), m_views.end(), [width, height](auto *view) {
            view->on_resize(width, height);
        });
    }

    virtual glm::dvec2 position() const
    {
        return glm::dvec2(0.0);
    }

    virtual void set_position(const glm::dvec2 &)
    {
        //
    }

    virtual glm::dvec2 size() const
    {
        return glm::dvec2(0.0);
    }

    virtual void set_size(const glm::dvec2 &)
    {
        //
    }

    virtual HitBox get_hitbox() const
    {
        return HitBox{position(), position() + size()};
    }

    void add_view(View *view)
    {
        m_views.push_back(view);
    }

    std::vector<View *> m_views;
    View *m_sticky_view = nullptr;
};
