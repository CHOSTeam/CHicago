/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 02 of 2021, at 12:42 BRT
 * Last edited on June 06 of 2021 at 11:23 BRT */

#pragma once

#include <efi/rs.h>

#define EFI_SIMPLE_TEXT_INPUT_EX_GUID { 0xDD9E7534, 0x7762, 0x4698, \
                                        { 0x8C, 0x14, 0xF5, 0x85, 0x17, 0xA6, 0x25, 0xAA } }
#define EFI_SIMPLE_TEXT_INPUT_GUID { 0x387477C1, 0x69C7, 0x11D2, \
                                     { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }
#define EFI_SIMPLE_TEXT_OUTPUT_GUID { 0x387477C2, 0x69C7, 0x11D2, \
                                      { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }
#define EFI_SIMPLE_POINTER_GUID { 0x31878C87, 0xB75, 0x11D5, \
                                  { 0x9A, 0x4F, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D } }
#define EFI_ABSOLUTE_POINTER_GUID { 0x8D59D32B, 0xC655, 0x4AE9, \
                                    { 0x9B, 0x15, 0xF2, 0x59, 0x04, 0x99, 0x2A, 0x43 } }
#define EFI_SIMPLE_FILE_SYSTEM_GUID { 0x964E5B22, 0x6459, 0x11D2, \
                                      { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }
#define EFI_FILE_INFO_GUID { 0x9576E92, 0x6D3F, 0x11D2, \
                             { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }
#define EFI_FILE_SYSTEM_INFO_GUID { 0x9576E93, 0x6D3F, 0x11D2, \
                                    { 0x8E, 0x39, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } }
#define EFI_FILE_SYSTEM_LABEL_GUID { 0xDB47D7D3, 0xFE81, 0x11D3, \
                                     { 0x9A, 0x35, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D } }

/* KeyState->Shift values. */

#define EFI_SHIFT_STATE_VALID 0x80000000
#define EFI_RIGHT_SHIFT_PRESSED 0x01
#define EFI_LEFT_SHIFT_PRESSED 0x02
#define EFI_RIGHT_CTRL_PRESSED 0x04
#define EFI_LEFT_CTRL_PRESSED 0x08
#define EFI_RIGHT_ALT_PRESSED 0x10
#define EFI_LEFT_ALT_PRESSED 0x20
#define EFI_RIGHT_LOGO_PRESSED 0x40
#define EFI_LEFT_LOGO_PRESSED 0x80
#define EFI_MENU_KEY_PRESSED 0x100
#define EFI_SYS_REQ_PRESSED 0x200

/* KeyState->Toggle values. */

#define EFI_TOGGLE_STATE_VALID 0x80
#define EFI_KEY_STATE_EXPOSED 0x40
#define EFI_SCROLL_LOCK_ACTIVE 0x01
#define EFI_NUM_LOCK_ACTIVE 0x02
#define EFI_CAPS_LOCK_ACTIVE 0x04

/* Standard unicode drawing characters. */

#define EFI_BOX_DRAW_HORIZONTAL 0x2500
#define EFI_BOX_DRAW_VERTICAL 0x2502
#define EFI_BOX_DRAW_DOWN_RIGHT 0x250C
#define EFI_BOX_DRAW_DOWN_LEFT 0x2510
#define EFI_BOX_DRAW_UP_RIGHT 0x2514
#define EFI_BOX_DRAW_UP_LEFT 0x2518
#define EFI_BOX_DRAW_VERTICAL_RIGHT 0x251C
#define EFI_BOX_DRAW_VERTICAL_LEFT 0x2524
#define EFI_BOX_DRAW_DOWN_HORIZONTAL 0x252C
#define EFI_BOX_DRAW_UP_HORIZONTAL 0x2534
#define EFI_BOX_DRAW_VERTICAL_HORIZONTAL 0x253C
#define EFI_BOX_DRAW_DOUBLE_HORIZONTAL 0x2550
#define EFI_BOX_DRAW_DOUBLE_VERTICAL 0x2551
#define EFI_BOX_DRAW_DOWN_RIGHT_DOUBLE 0x2552
#define EFI_BOX_DRAW_DOWN_DOUBLE_RIGHT 0x2553
#define EFI_BOX_DRAW_DOUBLE_DOWN_RIGHT 0x2554
#define EFI_BOX_DRAW_DOWN_LEFT_DOUBLE 0x2555
#define EFI_BOX_DRAW_DOWN_DOUBLE_LEFT 0x2556
#define EFI_BOX_DRAW_DOUBLE_DOWN_LEFT 0x2557
#define EFI_BOX_DRAW_UP_RIGHT_DOUBLE 0x2558
#define EFI_BOX_DRAW_UP_DOUBLE_RIGHT 0x2559
#define EFI_BOX_DRAW_DOUBLE_UP_RIGHT 0x255A
#define EFI_BOX_DRAW_UP_LEFT_DOUBLE 0x255B
#define EFI_BOX_DRAW_UP_DOUBLE_LEFT 0x255C
#define EFI_BOX_DRAW_DOUBLE_UP_LEFT 0x255D
#define EFI_BOX_DRAW_VERTICAL_RIGHT_DOUBLE 0x255E
#define EFI_BOX_DRAW_VERTICAL_DOUBLE_RIGHT 0x255F
#define EFI_BOX_DRAW_DOUBLE_VERTICAL_RIGHT 0x2560
#define EFI_BOX_DRAW_VERTICAL_LEFT_DOUBLE 0x2561
#define EFI_BOX_DRAW_VERTICAL_DOUBLE_LEFT 0x2562
#define EFI_BOX_DRAW_DOUBLE_VERTICAL_LEFT 0x2563
#define EFI_BOX_DRAW_DOWN_HORIZONTAL_DOUBLE 0x2564
#define EFI_BOX_DRAW_DOWN_DOUBLE_HORIZONTAL 0x2565
#define EFI_BOX_DRAW_DOUBLE_DOWN_HORIZONTAL 0x2566
#define EFI_BOX_DRAW_UP_HORIZONTAL_DOUBLE 0x2567
#define EFI_BOX_DRAW_UP_DOUBLE_HORIZONTAL 0x2568
#define EFI_BOX_DRAW_DOUBLE_UP_HORIZONTAL 0x2569
#define EFI_BOX_DRAW_VERTICAL_HORIZONTAL_DOUBLE 0x256A
#define EFI_BOX_DRAW_VERTICAL_DOUBLE_HORIZONTAL 0x256B
#define EFI_BOX_DRAW_DOUBLE_VERTICAL_HORIZONTAL 0x256C

/* EFI required block elements. */

#define EFI_BLOCK_ELEMENT_FULL_BLOCK 0x2588
#define EFI_BLOCK_ELEMENT_LIGHT_SHADE 0x2591

/* EFI required geometric shapes. */

#define EFI_GEOMETRIC_SHAPE_UP_TRIANGLE 0x25B2
#define EFI_GEOMETRIC_SHAPE_RIGHT_TRIANGLE 0x25BA
#define EFI_GEOMETRIC_SHAPE_DOWN_TRIANGLE 0x25BC
#define EFI_GEOMETRIC_SHAPE_LEFT_TRIANGLE 0x25C4

/* And, at last, EFI required arrow shapes (well, we still have more things, but they are for the other functions). */

#define EFI_ARROW_UP 0x2191
#define EFI_ARROW_DOWN 0x2193

/* SimpleTextOutput attribute/text colors. */

#define EFI_BLACK 0x00
#define EFI_BLUE 0x01
#define EFI_GREEN 0x02
#define EFI_CYAN 0x03
#define EFI_RED 0x04
#define EFI_MAGENTA 0x05
#define EFI_BROWN 0x06
#define EFI_LIGHT_GRAY 0x07
#define EFI_BRIGHT 0x08
#define EFI_DARK_GRAY 0x08
#define EFI_LIGHT_BLUE 0x09
#define EFI_LIGHT_GREEN 0x0A
#define EFI_LIGHT_CYAN 0x0B
#define EFI_LIGHT_RED 0x0C
#define EFI_LIGHT_MAGENTA 0x0D
#define EFI_YELLOW 0x0E
#define EFI_WHITE 0x0F

/* And a macro to convert the fg+bg into an attribute value/single byte (remembering that the only valid bg colors are:
 * black, blue, green, cyan, red, magenta, brown and light gray). */

#define EFI_ATTR(fg, bg) ((fg) | ((bg) << 4))

/* AbsolutePointerState->Buttons values/bits. */

#define EFI_ABS_PTR_TOUCH_ACTIVE 0x01
#define EFI_ABS_PTR_ALT_ACTIVE 0x02

#define EFI_FILE_PROTOCOL_REV_1 0x10000
#define EFI_FILE_PROTOCOL_REV_2 0x20000
#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_REV EFI_FILE_PROTOCOL_REV_1
#define EFI_FILE_PROTOCOL_REV EFI_FILE_PROTOCOL_REV_2

/* Mode and attribute for Open/OpenEx. */

#define EFI_FILE_MODE_CREATE 0x00
#define EFI_FILE_MODE_READ 0x01
#define EFI_FILE_MODE_WRITE 0x02

#define EFI_FILE_READ_ONLY 0x01
#define EFI_FILE_HIDDEN 0x02
#define EFI_FILE_SYSTEM 0x04
#define EFI_FILE_RESERVED 0x08
#define EFI_FILE_DIRECTORY 0x10
#define EFI_FILE_ARCHIVE 0x20
#define EFI_FILE_VALID_ATTR 0x37

typedef struct _EfiSimpleTextInputEx EfiSimpleTextInputEx;
typedef struct _EfiSimpleTextInput EfiSimpleTextInput;
typedef struct _EfiSimpleTextOutput EfiSimpleTextOutput;
typedef struct _EfiSimplePointer EfiSimplePointer;
typedef struct _EfiAbsolutePointer EfiAbsolutePointer;
typedef struct _EfiSimpleFileSystem EfiSimpleFileSystem;
typedef struct _EfiFile EfiFile;
typedef UInt8 EfiKeyToggleState;

typedef struct {
    UInt16 ScanCode;
    Char16 Unicode;
} EfiInputKey;

typedef struct {
    UInt32 Shift;
    EfiKeyToggleState Toggle;
} EfiKeyState;

typedef struct {
    EfiInputKey Key;
    EfiKeyState State;
} EfiKeyData;

typedef struct {
    Int32 MaxMode, Mode,
          Attribute, Column, Row;
    Boolean Visible;
} EfiTextOutputMode;

typedef struct {
    Int32 RelX, RelY, RelZ;
    Boolean Left, Right;
} EfiSimplePointerState;

typedef struct {
    UInt64 ResX, ResY, ResZ;
    Boolean Left, Right;
} EfiSimplePointerMode;

typedef struct {
    UInt64 X, Y, Z;
    UInt32 Buttons;
} EfiAbsolutePointerState;

typedef struct {
    UInt64 MinX, MinY, MinZ,
           MaxX, MaxY, MaxZ;
    UInt32 Attributes;
} EfiAbsolutePointerMode;

typedef struct {
    EfiEvent Event;
    EfiStatus Status;
    UIntN Size;
    Void *Buffer;
} EfiFileIoToken;

typedef struct {
    UInt64 Size, FileSize, PhysicalSize;
    EfiTime CreateTime, LastAccessTime, ModificationTime;
    UInt64 Attribute;
    Char16 FileName[];
} EfiFileInfo;

typedef struct {
    UInt64 Size;
    Boolean ReadOnly;
    UInt64 VolumeSize, FreeSpace;
    UInt32 BlockSize;
    Char16 VolumeLabel[];
} EfiFileSystemInfo;

typedef struct {
    Char16 VolumeLabel[1];
} EfiFileSystemVolumeLabel;

typedef EfiStatus (*_EfiKeyNotify)(EfiKeyData*);

typedef EfiStatus (*_EfiInputResetEx)(EfiSimpleTextInputEx*, Boolean);
typedef EfiStatus (*_EfiInputReadKeyEx)(EfiSimpleTextInputEx*, EfiKeyData*);
typedef EfiStatus (*_EfiSetState)(EfiSimpleTextInputEx*, EfiKeyToggleState*);
typedef EfiStatus (*_EfiRegisterKeyStrokeNotify)(EfiSimpleTextInputEx*, EfiKeyData*, _EfiKeyNotify, Void**);
typedef EfiStatus (*_EfiUnregisterKeyStrokeNotify)(EfiSimpleTextInputEx*, Void*);

typedef EfiStatus (*_EfiInputReset)(EfiSimpleTextInput*, Boolean);
typedef EfiStatus (*_EfiInputReadKey)(EfiSimpleTextInput*, EfiInputKey*);

typedef EfiStatus (*_EfiTextReset)(EfiSimpleTextOutput*, Boolean);
typedef EfiStatus (*_EfiTextString)(EfiSimpleTextOutput*, Char16*);
typedef EfiStatus (*_EfiTextTestString)(EfiSimpleTextOutput*, Char16*);
typedef EfiStatus (*_EfiTextQueryMode)(EfiSimpleTextOutput*, UIntN, UIntN*, UIntN*);
typedef EfiStatus (*_EfiTextSetMode)(EfiSimpleTextOutput*, UIntN);
typedef EfiStatus (*_EfiTextSetAttribute)(EfiSimpleTextOutput*, UIntN);
typedef EfiStatus (*_EfiTextClearScreen)(EfiSimpleTextOutput*);
typedef EfiStatus (*_EfiTextSetCursorPosition)(EfiSimpleTextOutput*, UIntN, UIntN);
typedef EfiStatus (*_EfiTextEnableCursor)(EfiSimpleTextOutput*, Boolean);

typedef EfiStatus (*_EfiSimplePointerReset)(EfiSimplePointer*, Boolean);
typedef EfiStatus (*_EfiSimplePointerGetState)(EfiSimplePointer*, EfiSimplePointerState*);

typedef EfiStatus (*_EfiAbsolutePointerReset)(EfiAbsolutePointer*, Boolean);
typedef EfiStatus (*_EfiAbsolutePointerGetState)(EfiAbsolutePointer*, EfiAbsolutePointerState*);

typedef EfiStatus (*_EfiFileOpenVolume)(EfiSimpleFileSystem*, EfiFile**);
typedef EfiStatus (*_EfiFileOpen)(EfiFile*, EfiFile**, Char16*, UInt64, UInt64);
typedef EfiStatus (*_EfiFileClose)(EfiFile*);
typedef EfiStatus (*_EfiFileDelete)(EfiFile*);
typedef EfiStatus (*_EfiFileRead)(EfiFile*, UIntN*, Void*);
typedef EfiStatus (*_EfiFileWrite)(EfiFile*, UIntN*, Void*);
typedef EfiStatus (*_EfiFileGetPosition)(EfiFile*, UInt64*);
typedef EfiStatus (*_EfiFileSetPosition)(EfiFile*, UInt64);
typedef EfiStatus (*_EfiFileGetInfo)(EfiFile*, EfiGuid*, UIntN*, Void*);
typedef EfiStatus (*_EfiFileSetInfo)(EfiFile*, EfiGuid*, UIntN, Void*);
typedef EfiStatus (*_EfiFileFlush)(EfiFile*);
typedef EfiStatus (*_EfiFileOpenEx)(EfiFile*, EfiFile**, Char16*, UInt64, UInt64, EfiFileIoToken*);
typedef EfiStatus (*_EfiFileReadEx)(EfiFile*, EfiFileIoToken*);
typedef EfiStatus (*_EfiFileWriteEx)(EfiFile*, EfiFileIoToken*);
typedef EfiStatus (*_EfiFileFlushEx)(EfiFile*, EfiFileIoToken*);

struct _EfiSimpleTextInputEx {
    _EfiInputResetEx Reset;
    _EfiInputReadKeyEx ReadKeyStrokeEx;
    EfiEvent WaitForKeyEx;
    _EfiSetState SetState;
    _EfiRegisterKeyStrokeNotify RegisterKeyNotify;
    _EfiUnregisterKeyStrokeNotify UnregisterKeyNotify;
};

struct _EfiSimpleTextInput {
    _EfiInputReset Reset;
    _EfiInputReadKey ReadKeyStroke;
    EfiEvent WaitForKey;
};

struct _EfiSimpleTextOutput {
    _EfiTextReset Reset;
    _EfiTextString OutputString;
    _EfiTextTestString TestString;
    _EfiTextQueryMode QueryMode;
    _EfiTextSetMode SetMode;
    _EfiTextSetAttribute SetAttribute;
    _EfiTextClearScreen ClearScreen;
    _EfiTextSetCursorPosition SetCursorPosition;
    _EfiTextEnableCursor EnableCursor;
    EfiTextOutputMode *Mode;
};

struct _EfiSimplePointer {
    _EfiSimplePointerReset Reset;
    _EfiSimplePointerGetState GetState;
    EfiEvent WaitForInput;
    EfiSimplePointerMode *Mode;
};

/* AbsolutePointer also goes here because it is SimplePointer, but for absolute devices (mouses are relative devices,
 * drawing tablets and touchscreen are absolute devices). */

struct _EfiAbsolutePointer {
    _EfiAbsolutePointerReset Reset;
    _EfiAbsolutePointerGetState GetState;
    EfiEvent WaitForInput;
    EfiAbsolutePointerMode *Mode;
};

struct _EfiSimpleFileSystem {
    UInt64 Revision;
    _EfiFileOpenVolume OpenVolume;
};

struct _EfiFile {
    UInt64 Revision;
    _EfiFileOpen Open;
    _EfiFileClose Close;
    _EfiFileDelete Delete;
    _EfiFileRead Read;
    _EfiFileWrite Write;
    _EfiFileGetPosition GetPosition;
    _EfiFileSetPosition SetPosition;
    _EfiFileGetInfo GetInfo;
    _EfiFileSetInfo SetInfo;
    _EfiFileFlush Flush;
    _EfiFileOpenEx OpenEx;
    _EfiFileReadEx ReadEx;
    _EfiFileWriteEx WriteEx;
    _EfiFileFlushEx FlushEx;
};
