#pragma once
#include "../../../includes/public.h"
#include "../../../includes/buffer.h"

void encoder_run(const char* raw, const char* key, char* dest, bool isEncode);
Buffer* encoder_buffer_run(Buffer* buf, const char* key, bool isEncode, int bias);
