_buffer_deduction = 0;
_buffer_size_unaligned = 1120*11 + 1024*48;
_buffer_size = ALIGN(_buffer_size_unaligned, 32);
_shm_size             = 24K;
MEMORY
{
    ILM (rx) : ORIGIN = 0x00000000, LENGTH = 32K
    DLMROM (rw) : ORIGIN = 0x00200000, LENGTH = 43K
    DLM (rw) : ORIGIN = 0x0020AC00, LENGTH = 192K - 43K

    SHM (rw)    : ORIGIN = 0x40000000, LENGTH = _shm_size
    BUFFER (rw) : ORIGIN = 0x40000000 + _shm_size, LENGTH = _buffer_size
    EXTRAM3(rw) : ORIGIN = 0x40000000 + _shm_size + _buffer_size + 1K, LENGTH = 104K - _buffer_deduction - _shm_size - _buffer_size - 1K

    N22RRAM (rw) : ORIGIN = 0x4001A000, LENGTH = 1920
    PMRRAM (rw) : ORIGIN = 0x4001A780, LENGTH = 128
    SHMWRAM (rw) : ORIGIN = 0x4001A800, LENGTH = 19K + 640
    SHMWDATA (rw) : ORIGIN = 0x4001F680, LENGTH = 1K
    SHMRRAM (rw) : ORIGIN = 0x4001FA80, LENGTH = 2K - 640
    FLASH (rw) : ORIGIN = 0x80080000, LENGTH = 0x180000
    EXTRAM1(rw) : ORIGIN = 0xA0000000, LENGTH = 4K
    EXTRAM2(rw) : ORIGIN = 0xA0200000, LENGTH = 112K
}
_STACK_SIZE = 2K;
ENTRY(_start)
SECTIONS
{
    .head :
    {
        . = . + 0x20;
    } > FLASH
    .text.boot :
    {
        __stext = .;
        *(.init)
        *(.text.reset_handler)
        *(.text.cpu_init)
        *(.text.c_startup_common)
        *(.xip.boot)
        KEEP(*(.version))
    } > FLASH
    .text.rram : {
        *(.rram)
    } > N22RRAM AT > FLASH
     .text.ram : ALIGN(4)
     {
        *libc.a:lib_a-memcpy.o(.text* .iram.text .rodata* .srodata*)
        *libc.a:lib_a-memcmp.o(.text* .iram.text .rodata* .srodata*)
        *libc.a:lib_a-memset.o(.text* .iram.text .rodata* .srodata*)
        *libc.a:lib_a-strcmp.o(.text* .iram.text .rodata* .srodata*)
        *libc.a:lib_a-strncmp.o(.text* .iram.text .rodata* .srodata*)
        *libc.a:lib_a-strcpy.o(.text* .iram.text .rodata* .srodata*)
        *libc.a:lib_a-strlen.o(.text* .iram.text .rodata* .srodata*)
        *libc.a:lib_a-ctype_.o(.text* .iram.text .rodata* .srodata*)
        *(.ilm.ble.text)
        *(.ilm.wlan.tx.text)
        *(.ilm.wlan.text)
        *(.ilm.text)
        *(.exec.itable )
        . = ALIGN(4);
        __etext = .;
    } > ILM AT > FLASH
    .text.xip : ALIGN(4)
    {
        EXCLUDE_FILE (*.o) *(.text._GLOBAL__sub_I_*)
        __stext1 = .;
        *(.text)
        *(.text*)
        *(.xip.text)
        *(.iram.text)
        __etext1 = .;
    } > FLASH AT > FLASH
    .init_array : ALIGN(4)
    {
        __init_array_start = .;
        KEEP(*(.text._GLOBAL__sub_I_*))
        KEEP(*(.init_array))
        KEEP(*(.ctors))
        __init_array_end = .;
    } > FLASH AT > FLASH
    .rodata : ALIGN(4)
    {
        *(.rodata*)
        *(.rodata)
        *(.srodata*)
        KEEP(*(.eh_frame*))
    } > FLASH AT > FLASH
    .data : ALIGN(4)
    {
        *(.data)
        *(.data*)
        *(.iram.data)
        . = ALIGN(4);
        *(.sdata)
        *(.sdata*)
        . = ALIGN(4);
        KEEP(*(SORT(.wits_list_*)));
        . = ALIGN(4);
        __initcall_start = .; KEEP(*(SORT(.initcall*))); __initcall_end = .;
        __device_tab_start = .; KEEP(*(SORT(.device_tab*))); __device_tab_end = .;;
        __driver_tab_start = .; KEEP(*(SORT(.driver_tab*))); __driver_tab_end = .;
        . = ALIGN(4);
        *(.data.impure_data);
        *(.data._impure_ptr);
        . = ALIGN(4);
    } > DLM AT > FLASH
    .bssreloc (NOLOAD) : ALIGN(4)
    {
        *(.bss*)
        *(.sbss*)
        . = ALIGN(4);
        *(COMMON)
    } > DLM
    .node.data (NOLOAD) : ALIGN(4)
    {
        *(.node.data)
        . = ALIGN(4);
    } > DLM
    .stack (NOLOAD) : ALIGN(16)
    {
        . += _STACK_SIZE;
    } > DLM
    .heap (NOLOAD) : ALIGN(16)
    {
        . = ORIGIN(DLM) + LENGTH(DLM);
    } > DLM
    .heap_ext1 (NOLOAD) : ALIGN(16)
    {
        . = ORIGIN(EXTRAM1) + LENGTH(EXTRAM1);
    } > EXTRAM1
    .dma (NOLOAD) : ALIGN(16)
    {
        *(.dma_buffer)
        . = ALIGN(32);
        *(.dma_desc)
    } > EXTRAM2
    .heap_ext2 (NOLOAD) : ALIGN(16)
    {
        . = ORIGIN(EXTRAM2) + LENGTH(EXTRAM2);
    } > EXTRAM2

   .shm (NOLOAD) : {
        . = ORIGIN(SHM) + LENGTH(SHM);
    } > SHM

    .bufreloc (NOLOAD) : ALIGN(4)
    {
        *(.buffer_ext*)
        . = ALIGN(32);
        *(.buffer_chunk*)
        . = ALIGN(32);
        *(.buffer_cache*)
        . = ALIGN(32);
    } > BUFFER
    .heap_ext3 (NOLOAD) : ALIGN(16)
    {
        . = ORIGIN(EXTRAM3) + LENGTH(EXTRAM3);
    } > EXTRAM3
    .pmrram (NOLOAD) : ALIGN(4)
    {
        *(.pmrram)
    } > PMRRAM
    .shmwram (NOLOAD) : ALIGN(4)
    {
        *(.watcher)
    } > SHMWRAM
    .shrram (NOLOAD) : ALIGN(4)
    {
        *(.waon)
    } > SHMRRAM
    __init_start = ADDR(.text.boot); __init_end = ADDR(.text.boot) + SIZEOF(.text.boot); __init_size = SIZEOF(.text.boot); __init_lma = LOADADDR(.text.boot);
    __rram_start = ADDR(.text.rram); __rram_end = ADDR(.text.rram) + SIZEOF(.text.rram); __rram_size = SIZEOF(.text.rram); __rram_lma = LOADADDR(.text.rram);
    __text_start = ADDR(.text.ram); __text_end = ADDR(.text.ram) + SIZEOF(.text.ram); __text_size = SIZEOF(.text.ram); __text_lma = LOADADDR(.text.ram);
    __rodata_start = ADDR(.rodata); __rodata_end = ADDR(.rodata) + SIZEOF(.rodata); __rodata_size = SIZEOF(.rodata); __rodata_lma = LOADADDR(.rodata);
    __data_start = ADDR(.data); __data_end = ADDR(.data) + SIZEOF(.data); __data_size = SIZEOF(.data); __data_lma = LOADADDR(.data);
    __bss_start = ADDR(.bssreloc); __bss_end = ADDR(.bssreloc) + SIZEOF(.bssreloc); __bss_size = SIZEOF(.bssreloc); __bss_lma = LOADADDR(.bssreloc);
    __nodebss_start = ADDR(.node.data); __nodebss_end = ADDR(.node.data) + SIZEOF(.node.data); __nodebss_size = SIZEOF(.node.data); __nodebss_lma = LOADADDR(.node.data);
    __buffer_start = ADDR(.bufreloc); __buffer_end = ADDR(.bufreloc) + SIZEOF(.bufreloc); __buffer_size = SIZEOF(.bufreloc); __buffer_lma = LOADADDR(.bufreloc);
    __pmrram_start = ADDR(.pmrram); __pmrram_end = ADDR(.pmrram) + SIZEOF(.pmrram); __pmrram_size = SIZEOF(.pmrram); __pmrram_lma = LOADADDR(.pmrram);
    __watcher_start = ADDR(.shmwram); __watcher_end = ADDR(.shmwram) + SIZEOF(.shmwram); __watcher_size = SIZEOF(.shmwram); __watcher_lma = LOADADDR(.shmwram);
    __waon_start = ADDR(.shrram); __waon_end = ADDR(.shrram) + SIZEOF(.shrram); __waon_size = SIZEOF(.shrram); __waon_lma = LOADADDR(.shrram);
    __heap_start = ADDR(.heap); __heap_end = ADDR(.heap) + SIZEOF(.heap); __heap_size = SIZEOF(.heap); __heap_lma = LOADADDR(.heap);
    __heapext1_start = ADDR(.heap_ext1); __heapext1_end = ADDR(.heap_ext1) + SIZEOF(.heap_ext1); __heapext1_size = SIZEOF(.heap_ext1); __heapext1_lma = LOADADDR(.heap_ext1);
    __heapext2_start = ADDR(.heap_ext2); __heapext2_end = ADDR(.heap_ext2) + SIZEOF(.heap_ext2); __heapext2_size = SIZEOF(.heap_ext2); __heapext2_lma = LOADADDR(.heap_ext2);
    __heapext3_start = ADDR(.heap_ext3); __heapext3_end = ADDR(.heap_ext3) + SIZEOF(.heap_ext3); __heapext3_size = SIZEOF(.heap_ext3); __heapext3_lma = LOADADDR(.heap_ext3);
    __stack_start = ADDR(.stack); __stack_end = ADDR(.stack) + SIZEOF(.stack); __stack_size = SIZEOF(.stack); __stack_lma = LOADADDR(.stack);
    __vma_start = __init_start;
    PROVIDE(end = .);
    PROVIDE(_stack = __stack_end);
}
