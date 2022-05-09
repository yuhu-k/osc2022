int log(int base, int logarithm){
    int tmp = 1;
    int exp = 0;
    while(tmp <= logarithm){
        tmp *= base;
        exp ++;
    }
    return exp-1;
}

int exp(int base, int exponent){
    int tmp = 1;
    for(int i=0;i<exponent;i++){
        tmp *= base;
    }
    return tmp;
}

int log2(int logarithm){
    return log(2,logarithm);
}
int exp2(int exponent){
    return exp(2,exponent);
}