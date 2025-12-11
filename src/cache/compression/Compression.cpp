#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "Compression.hpp"

t_CompressionData* c_Compression::f_convertBuffer2Array(t_Data p_buffer, t_BlockOffset p_size, t_Size p_step) {
    t_CompressionData *l_values = (t_CompressionData *)malloc(sizeof(t_CompressionData) * p_size / p_step);
    t_Size i, j;
    for (i = 0; i < p_size / p_step; i++)    {
        l_values[i] = 0;
    }
    for (i = 0; i < p_size; i += p_step)    {
        for (j = 0; j < p_step; j++)  {
            l_values[i / p_step] += (t_CompressionData)((unsigned char)p_buffer[i + j]) << (8 * j);
        }
    }
    return l_values;
}

t_Size c_Compression::f_isZeroPackable(t_CompressionData *p_values, t_BlockOffset p_size)   {
    bool l_nonZero = false;
    t_Size i;
    for (i = 0; i < p_size; i++)    {
        if (p_values[i] != 0) {
            l_nonZero = 1;
            break;
        }
    }
    return l_nonZero ? p_size : 1;
}

t_Size c_Compression::f_isSameValuePackable(t_CompressionData *p_values, t_BlockOffset p_size)  {
    bool l_notSame = false;
    t_Size i;
    for (i = 0; i < p_size; i++)  {
        if (p_values[0] != p_values[i]) {
            l_notSame = true;
            break;
        }
    }
    return l_notSame ? p_size : 8;
}

t_Size c_Compression::f_doubleExponentCompression(t_CompressionData *p_values, t_BlockOffset p_size, t_Size p_blimit, t_Size p_bsize)  {
   t_CompressionData l_limit = 0;
    switch (p_blimit)    {
    case 1:
        l_limit = 56;
        break;
    case 2:
        l_limit = 48;
        break;
    default:
        assert(false);
    }
    t_Size l_compCount = 0;
    t_Size i;
    for (i = 0; i < p_size; i++)    {
        if ((p_values[0] >> l_limit) == (p_values[i] >> l_limit))        {
            l_compCount++;
        }
    }
    if (l_compCount != p_size)
        return p_size * p_bsize;
    return p_size * p_bsize - (l_compCount - 1) * p_blimit;
}

t_Size c_Compression::f_multBaseCompression(t_CompressionData *p_values, t_BlockOffset p_size, t_Size p_blimit, t_Size p_bsize)    {
    t_CompressionData l_limit = 0;
    t_Size BASES = 2;
    switch (p_blimit)    {
    case 1:
        l_limit = 0xFF;
        break;
    case 2:
        l_limit = 0xFFFF;
        break;
    case 4:
        l_limit = 0xFFFFFFFF;
        break;
    default:
        assert(false);
    }
    t_CompressionData l_mbases[64];
    t_Size l_baseCount = 1;
    l_mbases[0] = 0;
    t_Size i, j;
    for (i = 0; i < p_size; i++)    {
        for (j = 0; j < l_baseCount; j++)        {
            t_CompressionData l_data  = (l_mbases[j] - p_values[i]);
            if (LLABS(l_data) > l_limit)            {
                l_mbases[l_baseCount++] = p_values[i];
            }
        }
        if (l_baseCount >= BASES)
            break;
    }
    t_Size l_compCount = 0;
    for (i = 0; i < p_size; i++)    {
        for (j = 0; j < l_baseCount; j++) {
             t_CompressionData l_data  = l_mbases[j] - p_values[i];
            if (LLABS(l_data) <= l_limit)  {
                l_compCount++;
                break;
            }
        }
    }
    t_Size l_mCompSize = p_blimit * l_compCount + p_bsize * BASES + (p_size - l_compCount) * p_bsize;
    if (l_compCount < p_size)
        return p_size * p_bsize;
    return l_mCompSize;
}

t_Size c_Compression::f_BDICompress(t_Data p_buffer, t_BlockOffset p_blockSize) {
    t_CompressionData *l_values = f_convertBuffer2Array(p_buffer, p_blockSize, 8);
    t_Size l_bestCSize = p_blockSize;
    t_Size l_currCSize = p_blockSize;
    if (f_isZeroPackable(l_values, p_blockSize / 8))
        l_bestCSize = 1;
    if (f_isSameValuePackable(l_values, p_blockSize / 8))
        l_currCSize = 8;
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize : l_bestCSize;
    l_currCSize = f_multBaseCompression(l_values, p_blockSize / 8, 1, 8);
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize : l_bestCSize;
    l_currCSize = f_multBaseCompression(l_values, p_blockSize / 8, 2, 8);
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize : l_bestCSize;
    l_currCSize = f_multBaseCompression(l_values, p_blockSize / 8, 4, 8);
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize : l_bestCSize;
    free(l_values);
    l_values = f_convertBuffer2Array(p_buffer, p_blockSize, 4);
    if (f_isSameValuePackable(l_values, p_blockSize / 4))
        l_currCSize = 4;
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize : l_bestCSize;
    l_currCSize = f_multBaseCompression(l_values, p_blockSize / 4, 1, 4);
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize : l_bestCSize;
    l_currCSize = f_multBaseCompression(l_values, p_blockSize / 4, 2, 4);
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize : l_bestCSize;
    free(l_values);
    l_values = f_convertBuffer2Array(p_buffer, p_blockSize, 2);
    l_currCSize = f_multBaseCompression(l_values, p_blockSize / 2, 1, 2);
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize : l_bestCSize;
    free(l_values);

    /*l_values = f_convertBuffer2Array(p_buffer, p_blockSize, 8);
    l_currCSize = f_doubleExponentCompression(l_values, p_blockSize / 8, 2, 8);
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize: l_bestCSize;
    l_currCSize = f_doubleExponentCompression(l_values, p_blockSize / 8, 1, 8);
    l_bestCSize = l_bestCSize > l_currCSize ? l_currCSize: l_bestCSize;
    free(l_values);*/

    p_buffer = NULL;
    l_values = NULL;
    return l_bestCSize;
}

t_Size c_Compression::f_FPCCompress(t_Data p_buffer, t_BlockOffset p_size)   {
    t_CompressionData *l_values = f_convertBuffer2Array(p_buffer, p_size * 4, 4);
    t_Size l_compressable = 0;
    t_Size i;
    for (i = 0; i < p_size; i++)    {
        //  000
        if (l_values[i] == 0) {
            l_compressable += 1;
            continue;
        }
        // 001 010
        t_Size l_data = l_values[i];
        if (ABS(l_data) <= 0xFF)   {
            l_compressable += 1;
            continue;
        }
        // 011
        if (ABS(l_data) <= 0xFFFF) {
            l_compressable += 2;
            continue;
        }
        // 100
        if ((l_values[i] & 0xFFFF) == 0)  {
            l_compressable += 2;
            continue;
        }
        // 101
        t_Size l_data_a = l_values[i] & 0xFFFF;
        t_Size l_data_b = (l_values[i] >> 16) & 0xFFFF;
        if (ABS(l_data_a) <= 0xFF && ABS(l_data_b) <= 0xFF)  {
            l_compressable += 2;
            continue;
        }
        // 110
        t_Size byte0 = (l_values[i]) & 0xFF;
        t_Size byte1 = (l_values[i] >> 8) & 0xFF;
        t_Size byte2 = (l_values[i] >> 16) & 0xFF;
        t_Size byte3 = (l_values[i] >> 24) & 0xFF;
        if (byte0 == byte1 && byte0 == byte2 && byte0 == byte3)        {
            l_compressable += 1;
            continue;
        }
        // 111
        l_compressable += 4;
    }
    free(l_values);
    // 6 bytes for 3 bit per every 4-byte word in a 64 byte cache line
    t_BlockOffset l_compSize = l_compressable + p_size * 3 / 8;
    if (l_compSize < p_size * 4)
        return l_compSize;
    else
        return p_size * 4;
}

t_Size c_Compression::f_generalCompress(t_Data p_buffer, t_BlockOffset p_blockSize, t_CompressionAlgorithm p_compressAlgorithm)  { 
    try {
        t_Size l_BDISize, l_FPCSize;
        t_CompressionData *l_values;
        switch (p_compressAlgorithm)    {
        case E_COMPRESS_NO:
            return p_blockSize;
        case E_COMPRESS_ZERO:
            l_values = f_convertBuffer2Array(p_buffer, p_blockSize * 4, 4);
            return f_isZeroPackable(l_values, p_blockSize);
        case E_COMPRESS_FVC:
            l_values = f_convertBuffer2Array(p_buffer, p_blockSize * 4, 4);
            return f_isSameValuePackable(l_values, p_blockSize);
        case E_COMPRESS_BDI_ALGO:
            return f_BDICompress(p_buffer, p_blockSize);
        case E_COMPRESS_FPC_ALGO:
            return f_FPCCompress(p_buffer, p_blockSize / 4);
        case E_COMPRESS_BEST:
            l_BDISize = f_BDICompress(p_buffer, p_blockSize);
            l_FPCSize = f_FPCCompress(p_buffer, p_blockSize / 4);
            if (l_BDISize <= l_FPCSize)
                return l_BDISize;
            else
                return l_FPCSize;
        default:
            assert(false);
        }
    }
    catch(const std::exception& e)    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
}