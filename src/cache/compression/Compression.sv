`include "types.sv"

function logic signed [31:0] ABS(input logic signed [31:0] x);
    return (x < 0) ? -x : x;
endfunction

function logic signed [63:0] LLABS(input logic signed [63:0] x);
    return (x < 0) ? -x : x;
endfunction

module c_convertBuffer2Array #(
    parameter t_BlockOffset p_size,
    parameter t_Size p_step
) (
    // rest all inputs
    input t_Data p_buffer,
    output t_CompressionData [(p_size / p_step)-1:0] bufferArray
);
always_comb begin
    t_Size i, j;
    for (i = 0; i < p_size; i += p_step) begin
        bufferArray[i / p_step] = 0;
        for (j = 0; j < p_step; j++) begin
            t_CompressionData val = t_CompressionData'(p_buffer[i + j]) << (8 * j);
            bufferArray[i / p_step] += val;
        end
    end
end
endmodule

module c_isZeroPackable #(
    parameter t_BlockOffset p_size,
    parameter t_Size p_step
) (
    input t_CompressionData [(p_size / p_step)-1:0] p_values,
    output t_Size l_nonZero
);
always_comb begin
    l_nonZero = 0;
    for (int i = 0; i < p_size / p_step; i++) begin
        if (p_values[i] != 0) begin
            l_nonZero = p_size;
            disable for;
        end
    end
    if (l_nonZero == 0) l_nonZero = 1;
end
endmodule

module c_isSameValuePackable #(
    parameter t_BlockOffset p_size,
    parameter t_Size p_step
) (
    input t_CompressionData [(p_size / p_step)-1:0] p_values,
    output t_Size l_notSame
);
always_comb begin
    l_notSame = 0;
    for (int i = 0; i < p_size / p_step; i++) begin
        if (p_values[i] != p_values[0]) begin
            l_notSame = p_size;
            disable for;
        end
    end
    if (l_notSame == 0) l_notSame = 8;
end
endmodule

module c_doubleExponentCompression #(
    parameter t_BlockOffset p_size,
    parameter t_Size p_step
) (
    input t_CompressionData [(p_size / p_step)-1:0] p_values, 
    input t_Size p_blimit, 
    input t_Size p_bsize,
    output t_Size ans
);
always_comb begin
    t_Size l_limit;
    case (p_blimit)
        1: l_limit = 56;
        2: l_limit = 48;
        default: l_limit = 0;
    endcase

    t_Size l_compCount = 0;
    for (int i = 0; i < p_size / p_step; i++) begin
        if ((p_values[0] >> l_limit) == (p_values[i] >> l_limit)) begin
            l_compCount++;
        end
    end

    if (l_compCount != (p_size / p_step))
        ans = p_size * p_bsize;
    else
        ans = p_size * p_bsize - (l_compCount - 1) * p_blimit;
end
endmodule

module c_multBaseCompression #(
    parameter t_BlockOffset p_size,
    parameter t_Size p_step
) (
    input t_CompressionData [(p_size / p_step)-1:0] p_values, 
    input t_Size p_blimit, 
    input t_Size p_bsize,
    output t_Size ans
);
always_comb begin
    t_CompressionData l_limit;
    case (p_blimit)
        1: l_limit = 8'hFF;
        2: l_limit = 16'hFFFF;
        4: l_limit = 32'hFFFFFFFF;
        default: l_limit = 0;
    endcase

    t_CompressionData l_mbases[64];
    int l_baseCount = 1;
    l_mbases[0] = 0;

    for (int i = 0; i < p_size / p_step; i++) begin
        for (int j = 0; j < l_baseCount; j++) begin
            t_CompressionData l_data = l_mbases[j] - p_values[i];
            if (LLABS(l_data) > l_limit) begin
                l_mbases[l_baseCount++] = p_values[i];
                break;
            end
        end
        if (l_baseCount >= 2) break;
    end

    int l_compCount = 0;
    for (int i = 0; i < p_size / p_step; i++) begin
        for (int j = 0; j < l_baseCount; j++) begin
            t_CompressionData l_data = l_mbases[j] - p_values[i];
            if (LLABS(l_data) <= l_limit) begin
                l_compCount++;
                break;
            end
        end
    end

    t_Size l_mCompSize = p_blimit * l_compCount + p_bsize * 2 + ((p_size / p_step) - l_compCount) * p_bsize;
    if (l_compCount < p_size)
        ans = p_size * p_bsize;
    else
        ans = l_mCompSize;
end
endmodule

module c_BDICompress #(
    parameter t_BlockOffset p_size,
    parameter t_Size p_step
) (
    input t_Data p_buffer, 
    input t_BlockOffset p_blockSize,
    output t_Size l_bestCSize
);

always_comb begin
    t_CompressionData [(p_blockSize/8)-1:0] l_values8;
    c_convertBuffer2Array #(p_blockSize, 8) conv8(.p_buffer(p_buffer), .l_values(l_values8));
    t_Size l_bestCSize = p_blockSize;
    t_Size l_currCSize = p_blockSize;

    t_Size l_nonZero8;
    c_isZeroPackable #(p_size, p_blockSize/8) zero_chk8(.l_values(l_values8), .l_nonZero(l_nonZero8));
    t_Size l_notSame8;
    c_isSameValuePackable #(p_size, p_blockSize/8) same_chk8(.l_values(l_values8), .l_notSame(l_notSame8));
    if (l_nonZero8 == 1)
        l_bestCSize = 1;
    if (l_notSame8 == 1)
        l_currCSize = 8;

    l_bestCSize = (l_bestCSize > l_currCSize) ? l_currCSize : l_bestCSize;
    c_multBaseCompression #(p_blockSize/8, p_step) mbc8_1(.p_values(l_values8), .p_blimit(1), .p_bsize(8), .ans(l_currCSize));
    l_bestCSize = (l_bestCSize > l_currCSize) ? l_currCSize : l_bestCSize;
    c_multBaseCompression #(p_blockSize/8, p_step) mbc8_2(.p_values(l_values8), .p_blimit(2), .p_bsize(8), .ans(l_currCSize));
    l_bestCSize = (l_bestCSize > l_currCSize) ? l_currCSize : l_bestCSize;
    c_multBaseCompression #(p_blockSize/8, p_step) mbc8_4(.p_values(l_values8), .p_blimit(4), .p_bsize(8), .ans(l_currCSize));
    l_bestCSize = (l_bestCSize > l_currCSize) ? l_currCSize : l_bestCSize;

    t_CompressionData [(p_blockSize/4)-1:0] l_values4;
    c_convertBuffer2Array #(p_blockSize, 4) conv4(.p_buffer(p_buffer), .l_values(l_values4));
    t_Size l_notSame4;
    c_isSameValuePackable #(p_size, p_blockSize/4) same_chk4(.l_values(l_values4), .l_notSame(l_notSame4));
    if (l_notSame4 == 1)
        l_currCSize = 4;

    l_bestCSize = (l_bestCSize > l_currCSize) ? l_currCSize : l_bestCSize;
    c_multBaseCompression #(p_blockSize/4, p_step) mbc4_1(.p_values(l_values4), .p_blimit(1), .p_bsize(4), .ans(l_currCSize));
    l_bestCSize = (l_bestCSize > l_currCSize) ? l_currCSize : l_bestCSize;
    c_multBaseCompression #(p_blockSize/4, p_step) mbc4_2(.p_values(l_values4), .p_blimit(2), .p_bsize(4), .ans(l_currCSize));
    l_bestCSize = (l_bestCSize > l_currCSize) ? l_currCSize : l_bestCSize;

    t_CompressionData [(p_blockSize/2)-1:0] l_values2;
    c_convertBuffer2Array #(p_blockSize, 2) conv2(.p_buffer(p_buffer), .l_values(l_values2));
    c_multBaseCompression #(p_blockSize/2, p_step) mbc2_1(.p_values(l_values2), .p_blimit(1), .p_bsize(2), .ans(l_currCSize));
    l_bestCSize = (l_bestCSize > l_currCSize) ? l_currCSize : l_bestCSize;
end
endmodule

module c_FPCCompress #(
    parameter t_BlockOffset p_size,
    parameter t_Size p_step
) (
    input t_Data p_buffer,
    output t_Size fpc_ans
);
always_comb begin
    t_CompressionData [p_size-1:0] l_values;
    c_convertBuffer2Array #(p_size*4, 4) conv2(.p_buffer(p_buffer), .l_values(l_values));
    t_Size l_compressable = 0;
    t_Size i;
    for (i = 0; i < p_size; i++)  begin
        //  000
        if (l_values[i] == 0) begin
            l_compressable += 1;
            continue;
        end
        // 001 010
        t_Size l_data = l_values[i];
        if (ABS(l_data) <= 0xFF)  begin
            l_compressable += 1;
            continue;
        end
        // 011
        if (ABS(l_data) <= 0xFFFF) begin
            l_compressable += 2;
            continue;
        end
        // 100
        if ((l_values[i] & 0xFFFF) == 0) begin
            l_compressable += 2;
            continue;
        end
        // 101
        t_Size l_data_a = l_values[i] & 0xFFFF;
        t_Size l_data_b = (l_values[i] >> 16) & 0xFFFF;
        if (ABS(l_data_a) <= 0xFF && ABS(l_data_b) <= 0xFF)  begin
            l_compressable += 2;
            continue;
        end
        // 110
        t_Size byte0 = (l_values[i]) & 0xFF;
        t_Size byte1 = (l_values[i] >> 8) & 0xFF;
        t_Size byte2 = (l_values[i] >> 16) & 0xFF;
        t_Size byte3 = (l_values[i] >> 24) & 0xFF;
        if (byte0 == byte1 && byte0 == byte2 && byte0 == byte3)  begin
            l_compressable += 1;
            continue;
        end
        // 111
        l_compressable += 4;
    end
    // 6 bytes for 3 bit per every 4-byte word in a 64 byte cache line
    t_BlockOffset l_compSize = l_compressable + p_size * 3 / 8;
    t_Size ans;
    if (l_compSize < p_size * 4)
        fpc_ans = l_compSize;
    else
        fpc_ans = p_size * 4;
    
end
endmodule

module c_Compression #(
    parameter t_Size p_step,
    parameter t_BlockOffset p_size
) (
    input t_Data p_buffer, 
    input t_BlockOffset p_blockSize, 
    input t_CompressionAlgorithm p_compressAlgorithm,
    output t_Size compressed
); // any input outputs if required that is

always_comb begin
    t_Size l_BDISize, l_FPCSize;
    t_CompressionData [p_blockSize-1:0] l_values;
    // l_values = f_convertBuffer2Array(p_buffer, p_blockSize * 4, 4);
    c_convertBuffer2Array #(p_blockSize*4, 4) o1(.p_buffer(p_buffer), .bufferArray(l_values))

    case (p_compressAlgorithm)  begin
    E_COMPRESS_NO: return p_blockSize;
    E_COMPRESS_ZERO: begin
        t_Size compress_zero;
        c_isZeroPackable #(p_blockSize*4, 4) o2(.p_values(l_values), .l_notSame(compress_zero));
        return compress_zero;
    end
    E_COMPRESS_FVC: begin
        // l_values = f_convertBuffer2Array(p_buffer, p_blockSize * 4, 4);
        t_Size compress_fvc;
        c_isSameValuePackable #(p_blockSize*4, 4) o2(.p_values(l_values), .l_notSame(compress_fvc));
        return compress_fvc;
    end
    E_COMPRESS_BDI_ALGO: begin
        // return f_BDICompress(p_buffer, p_blockSize);
        t_Size compress_bdi;
        c_BDICompress o3(.p_buffer(p_buffer), .p_blockSize(p_blockSize), .l_bestCSize(compress_bdi));
        return compress_bdi;
    end
    E_COMPRESS_FPC_ALGO: begin
        // return f_BDICompress(p_buffer, p_blockSize);
        // return f_FPCCompress(p_buffer, p_blockSize / 4);
        t_Size compress_fpc;
        c_FPCCompress #(p_size, p_step) o4(.p_buffer(p_buffer), .fpc_ans(compress_fpc));
        return compress_fpc;
    end
    E_COMPRESS_BEST: begin
        t_Size compress_fpc;
        c_FPCCompress #(p_size, p_step) o4(.p_buffer(p_buffer), .fpc_ans(compress_fpc));
        t_Size compress_bdi;
        c_BDICompress #(p_size, p_step) o3(.p_buffer(p_buffer), .p_blockSize(p_blockSize), .l_bestCSize(compress_bdi));

        if (compress_bdi <= compress_fpc)
            return l_BDISize;
        else
            return l_FPCSize;
    end
    default:
        assert(0) else begin
            $display("Error: Invalid compression algorithm encountered.");
            $fatal(1, "Exiting due to invalid compression algorithm.");
        end
    end
    endcase
    
end
endmodule

// make everything module format