#pragma once

struct utf8_str {
	char* str;
	size_t size;
};

void ToUTF8(utf8_str& output, wchar_t* input);
