#include "uint.h"
#include "vfs.h"
#include "allocator.h"
#include "cpio.h"
#include "string.h"
#include "list.h"
#include "mini_uart.h"

struct mount* rootfs;
struct link_list* filesystem_pool = NULL;


struct filesystem* find_filesystem(const char *name){
  struct filesystem* tmpfs = filesystem_pool;
  while(tmpfs != NULL) if(strcmp(tmpfs->name,name)) return tmpfs;
  return NULL;
}

void vfs_init(){
  rootfs = malloc(sizeof(struct mount));
}

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  // you can also initialize memory pool of the file system here.
  struct link_list *tmp_pool;
  if(filesystem_pool == NULL){
    tmp_pool = malloc(sizeof(struct link_list));
    filesystem_pool = tmp_pool;
  }else{
    tmp_pool = filesystem_pool;
    while(tmp_pool->next != NULL) tmp_pool = tmp_pool->next;
    tmp_pool->next = malloc(sizeof(struct link_list));
    tmp_pool = tmp_pool->next;
  }
  tmp_pool->next = NULL;
  tmp_pool->entry = fs;
  return 0;
}

int vfs_open(const char* pathname, int flags, struct file** target) {
  // 1. Lookup pathname
  // 2. Create a new file handle for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags and vnode not found
  // lookup error code shows if file exist or not or other error occurs
  // 4. Return error code if fails
  struct vnode* filenode;
  vfs_lookup(pathname,&filenode);
  int return_value = filenode->f_ops->open(filenode,target);
  (*target)->flags = flags;
  return return_value;
}

int vfs_close(struct file* file) {
  // 1. release the file handle
  // 2. Return error code if fails
  return file->f_ops->close(file);
}

int vfs_write(struct file* file, const void* buf, size_t len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
  return file->f_ops->write(file,buf,len);
}

int vfs_read(struct file* file, void* buf, size_t len) {
  // 1. read min(len, readable size) byte to buf from the opened file.
  // 2. block if nothing to read for FIFO type
  // 2. return read size or error code if an error occurs.
  return file->f_ops->read(file,buf,len);
}

int vfs_mkdir(const char* pathname){

}

int vfs_mount(const char* target, const char* filesystem){
  struct vnode* mountpoint;
  vfs_lookup(target,&mountpoint);
  struct filesystem *fs = find_filesystem(filesystem);
  mountpoint->mount->fs = fs;
  fs->setup_mount(fs,mountpoint->mount);
}

int vfs_lookup(const char* pathname, struct vnode** target){
  struct vnode* CurrWorkDir = rootfs->root;
  char *parse = malloc(16);
  memset(parse, 0, 16);
  int idx = 0;
  for(int i=0;i<256;i++){
    if(pathname[i] == '/'){
      if(i == 0){}
      else{
        struct vnode* tmp;
        CurrWorkDir->v_ops->lookup(CurrWorkDir,&tmp,parse);
        CurrWorkDir = tmp;
        memset(parse, 0, 16);
        idx = 0;
      }
    }else if(pathname[i] == 0){
      if(idx == 0){
        *target = CurrWorkDir;
        return 0;
      }
      else{
        struct vnode* tmp;
        return CurrWorkDir->v_ops->lookup(CurrWorkDir,target,parse);
      }
    }else{
      parse[idx++] = pathname[i];
    }
  }
}

void vfs_ls(){
  struct vnode *tmp;
  vfs_lookup("/",&tmp);
  struct link_list* ll = tmp->dt->childs;
  while(ll != NULL){
    struct dentry* tmp2 = ll->entry;
    uart_printf("%s ",tmp2->name);
    ll = ll->next;
  }
  uart_printf("\n");
}