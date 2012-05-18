//##################################################################################################
//
//   Custom Visualization Core library
//   Copyright (C) 2011-2012 Ceetron AS
//    
//   This library is free software: you can redistribute it and/or modify 
//   it under the terms of the GNU General Public License as published by 
//   the Free Software Foundation, either version 3 of the License, or 
//   (at your option) any later version. 
//    
//   This library is distributed in the hope that it will be useful, but WITHOUT ANY 
//   WARRANTY; without even the implied warranty of MERCHANTABILITY or 
//   FITNESS FOR A PARTICULAR PURPOSE.   
//    
//   See the GNU General Public License at <<http://www.gnu.org/licenses/gpl.html>> 
//   for more details. 
//
//##################################################################################################

#include "cvfBase.h"
#include "cvfOverlayColorLegend.h"
#include "cvfOpenGL.h"
#include "cvfOpenGLResourceManager.h"
#include "cvfGeometryBuilderDrawableGeo.h"
#include "cvfGeometryUtils.h"
#include "cvfViewport.h"
#include "cvfCamera.h"
#include "cvfTextDrawer.h"
#include "cvfFont.h"
#include "cvfShaderProgram.h"
#include "cvfShaderProgramGenerator.h"
#include "cvfShaderSourceProvider.h"
#include "cvfShaderSourceRepository.h"
#include "cvfUniform.h"
#include "cvfMatrixState.h"
#include "cvfBufferObjectManaged.h"
#include "cvfGlyph.h"

#ifndef CVF_OPENGL_ES
#include "cvfRenderState_FF.h"
#else
#include "cvfRenderState.h"
#endif
#include "cvfLegendScalarMapper.h"

namespace cvf {


//==================================================================================================
///
/// \class cvf::OverlayColorLegend
/// \ingroup Render
///
/// 
///
//==================================================================================================

//--------------------------------------------------------------------------------------------------
/// Constructor
//--------------------------------------------------------------------------------------------------
OverlayColorLegend::OverlayColorLegend(Font* font)
:   m_sizeHint(200, 200),
    m_color(Color3::BLACK),
    m_font(font)
{
    CVF_ASSERT(font);
    CVF_ASSERT(!font->isEmpty());

    m_tickValues.reserve(3);
    m_tickValues.add(0.0);
    m_tickValues.add(0.5);
    m_tickValues.add(1.0);
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
OverlayColorLegend::~OverlayColorLegend()
{
    // Empty destructor to avoid errors with undefined types when cvf::ref's destructor gets called
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
cvf::Vec2ui OverlayColorLegend::sizeHint()
{
    return m_sizeHint;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
cvf::Vec2ui OverlayColorLegend::maximumSize()
{
    return m_sizeHint;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
cvf::Vec2ui OverlayColorLegend::minimumSize()
{
    return Vec2ui(100,100);
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::setScalarMapper(const LegendScalarMapper* scalarMapper)
{
    m_scalarMapper = scalarMapper;
  
    if (m_scalarMapper.notNull())
    {
        std::vector<double> levelValues;
        m_scalarMapper->majorLevels(&levelValues);

        m_tickValues.assign(levelValues);
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::setSizeHint(const Vec2ui& size)
{
    m_sizeHint = size;
}


//--------------------------------------------------------------------------------------------------
/// Set color of the text and lines to be rendered
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::setColor(const Color3f& color)
{
    m_color = color;
}


//--------------------------------------------------------------------------------------------------
/// Returns the color of the text and lines
//--------------------------------------------------------------------------------------------------
const Color3f&  OverlayColorLegend::color() const
{
    return m_color;
}


//--------------------------------------------------------------------------------------------------
/// Set the title (text that will be rendered above the legend)
/// 
/// The legend supports multi-line titles. Separate each line with a '\n' character
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::setTitle(const String& title)
{
    // Title
    if (title.isEmpty())
    {
        m_titleStrings.clear();
    }
    else
    {
        m_titleStrings = title.split("\n");
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
String OverlayColorLegend::title() const
{
    String title;
    for (size_t i = 0; i < m_titleStrings.size(); ++i)
    {
        title += m_titleStrings[i];

        if (i != m_titleStrings.size() - 1)
        {
            title += "\n";
        }
    }

    return title;
}


//--------------------------------------------------------------------------------------------------
/// Hardware rendering using shader programs
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::render(OpenGLContext* oglContext, const Vec2ui& position, const Vec2ui& size)
{
    render(oglContext, position, size, false);
}


//--------------------------------------------------------------------------------------------------
/// Software rendering using software
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::renderSoftware(OpenGLContext* oglContext, const Vec2ui& position, const Vec2ui& size)
{
    render(oglContext, position, size, true);
}


//--------------------------------------------------------------------------------------------------
/// Set up camera/viewport and render
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::render(OpenGLContext* oglContext, const Vec2ui& position, const Vec2ui& size, bool software)
{   
    if (size.x() <= 0 || size.y() <= 0)
    {
        return;
    }

    Camera camera;
    camera.setViewport(position.x(), position.y(), size.x(), size.y());
    camera.setProjectionAsPixelExact2D();
    camera.setViewMatrix(Mat4d::IDENTITY);
    camera.applyOpenGL();
    camera.viewport()->applyOpenGL(oglContext, Viewport::CLEAR_DEPTH);

    // Get layout information
    // Todo: Cache this between renderings. Update only when needed.
    OverlayColorLegendLayoutInfo layout(position, size);
    layoutInfo(&layout);

    // Set up text drawer
    TextDrawer textDrawer(m_font.p());
    setupTextDrawer(&textDrawer, &layout);

    // Do the actual rendering
    if (software)
    {
        renderLegendImmediateMode(oglContext, &layout);
        textDrawer.renderSoftware(oglContext, camera);
    }
    else
    {
        const MatrixState matrixState(camera);
        renderLegend(oglContext, &layout, matrixState);
        textDrawer.render(oglContext, camera);
    }

    CVF_CHECK_OGL(oglContext);
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::setupTextDrawer(TextDrawer* textDrawer, OverlayColorLegendLayoutInfo* layout)
{
    CVF_ASSERT(layout);

    textDrawer->setVerticalAlignment(TextDrawer::CENTER);
    textDrawer->setTextColor(m_color);

    m_visibleTickLabels.clear();

    const float textX = layout->tickX + 5;

    const float overlapTolerance = 1.2f * layout->charHeight;
    float lastVisibleTextY = 0.0;

    size_t numTicks = m_tickValues.size();
    size_t it;
    for (it = 0; it < numTicks; it++)
    {
        float textY = static_cast<float>(layout->legendRect.min().y() + layout->tickPixelPos->get(it));
        
        // Always draw first and last tick label. For all others, skip drawing if text ends up
        // on top of the previous label. 
        if (it != 0 && it != (numTicks - 1))
        {
            if (cvf::Math::abs(textY - lastVisibleTextY) < overlapTolerance)
            {
                m_visibleTickLabels.push_back(false);
                continue;
            }
            // Make sure it does not overlap the last tick as well

            float lastTickY = static_cast<float>(layout->legendRect.max().y() );

            if (cvf::Math::abs(textY - lastTickY) < overlapTolerance)
            {
                m_visibleTickLabels.push_back(false);
                continue;
            }
        }

        double tickValue = m_tickValues[it];
        String valueString = String::number(tickValue);
        Vec2f pos(textX, textY);
        textDrawer->addText(valueString, pos);

        lastVisibleTextY = textY;
        m_visibleTickLabels.push_back(true);
    }

    float titleY = static_cast<float>(layout->size.y()) - layout->margins.y() - layout->charHeight/2.0f;
    for (it = 0; it < m_titleStrings.size(); it++)
    {
        Vec2f pos(layout->margins.x(), titleY);
        textDrawer->addText(m_titleStrings[it], pos);

        titleY -= layout->lineSpacing;
    }

}


//--------------------------------------------------------------------------------------------------
/// Draw the legend using shader programs
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::renderLegend(OpenGLContext* oglContext, OverlayColorLegendLayoutInfo* layout, const MatrixState& matrixState)
{
    CVF_CALLSITE_OPENGL(oglContext);

    CVF_TIGHT_ASSERT(layout);
    CVF_TIGHT_ASSERT(layout->size.x() > 0);
    CVF_TIGHT_ASSERT(layout->size.y() > 0);

    Depth depth(false);
    depth.applyOpenGL(oglContext);

    // All vertices. Initialized here to set Z to zero once and for all.
    static float vertexArray[] = 
    {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f
    };

    // Per vector convenience pointers
    float* v0 = &vertexArray[0]; 
    float* v1 = &vertexArray[3]; 
    float* v2 = &vertexArray[6]; 
    float* v3 = &vertexArray[9]; 
    float* v4 = &vertexArray[12];

    // Constant coordinates
    v0[0] = v3[0] = layout->x0;
    v1[0] = v4[0] = layout->x1;

    // Connects
    static const ushort trianglesConnects[] = { 0, 1, 4, 0, 4, 3 };

    ref<ShaderProgram> shaderProgram = oglContext->resourceManager()->getLinkedUnlitColorShaderProgram(oglContext);
    CVF_TIGHT_ASSERT(shaderProgram.notNull());

    if (shaderProgram->useProgram(oglContext))
    {
        shaderProgram->clearUniformApplyTracking();
        shaderProgram->applyFixedUniforms(oglContext, matrixState);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(ShaderProgram::VERTEX);
    glVertexAttribPointer(ShaderProgram::VERTEX, 3, GL_FLOAT, GL_FALSE, 0, vertexArray);

    // Render color bar as one colored quad per pixel

    int legendHeightPixelCount = static_cast<int>(layout->tickPixelPos->get(m_tickValues.size()-1) - layout->tickPixelPos->get(0) + 0.01);
    if (m_scalarMapper.notNull())
    {
        int iPx;
        for (iPx = 0; iPx < legendHeightPixelCount; iPx++)
        {
            const Color3ub& clr = m_scalarMapper->mapToColor(m_scalarMapper->domainValue((iPx+0.5)/legendHeightPixelCount));
            float y0 = static_cast<float>(layout->legendRect.min().y() + iPx);
            float y1 = static_cast<float>(layout->legendRect.min().y() + iPx + 1);

            // Dynamic coordinates for rectangle
            v0[1] = v1[1] = y0;
            v3[1] = v4[1] = y1;

            // Draw filled rectangle elements
            {
                UniformFloat uniformColor("u_color", Color4f(Color3f(clr)));
                shaderProgram->applyUniform(oglContext, uniformColor);

#ifdef CVF_OPENGL_ES
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, trianglesConnects);
#else
                glDrawRangeElements(GL_TRIANGLES, 0, 4, 6, GL_UNSIGNED_SHORT, trianglesConnects);
#endif
            }
        }
    }

    // Render frame

    // Dynamic coordinates for  tickmarks-lines
    bool isRenderingFrame = true;
    if (isRenderingFrame)
    {
        v0[0] = v2[0] = layout->legendRect.min().x()-0.5f;
        v1[0] = v3[0] = layout->legendRect.max().x()-0.5f;
        v0[1] = v1[1] = layout->legendRect.min().y()-0.5f;
        v2[1] = v3[1] = layout->legendRect.max().y()-0.5f;
        static const ushort frameConnects[] = { 0, 1, 1, 3, 3, 2, 2, 0};

        UniformFloat uniformColor("u_color", Color4f(m_color));
        shaderProgram->applyUniform(oglContext, uniformColor);

#ifdef CVF_OPENGL_ES
        glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, frameConnects);
#else
        glDrawRangeElements(GL_LINES, 0, 3, 8, GL_UNSIGNED_SHORT, frameConnects);
#endif
    }

    // Render tickmarks
    bool isRenderingTicks = true;

    if (isRenderingTicks)
    {
        // Constant coordinates
        v0[0] = layout->x0;
        v1[0] = layout->x1 - 0.5f*(layout->tickX - layout->x1) - 0.5f;
        v2[0] = layout->x1;
        v3[0] = layout->tickX - 0.5f*(layout->tickX - layout->x1) - 0.5f;
        v4[0] = layout->tickX;

        static const ushort tickLinesWithLabel[] = { 0, 4 };
        static const ushort tickLinesWoLabel[] = { 2, 3 };

        size_t ic;
        for (ic = 0; ic < m_tickValues.size(); ic++)
        {
                float y0 = static_cast<float>(layout->legendRect.min().y() + layout->tickPixelPos->get(ic) - 0.5f);

                // Dynamic coordinates for  tickmarks-lines
                v0[1] = v1[1] = v2[1] = v3[1] = v4[1] = y0;

                UniformFloat uniformColor("u_color", Color4f(m_color));
                shaderProgram->applyUniform(oglContext, uniformColor);
                const ushort * linesConnects;

                if ( m_visibleTickLabels[ic])
                {
                    linesConnects = tickLinesWithLabel;
                }
                else
                {
                    linesConnects = tickLinesWoLabel;
                }

#ifdef CVF_OPENGL_ES
                glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, linesConnects);
#else
                glDrawRangeElements(GL_LINES, 0, 4, 2, GL_UNSIGNED_SHORT, linesConnects);
#endif
        }
    }

    glDisableVertexAttribArray(ShaderProgram::VERTEX);

    CVF_TIGHT_ASSERT(shaderProgram.notNull());
    shaderProgram->useNoProgram(oglContext);

    // Reset render states
    Depth resetDepth;
    resetDepth.applyOpenGL(oglContext);

    CVF_CHECK_OGL(oglContext);
}


//--------------------------------------------------------------------------------------------------
/// Draw the legend using immediate mode OpenGL
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::renderLegendImmediateMode(OpenGLContext* oglContext, OverlayColorLegendLayoutInfo* layout)
{
#ifdef CVF_OPENGL_ES
    CVF_UNUSED(layout);
    CVF_FAIL_MSG("Not supported on OpenGL ES");
#else
    CVF_TIGHT_ASSERT(layout);
    CVF_TIGHT_ASSERT(layout->size.x() > 0);
    CVF_TIGHT_ASSERT(layout->size.y() > 0);

    Depth depth(false);
    depth.applyOpenGL(oglContext);

    Lighting_FF lighting(false);
    lighting.applyOpenGL(oglContext);

    // All vertices. Initialized here to set Z to zero once and for all.
    static float vertexArray[] = 
    {
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,
    };

    // Per vector convenience pointers
    float* v0 = &vertexArray[0];    
    float* v1 = &vertexArray[3];    
    float* v2 = &vertexArray[6];    
    float* v3 = &vertexArray[9];    
    float* v4 = &vertexArray[12];   

    // Constant coordinates
    v0[0] = v3[0] = layout->x0;
    v1[0] = v4[0] = layout->x1;

    // Render color bar as one colored quad per pixel

    int legendHeightPixelCount = static_cast<int>(layout->tickPixelPos->get(m_tickValues.size() - 1) - layout->tickPixelPos->get(0) + 0.01);
    if (m_scalarMapper.notNull())
    {
        int iPx;
        for (iPx = 0; iPx < legendHeightPixelCount; iPx++)
        {
            const Color3ub& clr = m_scalarMapper->mapToColor(m_scalarMapper->domainValue((iPx+0.5)/legendHeightPixelCount));
            float y0 = static_cast<float>(layout->legendRect.min().y() + iPx);
            float y1 = static_cast<float>(layout->legendRect.min().y() + iPx + 1);

            // Dynamic coordinates for rectangle
            v0[1] = v1[1] = y0;
            v3[1] = v4[1] = y1;

            // Draw filled rectangle elements
            glColor3ubv(clr.ptr());
            glBegin(GL_TRIANGLE_FAN);
            glVertex3fv(v0);
            glVertex3fv(v1);
            glVertex3fv(v4);
            glVertex3fv(v3);
            glEnd();
        }
    }

    // Render frame

    // Dynamic coordinates for  tickmarks-lines
    bool isRenderingFrame = true;
    if (isRenderingFrame)
    {
        v0[0] = v2[0] = layout->legendRect.min().x()-0.5f;
        v1[0] = v3[0] = layout->legendRect.max().x()-0.5f;
        v0[1] = v1[1] = layout->legendRect.min().y()-0.5f;
        v2[1] = v3[1] = layout->legendRect.max().y()-0.5f;

        glColor3fv(m_color.ptr());
        glBegin(GL_LINES);
        glVertex3fv(v0);
        glVertex3fv(v1);
        glVertex3fv(v1);
        glVertex3fv(v3);
        glVertex3fv(v3);
        glVertex3fv(v2);
        glVertex3fv(v2);
        glVertex3fv(v0);
        glEnd();

    }

    // Render tickmarks
    bool isRenderingTicks = true;

    if (isRenderingTicks)
    {
        // Constant coordinates
        v0[0] = layout->x0;
        v1[0] = layout->x1 - 0.5f*(layout->tickX - layout->x1) - 0.5f;
        v2[0] = layout->x1;
        v3[0] = layout->tickX - 0.5f*(layout->tickX - layout->x1) - 0.5f;
        v4[0] = layout->tickX;

        size_t ic;
        for (ic = 0; ic < m_tickValues.size(); ic++)
        {
            float y0 = static_cast<float>(layout->legendRect.min().y() + layout->tickPixelPos->get(ic) - 0.5f);

            // Dynamic coordinates for  tickmarks-lines
            v0[1] = v1[1] = v2[1] = v3[1] = v4[1] = y0;

            glColor3fv(m_color.ptr());
            glBegin(GL_LINES);
            if ( m_visibleTickLabels[ic])
            {
                glVertex3fv(v0);
                glVertex3fv(v4); 
            }
            else
            {
                glVertex3fv(v2);
                glVertex3fv(v3);
            }
            glEnd();
        }
    }

    // Reset render states
    Lighting_FF resetLighting;
    resetLighting.applyOpenGL(oglContext);
    Depth resetDepth;
    resetDepth.applyOpenGL(oglContext);

    CVF_CHECK_OGL(oglContext);
#endif // CVF_OPENGL_ES
}


//--------------------------------------------------------------------------------------------------
/// Get layout information
//--------------------------------------------------------------------------------------------------
void OverlayColorLegend::layoutInfo(OverlayColorLegendLayoutInfo* layout)
{
    CVF_TIGHT_ASSERT(layout);

    ref<Glyph> glyph = m_font->getGlyph(L'A');
    layout->charHeight = static_cast<float>(glyph->height());
    layout->lineSpacing = layout->charHeight*1.5f;
    layout->margins = Vec2f(4.0f, 4.0f);

    float legendWidth = 25.0f;
    float legendHeight = static_cast<float>(layout->size.y()) - 2*layout->margins.y() - static_cast<float>(m_titleStrings.size())*layout->lineSpacing - layout->lineSpacing;
    layout->legendRect = Rectf(layout->margins.x(), layout->margins.y() + layout->charHeight/2.0f, legendWidth, legendHeight);

    if (layout->legendRect.width() < 1 || layout->legendRect.height() < 1)
    {
        return;
    }

    layout->x0 = layout->margins.x();
    layout->x1 = layout->margins.x() + layout->legendRect.width();
    layout->tickX = layout->x1 + 5;

    // Build array containing the pixel positions of all the ticks
    size_t numTicks = m_tickValues.size();
    layout->tickPixelPos = new DoubleArray(numTicks);

    size_t i;
    for (i = 0; i < numTicks; i++)
    {
        double t;
        if (m_scalarMapper.isNull()) t = 0;
        else                         t = m_scalarMapper->normalizedLevelPosition(m_tickValues[i]);
        t = Math::clamp(t, 0.0, 1.1);
        layout->tickPixelPos->set(i, t*layout->legendRect.height());
    }
  
}


} // namespace cvf

