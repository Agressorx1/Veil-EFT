#pragma once
#include <render/render.h>
#include <string>
#include <sstream>

#include <menu/imgui/imgui.h>
#include <menu/imgui/imgui_internal.h>


namespace draw
{
	struct color32
	{
		float R;
		float G;
		float B;
		float A;

		/*
		// Standard ABGR bit order
		IM_COL32_R_SHIFT // "Red" bits start (end: 1 byte)
		IM_COL32_G_SHIFT // "Green" bits start (end: 1 byte)
		IM_COL32_B_SHIFT // "Blue" bits start (end: 1 byte)
		IM_COL32_A_SHIFT // "Alpha" bits start (end: 1 byte)
		*/
		color32() :
			R(1.0f), G(1.0f), B(1.0f), A(1.0f) {}

		color32(const ImVec4& col)
		{
			this->R = col.x;
			this->G = col.y;
			this->B = col.z;
			this->A = col.w;
		}
		color32(const uint32_t col)
		{
			float s = 1.0f / 255.0f;
			this->R = (uint8_t)(col & (0xFF << IM_COL32_R_SHIFT)) * s;
			this->G = (uint8_t)(col & (0xFF << IM_COL32_G_SHIFT)) * s;
			this->B = (uint8_t)(col & (0xFF << IM_COL32_B_SHIFT)) * s;
			this->A = (uint8_t)(col & (0xFF << IM_COL32_A_SHIFT)) * s;
		}

		color32(float R, float G, float B) :
			R(R), G(G), B(B), A(1.0f) {}
		color32(float R, float G, float B, float A) :
			R(R), G(G), B(B), A(A) {}// yeah what he said

		operator uint32_t() const
		{ return get_int_color(); }

		operator ImVec4() const
		{ return get_float_color(); }

		const uint32_t get_int_color() const
		{ return ImGui::GetColorU32(ImVec4(R, G, B, A)); }

		const ImVec4 get_float_color() const
		{
			return ImVec4(
				(this->R),
				(this->G),
				(this->B),
				(this->A));
		}


		static color32 from_rgba(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255)
		{
			float factor = 1.0f / 255.0f;
			return color32(
				R * factor,
				G * factor,
				B * factor,
				A * factor);
		}
	};

	void filled_rectangle(int x, int y, int w, int h, color32 color);
	void box(int x, int y, int w, int h, color32 color);
	void rectangle(int x, int y, int w, int h, color32 color, int thickness);
	void filled_circle(int x, int y, int radius, color32 color, int segments);
	void circle(int x, int y, int radius, color32 color, int segments);
	void triangle(int x1, int y1, int x2, int y2, int x3, int y3, color32 color, float thickness);
	void filled_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color32 color);
	void line(int x1, int y1, int x2, int y2, color32 color, int thickness);
	void bottom_healthbar(int x, int y, int w, int h, int borderPx, color32 color);
	void corner_box(int x, int y, int w, int h, int borderPx, color32 color);
	void border_box(int x, int y, int x2, int y2, int thickness, color32 color);
	void normal_box(int x, int y, int w, int h, int borderPx, color32 color);
	void filled_normal_box(int x, int y, int w, int h, int borderPx, color32 color);
	void text(const std::string& text, const ImVec2& pos, float size, color32 color, bool center);
	void text(int x, int y, color32 color, std::string str);
}