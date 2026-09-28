/* vendor-derived: khasinski/parasite-eve-decomp@3067919e7b82f0334b873117eef07e7157f4ed69 src/main/psyq/libcd/StSetRing.c — attribution khasinski (Chris Hasinski) */
/* Imported under the owner's explicit authorization (2026-09-28, via Devbox Coordinator, vendor lane).
 * Upstream tree kept verbatim in vendor/khasinski-parasite-eve-decomp/ (see its PROVENANCE.md).
 * This file is that vendor TU run through the era cpp with the vendor CPP flags
 * (-undef -P, -I include, -I tools/m2c), `StSetRing` renamed to `func_8007A214`, and vendor symbol
 * names mapped (post-cpp, token-wise) to this repo's address names from the vendor sym tables. */
/* vendor markers: ASSEMBLER: GNU */
/* Single-function extract: every other function definition of the vendor TU is reduced to a
 * prototype so this file compiles only `func_8007A214` (the TU spans several of this repo's yaml functions). */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef void (*PsyqInterruptHandler)(void);
int func_80073C94(void);
int func_80073D88(void);
int func_80073DB8(void);
int func_80073DE8(void);
int func_80073D24(PsyqInterruptHandler callback);
PsyqInterruptHandler func_80073CC4(int channel, PsyqInterruptHandler callback);
PsyqInterruptHandler func_80073CF4(int channel, PsyqInterruptHandler callback);
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef void (*CdlCB)(u_char event, u_char *result);
typedef void (*DsCallback)(void);
typedef void (*DsEventCallback)(u_char event, u_char *result);
typedef struct CdInterruptEvents {
    volatile u8 sync;
    volatile u8 ready;
    volatile u8 end;
} CdInterruptEvents;
extern CdInterruptEvents D_8009B294;
extern volatile u8 *g_CdRegIndexBase asm("D_8009B27C");
extern volatile u8 *g_CdRegPort1 asm("D_8009B280");
extern volatile u8 *g_CdRegDataWrite asm("D_8009B284");
extern volatile u8 *D_8009B288;
extern u32 *D_8009B28C;
typedef union CdDmaInterruptRegister {
    volatile u32 word;
    volatile u8 bytes[4];
} CdDmaInterruptRegister;
extern CdDmaInterruptRegister *D_8009B348;
extern volatile u32 *D_8009B344;
extern volatile u8 *D_8009B32C;
extern char D_80011C2C[];
extern u8 *D_800C0DB8;
typedef struct CdCallbackDataPage {
    char reserved00[4];
    CdlCB sync;
    CdlCB ready;
    int reserved0C;
    CdlCB func_80072754;
    int status;
    char reserved18[0x5038];
} CdCallbackDataPage;
typedef struct CdlLOC {
    u_char minute;
    u_char second;
    u_char sector;
    u_char track;
} CdlLOC;
typedef struct StHEADER {
    u_short id;
    u_short type;
    u_short secCount;
    u_short nSectors;
    u32 frameCount;
    u32 frameSize;
    u_short width;
    u_short height;
    u32 dummy1;
    u32 dummy2;
    CdlLOC loc;
} StHEADER;
typedef char pe1_static_assert_st_header_size [( sizeof(StHEADER) == 0x20 ) ? 1 : -1] ;
typedef char pe1_static_assert_st_header_frame_count_offset [( ((u32)&((( StHEADER  *)0)->  frameCount ))  == 0x08 ) ? 1 : -1] ;
typedef char pe1_static_assert_st_header_location_offset [( ((u32)&((( StHEADER  *)0)->  loc ))  == 0x1C ) ? 1 : -1] ;
extern StHEADER *StRingAddr asm("D_800C0DC8");
extern u32 StRingSize asm("D_800C20C4");
extern u8 *g_CdRingBufPtr asm("D_800C0DC8");
extern int D_800BE9E4;
extern int D_800B89F4;
extern int D_800BE998;
extern CdlLOC D_800A3490;
extern int D_800A3494;
extern int D_800A8020;
extern DsCallback D_800B0CC8;
int func_8007AA34(CdlLOC *p);
CdlLOC *func_8007A930(int i, CdlLOC *p);
void func_8007C214(void);
int func_8007C2A0(CdlLOC *position);
typedef struct CdlATV {
    u_char val0;
    u_char val1;
    u_char val2;
    u_char val3;
} CdlATV;
typedef struct DslFILE {
    CdlLOC pos;
    u_int size;
    char name[16];
} DslFILE;
typedef struct DslDirectoryCacheEntry {
    int directoryId;
    int parentDirectoryId;
    u_int sector;
    char name[32];
} DslDirectoryCacheEntry;
enum {
    DSL_MAX_FILE = 64,
    DSL_MAX_DIR = 128,
};
typedef struct DsDecodedEventFlags {
    u_char bit7;
    u_char bit6;
    u_char bit5;
    u_char bit1;
} DsDecodedEventFlags;
typedef struct DsReadStatusBlock {
    u_int status;
    u_int command;
    u_int sector;
    u_char lastCommand;
    u_char commandMode;
    CdlLOC currentPos;
    u_char retryCount;
    u_char commandParam;
    DsDecodedEventFlags eventFlags;
    u_int reserved18;
    u_int discType;
    u_int syncResult;
    u_int readyResult;
} DsReadStatusBlock;
typedef struct CdRomCommandState {
    u_char eventStatus;
    u_char reserved01[3];
    u_int eventValue;
    DsReadStatusBlock func_80072754;
    u_int retryAttempts;
    u_int reserved34;
} CdRomCommandState;
typedef struct CdRomSystemState {
    u_int enabled;
    u_char pendingCommand;
    u_char pendingMode;
    u_char reserved06[6];
    u_char preSeekState[0x0C];
    CdRomCommandState command;
} CdRomSystemState;
typedef struct CdRomEventCommandState {
    u_char pendingCommand;
    u_char pendingParamBytes[4];
    u_char reserved05[3];
    u_char *pendingParams;
    u_char reserved0C[8];
    CdRomCommandState command;
} CdRomEventCommandState;
typedef char pe1_static_assert_ds_read_status_block_size [( sizeof(DsReadStatusBlock) == 0x28 ) ? 1 : -1] ;
typedef char pe1_static_assert_dsl_file_size [( sizeof(DslFILE) == 0x18 ) ? 1 : -1] ;
typedef char pe1_static_assert_dsl_file_size_offset [( ((u32)&((( DslFILE  *)0)->  size ))  == 0x04 ) ? 1 : -1] ;
typedef char pe1_static_assert_dsl_file_name_offset [( ((u32)&((( DslFILE  *)0)->  name ))  == 0x08 ) ? 1 : -1] ;
typedef char pe1_static_assert_dsl_directory_cache_entry_size [( sizeof(DslDirectoryCacheEntry) == 0x2C ) ? 1 : -1] ;
typedef char pe1_static_assert_dsl_directory_cache_id_offset [( ((u32)&((( DslDirectoryCacheEntry  *)0)->  directoryId ))  ==
                      0x00 ) ? 1 : -1] ;
typedef char pe1_static_assert_dsl_directory_cache_parent_id_offset [( ((u32)&((( DslDirectoryCacheEntry  *)0)->  parentDirectoryId ))  ==
                      0x04 ) ? 1 : -1] ;
typedef char pe1_static_assert_dsl_directory_cache_sector_offset [( ((u32)&((( DslDirectoryCacheEntry  *)0)->  sector ))  == 0x08 ) ? 1 : -1] ;
typedef char pe1_static_assert_dsl_directory_cache_name_offset [( ((u32)&((( DslDirectoryCacheEntry  *)0)->  name ))  == 0x0C ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_callback_data_page_size [( sizeof(CdCallbackDataPage) == 0x5050 ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_callback_data_page_sync_offset [( ((u32)&((( CdCallbackDataPage  *)0)->  sync ))  == 0x04 ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_callback_data_page_ready_offset [( ((u32)&((( CdCallbackDataPage  *)0)->  ready ))  == 0x08 ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_callback_data_page_read_offset [( ((u32)&((( CdCallbackDataPage  *)0)->  func_80072754 ))  == 0x10 ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_callback_data_page_status_offset [( ((u32)&((( CdCallbackDataPage  *)0)->  status ))  == 0x14 ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_read_status_last_command_offset [( ((u32)&((( DsReadStatusBlock  *)0)->  lastCommand ))  == 0x0C ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_read_status_current_pos_offset [( ((u32)&((( DsReadStatusBlock  *)0)->  currentPos ))  == 0x0E ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_read_status_retry_count_offset [( ((u32)&((( DsReadStatusBlock  *)0)->  retryCount ))  == 0x12 ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_read_status_event_flags_offset [( ((u32)&((( DsReadStatusBlock  *)0)->  eventFlags ))  == 0x14 ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_read_status_disc_type_offset [( ((u32)&((( DsReadStatusBlock  *)0)->  discType ))  == 0x1C ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_read_status_ready_result_offset [( ((u32)&((( DsReadStatusBlock  *)0)->  readyResult ))  == 0x24 ) ? 1 : -1] ;
typedef char pe1_static_assert_cdrom_command_state_size [( sizeof(CdRomCommandState) == 0x38 ) ? 1 : -1] ;
typedef char pe1_static_assert_cdrom_command_read_offset [( ((u32)&((( CdRomCommandState  *)0)->  func_80072754 ))  == 0x08 ) ? 1 : -1] ;
typedef char pe1_static_assert_cdrom_system_state_size [( sizeof(CdRomSystemState) == 0x50 ) ? 1 : -1] ;
typedef char pe1_static_assert_cdrom_system_pending_command_offset [( ((u32)&((( CdRomSystemState  *)0)->  pendingCommand ))  == 0x04 ) ? 1 : -1] ;
typedef char pe1_static_assert_cdrom_system_command_offset [( ((u32)&((( CdRomSystemState  *)0)->  command ))  == 0x18 ) ? 1 : -1] ;
typedef char pe1_static_assert_cdrom_event_command_offset [( ((u32)&((( CdRomEventCommandState  *)0)->  command ))  == 0x14 ) ? 1 : -1] ;
typedef char pe1_static_assert_cdrom_event_pending_params_offset [( ((u32)&((( CdRomEventCommandState  *)0)->  pendingParams ))  == 0x08 ) ? 1 : -1] ;
extern CdRomSystemState D_8009B554;
extern CdRomCommandState D_8009B56C;
extern CdRomEventCommandState g_CdRomEventCommandState
    __asm__("D_8009B558");
extern int g_CdRomCmdTimeout __asm__("D_8009B598");
extern int g_CdRomCmdRetryState __asm__("D_8009B59C");
extern int D_8009B5A4[];
extern DslFILE g_DslFileCache[DSL_MAX_FILE] __asm__("D_800A36B0");
extern DslDirectoryCacheEntry g_DslDirectoryCache[DSL_MAX_DIR]
    __asm__("D_800A3CB0");
typedef struct DsReadyEventWindow {
    u_char eventStatus;
    u_char unk_01[7];
    u_int status;
    u_int command;
} DsReadyEventWindow;
typedef struct CdQueuedCmdSlot {
    u_int state;
    u_char result;
    u_char payload[8];
    u_char unk_0D[3];
} CdQueuedCmdSlot;
int func_8007EE84(int command, void *parameter, int arg2, int arg3);
int func_80080D34(u_char command, u_char *parameter);
typedef struct CdDsReadQueueEntry {
    u_int active;
    u_char command;
    u_char payload[4];
    u_char unk_09[3];
    void *parameter;
    u_int arg10;
    u_int arg14;
} CdDsReadQueueEntry;
typedef struct CdDsReadQueueWindow {
    CdDsReadQueueEntry entries[8];
    int queue_state;
    int read_index;
    int pending_count;
} CdDsReadQueueWindow;
typedef char pe1_static_assert_ds_queue_pending_count_offset [( ((u32)&((( CdDsReadQueueWindow  *)0)->  pending_count ))  == 0xC8 ) ? 1 : -1] ;
typedef struct DsReadCallbackSlot {
    int value;
    u_char command;
    u_char payload[8];
    u_char reserved0D[3];
} DsReadCallbackSlot;
typedef char pe1_static_assert_ds_read_callback_slot_size [( sizeof(DsReadCallbackSlot) == 0x10 ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_read_callback_slot_command_offset [( ((u32)&((( DsReadCallbackSlot  *)0)->  command ))  == 0x04 ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_read_callback_slot_payload_offset [( ((u32)&((( DsReadCallbackSlot  *)0)->  payload ))  == 0x05 ) ? 1 : -1] ;
typedef void (*DsAsyncReadCallback)(int status, void *data, void *detail);
int func_80081D74(DsAsyncReadCallback callback, int callbackArg);
void func_80080F98(int status, void *data, void *detail);
typedef struct DsAsyncReadState {
    int nextSector;
    int lastDeliveredSector;
    DsAsyncReadCallback callback;
    int retryPending;
    int retriesRemaining;
    DsEventCallback saved_sync_callback;
    DsEventCallback saved_ready_callback;
    int reserved1C;
    int active;
} DsAsyncReadState;
typedef char pe1_static_assert_ds_async_read_state_size [( sizeof(DsAsyncReadState) == 0x24 ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_async_read_retry_offset [( ((u32)&((( DsAsyncReadState  *)0)->  retryPending ))  == 0x0C ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_async_read_retries_offset [( ((u32)&((( DsAsyncReadState  *)0)->  retriesRemaining ))  == 0x10 ) ? 1 : -1] ;
typedef char pe1_static_assert_ds_async_read_active_offset [( ((u32)&((( DsAsyncReadState  *)0)->  active ))  == 0x20 ) ? 1 : -1] ;
extern int D_8009B70C;
typedef struct CdCallbackDataWindow {
    CdlCB syncCallback;
    u_char pad_AFB8[0x5048];
} CdCallbackDataWindow;
typedef char pe1_static_assert_cd_callback_data_window_size [( sizeof(CdCallbackDataWindow) == 0x504C ) ? 1 : -1] ;
typedef struct CdReadProgressState {
    int reserved00[2];
    int sectorSize;
    int destination;
    int remainingSectors;
    int flags;
    int eventData;
    DsCallback dataCallback;
    int startVsync;
    int currentVsync;
    int inProgress;
} CdReadProgressState;
typedef void (*CdReadCompleteCallback)(int event, int data);
typedef char pe1_static_assert_cd_read_progress_data_callback_offset [( ((u32)&((( CdReadProgressState  *)0)->  dataCallback ))  == 0x1C ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_read_progress_state_size [( sizeof(CdReadProgressState) == 0x2C ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_read_progress_sector_size_offset [( ((u32)&((( CdReadProgressState  *)0)->  sectorSize ))  == 0x08 ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_read_progress_current_vsync_offset [( ((u32)&((( CdReadProgressState  *)0)->  currentVsync ))  == 0x24 ) ? 1 : -1] ;
extern CdDsReadQueueEntry D_800A3540[];
extern int D_800A3604;
extern int D_800A3600;
extern int D_800A3608;
typedef struct DsCallbackRegistry {
    int start;  
    DsEventCallback sync;
    DsEventCallback ready;
} DsCallbackRegistry;
typedef char pe1_static_assert_ds_callback_registry_size [( sizeof(DsCallbackRegistry) == 12 ) ? 1 : -1] ;
extern DsCallbackRegistry g_DsReadCallbackState __asm__("D_800B8AB0");
extern CdQueuedCmdSlot g_CdQueuedCmdSlots[3] __asm__("D_800A3510");
extern DsReadCallbackSlot g_DsReadCallbackSlots[8] __asm__("D_800A3610");
extern int g_DsReadCallbackCursor __asm__("D_800A3690");
extern CdReadProgressState g_CdReadProgress __asm__("D_8009B6A4");
extern CdReadCompleteCallback D_8009B6D0;
void func_800808BC(void);
DslFILE *func_80081414(DslFILE *file, char *name);
void func_8007E594(void *queue);
void func_800822BC(void);
DsCallback func_80081E5C(DsCallback callback);
void func_80080930(void);
int func_80080940(void);
void func_8007F994(void);
void func_8007FBCC(unsigned int value);
void func_8007FBD8(unsigned int value);
void func_8007FBE4(unsigned int value);
void func_8007FBC0(unsigned int value);
void func_8007E964(int event, u8 *result);
int func_8007F88C(int command, u8 *payload);
void func_8007F960(void);
void func_8007F7E8(void);
extern u8 D_8009AFD5;
extern char *D_8009AFDC[], *D_8009B05C[];
extern volatile u8 *D_8009B27C;
extern u8 D_800A3460[8], D_800A3468[8], D_800A3470[8];
extern int D_800A3478, D_800A347C;
extern char *D_800A3480;
extern char D_80011B18[], D_80011B28[], D_80011BA0[], D_80011BA8[];
int func_8007AAB4(void);
void func_8007B9EC(void);
typedef struct CdCommandTables {
    int ready_flags[32];
    int update_status_on_ack[32];
    int parameter_counts[32];
} CdCommandTables;
typedef char pe1_static_assert_cd_command_parameter_counts_offset [( ((u32)&((( CdCommandTables  *)0)->  parameter_counts ))  == 0x100 ) ? 1 : -1] ;
typedef char pe1_static_assert_cd_command_tables_size [( sizeof(CdCommandTables) == 0x180 ) ? 1 : -1] ;
extern CdCommandTables D_8009B0FC;
extern int D_8009B1FC[];
extern int D_8009AFC0;
extern u8 D_8009AFD0[4], D_8009AFD4;
extern volatile u8 *D_8009B280, *D_8009B284;
extern char D_80011BB4[], D_80011BBC[], D_80011BCC[];
int func_8007B010(int mode, u8 *result);
int func_8007B290(int mode, u8 *result);
int func_8007A468(int mode, u8 *result);
int func_8007A488(int mode, u8 *result);
void func_8007FC64(u8 *result);
void func_8007FC88(u8 *result);
int func_8007BDDC(int mode);
int func_8007A910(int mode);
int CdStatus(void);
int CdMode(void);
int CdLastCom(void);
void func_8007A244(void);
void func_8007C444(int start, u32 count);
void func_8007C544(u32 mask, u32 start, u32 end);
void func_8007C304(u32 mode, u32 startFrame, u32 endFrame,
                 void (*callback1)(void), void (*callback2)(void));
void func_8007A2A4(void);
u32 func_8007C394(u32 *base);
u32 func_8007C484(u32 **addr, u32 **header);
void func_8007C564(void);
int CdRom_DataSync(int mode);
int func_80073A44(int mode);
int func_80080AE4();
int func_80080B04();
void func_80081268(void);
int func_800822AC(void);
int func_80082314(void);
extern volatile int D_800B28F8;
CdlLOC *func_80080CDC(CdlLOC *destination);
int func_8007F0C8();
DsEventCallback func_800824C8(DsEventCallback callback);
DsEventCallback func_800824DC(DsEventCallback callback);
DsCallback func_800824F0(DsCallback callback);
DsCallback func_8007A8EC(DsCallback callback);
void func_80080F64(u_char event);
void func_80081110(void);
extern CdlLOC g_CdLastPos asm("D_8009AFD0");
extern CdlLOC D_8009B582;
extern int D_8009B260;
extern u32 D_800C0DC0;
extern u32 D_800B6918;
extern u32 D_800C0DBC;
extern u32 D_8009AF2C[];
extern CdlCB D_8009AFB4;
extern CdlCB D_8009AFB8;
extern CdlCB g_CdSyncCallback asm("D_8009AFB4");
extern CdlCB g_CdReadyCallback asm("D_8009AFB8");
extern CdlCB g_CdReadCallback asm("D_8009AFC0");
CdlCB func_8007A3EC(CdlCB callback);
CdlCB func_8007A4A8(CdlCB callback);
CdlCB func_8007A4BC(CdlCB callback);
CdlCB CdRom_SetReadCallback(CdlCB callback);
extern u32 D_8009AFC4;
extern u32 D_8009AFC8;  
int func_8007B558(int command, void *parameters, u8 *result, int mode);
void func_8007CEAC(int channel, u32 address, int blockCount, int blockSize,
                 volatile u32 control, u8 interrupt, int reserved);
void func_8007CE80(void *destination, void *source, unsigned int count, int reserved);
int func_8007A4D0(u_char command, u_char *parameters, u_char *result);
int func_8007A60C(u_char command, u_char *parameters);
int func_8007A740(u_char command, u_char *parameters, u_char *result);
int CdMix(CdlATV *volume);
int CdGetSector(void *address, int size);
int func_8007A8AC(void *address, int size);
DsCallback func_8007A8EC(DsCallback callback);
int func_8007A910(int mode);
extern DsEventCallback volatile g_DsSyncCallback __asm__("D_800A36A4");
extern DsEventCallback volatile g_DsReadyCallback __asm__("D_800A36A8");
void func_8008068C(int event, u8 *result);
extern StHEADER *D_800C0DC8;
extern u32 D_800C20C4;
void func_8007A214(u32 *ring_addr, u32 ring_size) {
    D_800C0DC8 = (StHEADER *)ring_addr;
    D_800C20C4 = ring_size;
    func_8007A244();
}
