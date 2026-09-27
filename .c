typedef struct {
    unsigned char f0;
    unsigned char f1;
    unsigned short v[3];
    unsigned char pad8[6];
    unsigned short nb[3];
    unsigned char pad14[2];
} EntA; /* 22 bytes */

typedef struct {
    unsigned char pad0[8];
    unsigned short v[3];
    unsigned char padE[6];
    unsigned short nb[3];
    unsigned char pad1A[2];
} EntB; /* 28 bytes */

typedef struct {
    unsigned short x;
    unsigned short z;
} VtxA;

typedef struct {
    unsigned short x;
    unsigned short y;
    unsigned short z;
} VtxB;

typedef struct {
    unsigned char pad0[0x18];
    void *f18;
    void *f1C;
} Hdr;

extern void *D_8009D1D8;
extern Hdr *D_8009D1FC;

extern int func_8001C614(void *a0, int a1, int a2);

void *func_8001C164(void *t, void *exclude, short x0, short z0, short x1, short z1) {
    int dx;
    int dz;
    short xmax;
    short xmin;
    short zmax;
    short zmin;
    unsigned short cz;
    unsigned short cx;
    unsigned short i;
    unsigned short pz;
    unsigned short px;
    void *r;
    int ex;
    int ez;
    int den;
    int n;

    dx = x1 - x0;
    dz = z1 - z0;
    if (dx < 0) {
        xmax = x0;
        xmin = x1;
    } else {
        xmax = x1;
        xmin = x0;
    }
    if (dz < 0) {
        zmax = z0;
        zmin = z1;
    } else {
        zmax = z1;
        zmin = z0;
    }
    if (D_8009D1D8) {
        VtxB *v = (VtxB *)D_8009D1FC->f18 + ((EntB *)t)->v[2];
        cx = v->x;
        cz = v->z;
    } else {
        VtxA *v = (VtxA *)D_8009D1FC->f18 + ((EntA *)t)->v[2];
        cx = v->x;
        cz = v->z;
    }
    for (i = 0; i < 3; i++) {
        px = cx;
        pz = cz;
        if (D_8009D1D8) {
            VtxB *v = (VtxB *)D_8009D1FC->f18 + ((EntB *)t)->v[i];
            cx = v->x;
            cz = v->z;
        } else {
            VtxA *v = (VtxA *)D_8009D1FC->f18 + ((EntA *)t)->v[i];
            cx = v->x;
            cz = v->z;
        }
        ex = (short)cx - (short)px;
        if (ex > 0) {
            if (xmax < (short)px) continue;
            if (!(xmin < (short)cx)) continue;
        } else {
            if (xmax < (short)cx) continue;
            if (!(xmin < (short)px)) continue;
        }
        ez = (short)cz - (short)pz;
        if (ez > 0) {
            if (zmax < (short)pz) continue;
            if (!(zmin < (short)cz)) continue;
        } else {
            if (zmax < (short)cz) continue;
            if (!(zmin < (short)pz)) continue;
        }
        den = dz * ex - dx * ez;
        n = ez * (x0 - (short)cx) - ex * (z0 - (short)cz);
        if (den > 0) {
            if (n < 0) continue;
            if (den < n) continue;
        } else {
            if (n > 0) continue;
            if (n < den) continue;
        }
        n = dx * (z0 - (short)cz) - dz * (x0 - (short)cx);
        if (den > 0) {
            if (n < 0) continue;
            if (den < n) continue;
        } else {
            if (n > 0) continue;
            if (n < den) continue;
        }
        if (den == 0) continue;
        if (D_8009D1D8 == 0) {
            r = (EntA *)D_8009D1FC->f1C + ((EntA *)t)->nb[i];
            if (r == exclude) continue;
            if (func_8001C614(r, x0, z0)) return r;
            r = func_8001C164(r, t, x0, z0, x1, z1);
            if (r) return r;
        } else {
            r = (EntB *)D_8009D1FC->f1C + ((EntB *)t)->nb[i];
            if (r == exclude) continue;
            if (func_8001C614(r, x0, z0)) return r;
            r = func_8001C164(r, t, x0, z0, x1, z1);
            if (r) return r;
        }
    }
    return 0;
}
