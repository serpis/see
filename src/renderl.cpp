#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <cstring>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "renderl.hpp"

extern int window_width;
extern int window_height;

void render_print_errors(const char *scope)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        fprintf(stderr, "[%s] gl error: %s\n", scope, (char *)gluErrorString(err));
        err = glGetError();
    }
}

static renderl_uniform_buffer_t fragment_uniform_buffer;
static renderl_uniform_buffer_t vertex_uniform_buffer;

void renderl_init()
{
    fragment_uniform_buffer = renderl_upload_uniform_buffer(NULL, 0);
    vertex_uniform_buffer = renderl_upload_uniform_buffer(NULL, 0);
}

renderl_batch_t create_default_batch()
{
    renderl_batch_t b;
    memset(&b, 0, sizeof(renderl_batch_t));
    return b;
}

renderl_texture_t renderl_upload_texture_adv(int width, int height, int target_format, int source_format, int source_type, void *data)
{
    unsigned int handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    gluBuild2DMipmaps(GL_TEXTURE_2D, target_format, width, height, source_format, source_type, data);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    renderl_texture_t res;
    res.handle = handle;
    res.width = width;
    res.height = height;
    return res;
}

renderl_texture_t renderl_upload_texture(int width, int height, int source_format, void *data)
{
    return renderl_upload_texture_adv(width, height, GL_RGBA, source_format, GL_UNSIGNED_BYTE, data);
}

void renderl_delete_texture(renderl_texture_t texture)
{
    glDeleteTextures(1, &texture.handle);
}

renderl_frame_buffer_t renderl_create_frame_buffer(int width, int height, int color_attachment_count, int color_format, bool stencil_buffer)
{
    renderl_frame_buffer_t res;

    unsigned int handle;
    assert(glGenFramebuffersEXT);
    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);

    res.handle = handle;
    res.texture_count = color_attachment_count;

    unsigned int depth_tex;
    if (stencil_buffer)
    {
        glGenRenderbuffers(1, &depth_tex);
        glBindRenderbuffer(GL_RENDERBUFFER, depth_tex);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_tex);
    }
    else
    {
        glGenTextures(1, &depth_tex);
        glBindTexture(GL_TEXTURE_2D, depth_tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);
    }

    res.depth_texture.handle = depth_tex;
    res.depth_texture.width = width;
    res.depth_texture.height = height;

    unsigned int stencil_buf;
    glGenRenderbuffers(1, &stencil_buf);
    render_print_errors("0");
    glBindRenderbuffer(GL_RENDERBUFFER, stencil_buf);
    render_print_errors("1");
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
    render_print_errors("2");
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    render_print_errors("3");
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencil_buf);
    render_print_errors("4");

    for (int i = 0; i < color_attachment_count; i++)
    {
        unsigned int tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex, 0);
        //glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0);

        res.textures[i].handle = tex;
        res.textures[i].width = width;
        res.textures[i].height = height;
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return res;
}

renderl_frame_buffer_t renderl_assemble_frame_buffer(int width, int height, const renderl_texture_t &depth_texture, int color_attachment_count, const renderl_texture_t *color_textures)
{
    renderl_frame_buffer_t res;

    unsigned int handle;
    assert(glGenFramebuffersEXT);
    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);

    res.handle = handle;
    res.texture_count = color_attachment_count;

    res.depth_texture.handle = depth_texture.handle;
    res.depth_texture.width = depth_texture.width;
    res.depth_texture.height = depth_texture.height;

    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture.handle, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_texture.handle);

    for (int i = 0; i < color_attachment_count; i++)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, color_textures[i].handle, 0);

        res.textures[i].handle = color_textures[i].handle;
        res.textures[i].width = color_textures[i].width;
        res.textures[i].height = color_textures[i].height;
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return res;
}

static int upload_shader(const renderl_source_t &source)
{
    int shader = glCreateShader(source.type);
    assert(shader != 0);
    glShaderSource(shader, 1, (const char **)&source.source, &source.source_size);
    glCompileShader(shader);
    int result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE)
    {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log) - 1, NULL, log);
        printf("Error compiling '%s':\n%s\n", source.name, log);
        return 0;
        //assert(0);
    }

    return shader;
}

renderl_program_t renderl_upload_program(int shader_source_count, const renderl_source_t *shader_sources)
{
    int program = glCreateProgram();

    for (int i = 0; i < shader_source_count; i++)
    {
        int shader = upload_shader(shader_sources[i]);
        if (shader == 0)
        {
            glDeleteProgram(program);
            renderl_program_t res;
            res.handle = 0;
            return res;
        }

        glAttachShader(program, shader);

        // NOTE: this will not actually be deleted until the corresponding program is deleted
        glDeleteShader(shader);
    }

    glLinkProgram(program);
    int result;
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    if (result != GL_TRUE)
    {
        char log[1024];
        glGetProgramInfoLog(program, sizeof(log) - 1, NULL, log);
        printf("Error linking program:\n%s\n", log);
        //assert(0);
        renderl_program_t res;
        res.handle = 0;
        return res;
    }

    renderl_program_t res;
    res.handle = program;
    return res;
}

void renderl_delete_program(renderl_program_t program)
{
    glDeleteProgram(program.handle);
}

renderl_vertex_buffer_t renderl_upload_vertex_buffer(int type, int component_count, const void *data, int size)
{
    unsigned int handle;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    renderl_vertex_buffer_t res;
    res.handle = handle;
    res.type = type;
    res.component_count = component_count;
    return res;
}

renderl_index_buffer_t renderl_upload_index_buffer(int type, const void *data, int size)
{
    unsigned int handle;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    renderl_index_buffer_t res;
    res.handle = handle;
    res.type = type;
    return res;
}

renderl_uniform_buffer_t renderl_upload_uniform_buffer(const void *data, int size)
{
    unsigned int handle;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    renderl_uniform_buffer_t res;
    res.handle = handle;
    return res;
}

void renderl_update_uniform_buffer(renderl_uniform_buffer_t &uniform_buffer, const void *data, int size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer.handle);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void renderl_push_batch(const renderl_batch_t &batch)
{
    if (batch.use_depth_test)
    {
        glEnable(GL_DEPTH_TEST);
    }
    if (batch.use_back_face_culling)
    {
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
    }
    if (batch.use_blending)
    {
        glEnable(GL_BLEND);
        glBlendFunc(batch.src_blend_func, batch.dst_blend_func);
    }

    renderl_update_uniform_buffer(fragment_uniform_buffer, batch.fragment_parameters, batch.fragment_parameters_size);
    renderl_update_uniform_buffer(vertex_uniform_buffer, batch.vertex_parameters, batch.vertex_parameters_size);

    glUseProgram(batch.program->handle);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, vertex_uniform_buffer.handle);
    glUniformBlockBinding(batch.program->handle, glGetUniformBlockIndex(batch.program->handle, "vertex_uniforms"), 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, fragment_uniform_buffer.handle);
    glUniformBlockBinding(batch.program->handle, glGetUniformBlockIndex(batch.program->handle, "fragment_uniforms"), 1);

    const int texture_units[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    glUniform1iv(glGetUniformLocation(batch.program->handle, "tex"), batch.texture_count, texture_units);

    for (int i = 0; i < batch.texture_count; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, batch.textures[i]->handle);
    }
    glActiveTexture(GL_TEXTURE0);

    // gah... words cannot describe the ugliness of
    // creating a vertex array for every batch
    // just to remove it afterwards
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    for (int i = 0; i < batch.vertex_buffer_count; i++)
    {
        const renderl_vertex_buffer_t *buf = batch.vertex_buffers[i];
        if (buf->handle != 0)
        {
            glEnableVertexAttribArray(i);
            render_print_errors("0");
            glBindBuffer(GL_ARRAY_BUFFER, buf->handle);
            render_print_errors("1");
            glVertexAttribPointer(i, buf->component_count, buf->type, GL_FALSE, 0, 0);
            render_print_errors("2");
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.index_buffer->handle);

    render_print_errors("3");
    // do actual drawing
    glDrawElements(batch.primitive_type, batch.index_count, batch.index_buffer->type, 0);

    render_print_errors("4");
    // clean up...

    // this also clears bound buffers
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertex_array);

    for (int i = 0; i < batch.texture_count; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, 0);

    glUseProgram(0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
}

void renderl_bind_frame_buffer(const renderl_frame_buffer_t *fbo)
{
    const GLenum bufs[] =
    {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4,
        GL_COLOR_ATTACHMENT5,
        GL_COLOR_ATTACHMENT6,
        GL_COLOR_ATTACHMENT7,
    };

    if (fbo)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
        glDrawBuffers(fbo->texture_count, bufs);
        glViewport(0, 0, fbo->textures[0].width, fbo->textures[0].height);
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_width, window_height);
    }
}

