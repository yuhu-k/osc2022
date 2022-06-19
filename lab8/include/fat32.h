#pragma once
#include "uint.h"
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define EOF 0xFFFFFFF
#define EOC 0xFFFFFF8
#define BLOCK_SIZE 512
#define FAT_ENTRY_PER_BLOCK (BLOCK_SIZE / sizeof(int))
#define DIRENT_PER_CLUSTER (BLOCK_SIZE / sizeof(struct fat32_dirent))
struct fat32_boot_sector {
    char jump[3];  // 0x0
    char oem[8];   // 0x3

    // BIOS Parameter Block
    uint16_t bytes_per_logical_sector;   // 0xB-0xC
    uint8_t logical_sector_per_cluster;  // 0xD
    uint16_t n_reserved_sectors;         // 0xE-0xF
    uint8_t n_file_alloc_tabs;           // 0x10
    uint16_t n_max_root_dir_entries_16;  // 0x11-0x12
    uint16_t n_logical_sectors_16;       // 0x13-0x14
    uint8_t media_descriptor;            // 0x15
    uint16_t logical_sector_per_fat_16;  // 0x16-0x17

    // DOS3.31 BPB
    uint16_t physical_sector_per_track;  // 0x18-0x19
    uint16_t n_heads;                    // 0x1A-0x1B
    uint32_t n_hidden_sectors;           // 0x1C-0x1F
    uint32_t n_sectors_32;               // 0x20-0x23

    // FAT32 Extended BIOS Parameter Block
    uint32_t n_sectors_per_fat_32;              // 0x24-0x27
    uint16_t mirror_flag;                       // 0x28-0x29
    uint16_t version;                           // 0x2A-0x2B
    uint32_t root_dir_start_cluster_num;        // 0x2C-0x2F
    uint16_t fs_info_sector_num;                // 0x30-0x31
    uint16_t boot_sector_bak_first_sector_num;  // 0x32-0x33
    uint32_t reserved[3];                       // 0x34-0x3F
    uint8_t physical_drive_num;                 // 0x40
    uint8_t unused;                             // 0x41
    uint8_t extended_boot_signature;            // 0x42
    uint32_t volume_id;                         // 0x43-0x46
    uint8_t volume_label[11];                   // 0x47-0x51
    uint8_t fat_system_type[8];                 // 0x52-0x59
} __attribute__((packed));

struct fat32_metadata{
    unsigned int data_region_blk_idx;
    unsigned int fat_region_blk_idx;
    unsigned int n_fat;
    unsigned int sector_per_fat;
    unsigned int first_cluster;
    unsigned int sector_per_cluster;
};

struct fat32_dirent {
    uint8_t name[8];            // 0x0-0x7
    uint8_t ext[3];             // 0x8-0xA
    uint8_t attr;               // 0xB
    uint8_t reserved;           // 0xC
    uint8_t create_time[3];     // 0xD-0xF
    uint16_t create_date;       // 0x10-0x11
    uint16_t last_access_date;  // 0x12-0x13
    uint16_t cluster_high;      // 0x14-0x15
    uint32_t ext_attr;          // 0x16-0x19
    uint16_t cluster_low;       // 0x1A-0x1B
    uint32_t size;              // 0x1C-0x1F
} __attribute__((packed));

void fat32_init(struct fat32_metadata* metadata);

struct page_cache{
    unsigned char modified;
    char *buf;
};

struct file_internal{
    unsigned int dentry_cluster_idx;
    unsigned int cluster_idx;
    int page_num;
    struct page_cache* pages;
};
