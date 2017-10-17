#ifndef MD5_H_
#define MD5_H_
#include<iostream>
#include<string>
#include<cstring>
using namespace std;

// 四个轮函数
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// x循环左移n位
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

// 四个转换函数
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + ac; \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + ac; \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + ac; \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + ac; \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
}

/* Constants for MD5Transform routine. */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21


class Md5 {
private:
    typedef unsigned char BYTE;   // one byte, 8 bits
    typedef unsigned short BYTE2; // two bytes, 16 bits
    typedef unsigned long BYTE4;  // four bytes, 32 bits;
private:
    static const unsigned char PADDING[64];
    static const char HEX[16];
private:
    BYTE4 state[4];   // state (ABCD)
    BYTE4 count[2];   // number of bits, modulo 2^64(lsb first)
    BYTE buffer[64];  // input buffer, 512 bits
    BYTE digest[16];  // store the result, 128 bits

    bool working;
private:
    void init();

    void update(BYTE* input, unsigned input_len);
    void final();
    void transform(BYTE block[64]);

    void encode(BYTE* output, BYTE4* input, unsigned int len);
    void decode(BYTE4* output, BYTE* input, unsigned int len);

    void md5_memcpy(BYTE* output, BYTE* input, unsigned int len);
    void md5_memset(BYTE* output, int value, unsigned int len);
public:
    Md5();

    void update(const char* text);
    void update(const string text);

    string hexdigest();
};

// 用于第一步填充padding，由于填充长度为1~512，该数组共有512位
// 第一位为1，其余位为0(PADDING[0]==0x80,PADDING[1~63]==0)
const unsigned char Md5::PADDING[64] = { 0x80 };

const char Md5::HEX[16] = {'0', '1', '2', '3',
                           '4', '5', '6', '7',
                           '8', '9', 'a', 'b',
                           'c', 'd', 'e', 'f'};

Md5::Md5() {
    this->init();
}

void Md5::update(const char* text) {
    working = true;
    update((BYTE*)text, strlen(text));
}
void Md5::update(const string text) {
    working = true;
    update((BYTE*)text.c_str(), text.size());
}

string Md5::hexdigest() {
    string res = "";
    if (!working) {
        cout << "[ERROR] use update function first!\n";
        return res;
    }
    // 获得结果之前才进行final操作，因为多次update是允许的
    final();

    for(int i = 0; i < 16; i++) {
        int t = digest[i];
        int a = t / 16;
        int b = t % 16;
        res += HEX[a];
        res += HEX[b];
    }
    // 输出结果后，初始化Md5
    this->init();
    return res;
}

void Md5::init() {
    working = false;

    count[0] = count[1] = 0;
    // 初始化4个寄存器
    state[0] = 0x67452301;
    state[1] = 0xefcdab89;
    state[2] = 0x98badcfe;
    state[3] = 0x10325476;
}

/*
 * 总控流程
 * 对前L-1个消息分组执行压缩算法。
 */
void Md5::update(BYTE* input, unsigned input_len) {
    unsigned int i, index, part_len;

    index = (unsigned int)((count[0] >> 3) & 0x3f);

    if ((count[0] += ((BYTE4)input_len << 3)) < ((BYTE4)input_len << 3))
        count[1]++;
    count[1] += ((BYTE4)input_len >> 29);

    part_len = 64 - index;

    if (input_len >= part_len) {
        md5_memcpy(&buffer[index], input, part_len);
        transform(buffer);

        for (i = part_len; i + 63 < input_len; i += 64)
            transform(&input[i]);
        index = 0;
    } else {
        i = 0;
    }

    md5_memcpy(&buffer[index], &input[i], input_len - i);
}

/*
 * 算法的第一步是在原始消息数据尾部填充标识 100…0，填充后的消息
 * 位数 L=448 (mod 512)。至少要填充1个位，所以标识长度 1~512位。
 * 所以实际上final函数的作用就是：填充padding并且对最后这个填充
 * 出来的512位的消息分组进行update操作，即进行最后一次的压缩算法。
 * 由于这是流程的第一步，可能会误以为一定要在一开始进行填充，其实
 * 不必，在最后再填充并且走一遍压缩函数也是一样的。
 */
void Md5::final() {
    BYTE bits[8];
    unsigned int index, pad_len;

    // bits是填充在最末尾的原消息长度值的低64位
    encode(bits, this->count, 8);
    // 在原始消息数据尾部填充标识 100…0，填充后的消息位数
    // L = 448 (mod 512)
    index = (count[0] >> 3) & 0x3f;
    pad_len = index < 56 ? 56 - index : 120 - index;
    update((BYTE*)PADDING, pad_len);

    update(bits, 8);

    // 得到最后的结果，储存在this->digest中。
    encode(this->digest, this->state, 16);

    //md5_memset()

}

/*
 * MD5压缩函数
 * @param block 代表当前的消息分组，512 bits
 */
void Md5::transform(BYTE block[64]) {
    // state保存的是上一次迭代的结果
    // 上一次迭代的结果作为这次迭代的输入
    BYTE4 a = state[0];
    BYTE4 b = state[1];
    BYTE4 c = state[2];
    BYTE4 d = state[3];
    // x[]是这一次参与迭代的512位消息分组
    BYTE4 x[16];
    decode(x, block, 64);

    // 经过四轮循环
    /* Round 1 */
    FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
    FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
    FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
    FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
    FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
    FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
    FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
    FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
    FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

    /* Round 2 */
    GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
    GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
    GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
    GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
    GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
    GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
    GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
    GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
    GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
    GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
    GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

    /* Round 3 */
    HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
    HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
    HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
    HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
    HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
    HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
    HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
    HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
    HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
    HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

    /* Round 4 */
    II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
    II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
    II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
    II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
    II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
    II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
    II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
    II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
    II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
    II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

/*
 * Encode input(BYTE4, 32 bits unit) into output(BYTE, 8 bits unit).
 * Assumes len is a multiple of 4.
 * @param len the length of input
 */
void Md5::encode(BYTE* output, BYTE4* input, unsigned int len) {
    unsigned int i, j;
    for (i = 0, j = 0; j < len; ++i, j += 4) {
        output[j] = (BYTE)(input[i] & 0xff);
        output[j + 1] = (BYTE)((input[i] >> 8) & 0xff);
        output[j + 2] = (BYTE)((input[i] >> 16) & 0xff);
        output[j + 3] = (BYTE)((input[i] >> 24) & 0xff);
    }
}

/*
 * Decode input(BYTE, 8 bits unit) into output(BYTE4, 32 bits unit).
 * Assumes len is a multiple of 4.
 * @param len the length of input
 */
void Md5::decode(BYTE4* output, BYTE* input, unsigned int len) {
    unsigned int i, j;
    for (i = 0, j = 0; j < len; ++i, j += 4) {
        output[i] = ((BYTE4)input[j] |
                     (((BYTE4)input[j + 1]) << 8) |
                     (((BYTE4)input[j + 2]) << 16) |
                     (((BYTE4)input[j + 3]) << 24));
    }
}

void Md5::md5_memcpy(BYTE* output, BYTE* input, unsigned int len) {
    for (unsigned int i = 0; i < len; ++i)
        output[i] = input[i];
}

void Md5::md5_memset(BYTE* output, int value, unsigned int len) {
    for (unsigned int i = 0; i < len; ++i)
        ((char*)output)[i] = (char)value;
}

#endif
