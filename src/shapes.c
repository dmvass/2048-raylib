#include <math.h>  // Required for: sinf(), cosf()
#include "shapes.h"

//-------------------------------------------------------------------------------------------------
// Variables Definition 
//-------------------------------------------------------------------------------------------------
static Texture2D texShapes = {0};
static Rectangle recTexShapes = {0};

//-------------------------------------------------------------------------------------------------
// Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static Texture2D GetShapesTexture(void);
static void DrawCirclePro(Vector2 center, float radius, Color color,
                          int startAngleRadians, int endAngleRadians);

//-------------------------------------------------------------------------------------------------
// Functions Definition
//-------------------------------------------------------------------------------------------------
void DrawRoundedRectangleRec(Rectangle rec, float radius, Color color)
{
    Rectangle recMiddle, recLeft, recRight;

    /* Make sure the rectangle is at least as wide and tall as
     * the rounded corners.
     */
    if (rec.width < (radius * 2)) 
        rec.width = radius * 2;

    if (rec.height < (radius * 2)) 
        rec.height = radius * 2;

    recMiddle.x = rec.x + radius;
    recMiddle.y = rec.y; 
    recMiddle.width = rec.width - radius * 2;
    recMiddle.height = rec.height;

    /* Left rectangle */
    recLeft.x = rec.x;
    recLeft.y = rec.y + radius; 
    recLeft.width = recMiddle.x - recLeft.x;
    recLeft.height = rec.height - radius * 2;

    /* Right rectangle */
    recRight.x = recMiddle.x + recMiddle.width;
    recRight.y = recLeft.y; 
    recRight.width = recLeft.width;
    recRight.height = recLeft.height; 

    DrawCirclePro((Vector2){recMiddle.x, recLeft.y}, radius, color, 180, 270);                    // Draw Left-Up corner
    DrawCirclePro((Vector2){recMiddle.x, recLeft.y + recLeft.height}, radius, color, 270, 360);   // Draw Left-Down corner
    DrawCirclePro((Vector2){recMiddle.x + recMiddle.width, recRight.y}, radius, color, 90, 180);  // Draw Right-Up corner
    DrawCirclePro((Vector2){recMiddle.x + recMiddle.width,
                            recRight.y + recRight.height}, radius, color, 0, 90);                 // Draw Right-Down corner
    DrawRectangleRec(recMiddle, color);  // Draw vertical rectangle
    DrawRectangleRec(recLeft, color);    // Draw horizontal rectangle
    DrawRectangleRec(recRight, color);   // Draw horizontal rectangle
}

//-------------------------------------------------------------------------------------------------
// Local Functions Definition
//-------------------------------------------------------------------------------------------------

// Get texture to draw shapes (RAII)
static Texture2D GetShapesTexture(void)
{
    if (texShapes.id <= 0)
    {
#if defined(SUPPORT_FONT_TEXTURE)
        /* Use font texture white character */
        texShapes = GetFontDefault().texture;
        Rectangle rec = GetFontDefault().chars[95].rec;
        /* NOTE: We setup a 1px padding on char rectangle to avoid
         *       texture bleeding on MSAA filtering
         */
        recTexShapes = (Rectangle){ rec.x + 1, rec.y + 1, rec.width - 2, rec.height - 2 };
#else
        texShapes = GetTextureDefault();  // Use default white texture
        recTexShapes = { 0.0f, 0.0f, 1.0f, 1.0f };
#endif
    }

    return texShapes;
}

// Draw a color-filled circle (Vector version)
// NOTE: On OpenGL 3.3 and ES2 we use QUADS to avoid drawing order issues (view rlglDraw)
static void DrawCirclePro(Vector2 center, float radius, Color color,
                          int startAngleRadians, int endAngleRadians)
{
#if defined(SUPPORT_QUADS_DRAW_MODE)
    if (rlCheckBufferLimit(RL_QUADS, 4*(36/2))) rlglDraw();

    rlEnableTexture(GetShapesTexture().id);

    rlBegin(RL_QUADS);
        for (int i = startAngleRadians; i <= endAngleRadians; i += 20)
        {
            rlColor4ub(color.r, color.g, color.b, color.a);

            rlTexCoord2f(recTexShapes.x/texShapes.width, recTexShapes.y/texShapes.height);
            rlVertex2f(center.x, center.y);

            rlTexCoord2f(recTexShapes.x/texShapes.width, (recTexShapes.y + recTexShapes.height)/texShapes.height);
            rlVertex2f(center.x + sinf(DEG2RAD*i)*radius, center.y + cosf(DEG2RAD*i)*radius);

            rlTexCoord2f((recTexShapes.x + recTexShapes.width)/texShapes.width, (recTexShapes.y + recTexShapes.height)/texShapes.height);
            rlVertex2f(center.x + sinf(DEG2RAD*(i + 10))*radius, center.y + cosf(DEG2RAD*(i + 10))*radius);

            rlTexCoord2f((recTexShapes.x + recTexShapes.width)/texShapes.width, recTexShapes.y/texShapes.height);
            rlVertex2f(center.x + sinf(DEG2RAD*(i + 20))*radius, center.y + cosf(DEG2RAD*(i + 20))*radius);
        }
    rlEnd();

    rlDisableTexture();
#else
    if (rlCheckBufferLimit(RL_TRIANGLES, 3*(36/2))) rlglDraw();

    rlBegin(RL_TRIANGLES);
        for (int i = startAngleRadians; i <= endAngleRadians; i += 10)
        {
            rlColor4ub(color.r, color.g, color.b, color.a);

            rlVertex2f(center.x, center.y);
            rlVertex2f(center.x + sinf(DEG2RAD*i)*radius, center.y + cosf(DEG2RAD*i)*radius);
            rlVertex2f(center.x + sinf(DEG2RAD*(i + 10))*radius, center.y + cosf(DEG2RAD*(i + 10))*radius);
        }
    rlEnd();
#endif
}
