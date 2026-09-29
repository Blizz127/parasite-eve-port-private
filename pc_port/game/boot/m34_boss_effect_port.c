/* M34 effect 8: original Disc 1 C2, LBA16597, 100 sectors.
 * Overlay SHA256 0eb2efb10e4779672a00f6da46c2d54f915f1b3e433048513fd08de296eedd5a.
 * Persistent state remains in guest RAM. Untranslated child callbacks still
 * stop through the shared callback registry; no effect is silently omitted. */
#include "psx_compat.h"
#include "pe_port_compat.h"
#include "pe_sdk.h"
#include "game_port.h"

/* Retained words below the original F434 entry SP. The original field
 * call history (M34_FRAME_STACK_PRESERVATION.md) establishes the writers.
 * Preserve data from calls, never a captured tuple. Unverified callback
 * graphs invalidate the record; subsequent projectiles need fresh writers.
 * This does not model asynchronous BIOS/IRQ stack activity. */
static struct {
    uint64_t generation;
    unsigned field,vm,known,snaps;
    pe_addr_t actor,opcode,sound_actor,callback;
    int32_t words[6];
    int32_t sound_words[3];
    int32_t construct_word;
    int32_t command_words[2];
} m34_stack;

static void m34_stack_trace(const char *event,pe_addr_t detail)
{
    if(!getenv("PE_M34_STACK_TRACE") || !PE_M34BossEffectOverlay())return;
    fprintf(stderr,"M34_STACK %s detail=%08X tick=%08X mask=%02X words=%08X,%08X,%08X,%08X,%08X,%08X\n",
        event,detail,D_8009D250,m34_stack.known,(uint32_t)m34_stack.words[0],
        (uint32_t)m34_stack.words[1],(uint32_t)m34_stack.words[2],(uint32_t)m34_stack.words[3],
        (uint32_t)m34_stack.words[4],(uint32_t)m34_stack.words[5]);
}

static int m34_stack_keep(void)
{
    /* Original F434 words live on the CPU stack across Aya's nested field/VM
     * work. Connected kite runs reached known=0x38/0x32 because those paths
     * wiped the bit-mask while ClipSound snapshot words survived. */
    return m34_stack.known && m34_stack.field && PE_M34BossEffectOverlay();
}
void PE_M34StackInvalidate(void) { m34_stack.known=0u; }
void PE_M34StackField(int begin)
{
    if(m34_stack.generation!=PE_RamGeneration()) {
        memset(&m34_stack,0,sizeof(m34_stack));
        m34_stack.generation=PE_RamGeneration();
    }
    if(!PE_M34BossEffectOverlay())m34_stack.known=0u;
    else if(begin && m34_stack.field && !m34_stack_keep())m34_stack.known=0u;
    m34_stack.field=begin!=0;
    m34_stack.actor=0u;m34_stack.opcode=0u;m34_stack.vm=0u;
    m34_stack.callback=0u;
}
void PE_M34StackActor(pe_addr_t actor) { m34_stack.actor=actor; }
void PE_M34StackVm(int begin)
{
    if(begin) {
        m34_stack.vm++;
        if((m34_stack.vm!=1u || !m34_stack.field) && !m34_stack_keep())
            m34_stack.known=0u;
    } else if(m34_stack.vm)m34_stack.vm--;
}
void PE_M34StackOpcode(pe_addr_t fn)
{
    m34_stack.opcode=fn;
    if(!fn)return;
    /* Original dispatches in the continuously traced M34 command history.
     * Sound/effect calls have additional writer hooks below. */
    switch(fn) {
    case 0x80012850u:case 0x8001731Cu:case 0x80017294u:case 0x800172E0u:
    case 0x80014DA0u:case 0x800179F8u:case 0x800173F4u:case 0x80018004u:
    case 0x80014694u:case 0x80017A50u:case 0x80014228u:case 0x80019154u:
    case 0x80018080u:case 0x80019C4Cu:case 0x80012E7Cu:case 0x80015648u:
    case 0x800143B0u:case 0x80019BE4u:case 0x800184ECu:case 0x800187C0u:
    case 0x80018774u:case 0x800172FCu:return;
    default:
        if(!m34_stack_keep())m34_stack.known=0u;
        return;
    }
}
static int m34_stack_script(pe_addr_t opcode)
{
    return m34_stack.generation==PE_RamGeneration() && m34_stack.field &&
        m34_stack.actor && m34_stack.vm==1u && m34_stack.opcode==opcode &&
        PE_M34BossEffectOverlay();
}
void PE_M34StackClipSound(pe_addr_t body,pe_addr_t actor,uint32_t volume)
{
    /* Once effect-8 construct+command are sampled, Aya's pistol 184EC must
     * not steal sound_actor (connected kite left boss snap then overwrote). */
    if(PE_M34BossEffectOverlay()) {
        if((m34_stack.known&7u)==7u)return;
        if((m34_stack.snaps&6u)==6u && m34_stack.sound_actor &&
           actor!=m34_stack.sound_actor)
            return;
    }
    if(!m34_stack_script(0x800184ECu) || volume>127u) {
        if(!m34_stack_keep())m34_stack.known=0u;
        return;
    }
    /* 6DED8/6DEE0 save the 2FAF8 caller's body/target registers;
     * 6E184 writes the computed volume into the preceding word. */
    m34_stack.words[0]=(int32_t)volume;m34_stack.words[1]=(int32_t)body;
    m34_stack.words[2]=(int32_t)actor;m34_stack.sound_actor=actor;
    m34_stack.sound_words[0]=(int32_t)volume;m34_stack.sound_words[1]=(int32_t)body;
    m34_stack.sound_words[2]=(int32_t)actor;
    m34_stack.snaps|=1u;
    m34_stack.known|=7u;
    m34_stack_trace("clip-sound",actor);
}
void PE_M34StackConstruct(uint32_t code)
{
    if(!m34_stack_script(0x80018774u) || code!=8u ||
       PE_LoadU8(0x800B0DC7u)!=0u || !(D_8009D1A0&0x80u) ||
       (PE_LoadU32(0x800B0CD8u)&8u)) {
        if(!m34_stack_keep())m34_stack.known=0u;
        return;
    }
    /* 17020/17024 initialize S1 to this original decode table. 6916C
     * saves that register through 18774 -> 6F39C -> 6914C. */
    m34_stack.words[3]=(int32_t)0x80010690u;
    m34_stack.construct_word=(int32_t)0x80010690u;
    m34_stack.snaps|=2u;
    m34_stack.known|=8u;
    m34_stack_trace("constructor",code);
}
void PE_M34StackCallback(pe_addr_t fn)
{
    m34_stack.callback=fn;
    if(!PE_M34BossEffectOverlay()) {
        if(m34_stack.known)m34_stack_trace("unknown-callback",fn);
        m34_stack.known=0u;
        return;
    }
    switch(fn) {
    case 0x8018F0E4u:case 0x8018F12Cu:case 0x8018F1B8u:
    case 0x8018F23Cu:case 0x8018F244u:case 0x8018F24Cu:
    case 0x8018F36Cu:case 0x8018F374u:case 0x8018F380u:
    case 0x8018F3BCu:case 0x8018F3C4u:case 0x8018F434u:
    case 0x8018F830u:case 0x8018FC54u:case 0x8018FDD4u:
    case 0x8018FDE4u:case 0x8018FEE0u:
        return;
    /* Translated PE_WeaponCallback leaves. Aya's pistol draw/update during
     * M34 used to clear the F434 record (kite/reload pilot, f=60323). */
    case 0x800CE1FCu:case 0x800CE2B4u:case 0x800CE3B4u:case 0x800CE3ACu:
    case 0x800CE464u:case 0x800CE470u:
    case 0x800CD980u:case 0x800CDA5Cu:case 0x800CDC24u:
    case 0x800CDD0Cu:case 0x800CDE90u:case 0x800CDD04u:
    case 0x800C9C20u:case 0x800C9C8Cu:case 0x800C9D9Cu:
    case 0x800C9EA8u:case 0x800C9FD8u:case 0x800C9EA0u:
    case 0x800CA4A8u:case 0x800CDF40u:case 0x800CA4B4u:
    case 0x800CA540u:case 0x800CDF4Cu:case 0x800CDFE0u:
        return;
    default:
        /* Connected known=0x32: an unlisted callback wiped the mask while
         * ClipSound snapshot + words[3..5] survived. Keep overlay-live data. */
        if(m34_stack_keep()) {
            m34_stack_trace("kept-callback",fn);
            return;
        }
        if(m34_stack.known)m34_stack_trace("unknown-callback",fn);
        m34_stack.known=0u;
        return;
    }
}
static int m34_stack_drawing(pe_addr_t callback)
{
    return m34_stack.generation==PE_RamGeneration() && m34_stack.field &&
        m34_stack.callback==callback && PE_M34BossEffectOverlay();
}
static void m34_stack_low_half(unsigned word,int16_t value)
{
    m34_stack.words[word]=(int32_t)(((uint32_t)m34_stack.words[word]&0xFFFF0000u)|(uint16_t)value);
    /* A partial store cannot establish the untouched high half. */
}
void PE_M34StackPointQuad(const int16_t vertices[4][4])
{
    if(!m34_stack_drawing(0x8018F830u))return;
    /* C62A0/C6264/C625C/C62B8 at the F830 -> C61A8 depth.
     * The fourth vertex's Y is zero; the surrounding padding is retained. */
    m34_stack_low_half(0,vertices[2][2]);
    m34_stack.words[1]=(int32_t)((uint16_t)vertices[3][0]|((uint32_t)(uint16_t)vertices[3][1]<<16u));
    m34_stack_low_half(2,vertices[3][2]);m34_stack.known|=2u;
}
int PE_M34StackRead(int32_t retained[6])
{
    if(m34_stack.generation!=PE_RamGeneration() || !m34_stack.field ||
       !PE_M34BossEffectOverlay())return 0;
    if((m34_stack.known&0x38u)==0x38u && (m34_stack.known&7u)!=7u &&
       m34_stack.sound_actor &&
       (uint32_t)m34_stack.words[2]==(uint32_t)m34_stack.sound_actor)
        m34_stack.known|=7u;
    if(m34_stack.known!=63u)return 0;
    memcpy(retained,m34_stack.words,sizeof(m34_stack.words));return 1;
}

static void boss_projectile_update(pe_addr_t slot,pe_addr_t rec,pe_addr_t data);
static void boss_trail_draw(pe_addr_t data);
static void boss_projectile_draw(pe_addr_t slot,pe_addr_t rec,pe_addr_t data);

int PE_M34BossEffectOverlay(void)
{
    return PE_LoadU32(0x8018F014u)==0x0C0308BEu &&
        PE_LoadU32(0x8018F020u)==0x2463FF98u &&
        PE_LoadU32(0x8018FF80u)==0x8018F00Cu &&
        PE_LoadU32(0x8018FF3Cu)==0x8018F434u;
}

/* Full 43 words 8018F00C..8018F0B8. */
int PE_M34BossEffectInit(pe_addr_t slot)
{
    PE_StoreU32(func_800C22F8(slot),0x8018FF98u);
    PE_StoreU8(0x80190054u,32u);PE_StoreU8(0x80190055u,3u);
    PE_StoreU8(0x80190064u,43u);PE_StoreU8(0x80190065u,2u);
    PE_StoreU16(0x8019006Au,128u);
    PE_StoreU16(0x80190058u,0u);PE_StoreU16(0x8019005Au,0u);
    PE_StoreU8(0x80190056u,0u);PE_StoreU16(0x80190068u,0u);
    PE_StoreU8(0x80190060u,128u);PE_StoreU8(0x80190061u,128u);
    PE_StoreU8(0x80190062u,128u);PE_StoreU8(0x80190066u,0u);
    return 0;
}

/* Full 11 words F0B8..F0E4. Args five/six are copied by F0C8/F0D0;
 * C2AF0 ignores them, but F434 later observes those retained stack words. */
int PE_M34BossEffectCommand(pe_addr_t slot,uint32_t mode,uint32_t index,uint32_t value,uint32_t extra0,uint32_t extra1)
{
    if(m34_stack_script(0x800187C0u)) {
        m34_stack.words[4]=(int32_t)extra0;m34_stack.words[5]=(int32_t)extra1;
        m34_stack.command_words[0]=(int32_t)extra0;
        m34_stack.command_words[1]=(int32_t)extra1;
        m34_stack.snaps|=4u;
        m34_stack.known|=48u;
        m34_stack_trace("command",slot);
    } else if(!m34_stack_keep()) {
        m34_stack.known=0u;
    }
    (void)func_800C2AF0(slot,(int32_t)mode,(int32_t)index,value);
    return 0;
}

/* Full 33 words 8018F1B8..8018F23C. */
int PE_M34BossEffectCleanup(pe_addr_t slot)
{
    PE_StoreU8(slot,4u);
    if ((unsigned)func_800C6CE0(slot)>=2u) {
        pe_addr_t body=PE_LoadU32(PE_LoadU32(slot+8u));
        if(body<0x200000u)body|=0x80000000u;
        PE_StoreU32(body,PE_LoadU32(body)&0xC0FFFFFFu);
        body=PE_LoadU32(PE_LoadU32(slot+8u));
        pe_addr_t action=PE_LoadU32((body<0x200000u?body|0x80000000u:body)+24u);
        PE_StoreU8(action<0x200000u?action|0x80000000u:action,4u);
    }
    return 0;
}

/* Full 18 words F0E4..F12C and 35 words F12C..F1B8. */
int PE_M34BossEffectDraw(pe_addr_t slot)
{
    if(func_800C6CE0(slot)==3)(void)func_800C2414(slot,0x8018FF48u);
    return 0;
}
int PE_M34BossEffectUpdate(pe_addr_t slot)
{
    int result=-1;
    if(func_800C6CE0(slot)==3) {
        result=func_800C251C(slot,0x8018FF5Cu);
        result|=func_800C2758(slot,0x8018FF34u,0x8018FF70u);
    }
    if(result==-1)(void)PE_M34BossEffectCleanup(slot);
    return 0;
}

/* Original grouped loads precede stores, including overlapping operands. */
static void copy_words(pe_addr_t dst,pe_addr_t src,unsigned count,unsigned batch)
{
    for(unsigned i=0;i<count;) {
        uint32_t words[4];unsigned n=count-i<batch?count-i:batch;
        for(unsigned j=0;j<n;j++)words[j]=PE_LoadU32(src+(i+j)*4u);
        for(unsigned j=0;j<n;j++)PE_StoreU32(dst+(i+j)*4u,words[j]);
        i+=n;
    }
}

/* Returns whether this original child entry is translated. */
int PE_M34BossEffectChild(pe_addr_t fn,pe_addr_t slot,pe_addr_t rec,pe_addr_t data)
{
    switch(fn) {
    case 0x8018F434u: {
        int32_t retained[6];
        pe_addr_t owner=PE_LoadU32(slot+8u);
        m34_stack_trace("projectile",data);
        /* Connected kite: words[] is mutated (PointQuad / other scripts) and
         * known bits are partial (0x32/0x38). F434 inputs are the three
         * writer snapshots — same data the original stack slots held. */
        if(owner!=m34_stack.sound_actor || m34_stack.snaps!=7u ||
           (uint32_t)m34_stack.sound_words[2]!=owner) {
            fprintf(stderr,
                    "[EFFECT] F434 stack not ready slot8=%08X sound=%08X known=%02X snaps=%u field=%u w2=%08X snap2=%08X w3=%08X cw=%08X\n",
                    owner,(unsigned)m34_stack.sound_actor,
                    m34_stack.known,m34_stack.snaps,m34_stack.field,
                    (uint32_t)m34_stack.words[2],(uint32_t)m34_stack.sound_words[2],
                    (uint32_t)m34_stack.words[3],(uint32_t)m34_stack.construct_word);
            return 0;
        }
        retained[0]=m34_stack.sound_words[0];
        retained[1]=m34_stack.sound_words[1];
        retained[2]=m34_stack.sound_words[2];
        retained[3]=m34_stack.construct_word;
        retained[4]=m34_stack.command_words[0];
        retained[5]=m34_stack.command_words[1];
        PE_M34BossProjectileInit(data,retained);
        /* The two 79754 rotation outputs stop before their translation
         * words. Full original F434 preserves all six retained inputs. */
        return 1;
    }
    case 0x8018F24Cu: {
        pe_addr_t actor=PE_LoadU32(slot+8u);
        PE_StoreU32(0x80190048u,actor);
        pe_addr_t matrix=PE_LoadU32(actor+0x238u);
        actor=PE_LoadU32(0x80190048u);
        copy_words(0x80190028u,matrix,8u,3u);
        matrix=PE_LoadU32(actor+0x238u);
        copy_words(0x80190070u,matrix+0x620u,8u,4u);
        if(PE_LoadU32(slot+8u)==PE_LoadU32(0x8009D254u))
            copy_words(0x80190070u,PE_LoadU32(PE_LoadU32(0x80190048u)+0x238u),8u,4u);
        return 1;
    }
    case 0x8018F36Cu:case 0x8018F3BCu:return 1;
    case 0x8018F374u:PE_StoreU8(rec+1u,2u);return 1;
    case 0x8018F380u:
        PE_StoreU16(data,0u);
        PE_StoreU16(data+2u,(uint16_t)PE_LoadU32(PE_LoadU32(0x800E2248u)+24u));return 1;
    case 0x8018F3C4u: {
        pe_addr_t value=PE_LoadU32(0x800E2248u)+24u;
        PE_StoreU32(value,(uint32_t)(int32_t)(int16_t)PE_LoadU16(data+2u));
        (void)func_800C2B90(slot,2u,0x8018FF70u,0x8018FF34u);
        PE_StoreU8(rec+1u,2u);return 1;
    }
    case 0x8018FDD4u:
        PE_StoreU8(data+2u,0u);PE_StoreU16(data+4u,128u);return 1;
    case 0x8018FC54u:boss_projectile_update(slot,rec,data);return 1;
    case 0x8018FDE4u:boss_trail_draw(data);return 1;
    case 0x8018F830u:boss_projectile_draw(slot,rec,data);return 1;
    case 0x8018FEE0u: {
        uint8_t time=(uint8_t)(PE_LoadU8(data+2u)+1u);
        PE_StoreU8(data+2u,time);
        if((int8_t)time>=21)PE_StoreU8(rec+1u,2u);
        int value=(int16_t)PE_LoadU16(data+4u);
        if(value>=9)PE_StoreU16(data+4u,(uint16_t)(value-8));
        return 1;
    }
    default:return 0;
    }
}

/* F434..F830, all 255 original words. The original retains translations
 * below the incoming stack pointer; callers must supply those six words.
 * The field dispatcher supplies a verified writer record when available. */
static void boss_rotate_vector(const int16_t rotation[9],const int16_t in[3],int16_t out[3])
{
    for(unsigned i=0;i<9;i++)g_pe_gte.rt[i/3u][i%3u]=rotation[i];
    PE_GTE_SetV0(in[0],in[1],in[2]);PE_GTE_MVMVA(0x486012u);
    for(unsigned i=0;i<3;i++)out[i]=(int16_t)g_pe_gte.ir[i];
}
static void boss_compose(pe_addr_t dest,const int16_t left[9],const int32_t translation[3],
                         const int16_t right[9],const int32_t position[3])
{
    for(unsigned i=0;i<9;i++)g_pe_gte.rt[i/3u][i%3u]=left[i];
    for(unsigned col=0;col<3;col++) {
        PE_GTE_SetIR(right[col],right[col+3u],right[col+6u]);
        PE_GTE_MVMVA(0x49E012u);
        for(unsigned row=0;row<3;row++)PE_StoreU16(dest+(row*3u+col)*2u,(uint16_t)g_pe_gte.ir[row]);
    }
    for(unsigned i=0;i<3;i++)g_pe_gte.tr[i]=translation[i];
    PE_GTE_SetV0((int16_t)position[0],(int16_t)position[1],(int16_t)position[2]);
    PE_GTE_MVMVA(0x480012u);
    for(unsigned i=0;i<3;i++)PE_StoreU32(dest+20u+i*4u,(uint32_t)g_pe_gte.ir[i]);
}
void PE_M34BossProjectileInit(pe_addr_t data,const int32_t retained[6])
{
    int16_t yaw[9],rotation[9],angles[3],in[3]={0,0,0},velocity[3];
    for(unsigned i=0;i<3;i++)angles[i]=(int16_t)PE_LoadU16(0x8018EFF4u+i*2u);
    pe_addr_t param=PE_LoadU32(0x800E2248u)+24u;
    pe_addr_t command=PE_LoadU32(0x800E2248u)+80u;
    int16_t turn[3]={0,(int16_t)(PE_LoadU32(param)+PE_LoadU32(command)),0};
    PE_RotMatrix79754_values(turn,yaw);
    command=PE_LoadU32(0x800E2248u)+72u;
    in[2]=(int16_t)(0u-PE_LoadU32(command));
    PeEffectMatrix matrix;PE_EffectReadMatrix(0x80190028u,&matrix);
    boss_rotate_vector(matrix.r,in,velocity);
    for(unsigned i=0;i<3;i++)PE_StoreU16(data+24u+i*2u,(uint16_t)velocity[i]);
    boss_rotate_vector(yaw,velocity,velocity);
    for(unsigned i=0;i<3;i++)PE_StoreU16(data+24u+i*2u,(uint16_t)velocity[i]);
    copy_words(data+36u,0x80190028u,8u,4u);
    PE_StoreU16(data+8u,(uint16_t)PE_LoadU32(0x80190084u));
    PE_StoreU16(data+10u,(uint16_t)(PE_LoadU16(0x800942ECu)-256u));
    uint32_t z=PE_LoadU32(0x8019008Cu);
    PE_StoreU16(data+4u,128u);
    PE_StoreU16(data+16u,0u);PE_StoreU16(data+18u,0u);PE_StoreU16(data+20u,0u);
    PE_StoreU16(data+6u,0u);PE_StoreU8(data+2u,0u);PE_StoreU8(data,1u);
    PE_StoreU16(data+12u,(uint16_t)z);
    PE_StoreU8(data+1u,(uint8_t)PE_LoadU32(PE_LoadU32(0x800E2248u)+20u));
    PE_RotMatrix79754_values(angles,rotation);
    PE_EffectReadMatrix(data+36u,&matrix);
    boss_compose(data+36u,matrix.r,matrix.t,rotation,retained);
    PE_EffectReadMatrix(data+36u,&matrix);
    boss_compose(data+36u,yaw,retained+3u,matrix.r,matrix.t);
}

/* Full 96 words FC54..FDD4. Keep both polygon calls: allocation can write
 * guest storage between them. The signed byte remainder follows MIPS div. */
static void boss_projectile_update(pe_addr_t slot,pe_addr_t rec,pe_addr_t data)
{
    uint16_t x=PE_LoadU16(data+8u),vx=PE_LoadU16(data+24u);
    uint16_t vy=PE_LoadU16(data+26u),vz=PE_LoadU16(data+28u);
    PE_StoreU16(data+8u,(uint16_t)(x+vx));
    uint16_t y=PE_LoadU16(data+10u),z=PE_LoadU16(data+12u);
    PE_StoreU16(data+10u,(uint16_t)(y+vy));
    uint16_t size=PE_LoadU16(data+4u);
    PE_StoreU16(data+12u,(uint16_t)(z+vz));
    int fade=(int16_t)PE_LoadU16(data+6u);
    PE_StoreU16(data+4u,(uint16_t)(size+30u));
    if(fade<129)PE_StoreU16(data+6u,(uint16_t)(fade+10));
    PE_StoreU8(data+2u,(uint8_t)(PE_LoadU8(data+2u)+1u));
    if(func_8001CAB0((int32_t)((uint32_t)PE_LoadU16(data+8u)<<16u),
                    (int32_t)((uint32_t)PE_LoadU16(data+12u)<<16u),
                    PE_LoadU32(0x8009D248u),PE_LoadU16(0x8009D1CCu)) &&
       (int8_t)PE_LoadU8(data+2u)%3==0) {
        pe_addr_t trail=func_800C2B90(slot,3u,0x8018FF70u,0x8018FF34u);
        if(trail) {
            PE_StoreU16(trail+8u,PE_LoadU16(data+8u));
            PE_StoreU16(trail+10u,PE_LoadU16(0x800942ECu));
            PE_StoreU16(trail+12u,PE_LoadU16(data+12u));
        }
    }
    if(!func_8001CAB0((int32_t)((uint32_t)PE_LoadU16(data+8u)<<16u),
                     (int32_t)((uint32_t)PE_LoadU16(data+12u)<<16u),
                     PE_LoadU32(0x8009D248u),PE_LoadU16(0x8009D1CCu))) {
        PE_StoreU8(rec+1u,2u);PE_StoreU8(data,0u);
    }
}

/* 78CC4 scales columns with wrapping low-word multiplication. Its last
 * store also replaces the matrix padding with the upper product halfword. */
static void boss_scale(PeEffectMatrix *matrix,const int32_t scale[3])
{
    for(unsigned i=0;i<9;i++) {
        int32_t value=(int32_t)((uint32_t)(int32_t)matrix->r[i]*(uint32_t)scale[i%3u])>>12;
        matrix->r[i]=(int16_t)value;
        if(i==8)matrix->pad=(int16_t)(value>>16);
    }
}

/* Full 63 words FDE4..FEE0, including the complete shared quad renderer. */
static void boss_trail_draw(pe_addr_t data)
{
    func_800C2EAC(0u);func_800C3098(16);func_800C2FF0(32u,32u);func_800C3238(2u);
    PeEffectMatrix matrix={{4096,0,0,0,4096,0,0,0,4096},0,{0,0,0}};
    PE_StoreU16(0x8019006Au,PE_LoadU16(data+4u));
    for(unsigned i=0;i<3;i++)matrix.t[i]=(int16_t)PE_LoadU16(data+8u+i*2u);
    int32_t scale[3];
    for(unsigned i=0;i<3;i++)scale[i]=(int32_t)PE_LoadU32(0x8018EFFCu+i*4u);
    boss_scale(&matrix,scale);
    /* FDE4 -> C42A4's C4544 writes only the low half of retained word3,
     * before camera transformation changes this local matrix translation. */
    if(m34_stack_drawing(0x8018FDE4u))m34_stack_low_half(3,(int16_t)matrix.t[2]);
    PE_EffectQuadC42A4(0x80190060u,&matrix,1u);
}

/* Full 265 words F830..FC54. Drawing also performs the original collision
 * and hit-flag writes; all five quads remain in original submission order. */
static void boss_projectile_draw(pe_addr_t slot,pe_addr_t rec,pe_addr_t data)
{
    const pe_addr_t style=0x80190050u;
    func_800C2EAC(0u);func_800C3098(16);func_800C2FF0(64u,32u);func_800C3238(2u);
    if((int8_t)PE_LoadU8(data)!=1)return;
    PE_StoreU8(style,16u);PE_StoreU8(style+1u,16u);PE_StoreU8(style+2u,32u);
    PE_StoreU16(style+10u,PE_LoadU16(data+6u));
    PeEffectMatrix matrix;PE_EffectReadMatrix(data+36u,&matrix);
    /* Retail F918/F930 subtract X velocity from Y and Z as well. */
    for(unsigned i=0;i<3;i++)matrix.t[i]=(int16_t)PE_LoadU16(data+8u+i*2u)-(int16_t)PE_LoadU16(data+24u);
    int32_t scale[3]={(int16_t)PE_LoadU16(data+4u)>>1,593,593};
    boss_scale(&matrix,scale);PE_EffectQuadC42A4(style,&matrix,0u);
    PE_StoreU8(style,32u);PE_StoreU8(style+1u,32u);PE_StoreU8(style+2u,64u);
    PE_EffectReadMatrix(data+36u,&matrix);
    for(unsigned i=0;i<3;i++)matrix.t[i]=(int16_t)PE_LoadU16(data+8u+i*2u);
    scale[0]=(int16_t)PE_LoadU16(data+4u);scale[1]=1096;scale[2]=1096;
    boss_scale(&matrix,scale);
    /* F830's second 78CC4 call replaces these complete words at
     * 78D0C/78D4C. Later non-billboard quads leave this matrix intact. */
    if(m34_stack_drawing(0x8018F830u)) {
        for(unsigned i=0;i<2;i++)m34_stack.words[4u+i]=(int32_t)((uint16_t)matrix.r[i*2u]|
            ((uint32_t)(uint16_t)matrix.r[i*2u+1u]<<16u));
        m34_stack.known|=48u;
    }
    PE_EffectQuadC42A4(style,&matrix,0u);
    int16_t point[3];pe_addr_t aya=PE_LoadU32(0x8009D254u);
    for(unsigned i=0;i<3;i++)point[i]=(int16_t)PE_LoadU16(aya+42u+i*4u);
    if(PE_EffectPointQuadC61A8(point,&matrix) && func_800C6CE0(slot)==3) {
        pe_addr_t body=PE_LoadU32(PE_LoadU32(0x8009D254u));
        PE_StoreU32(body+76u,PE_LoadU32(body+76u)|0x4000u);
        pe_addr_t owner=PE_LoadU32(slot+8u);
        if(owner) {
            body=PE_LoadU32(owner);PE_StoreU32(body,PE_LoadU32(body)|0x80000000u);
        }
        PE_StoreU8(rec+1u,2u);
    }
    PE_StoreU8(style,120u);PE_StoreU8(style+1u,240u);PE_StoreU8(style+2u,120u);
    matrix.t[1]=(int16_t)PE_LoadU16(data+10u)-256;
    PE_EffectQuadC42A4(style,&matrix,0u);
    for(unsigned age=1;age<=2;age++) {
        for(unsigned i=0;i<3;i++)matrix.t[i]=(int16_t)PE_LoadU16(data+8u+i*2u)-
            (int32_t)age*(int16_t)PE_LoadU16(data+24u+i*2u);
        PE_StoreU8(style,(uint8_t)(age==1?60:30));
        PE_StoreU8(style+1u,(uint8_t)(age==1?120:60));
        PE_StoreU8(style+2u,(uint8_t)(age==1?60:30));
        matrix.t[1]=(int16_t)PE_LoadU16(data+10u)-256;
        PE_EffectQuadC42A4(style,&matrix,0u);
    }
}

/* ── M34 phase 2 (sewer tunnel, room m0348i, token A8064448) ──────────────
 * Line-for-line from the matched room leaves src/overlays/room_m0348i/
 * func_8018F06C.c and func_8018F0C4.c (the overlay generator rejects both:
 * callee arity / pointer-to-pointer parameter).  Resident check = the
 * retail words at both entry points (build/extracted room_m0348i_c2.bin). */
int PE_M0348iEffectOverlay(void)
{
    return PE_LoadU32(0x8018F06Cu)==0x27BDFFE8u && PE_LoadU32(0x8018F074u)==0x0C0308BEu &&
        PE_LoadU32(0x8018F080u)==0x24632928u &&
        PE_LoadU32(0x8018F0D0u)==0x0C031B38u && PE_LoadU32(0x8018F0D8u)==0x24030003u;
}

/* func_8018F06C: *func_800C22F8() = (int)D_80192928; return 0; */
int PE_M0348iEffectInit(pe_addr_t slot)
{
    PE_StoreU32(func_800C22F8(slot), 0x80192928u);
    return 0;
}

/* func_8018F0C4: if (func_800C6CE0(o) == 3) { w = P(P(o, 8), 0);
 * *w &= 0xBFFFFFFF; func_800C2414(o, D_80192880); } return 0; */
int PE_M0348iEffectCallback(pe_addr_t o)
{
    if (func_800C6CE0(o) == 3) {
        pe_addr_t w = PE_LoadU32(PE_LoadU32(o + 8u));
        PE_StoreU32(w, PE_LoadU32(w) & 0xBFFFFFFFu);
        (void)func_800C2414(o, 0x80192880u);
    }
    return 0;
}

/* room_m0348i func_8018F1B4 (masked twin of room_m0034i func_8018F1B8 =
 * PE_M34BossEffectCleanup): o[0] = 4; if (func_800C6CE0(o) >= 2) {
 * W(P(P(o,8),0),0) &= 0xC0FFFFFF; *(u8 *)P(P(P(o,8),0),0x18) = 4; } */
int PE_M0348iEffectCleanup(pe_addr_t o)
{
    return PE_M34BossEffectCleanup(o);   /* identical body */
}

/* room_m0348i func_8018F128 (masked twin of room_m0034i func_8018F12C =
 * PE_M34BossEffectUpdate): state 3 runs the two effect-VM tables
 * D_801928B8 / (D_80192848, D_801928F0); -1 -> func_8018F1B4(o). */
int PE_M0348iEffectUpdate(pe_addr_t o)
{
    int r = -1;
    if (func_800C6CE0(o) == 3) {
        r = func_800C251C(o, 0x801928B8u);
        r |= func_800C2758(o, 0x80192848u, 0x801928F0u);
    }
    if (r == -1)
        (void)PE_M0348iEffectCleanup(o);
    return 0;
}

/* room_m0348i func_8018F240 (effect-VM init callback; masked twin of room_m0174i
 * func_80190358), line-for-line from src/overlays/room_m0348i/func_8018F240.c.
 * a0 = slot, a2 = effect data block. */
void func_800C2B40(unsigned int arg0);   /* generated TU */
pe_addr_t func_800C2B28(int arg0);       /* decomp_hand/small_hi_port.c */
static void m0348i_copy32(pe_addr_t dst, pe_addr_t src)
{
    uint32_t w[8];
    for (unsigned i = 0; i < 8u; i++) w[i] = PE_LoadU32(src + i * 4u);   /* loads first */
    for (unsigned i = 0; i < 8u; i++) PE_StoreU32(dst + i * 4u, w[i]);
}
void PE_M0348iEffectF240(pe_addr_t a0, pe_addr_t a2)
{
    pe_addr_t x, y, pkg;
    int32_t t;
    func_800C2B40(a2);
    pkg = PE_LoadU32(0x800B0E64u);
    if (pkg < 0x200000u) pkg |= 0x80000000u;
    t = (int32_t)func_8006E498(pkg, 0x118704u);
    x = PE_LoadU32(a0 + 8u);
    PE_StoreU32(a2, x);
    y = PE_LoadU32(x + 0x238u);
    PE_StoreU32(0x80192E1Cu, (uint32_t)t);
    m0348i_copy32(a2 + 4u, y);
    m0348i_copy32(a2 + 0x24u, PE_LoadU32(PE_LoadU32(a2) + 0x238u) + 0xA0u);
    PE_StoreU32(a2 + 0x44u, (uint32_t)func_8006DC18(9));
    PE_StoreU16(a2 + 0x4Au, 0);
    PE_StoreU16(a2 + 0x4Cu, 0);
    PE_StoreU16(a2 + 0x48u, 0x96u);
    PE_StoreU16(a2 + 0x4Eu, (uint16_t)PE_LoadU32(func_800C2B28(0)));
    PE_StoreU32(a2 + 0x50u, PE_LoadU32(func_800C2B28(1)));
    PE_StoreU32(a2 + 0x54u, PE_LoadU32(func_800C2B28(2)));
    PE_StoreU32(a2 + 0x58u, PE_LoadU32(func_800C2B28(3)));
    PE_StoreU8(0x80192E04u, 0x22u);
    PE_StoreU8(0x80192E05u, 0x30u);
    PE_StoreU8(0x80192E24u, 0x40u);
    PE_StoreU8(0x80192E25u, 0x5u);
    PE_StoreU16(0x80192E18u, 0x64u);
    PE_StoreU8(0x80192EB4u, 0x6u);
    PE_StoreU16(0x80192E08u, 0x0u);
    PE_StoreU16(0x80192E0Au, 0x80u);
    PE_StoreU8(0x80192E00u, 0x80u);
    PE_StoreU8(0x80192E01u, 0x80u);
    PE_StoreU8(0x80192E02u, 0x80u);
    PE_StoreU8(0x80192E06u, 0x0u);
    PE_StoreU16(0x80192E28u, 0x0u);
    PE_StoreU16(0x80192E2Au, 0x80u);
    PE_StoreU8(0x80192E20u, 0x80u);
    PE_StoreU8(0x80192E21u, 0x80u);
    PE_StoreU8(0x80192E22u, 0x80u);
    PE_StoreU8(0x80192E26u, 0x0u);
    PE_StoreU16(0x80192EC8u, 0x0u);
    PE_StoreU16(0x80192ECAu, 0x80u);
    PE_StoreU8(0x80192EC0u, 0x80u);
    PE_StoreU8(0x80192EC1u, 0x80u);
    PE_StoreU8(0x80192EC2u, 0x80u);
    PE_StoreU8(0x80192EC6u, 0x0u);
    PE_StoreU8(0x80192E14u, 0x68u);
    PE_StoreU8(0x80192E15u, 0x7u);
    PE_StoreU16(0x80192E1Au, 0x80u);
    PE_StoreU8(0x80192E10u, 0x80u);
    PE_StoreU8(0x80192E11u, 0x80u);
    PE_StoreU8(0x80192E12u, 0x80u);
    PE_StoreU8(0x80192E16u, 0x0u);
    PE_StoreU8(0x80192EB5u, 0x60u);
    PE_StoreU8(0x80192E44u, 0x2u);
    PE_StoreU16(0x80192EB8u, 0x0u);
    PE_StoreU16(0x80192EBAu, 0x60u);
    PE_StoreU8(0x80192EB0u, 0x80u);
    PE_StoreU8(0x80192EB1u, 0x80u);
    PE_StoreU8(0x80192EB2u, 0x80u);
    PE_StoreU8(0x80192EB6u, 0x0u);
    PE_StoreU8(0x80192E45u, 0x20u);
    PE_StoreU16(0x80192E48u, 0x0u);
    PE_StoreU16(0x80192E4Au, 0x60u);
    PE_StoreU8(0x80192E40u, 0x80u);
    PE_StoreU8(0x80192E41u, 0x80u);
    PE_StoreU8(0x80192E42u, 0x80u);
    PE_StoreU8(0x80192E46u, 0x0u);
    PE_StoreU8(0x80192E34u, 0x68u);
    PE_StoreU8(0x80192E35u, 0x7u);
    PE_StoreU16(0x80192E38u, 0x0u);
    PE_StoreU16(0x80192E3Au, 0x80u);
    PE_StoreU8(0x80192E36u, 0x0u);
}

/* ── M34 phase 2 (m0348i) effect-VM callbacks, round 2 ─────────────────────
 * Line-for-line from src/overlays/room_m0348i/func_{801923B0,8018F568,
 * 801918E8,80191E1C,80190D88}.c.  EXE helpers with matched C but no
 * pc_port definition are translated file-locally below (src/func_800C2B50.c,
 * src/func_800C2B68.c, src/func_800C3134.c).  Stack locals whose address is
 * passed to a callee live in the scratch window PE_M0348I_STACK. */
#define PE_M0348I_STACK 0x801FF480u   /* +0x00 MATRIX m, +0x20 VECTOR v, +0x30 VECTOR s */

int func_800C6800(pe_addr_t a0, pe_addr_t a1, pe_addr_t a2);
void func_800C2EAC(unsigned int a0);
int func_800C2FF0(unsigned char arg0, unsigned char arg1);
void func_800C3098(int32_t colors);
void func_800C3238(unsigned blend);
void func_800C42A4(pe_addr_t style, pe_addr_t matrix, unsigned billboard);
void func_800C4E50(pe_addr_t r);
pe_addr_t func_800C2B10(int arg0);
pe_addr_t func_80078CC4(pe_addr_t matrix, pe_addr_t scale);
pe_addr_t func_80071A44(pe_addr_t dst, int32_t fillbyte, int32_t len);
int func_800C6C18(pe_addr_t a0);

/* src/func_800C2B50.c: return D_800E2248[0x1C]; */
static pe_addr_t m0348i_c2b50(void) { return PE_LoadU32(PE_LoadU32(0x800E2248u) + 0x70u); }
/* src/func_800C2B68.c: (D_800E2248->value & 0xFFFF0000) == 0x01010000 */
static int m0348i_c2b68(void) { return (PE_LoadU32(PE_LoadU32(0x800E2248u) + 4u) & 0xFFFF0000u) == 0x01010000u; }
/* src/func_800C3134.c: colour-ramp lookup (4-byte keys: rgb + duration). */
static void m0348i_c3134(pe_addr_t a0, uint32_t a1, pe_addr_t a2)
{
    uint32_t acc = 0;
    pe_addr_t p = a0;
    for (;;) {
        uint32_t d = PE_LoadU8(p + 3u);
        pe_addr_t next;
        acc += d;
        next = p + 4u;
        if (a1 < acc) {
            uint32_t t = ((acc - a1) << 12) / d;
            uint32_t u = 0x1000u - t;
            uint32_t m0 = u * PE_LoadU8(p + 4u), m1 = t * PE_LoadU8(p + 0u);
            uint32_t m2 = u * PE_LoadU8(p + 5u), m3 = t * PE_LoadU8(p + 1u);
            uint32_t m4 = u * PE_LoadU8(p + 6u), m5 = t * PE_LoadU8(p + 2u);
            PE_StoreU8(a2 + 0u, (uint8_t)((m0 + m1) >> 12));
            PE_StoreU8(a2 + 1u, (uint8_t)((m2 + m3) >> 12));
            PE_StoreU8(a2 + 2u, (uint8_t)((m4 + m5) >> 12));
            return;
        }
        if (PE_LoadU8(p + 7u) != 0xFFu) { p = next; continue; }
        PE_StoreU8(a2 + 0u, PE_LoadU8(p + 4u));
        PE_StoreU8(a2 + 1u, PE_LoadU8(p + 5u));
        PE_StoreU8(a2 + 2u, PE_LoadU8(p + 6u));
        return;
    }
}
static void m0348i_copy_words(pe_addr_t dst, pe_addr_t src, unsigned n)
{
    uint32_t w[16];
    for (unsigned i = 0; i < n; i++) w[i] = PE_LoadU32(src + i * 4u);
    for (unsigned i = 0; i < n; i++) PE_StoreU32(dst + i * 4u, w[i]);
}
#define S16AT_(a) ((int32_t)(int16_t)PE_LoadU16(a))

/* func_801923B0: draw up to 8 flare sprites. */
static void m0348i_801923B0(pe_addr_t a2)
{
    const pe_addr_t m = PE_M0348I_STACK, v = PE_M0348I_STACK + 0x20u, s = PE_M0348I_STACK + 0x30u;
    pe_addr_t x = m0348i_c2b50();
    unsigned i;
    func_800C2EAC(PE_LoadU8(x + 0x44u));
    (void)func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    for (i = 0; i < 8u; i++) {
        if (PE_LoadU8(a2 + i + 0xB4u) != 0) {
            PE_StoreU16(m + 0x10u, 0x1000); PE_StoreU16(m + 0x08u, 0x1000); PE_StoreU16(m + 0x00u, 0x1000);
            PE_StoreU32(m + 0x1Cu, 0); PE_StoreU32(m + 0x18u, 0); PE_StoreU32(m + 0x14u, 0);
            PE_StoreU16(m + 0x0Eu, 0); PE_StoreU16(m + 0x0Cu, 0); PE_StoreU16(m + 0x0Au, 0);
            PE_StoreU16(m + 0x06u, 0); PE_StoreU16(m + 0x04u, 0); PE_StoreU16(m + 0x02u, 0);
            (void)func_80071A44(v, 0, 0x10);
            PE_StoreU32(v + 0u, (uint32_t)S16AT_(a2 + i * 2u + 0x84u));
            PE_StoreU32(v + 4u, (uint32_t)S16AT_(a2 + i * 2u + 0x84u));
            PE_StoreU32(v + 8u, 0x1000u);
            m0348i_copy_words(s, v, 4u);
            (void)func_80078CC4(m, s);
            PE_StoreU16(0x80192E3Au, PE_LoadU16(a2 + i * 2u + 0x94u));
            PE_StoreU32(m + 0x14u, (uint32_t)S16AT_(a2 + i * 8u + 4u));
            PE_StoreU32(m + 0x18u, (uint32_t)S16AT_(a2 + i * 8u + 6u));
            PE_StoreU32(m + 0x1Cu, (uint32_t)S16AT_(a2 + i * 8u + 8u));
            m0348i_c3134(0x801929E8u, (uint32_t)S16AT_(a2 + i * 2u + 0xA4u), 0x80192E30u);
            func_800C42A4(0x80192E30u, m, 1u);
        }
    }
}

/* func_8018F568: reload timer; retire the record when the scene flag is set. */
static void m0348i_8018F568(pe_addr_t a0, pe_addr_t a1, pe_addr_t t)
{
    if (PE_LoadU16(t + 0x4Au))
        PE_StoreU16(t + 0x4Au, (uint16_t)(PE_LoadU16(t + 0x4Au) - 1u));
    if ((int16_t)PE_LoadU16(t + 0x4Cu) == 1) {
        PE_StoreU16(t + 0x4Cu, 0);
        if (PE_LoadU16(t + 0x4Au) == 0) {
            PE_StoreU16(t + 0x4Au, PE_LoadU16(t + 0x48u));
            (void)func_800C6C18(a0);   /* retail also passes u in a1; the leaf takes one */
        }
    }
    if (m0348i_c2b68() == 1)
        PE_StoreU8(a1 + 1u, 2u);
}

/* func_801918E8: two ribbon emitters (D_80192E80 / D_80192E98 blocks). */
static void m0348i_801918E8(pe_addr_t a2)
{
    pe_addr_t x = m0348i_c2b50();
    const pe_addr_t b = 0x80192E8Cu;
    const pe_addr_t p = 0x80192C00u;
    m0348i_copy_words(a2, PE_LoadU32(PE_LoadU32(x) + 0x238u) + 0xA0u, 8u);
    PE_StoreU16(a2 + 0x30u, 0x4B0u);
    PE_StoreU16(a2 + 0x34u, 0xFFu);
    PE_StoreU16(b, 0x10u);
    PE_StoreU16(0x80192E92u, (uint16_t)-0x1F4);
    PE_StoreU16(0x80192E94u, 0x80u);
    PE_StoreU8(0x80192E88u, 0xFAu); PE_StoreU8(0x80192E89u, 0xFAu); PE_StoreU8(0x80192E8Au, 0x78u);
    PE_StoreU8(0x80192E84u, 0); PE_StoreU8(0x80192E85u, 0); PE_StoreU8(0x80192E86u, 0);
    PE_StoreU16(0x80192E8Eu, 0xC80u);
    PE_StoreU16(0x80192E90u, 0x578u);
    PE_StoreU32(b - 0xCu, p);
    func_800C4E50(b - 0xCu);
    PE_StoreU16(0x80192EA4u, 0x10u);
    PE_StoreU16(0x80192EAAu, (uint16_t)-0x1F4);
    PE_StoreU16(0x80192EACu, 0x80u);
    PE_StoreU8(0x80192EA0u, 0xFFu); PE_StoreU8(0x80192EA1u, 0xFFu); PE_StoreU8(0x80192EA2u, 0xFFu);
    PE_StoreU8(0x80192E9Cu, 0xFAu); PE_StoreU8(0x80192E9Du, 0xFAu); PE_StoreU8(0x80192E9Eu, 0x78u);
    PE_StoreU16(0x80192EA8u, 0);
    PE_StoreU16(0x80192EA6u, 0x578u);
    PE_StoreU32(b + 0xCu, p + 0x100u);
    func_800C4E50(b + 0xCu);
}

/* func_80191E1C: four random-sized glow particles. */
static void m0348i_80191E1C(pe_addr_t o)
{
    static const uint8_t col[4][3][2] = {   /* (offset, value) in source order */
        {{0x20,0x9F},{0x21,0x8A},{0x22,0x89}}, {{0x21,0x4C},{0x20,0x40},{0x22,6}},
        {{0x20,0x12},{0x21,0x20},{0x22,0x40}}, {{0x20,0x9F},{0x21,0x62},{0x22,0x89}}};
    pe_addr_t x = m0348i_c2b50();
    unsigned i, k;
    m0348i_copy_words(o, x + 4u, 8u);
    for (i = 0; i < 4u; i++) {
        uint32_t t = func_80071A54() % 40u + 10u;
        (void)func_80071A54();
        PE_StoreU16(o + i * 8u + 0x32u, (uint16_t)-200);
        PE_StoreU16(o + i * 8u + 0x34u, (uint16_t)-200);
        PE_StoreU16(o + i * 8u + 0x50u, (uint16_t)(0u - (t >> 1)));
        PE_StoreU16(o + i * 8u + 0x52u, (uint16_t)(0u - (t >> 1)));
        PE_StoreU16(o + i * 8u + 0x30u, 0);
        PE_StoreU16(o + i * 2u + 0x70u, (uint16_t)(t << 6));
        PE_StoreU16(o + i * 2u + 0x78u, 0xA0u);
        for (k = 0; k < 3u; k++)
            PE_StoreU8(o + i * 4u + col[i][k][0], col[i][k][1]);
    }
}

/* func_80190D88: 16-segment trail seeded from the scene block. */
static void m0348i_80190D88(pe_addr_t o, pe_addr_t p)
{
    pe_addr_t r = m0348i_c2b50();
    unsigned i;
    m0348i_copy_words(p + 0x160u, r + 4u, 8u);
    PE_StoreU16(p + 0x180u, (uint16_t)PE_LoadU32(func_800C2B10(1)));
    PE_StoreU16(p + 0x182u, (uint16_t)PE_LoadU32(func_800C2B28(8)));
    PE_StoreU16(p + 0x184u, 0);
    for (i = 0; i < 16u; i++) {
        PE_StoreU16(p + 0x60u + i * 8u + 0u, 0);
        PE_StoreU16(p + 0x60u + i * 8u + 2u, (uint16_t)-300);
        PE_StoreU16(p + 0x60u + i * 8u + 4u, (uint16_t)(i * 120u));
        PE_StoreU16(p + 0xE0u + i * 8u + 0u, 0);
        PE_StoreU16(p + 0xE0u + i * 8u + 2u, 0);
        PE_StoreU16(p + 0xE0u + i * 8u + 4u, 0);
        PE_StoreU16(p + 0x40u + i * 2u, 2048u);
        PE_StoreU16(p + 0x20u + i * 2u, (uint16_t)(96u - i * 3u));
        PE_StoreU8(p + i, 1u);
    }
    (void)func_800C6800(o, 1389u, p + 0x60u);
}

/* Weapon/effect-VM dispatch (PE_WeaponCallback): 1 = handled. */
int PE_M0348iEffectChild(pe_addr_t fn, pe_addr_t slot, pe_addr_t rec, pe_addr_t data)
{
    if (!PE_M0348iEffectOverlay()) return 0;
    switch (fn) {
    case 0x801923B0u: m0348i_801923B0(data); return 1;
    case 0x8018F568u: m0348i_8018F568(slot, rec, data); return 1;
    case 0x801918E8u: m0348i_801918E8(data); return 1;
    case 0x80191E1Cu: m0348i_80191E1C(data); return 1;
    case 0x80190D88u: m0348i_80190D88(slot, data); return 1;
    case 0x80192388u:   /* func_80192388: p->w0 = 0; p->flags[0..7] = 0 (+0xB4) */
        PE_StoreU32(data, 0);
        for (unsigned i = 0; i < 8u; i++) PE_StoreU8(data + 0xB4u + i, 0);
        return 1;
    case 0x8018F560u: return 1;   /* func_8018F560: empty leaf */
    default: return 0;
    }
}
