#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "types.hpp"

#define LLABS(x) (x ^ (x >> 63)) - (x >> 63)
#define ABS(x) (x ^ (x >> 31)) - (x >> 31)

class c_Compression {
public:
    static t_CompressionData *f_convertBuffer2Array(t_Data p_buffer, t_BlockOffset p_size, t_Size p_step);
    static t_Size f_isZeroPackable(t_CompressionData *p_values, t_BlockOffset p_size);
    static t_Size f_isSameValuePackable(t_CompressionData *p_values, t_BlockOffset p_size);
    static t_Size f_doubleExponentCompression(t_CompressionData *p_values, t_BlockOffset p_size, t_Size p_blimit, t_Size p_bsize);
    static t_Size f_multBaseCompression(t_CompressionData *p_values, t_BlockOffset p_size, t_Size p_blimit, t_Size p_bsize);
    static t_Size f_BDICompress(t_Data p_buffer, t_BlockOffset p_blockSize);
    static t_Size f_FPCCompress(t_Data p_buffer, t_BlockOffset p_size);
    static t_Size f_generalCompress(t_Data p_buffer, t_BlockOffset p_blockSize, t_CompressionAlgorithm p_compressAlgorithm);
};