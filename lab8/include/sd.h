#pragma once
struct mbr_partition{
    char         status;         // 0x0
    unsigned int start : 24;     // 0x1 ~ 0x3
    char         partition_type; // 0x4
    unsigned int end : 24;       // 0x5 ~ 0x7
    unsigned int lba;            // 0x8 ~ 0x11
    unsigned int partition_len;  // 0x12 ~ 0x15
}__attribute__((__packed__));
void readblock(int block_idx, void* buf);
void writeblock(int block_idx, void* buf);
void init_sd();