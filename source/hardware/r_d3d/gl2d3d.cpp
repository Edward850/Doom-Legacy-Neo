// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: gl2d3d.cpp,v 1.1 2000/10/01 15:14:31 hurdler Exp $
//
// Copyright (C) 1998-2000 by DooM Legacy Team.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
// $Log: gl2d3d.cpp,v $
// Revision 1.1  2000/10/01 15:14:31  hurdler
// Completely rewritten d3d driver... absolutely not finished at all
//
//
//
// DESCRIPTION:
//      
//
//-----------------------------------------------------------------------------

#include "gl2d3d.h"

void GL_2_D3D::Clear(GLbitfield mask)
{
	D3DRECT	rect = { 0, 0, screen_width, screen_height };
	int		m = 0;

	if (mask & GL_COLOR_BUFFER_BIT) m |= D3DCLEAR_TARGET;
	if (mask & GL_DEPTH_BUFFER_BIT) m |= D3DCLEAR_ZBUFFER;

	IDirect3DViewport3_Clear2(d3d_viewport, 1, &rect, m, 0, 1.0f, 0);
}


const GLubyte *GL_2_D3D::GetString(GLenum name)
{
    static char *vendor   = "Thierry Van Elsuwé";
    static char *renderer = "Mini OpenGL 1.1 to Direct3D 7.0 wrapper v0.1";
    static char *version  = "1.1";
    static char *unknown  = "<unknown>";

    switch (name)
    {
        case GL_VENDOR:   return (const GLubyte *) vendor;
        case GL_RENDERER: return (const GLubyte *) renderer;
        case GL_VERSION:  return (const GLubyte *) version;
    }
    return (const GLubyte *) unknown;
}


void GL_2_D3D::GetDoublev(unsigned int,double *)
{
}


void GL_2_D3D::GetIntegerv(unsigned int,int *)
{
}


void GL_2_D3D::LoadIdentity(void)
{
}


void GL_2_D3D::MatrixMode(GLenum mode)
{
}


void GL_2_D3D::Viewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
}


void GL_2_D3D::Scalef(GLfloat x, GLfloat y, GLfloat z)
{
}


void GL_2_D3D::PolygonOffset(GLfloat factor, GLfloat units)
{
}


void GL_2_D3D::TexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels)
{
}


void GL_2_D3D::BindTexture(GLenum target, GLuint texture)
{
}


void GL_2_D3D::DepthFunc(GLenum func)
{
}


void GL_2_D3D::DepthRange(GLclampd zNear, GLclampd zFar)
{
}


void GL_2_D3D::ClearDepth(GLclampd depth)
{
}


void GL_2_D3D::DepthMask(GLboolean flag)
{
}


void GL_2_D3D::ColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
}


void GL_2_D3D::BlendFunc(GLenum sfactor, GLenum dfactor)
{
}


void GL_2_D3D::Disable(GLenum cap)
{
}


void GL_2_D3D::AlphaFunc(GLenum func, GLclampf ref)
{
}


void GL_2_D3D::TexEnvf(GLenum target, GLenum pname, GLfloat param)
{
}


void GL_2_D3D::Enable(GLenum cap)
{
}


void GL_2_D3D::ShadeModel(GLenum mode)
{
}


void GL_2_D3D::DeleteTextures(GLsizei n, const GLuint *textures)
{
}


void GL_2_D3D::ReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
}

void GL_2_D3D::ClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
}


void GL_2_D3D::End(void)
{
}


void GL_2_D3D::Vertex3f(GLfloat x, GLfloat y, GLfloat z)
{
}


void GL_2_D3D::Begin(GLenum mode)
{
}


void GL_2_D3D::Color4fv(const GLfloat *v)
{
}


void GL_2_D3D::TexParameterf(GLenum target, GLenum pname, GLfloat param)
{
}


void GL_2_D3D::TexCoord2f(GLfloat s, GLfloat t)
{
}


void GL_2_D3D::Fogf(GLenum pname, GLfloat param)
{
}


void GL_2_D3D::Fogfv(GLenum pname, const GLfloat *params)
{
}


void GL_2_D3D::PopMatrix(void)
{
}


void GL_2_D3D::Translatef(GLfloat x, GLfloat y, GLfloat z)
{
}


void GL_2_D3D::Rotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
}


void GL_2_D3D::PushMatrix(void)
{
}


// *******************************************

void GLU_2_D3D::Perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
}


int GLU_2_D3D::Project(GLdouble objx, GLdouble objy, GLdouble objz, const GLdouble modelMatrix[16], const GLdouble projMatrix[16], const GLint viewport[4], GLdouble *winx, GLdouble *winy, GLdouble *winz)
{
    return 0;
}


int GLU_2_D3D::Build2DMipmaps(GLenum target, GLint components, GLint width, GLint height, GLenum format, GLenum type, const void  *data)
{
    return 0;
}


// *******************************************

HGLRC WGL_2_D3D::CreateContext(HDC)
{
    return (HGLRC) this;
}


HDC WGL_2_D3D::MakeCurrent(HDC, HGLRC)
{
    return (HDC) this;
}


void WGL_2_D3D::DeleteContext(HGLRC)
{
}

