#include "ui/ui_geometry.hpp"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 build_ui_quad_model(float x, float y, float width, float height){
    glm::mat4 model = glm::translate(glm::mat4(1.0f),
        glm::vec3(x + width / 2.0f, y + height / 2.0f, 0.0f)
    );
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    return model;
}

glm::mat4 build_ui_quad_model(const TextBlockLayout& block){
    return build_ui_quad_model(block.box_x, block.box_y, block.box_width, block.box_height);
}

glm::mat4 build_ui_quad_model(const UIBox& box){
    return build_ui_quad_model(box.x, box.y, box.width, box.height);
}

void draw_button(UIContext& ctx, UIPage page, const ButtonSpec& spec){
    auto [x,y,w,h] = spec.box;
    glm::mat4 button_model = build_ui_quad_model(spec.box);

    Color box_color = spec.box_color.value_or(
        spec.enabled ? Colors::WhiteHalfOpaque : Colors::WhiteQuarterOpaque
    );
    Color label_color = spec.label_color.value_or(
        spec.enabled ? Colors::White : Colors::WhiteHalfOpaque
    );

    ctx.shader.setVec4("color", box_color);
    ctx.quad_renderer.draw(ctx.shader, ctx.ui_projection, glm::mat4(1.0f), button_model);

    if (spec.label_texture){
        spec.label_texture->bind(0);
        ctx.textured_shader.setInt("Texture", 0);
        ctx.textured_shader.setVec4("tintColor", label_color);
        ctx.textured_quad_renderer.draw(
            ctx.textured_shader, ctx.ui_projection, glm::mat4(1.0f), button_model
        );
    } else {
        glm::vec2 label_pos = ctx.regular_text_renderer.center_text_in_box(
            spec.label, spec.scale, x + w * 0.5f, y + h * 0.5f
        );
        TextLine label(spec.label, label_pos, spec.scale, label_color);
        ctx.regular_text_renderer.render_text(label);
    }

    if (spec.enabled){
        ctx.input_manager.set_button_box(page, spec.action, spec.box);
    } else {
        ctx.input_manager.remove_button_box(page, spec.action);
    }
}