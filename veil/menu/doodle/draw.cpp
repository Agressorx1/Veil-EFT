#include "draw.h"



namespace draw
{
	void filled_rectangle(int x, int y, int w, int h, color32 color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
	}

	void box(int x, int y, int w, int h, color32 color)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
	}

	void rectangle(int x, int y, int w, int h, color32 color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0, thickness);
	}

	void filled_circle(int x, int y, int radius, color32 color, int segments)
	{
		ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(x, y), radius, color, segments);
	}

	void circle(int x, int y, int radius, color32 color, int segments)
	{
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, color, segments);
	}

	void triangle(int x1, int y1, int x2, int y2, int x3, int y3, color32 color, float thickness)
	{
		ImGui::GetOverlayDrawList()->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), color, thickness);
	}

	void filled_triangle(int x1, int y1, int x2, int y2, int x3, int y3, color32 color)
	{
		ImGui::GetOverlayDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), color);
	}

	void line(int x1, int y1, int x2, int y2, color32 color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, thickness);
	}

	void bottom_healthbar(int x, int y, int w, int h, int borderPx, color32 color)
	{
		filled_rectangle(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
		filled_rectangle(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	}

	void corner_box(int x, int y, int w, int h, int borderPx, color32 color)
	{
		int WidthThird = w / 4;
		int HeightThird = WidthThird;

		//topleft 
		filled_rectangle(x - w / 2 - (1), y - (1), WidthThird + 2, 3, { 0, 0,0, 255 }); //h
		filled_rectangle(x - w / 2 - (1), y - (1), 3, HeightThird + 2, { 0, 0,0, 255 }); //v

		//topright
		filled_rectangle((x + w / 2) - WidthThird - (1), y - (1), WidthThird + 2, 3, { 0, 0,0, 255 }); //h
		filled_rectangle(x + w / 2 - (1), y - (1), 3, HeightThird + 2, { 0, 0,0, 255 }); //v

		//bottom left
		filled_rectangle(x - w / 2 - (1), y + h - 4, WidthThird + 2, 3, { 0, 0,0, 255 }); //h
		filled_rectangle(x - w / 2 - (1), (y + h) - HeightThird - 4, 3, HeightThird + 2, { 0, 0,0, 255 });//v

		//bottom right
		filled_rectangle((x + w / 2) - WidthThird - (1), y + h - 4, WidthThird + 2, 3, { 0, 0,0, 255 });//h
		filled_rectangle(x + w / 2 - (1), (y + h) - HeightThird - 4, 3, HeightThird + 3, { 0, 0,0, 255 }); //v

		/* draw eight lines */
		//topleft 
		filled_rectangle(x - w / 2, y, WidthThird, 1, color); //h
		filled_rectangle(x - w / 2, y, 1, HeightThird, color); //v

		//topright
		filled_rectangle((x + w / 2) - WidthThird, y, WidthThird, 1, color); //h
		filled_rectangle(x + w / 2, y, 1, HeightThird, color); //v

		//bottom left
		filled_rectangle(x - w / 2, y + h - 3, WidthThird, 1, color); //h
		filled_rectangle(x - w / 2, (y + h) - HeightThird - 3, 1, HeightThird, color);//v

		//bottom right
		filled_rectangle((x + w / 2) - WidthThird, y + h - 3, WidthThird, 1, color);//h
		filled_rectangle(x + w / 2, (y + h) - HeightThird - 3, 1, HeightThird + 1, color); //v		
	}

	void border_box(int x, int y, int x2, int y2, int thickness, color32 color)
	{
		filled_rectangle(x, y, x2, thickness, color); // top
		filled_rectangle(x, y + y2, x2, thickness, color); // bottom
		filled_rectangle(x, y, thickness, y2, color); // left
		filled_rectangle(x + x2, y, thickness, y2 + thickness, color); // right
	}

	void normal_box(int x, int y, int w, int h, int borderPx, color32 color)
	{
		filled_rectangle(x + borderPx, y, w, borderPx, color); //top 
		filled_rectangle(x + w - w + borderPx, y, w, borderPx, color); //top 
		filled_rectangle(x, y, borderPx, h, color); //left 
		filled_rectangle(x, y + h - h + borderPx * 2, borderPx, h, color); //left 
		filled_rectangle(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
		filled_rectangle(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
		filled_rectangle(x + w + borderPx, y, borderPx, h, color);//right 
		filled_rectangle(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);//right 
	}

	void filled_normal_box(int x, int y, int w, int h, int borderPx, color32 color)
	{
		filled_rectangle(x + borderPx, y, w, borderPx, color); //top 
		filled_rectangle(x + w - w + borderPx, y, w, borderPx, color); //top 
		filled_rectangle(x, y, borderPx, h, color); //left 
		filled_rectangle(x, y + h - h + borderPx * 2, borderPx, h, color); //left 
		filled_rectangle(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
		filled_rectangle(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
		filled_rectangle(x + w + borderPx, y, borderPx, h, color);//right 
		filled_rectangle(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);//right 
	}

	void text(const std::string& text, const ImVec2& pos, float size, color32 color, bool center)
	{
		std::stringstream stream(text);
		std::string line;


		float y = 0.0f;
		int i = 0;

		//ImGui::PushFont(pFont);
		ImFont* pFont = ImGui::GetFont();
		while (std::getline(stream, line))
		{
			ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());
			if (center)
			{

				ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
				ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
				//ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
				//ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

				ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());
			}
			else
			{
				ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
				ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
				//ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
				//ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

				ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), color, line.c_str());
			}

			y = pos.y + textSize.y * (i + 1);
			i++;
		}
		//ImGui::PopFont();
	}

	void text(int x, int y, color32 color, std::string str)
	{
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y),
			color,
			str.c_str());
	}
}