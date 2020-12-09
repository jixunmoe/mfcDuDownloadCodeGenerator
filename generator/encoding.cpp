#include "stdafx.h"
#include "encoding.h"

void ToUTF8(utf8_str& output, wchar_t* input)
{
	size_t out_size = WideCharToMultiByte(CP_UTF8, 0, input, -1, nullptr, 0, NULL, NULL);
	output.str = (char*)calloc(out_size + 1, sizeof(char));
	output.size = out_size;
	WideCharToMultiByte(CP_UTF8, 0, input, -1, output.str, static_cast<int>(out_size), NULL, NULL);
}
