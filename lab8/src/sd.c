#include "sd.h"
#include "string.h"
#include "peripheral/sd.h"
#include "string.h"
#include "vfs.h"
#include "fat32.h"

int sd_mount();
void init_sd(){
    sd_init();
    sd_mount();
}

int sd_mount(){
    char buf[512];
    readblock(0,buf);
    if(buf[0x01FE] != 0x55 && buf[0x01FF] != 0xAA) return -1;
    struct mbr_partition mp;
    memcpy(&mp,buf+0x01BE,sizeof(struct mbr_partition));
    if(mp.partition_type == 0x0B){    // sd card with chs 
        readblock(mp.lba,buf);
        struct fat32_boot_sector *fbs = buf;
        struct fat32_metadata sd_metadeta;
        sd_metadeta.data_region_blk_idx = mp.lba + 
                                        fbs->n_sectors_per_fat_32 * fbs->n_file_alloc_tabs +
                                        fbs->n_reserved_sectors;
        sd_metadeta.fat_region_blk_idx = mp.lba + fbs-> n_reserved_sectors;
        sd_metadeta.n_fat = fbs->n_file_alloc_tabs;
        sd_metadeta.sector_per_fat = fbs->n_sectors_per_fat_32;
        sd_metadeta.sector_per_cluster = fbs->logical_sector_per_cluster;
        sd_metadeta.first_cluster = fbs->root_dir_start_cluster_num;
        fat32_init(&sd_metadeta);
        vfs_mount("/boot","fat32");
    }
}

/*int sd_mount(){
    // read MBR
    char buf[512];
    readblock(0, buf);

    // check boot signature
    if (buf[510] != 0x55 || buf[511] != 0xAA) {
        return -1;
    }

    // parse first partition only
    struct mbr_partition p1;
    memcpy(&p1, buf + 446, sizeof(struct mbr_partition));

    // mount partition
    readblock(p1.lba, buf);
    // route each filesystem
    if (p1.partition_type == 0x0b) {  // FAT32 with CHS addressing
        // create FAT32's root directory object

        // store metadata
        struct fat32_boot_sector* boot_sector = (struct fat32_boot_sector*)buf;
        struct fat32_metadata fd;
        fd.data_region_blk_idx = p1.lba +
                                             boot_sector->n_sectors_per_fat_32 * boot_sector->n_file_alloc_tabs +
                                             boot_sector->n_reserved_sectors;
        fd.fat_region_blk_idx = p1.lba + boot_sector->n_reserved_sectors;
        fd.n_fat = boot_sector->n_file_alloc_tabs;
        fd.sector_per_fat = boot_sector->n_sectors_per_fat_32;
        fd.first_cluster = boot_sector->root_dir_start_cluster_num;
        fd.sector_per_cluster = boot_sector->logical_sector_per_cluster;

        uart_printf("%d\n",fd.data_region_blk_idx);
    }

    return 0;
}*/