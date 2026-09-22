typedef struct Quad {
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
} Quad;

extern Quad *func_8007FC28(void);

Quad *func_80080CDC(Quad *dst)
{
    Quad *r;

    if (dst == 0) {
        r = func_8007FC28();
    } else {
        *dst = *func_8007FC28();
        r = dst;
    }
    return r;
}
