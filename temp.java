 private static byte[] getLengthByte(int length){
        if(length >> 28 > 0){
            throw new IllegalArgumentException("长度超出范围:0-0xfffffff");
        }
        byte[] full = new byte[4];
        int point = 0;
        while(length > 0){
            full[point] = (byte) (length % 128);
            length = length / 128;
            if(length > 0){
                full[point] = (byte) (full[point] | 0x80);
            }
            point++;
        }
        byte[] reByte = new byte[point];
        System.arraycopy(full, 0, reByte, 0, point);
        return reByte;
    }

    private static long getLengthLong(byte[] byteLength){
        long len = 0;
        int multiplier = 1;
        for(byte b: byteLength){
            len += (b & 0x7F) * multiplier;
            multiplier *= 0x80;
        }
        return len;
    }

The algorithm for encoding a decimal number (X) into the variable length encoding scheme is as follows:

    do
      digit = X MOD 128
      X = X DIV 128
      // if there are more digits to encode, set the top bit of this digit
      if ( X > 0 )
        digit = digit OR 0x80
      endif
      'output' digit
    while ( X> 0 )

where MOD is the modulo operator (% in C), DIV is integer division (/ in C), and OR is bit-wise or (| in C).The algorithm for decoding the Number Length field is as follows:

    multiplier = 1
    value = 0
    do
      digit = 'next digit from stream'
      value += (digit AND 127) * multiplier
      multiplier *= 128
    while ((digit AND 128) != 0)