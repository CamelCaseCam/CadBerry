#pragma once

#define IsStop(text, i) (text[i] == 'A' || text[i] == 'a') && (text[i + 1] == 'T' || text[i + 1] == 't') && (text[i + 2] == 'G' || text[i + 1] == 'g')