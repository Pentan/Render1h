/*
参照カウンタ付き基底クラス

//基本的に参照カウントは1から始まる
*a = new Obj(); // 1
a->release(); // 0 => delete

//想定される使い方
*a = Obj(); // 1
*h = new Holder(a); // 内部でretainして2
a->release(); // 1
delete h; // 内部でreleaseして0 => delete

//このような場合は
*h = new Holder(new Obj()); // 最初に1.内部でretainして2
delete h; // 内部でreleaseして1 => リーク!

//こうすることにした
*h = new Holder(RCO::rc0<Obj>(new Obj())); // カウンタを0に.内部でretainして1
delete h; // 内部でreleaseして0 => delete
*/

#ifndef R1H_REFCOUNTOBJ_H
#define R1H_REFCOUNTOBJ_H

#include <cstdio>

namespace r1h {

class RCO {
    int reference_counter_;
    
public:
    RCO(int i=1): reference_counter_(i) {}
    virtual ~RCO() {};
    
    void retain() { reference_counter_ += 1; }
    void release() {
        if(--reference_counter_ <= 0) {
            //printf("%p released and counter is 0. delete this.\n", this);
            delete this;
        }
    }
    
    template<class T> static T* rc0(T* p) {
        p->reference_counter_ = 0;
        return p;
    }
};

}

#endif
