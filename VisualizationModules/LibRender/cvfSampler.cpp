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
#include "cvfSampler.h"

#include "cvfOpenGL.h"

namespace cvf {

//==================================================================================================
///
/// \class cvf::Sampler
/// \ingroup Render
///
/// 
///
//==================================================================================================

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
Sampler::Sampler()
:   m_wrapModeS(REPEAT),
    m_wrapModeT(REPEAT),
    m_minFilter(NEAREST_MIPMAP_LINEAR),
    m_magFilter(LINEAR)
{
//    // Old defaults
//     m_wrapModeS(CLAMP_TO_EDGE),
//     m_wrapModeT(CLAMP_TO_EDGE),
//     m_minFilter(NEAREST),
//     m_magFilter(NEAREST)
}


//--------------------------------------------------------------------------------------------------
/// Sets wrap mode for both S and T
//--------------------------------------------------------------------------------------------------
void Sampler::setWrapMode(WrapMode wrapMode)
{
    setWrapModeS(wrapMode);
    setWrapModeT(wrapMode);
}

//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void Sampler::setWrapModeS(WrapMode wrapMode)
{
    m_wrapModeS = wrapMode;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void Sampler::setWrapModeT(WrapMode wrapMode)
{
    m_wrapModeT = wrapMode;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void Sampler::setMinFilter(Filter minFilter)
{
    m_minFilter = minFilter;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
void Sampler::setMagFilter(Filter magFilter)
{
    CVF_ASSERT(magFilter == NEAREST || magFilter == LINEAR);
    m_magFilter = magFilter;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
Sampler::WrapMode Sampler::wrapModeS() const
{
    return m_wrapModeS;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
Sampler::WrapMode Sampler::wrapModeT() const
{
    return m_wrapModeT;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
Sampler::Filter Sampler::minFilter() const
{
    return m_minFilter;
}


//--------------------------------------------------------------------------------------------------
/// 
//--------------------------------------------------------------------------------------------------
Sampler::Filter Sampler::magFilter() const
{
    return m_magFilter;
}

}  // namespace cvf
