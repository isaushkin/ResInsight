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
#include "cvfAssert.h"
#include "cvfMath.h"
#include "cvfPrimitiveSetIndexedUShort.h"
#include "cvfOpenGL.h"
#include "cvfOpenGLResourceManager.h"
#include "cvfBufferObjectManaged.h"

namespace cvf {



//==================================================================================================
///
/// \class cvf::PrimitiveSetIndexedUShort
/// \ingroup Render
///
/// 
///
//==================================================================================================

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
PrimitiveSetIndexedUShort::PrimitiveSetIndexedUShort(PrimitiveType primitiveType)
:   PrimitiveSet(primitiveType),
    m_minIndex(0),
    m_maxIndex(0)
{

}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
PrimitiveSetIndexedUShort::PrimitiveSetIndexedUShort(PrimitiveType primitiveType, UShortArray* indices)
:   PrimitiveSet(primitiveType),
    m_minIndex(0),
    m_maxIndex(0)
{
    setIndices(indices);
}


//--------------------------------------------------------------------------------------------------
/// Deletes  OpenGL resources created by this primitive set
//--------------------------------------------------------------------------------------------------
PrimitiveSetIndexedUShort::~PrimitiveSetIndexedUShort()
{
    releaseBufferObjectsGPU();
}


//--------------------------------------------------------------------------------------------------
/// Render primitives in this primitive set using vertex arrays
/// 
/// \warning Requires at least OpenGL 1.5
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShort::render(OpenGLContext* oglContext) const
{
    CVF_CALLSITE_OPENGL(oglContext);
    CVF_TIGHT_ASSERT(BufferObjectManaged::supportedOpenGL(oglContext));

    if (m_indices.isNull())
    {
        return;
    }

    GLsizei numIndices = static_cast<GLsizei>(m_indices->size());
    if (numIndices <= 0) 
    {
        return;
    }

    const GLvoid* ptrOrOffset = 0;
    if (m_indicesBO.notNull() && m_indicesBO->isUploaded())
    {
        m_indicesBO->bindBuffer(oglContext);
    }
    else
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        ptrOrOffset = m_indices->ptr();
    }

#ifdef CVF_OPENGL_ES
    glDrawElements(primitiveTypeOpenGL(), numIndices, GL_UNSIGNED_SHORT, ptrOrOffset);
#else
    glDrawRangeElements(primitiveTypeOpenGL(), m_minIndex, m_maxIndex, numIndices, GL_UNSIGNED_SHORT, ptrOrOffset);
#endif
}


//--------------------------------------------------------------------------------------------------
/// Create buffer object and upload data to the GPU
/// 
/// \warning The current render context must support buffer objects (OGL version >= 1.5)
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShort::createUploadBufferObjectsGPU(OpenGLContext* oglContext)
{
    CVF_TIGHT_ASSERT(oglContext);
    CVF_TIGHT_ASSERT(BufferObjectManaged::supportedOpenGL(oglContext));

    // Buffer object already in place?
    if (m_indicesBO.notNull() && m_indicesBO->isUploaded())
    {
        return;
    }

    if (m_indices.notNull())
    {
        size_t numIndices = m_indices->size();
        if (numIndices > 0) 
        {
            GLuint uiSizeInBytes = static_cast<GLuint>(numIndices*sizeof(GLushort));
            m_indicesBO = oglContext->resourceManager()->getOrCreateManagedBufferObject(oglContext, GL_ELEMENT_ARRAY_BUFFER, uiSizeInBytes, m_indices->ptr());
            CVF_CHECK_OGL(oglContext);
        }
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShort::releaseBufferObjectsGPU()
{
    m_indicesBO = NULL;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void PrimitiveSetIndexedUShort::setIndices(UShortArray* indices)
{
    m_minIndex = 0;
    m_maxIndex = 0;

    m_indices = indices;

    // Find min/max index present in array
    size_t numIndices = m_indices.notNull() ? m_indices->size() : 0;
    if (numIndices > 0)
    {
        ushort thisIndex = m_indices->get(0);
        m_minIndex = thisIndex;
        m_maxIndex = thisIndex;

        size_t i;
        for (i = 1; i < numIndices; i++)
        {
            thisIndex = indices->get(i);

            if (thisIndex < m_minIndex)
            {
                m_minIndex = thisIndex;
            }

            if (thisIndex > m_maxIndex)
            {
                m_maxIndex = thisIndex;
            }
        }
    }

    m_indicesBO = NULL;
}


//--------------------------------------------------------------------------------------------------
/// Get a const ptr to the indices (connectivities) stored in this primitive set
//--------------------------------------------------------------------------------------------------
const UShortArray* PrimitiveSetIndexedUShort::indices() const
{
    return m_indices.p();    
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
size_t PrimitiveSetIndexedUShort::indexCount() const
{
    return m_indices.notNull() ? m_indices->size() : 0;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
uint PrimitiveSetIndexedUShort::index(size_t i) const
{
    CVF_TIGHT_ASSERT(m_indices.notNull());
    return m_indices->get(i);
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
uint PrimitiveSetIndexedUShort::minIndex() const
{
    if (indexCount() > 0)
    {
        return m_minIndex;
    }
    else
    {
        return UNDEFINED_UINT;
    }
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
uint PrimitiveSetIndexedUShort::maxIndex() const
{
    if (indexCount() > 0)
    {
        return m_maxIndex;
    }
    else
    {
        return UNDEFINED_UINT;
    }
}


} // namespace cvf

