#pragma once

#define MOUNT_POINT "/sdcard"

#define MOUNT_FILE_TRANSF_SIZE_LIMIT 256
#define MOUNT_FILE_ALLOC_SIZE_EACH 128
#define MOUNT_FILE_MAX_OPEN_FILES_AT_ONCE 5
#define MOUNT_FILE_SPEED_KHZ 20000

#define MOUNT_FILE_PIN_MISO GPIO_NUM_19
#define MOUNT_FILE_PIN_MOSI GPIO_NUM_23
#define MOUNT_FILE_PIN_CLK  GPIO_NUM_18
#define MOUNT_FILE_PIN_CS   GPIO_NUM_5

// DELETE X if X != nullptr then X = nullptr;
#define DEL_IT(X) { if (X) delete X; X = nullptr; }
// DELETE ARRAY (X) if X != nullptr then X = nullptr;
#define DEL_EM(X) { if (X) delete[] X; X = nullptr; }
// TARG << FROM, FROM << FROM_VAL
#define EXCHANGE(TARG, FROM, FROM_VAL) { TARG = FROM; FROM = FROM_VAL; }
// TARG << FROM, FROM << NULL
#define EXC_NULL(TARG, FROM) EXCHANGE(TARG, FROM, nullptr)

#define READFILE_FULLY_TO(HEAP_OBJ, FILE_TO_READ_PATH, THROW_ERR_STR) { File fp(FILE_TO_READ_PATH, "rb"); if (!fp) throw Exception(THROW_ERR_STR); char _____minbuf[32]; while(!fp.eof()) { const size_t rd = fp.read(_____minbuf, 32); if (rd == 0) { break; } HEAP_OBJ.append(_____minbuf, rd); } fp.close(); }