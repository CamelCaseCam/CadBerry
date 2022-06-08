#include <cdbpch.h>
#include "RichText.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace CDB
{
	void ParseHex(std::string& Text, int i, float& R, float& G, float& B);

#define DefaultColour ImVec4(0.8, 0.8, 0.8, 1.0)
	struct TextFormat
	{
		bool IsBold = false;
		bool IsItalics = false;
		ImVec4 Colour = DefaultColour;
	};
	
#define CharWidth 0.00561f
	void RichText(std::string& Text, float x, float y, float width, float height)
	{
		bool SameLine = false;
		ImGui::PushStyleColor(ImGuiCol_Text, DefaultColour);
		int NumCharsTillWrap = width / CharWidth;
		//Get formatting and display text
		bool Escaped = false;
		TextFormat currentFormat;
		int Offset = 0;
		for (int i = 0; i < Text.length(); ++i)
		{
			int StartIdx = i;
			for (i; i < Text.length(); ++i)
			{
				switch (Text[i])
				{
				case '\\':
					Escaped = true;
					break;
				case '!':
					if (!Escaped)    //Make sure this '!' wasn't escaped
						goto breakout;
					break;
				case '\n':    //If this is a newline character, break
					--i;
					Escaped = false;
					goto breakout;
				default:
					Escaped = false;
					break;
				}
				//Now check if this char is over the word wrap limit
				if ((i + Offset) - StartIdx > NumCharsTillWrap)
					goto breakout;
			}
		breakout:
			if (SameLine)
			{
				ImGui::SameLine(0, 0);
			}
			else
			{
				SameLine = true;
			}
			ImGui::TextUnformatted(&Text[StartIdx], &Text[i]);

			//Now check if we need to update formatting
			if (Text[i] == '!')
			{
				++i;
				if (Text[i] == '#')    //Get colour
				{
					ImGui::PopStyleColor();
					++i;
					float Red, Green, Blue;
					ParseHex(Text, i, Red, Green, Blue);
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(Red, Green, Blue, 1.0f));
					i += 5;
					Offset -= 7;
				}
			}
			if ((i + Offset) - StartIdx >= NumCharsTillWrap || Text[i] == '\n')
			{
				++i;
				Offset = 0;
				SameLine = false;
			}
			else
			{
				Offset += i - StartIdx;
			}
		}
		ImGui::PopStyleColor();
	}


	void ParseHex(std::string& Text, int i, float& R, float& G, float& B)
	{
		unsigned short Red, Green, Blue;
		char str[7];
		str[0] = Text[i]; //[f]fffff
		str[1] = Text[i + 1]; //f[f]ffff
		str[2] = Text[i + 2]; //ff[f]fff
		str[3] = Text[i + 3]; //fff[f]ff
		str[4] = Text[i + 4]; //ffff[f]f
		str[5] = Text[i + 5]; //fffff[f]
		str[6] = '\0';

		sscanf(str, "%2hx%2hx%2hx", &Red, &Green, &Blue);
		R = (float)Red / 255;
		G = (float)Green / 255;
		B = (float)Blue / 255;
	}
}