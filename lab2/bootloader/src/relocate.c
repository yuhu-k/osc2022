extern unsigned char _begin, _end, __relocation;
void relocate(void){
    unsigned long long  length = ( &_end - &_begin);
    unsigned char* addr_after = (unsigned char*) &__relocation;
    unsigned char* addr_before = (unsigned char*) &_begin;
    for(unsigned int i=0; i<length ; i++){
        *(addr_after+i) = *(addr_before+i);
    }
    void (*startos)(void)=(void*)addr_after;
    startos();
}