#ifndef RENDERING_FUNCTIONS_H
#define RENDERING_FUNCTIONS_H

#include "FundamentalTypes.h"
#include <algorithm>

void draw_rectangle(GUIContext& ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color){
    if (x1 > x2 || y1 > y2) return;
    
    uint32_t height = ctx.screen_height;
    uint32_t width  = ctx.screen_width;

    uint32_t lower_bound_x = std::max(x1, ctx.clip_rect.x1);
    uint32_t lower_bound_y = std::max(y1, ctx.clip_rect.y1);
    uint32_t upper_bound_x = std::min(x2, std::min(width  - 1, ctx.clip_rect.x2));
    uint32_t upper_bound_y = std::min(y2, std::min(height - 1, ctx.clip_rect.y2));
    
    if (lower_bound_x > upper_bound_x || lower_bound_y > upper_bound_y) return;

    uint64_t index;
    for (uint32_t i = lower_bound_x; i <= upper_bound_x; i++){
        for (uint32_t j = lower_bound_y; j <= upper_bound_y; j++){
            index = i + j * width;
            ctx.pixels[index] = color;
        }
    }
}

void draw_glyph(GUIContext ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const bool glyph[7][7], uint32_t color){
    double scale_x = (double)(x2 - x1 + 1) / 7.0;
    double scale_y = (double)(y2 - y1 + 1) / 7.0;

    uint32_t rect_x1;
    uint32_t rect_y1;
    uint32_t rect_x2;
    uint32_t rect_y2;
    uint32_t next_x;
    uint32_t next_y;
    if ((scale_x > 0) && (scale_y > 0)){
        for (int row = 0; row < 7; row++){
            for (int col = 0; col < 7; col++){
                if (glyph[row][col]){
                    rect_x1 = x1 + static_cast<uint32_t>(col * scale_x);
                    rect_y1 = y1 + static_cast<uint32_t>(row * scale_y);

                    next_x = x1 + static_cast<uint32_t>((col + 1) * scale_x);
                    next_y = y1 + static_cast<uint32_t>((row + 1) * scale_y);

                    rect_x2 = (next_x > rect_x1) ? (next_x - 1) : rect_x1;
                    rect_y2 = (next_y > rect_y1) ? (next_y - 1) : rect_y1;

                    rect_x2 = std::min(rect_x2, x2);
                    rect_y2 = std::min(rect_y2, y2);
                    
                    draw_rectangle(ctx, rect_x1, rect_y1, rect_x2, rect_y2, color);
                }
            }
        }
    }
}

void draw_high_res_glyph(GUIContext ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const bool glyph[16][16], uint32_t color){
    double scale_x = (double)(x2 - x1 + 1) / 16.0;
    double scale_y = (double)(y2 - y1 + 1) / 16.0;

    uint32_t rect_x1;
    uint32_t rect_y1;
    uint32_t rect_x2;
    uint32_t rect_y2;
    uint32_t next_x;
    uint32_t next_y;
    if ((scale_x >= 1) && (scale_y >= 1)){
        for (int row = 0; row < 16; row++){
            for (int col = 0; col < 16; col++){
                if (glyph[row][col]){
                    // rect_x1 = x1 + (col * scale_x);
                    // rect_y1 = y1 + (row * scale_y);
                    // rect_x2 = rect_x1 + scale_x;
                    // rect_y2 = rect_y1 + scale_y;

                    // draw_rectangle(ctx, rect_x1, rect_y1, rect_x2, rect_y2, color);
                    rect_x1 = x1 + static_cast<uint32_t>(col * scale_x);
                    rect_y1 = y1 + static_cast<uint32_t>(row * scale_y);
                    
                    next_x = x1 + static_cast<uint32_t>((col + 1) * scale_x);
                    next_y = y1 + static_cast<uint32_t>((row + 1) * scale_y);

                    rect_x2 = (next_x > rect_x1) ? (next_x - 1) : rect_x1;
                    rect_y2 = (next_y > rect_y1) ? (next_y - 1) : rect_y1;

                    rect_x2 = std::min(rect_x2, x2);
                    rect_y2 = std::min(rect_y2, y2);

                    draw_rectangle(ctx, rect_x1, rect_y1, rect_x2, rect_y2, color);
                }
            }
        }
    }
}

uint32_t blend_color(uint32_t color1, uint32_t color2, double blend_factor){
    blend_factor = std::min(1.0, std::max(0.0, blend_factor));
    uint32_t result_color = 0xFF000000;

    result_color += (uint32_t)(((color1 >> 0) & 255)  + (int32_t)(((color2 >> 0) & 255)  - ((color1 >> 0) & 255)) *  blend_factor) <<  0;
    result_color += (uint32_t)(((color1 >> 8) & 255)  + (int32_t)(((color2 >> 8) & 255)  - ((color1 >> 8) & 255)) *  blend_factor) <<  8;
    result_color += (uint32_t)(((color1 >> 16) & 255) + (int32_t)(((color2 >> 16) & 255) - ((color1 >> 16) & 255)) * blend_factor) << 16;

    return result_color;
}

void draw_image_nearest_neighbor(GUIContext ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, ImageData& image_data){
    double ratio_x = static_cast<double>(image_data.width) / static_cast<double>(x2 - x1 + 1);
    double ratio_y = static_cast<double>(image_data.height) / static_cast<double>(y2 - y1 + 1);
    
    uint32_t width = ctx.screen_width;
    uint32_t height = ctx.screen_height;
    
    uint32_t lower_bound_x = std::max(x1, ctx.clip_rect.x1);
    uint32_t lower_bound_y = std::max(y1, ctx.clip_rect.y1);
    uint32_t upper_bound_x = std::min(x2, std::min(width  - 1, ctx.clip_rect.x2));
    uint32_t upper_bound_y = std::min(y2, std::min(height - 1, ctx.clip_rect.y2));
    
    uint64_t index;
    uint32_t src_x;
    uint32_t src_y;
    uint64_t image_index;
    for (uint32_t i = lower_bound_x; i <= upper_bound_x; i++){
        for (uint32_t j = lower_bound_y; j <= upper_bound_y; j++){
            index = i + j * width;
            src_x = static_cast<uint32_t>((i - x1) * ratio_x);
            src_y = static_cast<uint32_t>((j - y1) * ratio_y);
            image_index = static_cast<uint64_t>(src_x + static_cast<uint64_t>(src_y) * image_data.width);
            ctx.pixels[index] = image_data.data[image_index];
        }
    }
}

void draw_image_bilinear_interpolation(GUIContext ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, ImageData& image_data){
    double ratio_x = static_cast<double>(image_data.width) / static_cast<double>(x2 - x1 + 1);
    double ratio_y = static_cast<double>(image_data.height) / static_cast<double>(y2 - y1 + 1);
    
    uint32_t width = ctx.screen_width;
    uint32_t height = ctx.screen_height;
    
    uint32_t lower_bound_x = std::max(x1, ctx.clip_rect.x1);
    uint32_t lower_bound_y = std::max(y1, ctx.clip_rect.y1);
    uint32_t upper_bound_x = std::min(x2, std::min(width  - 1, ctx.clip_rect.x2));
    uint32_t upper_bound_y = std::min(y2, std::min(height - 1, ctx.clip_rect.y2));

    uint32_t last_image_width_index = image_data.width - 1;
    uint32_t last_image_height_index = image_data.height - 1;

    uint64_t index;
    double gx, gy;
    uint32_t gxi, gyi;
    double dx, dy;
    uint32_t X1, X2, Y1, Y2;
    uint32_t c_TL, c_TR, c_BL, c_BR;
    uint32_t top_row, bottom_row, final_color;
    for (uint32_t i = lower_bound_x; i <= upper_bound_x; i++){
        for (uint32_t j = lower_bound_y; j <= upper_bound_y; j++){
            gx = i * ratio_x;
            gy = j * ratio_y;

            gxi = static_cast<uint32_t>(gx);
            gyi = static_cast<uint32_t>(gy);

            dx = gx - gxi;
            dy = gy - gyi;

            X1 = gxi;
            Y1 = gyi;
            X2 = std::min(gxi + 1, last_image_width_index);
            Y2 = std::min(gyi + 1, last_image_height_index);

            c_TL = image_data.data[Y1 * image_data.width + X1];
            c_TR = image_data.data[Y1 * image_data.width + X2];
            c_BL = image_data.data[Y2 * image_data.width + X1];
            c_BR = image_data.data[Y2 * image_data.width + X2];

            top_row = blend_color(c_TL, c_TR, dx);
            bottom_row = blend_color(c_BL, c_BR, dx);
            final_color = blend_color(top_row, bottom_row, dy);

            index = i + j * width;
            ctx.pixels[index] = final_color;
        }
    }
}

// Same as nearest neighbor
void draw_image(GUIContext ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, ImageData& image_data){
    draw_image_nearest_neighbor(ctx, x1, y1, x2, y2, image_data);
}

#endif