#include "ImGuiOpenGLRenderer.h"

#ifndef IMGUI_DISABLE

#include <glad/glad.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

#define GL_CALL(call) call

struct ImGui_ImplOpenGL3_Data {
	char glsl_version_string[32] = "#version 130\n";
	GLuint shader_handle = 0;
	GLint attrib_location_tex = 0;
	GLint attrib_location_proj_mtx = 0;
	GLuint attrib_location_vtx_pos = 0;
	GLuint attrib_location_vtx_uv = 0;
	GLuint attrib_location_vtx_color = 0;
	GLuint vbo_handle = 0;
	GLuint elements_handle = 0;
	GLuint font_texture = 0;
};

static ImGui_ImplOpenGL3_Data* ImGui_ImplOpenGL3_GetBackendData() {
	return ImGui::GetCurrentContext() ? static_cast<ImGui_ImplOpenGL3_Data*>(ImGui::GetIO().BackendRendererUserData)
	                                : nullptr;
}

static void ImGui_ImplOpenGL3_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height,
                                               GLuint vertex_array_object) {
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glViewport(0, 0, static_cast<GLsizei>(fb_width), static_cast<GLsizei>(fb_height));
	const float L = draw_data->DisplayPos.x;
	const float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	const float T = draw_data->DisplayPos.y;
	const float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	const float ortho_projection[4][4] = {
	    {2.0f / (R - L), 0.0f, 0.0f, 0.0f},
	    {0.0f, 2.0f / (T - B), 0.0f, 0.0f},
	    {0.0f, 0.0f, -1.0f, 0.0f},
	    {(R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f},
	};

	glUseProgram(bd->shader_handle);
	glUniform1i(bd->attrib_location_tex, 0);
	glUniformMatrix4fv(bd->attrib_location_proj_mtx, 1, GL_FALSE, &ortho_projection[0][0]);

	glBindVertexArray(vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, bd->vbo_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bd->elements_handle);
	glEnableVertexAttribArray(bd->attrib_location_vtx_pos);
	glEnableVertexAttribArray(bd->attrib_location_vtx_uv);
	glEnableVertexAttribArray(bd->attrib_location_vtx_color);
	glVertexAttribPointer(bd->attrib_location_vtx_pos, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
	                      reinterpret_cast<void*>(offsetof(ImDrawVert, pos)));
	glVertexAttribPointer(bd->attrib_location_vtx_uv, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
	                      reinterpret_cast<void*>(offsetof(ImDrawVert, uv)));
	glVertexAttribPointer(bd->attrib_location_vtx_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert),
	                      reinterpret_cast<void*>(offsetof(ImDrawVert, col)));
}

static void ImGui_ImplOpenGL3_DrawCallback_ResetRenderState(const ImDrawList*, const ImDrawCmd*) {}

void ImGui_ImplOpenGL3_NewFrame() {
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();
	IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplOpenGL3_Init()?");

	if (bd->shader_handle == 0) {
		if (!ImGui_ImplOpenGL3_CreateDeviceObjects()) {
			IM_ASSERT(false && "ImGui_ImplOpenGL3_CreateDeviceObjects() failed!");
		}
	}
}

void ImGui_ImplOpenGL3_RenderDrawData(ImDrawData* draw_data) {
	const int fb_width = static_cast<int>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	const int fb_height = static_cast<int>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0) {
		return;
	}

	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();

	GLenum last_active_texture = 0;
	glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint*>(&last_active_texture));
	glActiveTexture(GL_TEXTURE0);
	GLuint last_program = 0;
	glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&last_program));
	GLuint last_texture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&last_texture));
	GLuint last_array_buffer = 0;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&last_array_buffer));
	GLuint last_vertex_array_object = 0;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, reinterpret_cast<GLint*>(&last_vertex_array_object));
	GLint last_polygon_mode[2] = {0, 0};
	glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4] = {0, 0, 0, 0};
	glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4] = {0, 0, 0, 0};
	glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb = 0;
	glGetIntegerv(GL_BLEND_SRC_RGB, reinterpret_cast<GLint*>(&last_blend_src_rgb));
	GLenum last_blend_dst_rgb = 0;
	glGetIntegerv(GL_BLEND_DST_RGB, reinterpret_cast<GLint*>(&last_blend_dst_rgb));
	GLenum last_blend_src_alpha = 0;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, reinterpret_cast<GLint*>(&last_blend_src_alpha));
	GLenum last_blend_dst_alpha = 0;
	glGetIntegerv(GL_BLEND_DST_ALPHA, reinterpret_cast<GLint*>(&last_blend_dst_alpha));
	GLenum last_blend_equation_rgb = 0;
	glGetIntegerv(GL_BLEND_EQUATION_RGB, reinterpret_cast<GLint*>(&last_blend_equation_rgb));
	GLenum last_blend_equation_alpha = 0;
	glGetIntegerv(GL_BLEND_EQUATION_ALPHA, reinterpret_cast<GLint*>(&last_blend_equation_alpha));
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	GLuint vertex_array_object = 0;
	glGenVertexArrays(1, &vertex_array_object);
	ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

	const ImVec2 clip_off = draw_data->DisplayPos;
	const ImVec2 clip_scale = draw_data->FramebufferScale;

	for (const ImDrawList* draw_list : draw_data->CmdLists) {
		const GLsizeiptr vtx_buffer_size = static_cast<GLsizeiptr>(draw_list->VtxBuffer.Size * sizeof(ImDrawVert));
		const GLsizeiptr idx_buffer_size = static_cast<GLsizeiptr>(draw_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		glBufferData(GL_ARRAY_BUFFER, vtx_buffer_size, draw_list->VtxBuffer.Data, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_size, draw_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++) {
			const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback != nullptr) {
				if (pcmd->UserCallback == ImGui_ImplOpenGL3_DrawCallback_ResetRenderState) {
					ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
				} else {
					pcmd->UserCallback(draw_list, pcmd);
				}
				continue;
			}

			const ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x,
			                      (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
			const ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x,
			                      (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
			if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) {
				continue;
			}

			glScissor(static_cast<int>(clip_min.x), static_cast<int>(static_cast<float>(fb_height) - clip_max.y),
			          static_cast<int>(clip_max.x - clip_min.x), static_cast<int>(clip_max.y - clip_min.y));
			glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(pcmd->GetTexID()));
			glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount),
			                         sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
			                         reinterpret_cast<void*>(static_cast<intptr_t>(pcmd->IdxOffset * sizeof(ImDrawIdx))),
			                         static_cast<GLint>(pcmd->VtxOffset));
		}
	}

	glDeleteVertexArrays(1, &vertex_array_object);

	if (last_program == 0 || glIsProgram(last_program)) {
		glUseProgram(last_program);
	}
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	last_enable_blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
	last_enable_cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	last_enable_depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	last_enable_stencil_test ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
	last_enable_scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT, static_cast<GLenum>(last_polygon_mode[0]));
	glPolygonMode(GL_BACK, static_cast<GLenum>(last_polygon_mode[1]));
	glViewport(last_viewport[0], last_viewport[1], static_cast<GLsizei>(last_viewport[2]),
	           static_cast<GLsizei>(last_viewport[3]));
	glScissor(last_scissor_box[0], last_scissor_box[1], static_cast<GLsizei>(last_scissor_box[2]),
	          static_cast<GLsizei>(last_scissor_box[3]));
	(void) bd;
}

static bool CheckShader(GLuint handle, const char* desc) {
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();
	GLint status = 0;
	GLint log_length = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
	glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
	if (status == GL_FALSE) {
		std::fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to compile %s! With GLSL: %s\n", desc,
		             bd->glsl_version_string);
	}
	if (log_length > 1) {
		ImVector<char> buf;
		buf.resize(log_length + 1);
		glGetShaderInfoLog(handle, log_length, nullptr, buf.begin());
		std::fprintf(stderr, "%s\n", buf.begin());
	}
	return status == GL_TRUE;
}

static bool CheckProgram(GLuint handle, const char* desc) {
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();
	GLint status = 0;
	GLint log_length = 0;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
	if (status == GL_FALSE) {
		std::fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to link %s! With GLSL: %s\n", desc,
		             bd->glsl_version_string);
	}
	if (log_length > 1) {
		ImVector<char> buf;
		buf.resize(log_length + 1);
		glGetProgramInfoLog(handle, log_length, nullptr, buf.begin());
		std::fprintf(stderr, "%s\n", buf.begin());
	}
	return status == GL_TRUE;
}

static bool ImGui_ImplOpenGL3_CreateFontsTexture() {
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();

	unsigned char* pixels = nullptr;
	int width = 0;
	int height = 0;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	GLint last_texture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &bd->font_texture);
	glBindTexture(GL_TEXTURE_2D, bd->font_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	io.Fonts->SetTexID(static_cast<ImTextureID>(bd->font_texture));
	glBindTexture(GL_TEXTURE_2D, last_texture);
	return true;
}

static void ImGui_ImplOpenGL3_DestroyFontsTexture() {
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();
	if (bd->font_texture != 0) {
		glDeleteTextures(1, &bd->font_texture);
		io.Fonts->SetTexID(ImTextureID_Invalid);
		bd->font_texture = 0;
	}
}

bool ImGui_ImplOpenGL3_CreateDeviceObjects() {
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();

	GLint last_texture = 0;
	GLint last_array_buffer = 0;
	GLint last_vertex_array = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar* vertex_shader =
	    "uniform mat4 ProjMtx;\n"
	    "in vec2 Position;\n"
	    "in vec2 UV;\n"
	    "in vec4 Color;\n"
	    "out vec2 Frag_UV;\n"
	    "out vec4 Frag_Color;\n"
	    "void main()\n"
	    "{\n"
	    "    Frag_UV = UV;\n"
	    "    Frag_Color = Color;\n"
	    "    gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
	    "}\n";

	const GLchar* fragment_shader =
	    "uniform sampler2D Texture;\n"
	    "in vec2 Frag_UV;\n"
	    "in vec4 Frag_Color;\n"
	    "out vec4 Out_Color;\n"
	    "void main()\n"
	    "{\n"
	    "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
	    "}\n";

	const GLchar* vertex_shader_with_version[2] = {bd->glsl_version_string, vertex_shader};
	GLuint vert_handle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_handle, 2, vertex_shader_with_version, nullptr);
	glCompileShader(vert_handle);
	if (!CheckShader(vert_handle, "vertex shader")) {
		return false;
	}

	const GLchar* fragment_shader_with_version[2] = {bd->glsl_version_string, fragment_shader};
	GLuint frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_handle, 2, fragment_shader_with_version, nullptr);
	glCompileShader(frag_handle);
	if (!CheckShader(frag_handle, "fragment shader")) {
		glDeleteShader(vert_handle);
		return false;
	}

	bd->shader_handle = glCreateProgram();
	glAttachShader(bd->shader_handle, vert_handle);
	glAttachShader(bd->shader_handle, frag_handle);
	glLinkProgram(bd->shader_handle);
	if (!CheckProgram(bd->shader_handle, "shader program")) {
		glDeleteShader(vert_handle);
		glDeleteShader(frag_handle);
		return false;
	}

	glDetachShader(bd->shader_handle, vert_handle);
	glDetachShader(bd->shader_handle, frag_handle);
	glDeleteShader(vert_handle);
	glDeleteShader(frag_handle);

	bd->attrib_location_tex = glGetUniformLocation(bd->shader_handle, "Texture");
	bd->attrib_location_proj_mtx = glGetUniformLocation(bd->shader_handle, "ProjMtx");
	bd->attrib_location_vtx_pos = static_cast<GLuint>(glGetAttribLocation(bd->shader_handle, "Position"));
	bd->attrib_location_vtx_uv = static_cast<GLuint>(glGetAttribLocation(bd->shader_handle, "UV"));
	bd->attrib_location_vtx_color = static_cast<GLuint>(glGetAttribLocation(bd->shader_handle, "Color"));

	glGenBuffers(1, &bd->vbo_handle);
	glGenBuffers(1, &bd->elements_handle);
	ImGui_ImplOpenGL3_CreateFontsTexture();

	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);
	return true;
}

void ImGui_ImplOpenGL3_DestroyDeviceObjects() {
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();
	if (bd == nullptr) {
		return;
	}

	ImGui_ImplOpenGL3_DestroyFontsTexture();
	if (bd->vbo_handle != 0) {
		glDeleteBuffers(1, &bd->vbo_handle);
		bd->vbo_handle = 0;
	}
	if (bd->elements_handle != 0) {
		glDeleteBuffers(1, &bd->elements_handle);
		bd->elements_handle = 0;
	}
	if (bd->shader_handle != 0) {
		glDeleteProgram(bd->shader_handle);
		bd->shader_handle = 0;
	}
}

bool ImGui_ImplOpenGL3_Init(const char* glsl_version) {
	ImGuiIO& io = ImGui::GetIO();
	IMGUI_CHECKVERSION();
	IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

	ImGui_ImplOpenGL3_Data* bd = IM_NEW(ImGui_ImplOpenGL3_Data)();
	io.BackendRendererUserData = bd;
	io.BackendRendererName = "imgui_impl_opengl3";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.DrawCallback_ResetRenderState = ImGui_ImplOpenGL3_DrawCallback_ResetRenderState;

	if (glsl_version != nullptr) {
		IM_ASSERT(static_cast<int>(std::strlen(glsl_version)) + 2 < IM_ARRAYSIZE(bd->glsl_version_string));
		std::snprintf(bd->glsl_version_string, sizeof(bd->glsl_version_string), "%s\n", glsl_version);
	}

	return true;
}

void ImGui_ImplOpenGL3_Shutdown() {
	ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();
	IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");

	ImGui_ImplOpenGL3_DestroyDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	io.BackendRendererName = nullptr;
	io.BackendRendererUserData = nullptr;
	io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
	platform_io.ClearRendererHandlers();
	IM_DELETE(bd);
}

#endif
