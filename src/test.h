#include <stdio.h>

#define pc(c) putchar_unlocked(c)
#define gc() getchar_unlocked()

void out(int a){
    char str[10];
    if(a < 0) a*=-1, pc('-');
    int i=0;
    do str[i++] = (char)(a%10)|0x30, a /= 10; while(a > 0);
    while(i--) pc(str[i]);
}

int in(){
    int sgn = 1, ret = 0, ch;
    while((ch = gc()) > ' '){
        if(ch == '-') {sgn = -1; continue;} 
        ret = ret*10+(ch&0xf);
    }
    return sgn*ret;
}
