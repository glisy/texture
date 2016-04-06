#include <glisy/texture.h>
#include <glisy/gl.h>
#include <string.h>
#include <math.h>

static GLuint
channel(GLenum format) {
  switch (format) {
    case GL_DEPTH_COMPONENT:
    case GL_ALPHA:
      return 1;
    case GL_RGB:
      return 3;
    default:
      return 4;
  }
}

static void
texImage2D(GlisyTexture *texture,
           GLuint target,
           GLvoid *pixels,
           GLsizei size,
           GLuint *offset,
           GLuint level) {
  GLuint height = texture->shape[0];
  GLuint width = texture->shape[0];

  if (offset) {
    if (texture->compressed) {
      glCompressedTexSubImage2D(target,
                                level,
                                offset[0],
                                offset[1],
                                width,
                                height,
                                texture->format,
                                size,
                                pixels);
    } else {
      glTexSubImage2D(target,
                      level,
                      offset[0],
                      offset[1],
                      width,
                      height,
                      texture->format,
                      texture->type,
                      pixels);
    }
  } else {
    if (texture->compressed) {
      glCompressedTexImage2D(target,
                             level,
                             texture->format,
                             width,
                             height,
                             0,
                             size,
                             pixels);
    } else {
      glTexImage2D(target,
                   level,
                   texture->format,
                   width,
                   height,
                   0,
                   texture->format,
                   texture->type,
                   pixels);
    }
  }
}

void
glisyTextureInit(GlisyTexture *texture, GLenum target) {
  if (!texture) return;
  if (texture->handle) glisyTextureDispose(texture);
  memset(texture, 0, sizeof(GlisyTexture));

  texture->name = "";
  texture->type = GL_UNSIGNED_BYTE;
  texture->wrapS = GL_CLAMP_TO_EDGE;
  texture->wrapT = GL_CLAMP_TO_EDGE;
  texture->handle = 0;
  texture->format = GL_RGB;
  texture->target = target;
  texture->mapping = GLISY_UV_MAPPING;
  texture->channels = channel(texture->format);
  texture->minFilter = GL_LINEAR;
  texture->magFilter = GL_LINEAR;
  texture->shape[0] = 0;
  texture->shape[1] = 0;
  texture->shape[2] = channel(texture->format);
  texture->compressed = GL_FALSE;
  texture->unpackAlignment = 1;

  glGenTextures(1, &texture->handle);
}

void
glisyTextureBind(GlisyTexture *texture) {
  if (!texture || !texture->handle || !texture->target) return;
  glBindTexture(texture->handle, texture->target);
}

void
glisyTextureBindSlot(GlisyTexture *texture, GLuint slot) {
  if (!texture || !texture->handle || !texture->target) return;
  glActiveTexture(GL_TEXTURE1 + slot);
  glisyTextureBind(texture);
}

void
glisyTextureDispose(GlisyTexture *texture) {
  if (!texture || !texture->handle) return;
  glDeleteTextures(1, &texture->handle);
}

void
glisyTextureGenerateMipmap(GlisyTexture *texture) {
  if (!texture || !texture->handle) return;
  glisyTextureBind(texture);
  glGenerateMipmap(texture->target);
}

void
glisyTextureUpdate(GlisyTexture *texture,
                   GLvoid **pixels,
                   GLuint *shape,
                   GLsizei size,
                   GLuint *offset,
                   GLuint level) {
  if (!texture) return;
  glisyTextureBind(texture);
  if (!level) {
    switch (texture->target) {
      case GL_TEXTURE_2D:
        if (shape) {
          texture->shape[0] = shape[0];
          texture->shape[1] = shape[1];
          texture->shape[2] = channel(texture->format);
          // set texture wrap
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrapS);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrapT);
          // Set texture filtering
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->minFilter);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->magFilter);
        }
        break;

      case GL_TEXTURE_CUBE_MAP:
        if (shape) {
          texture->shape[0] = shape[0];
          texture->shape[1] = shape[1];
          texture->shape[2] = 6;
          texture->shape[3] = channel(texture->format);
        }
        break;
    }
  }
  glPixelStorei(GL_UNPACK_ALIGNMENT, texture->unpackAlignment);
  switch (texture->target) {
    case GL_TEXTURE_2D:
      texImage2D(texture,
                 texture->target,
                 pixels,
                 size,
                 offset,
                 level);
      break;

    case GL_TEXTURE_CUBE_MAP:
      for (int i = 0; i < 6; ++i) {
        GLuint target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
        texImage2D(texture,
                  target,
                  pixels[i],
                  size,
                  offset,
                  level);
      }
      break;
  }
}
