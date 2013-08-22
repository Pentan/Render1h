
#include <vector>
#include "aabb.h"
#include "bvhnode.h"

using namespace r1h;

int BVHNode::buildAABBTree(AABB *aabbArray, const int aabbnum) {
    int maxdepth = recurseBuildTree(*this, aabbArray, aabbnum, 1);
    //std::cout << "max BVH depth:" << maxdepth << std::endl;
    return maxdepth;
}

int BVHNode::recurseBuildTree(BVHNode &node, AABB *aabbArray, const int aabbnum, const int depth) {
    int ret;
    
    if(aabbnum > 1) {
        // make contains AABB
        AABB centroAABB;
        node.aabb.clear();
        for(int i = 0; i < aabbnum; i++) {
            const AABB &ab = aabbArray[i];
            node.aabb.expand(ab);
            centroAABB.expand(ab);
        }
        
        // detect divide axis
        Vec centroSize = centroAABB.getSize();
        int chooseaxis = (centroSize.x_ > centroSize.y_)? 0 : 1;
        if(centroSize.z_ > centroSize.v[chooseaxis]) {
            chooseaxis = 2;
        }
        
        // TODO? direct sorting AABB structure array
        // sort AABB array
        AABBAxisComparator comparator(chooseaxis);
        std::sort(aabbArray, aabbArray + aabbnum, comparator);
        /*
        //+++++
        std::cout << "BVHNode(" << aabbnum << " tri) divided[" << divaxis << "]:";
        //std::cout << "(" << node.aabb.min.x_ <<  "," << node.aabb.min.y_ <<  "," << node.aabb.min.z_ << ")";
        //std::cout << ",(" << node.aabb.max.x_ <<  "," << node.aabb.max.y_ <<  "," << node.aabb.max.z_ << ")";
        Vec aabbsize = node.aabb.getSize();
        std::cout << "size:(" << aabbsize.x_ <<  "," << aabbsize.y_ <<  "," << aabbsize.z_ << ")";
        std::cout << "v:" << (aabbsize.x_ * aabbsize.y_ * aabbsize.z_);
        std::cout << std::endl;
        //+++++
        */
        node.allocChildren(2);
        int halfnum = aabbnum / 2;
        int d1 = recurseBuildTree(node.children[0], aabbArray, halfnum, depth+1);
        int d2 = recurseBuildTree(node.children[1], aabbArray + halfnum, aabbnum - halfnum, depth+1);
        
        ret = (d1 > d2)? d1:d2;
    } else {
        // contain one triangle.
        const AABB &curaabb = aabbArray[0];
        node.dataId = curaabb.dataId;
        node.aabb = curaabb;
        
        ret = depth;
        /*
        //+++++
        std::cout << "tri(d:" << depth << ")[f:" << curaabb.dataId << "]:";
        //std::cout << "(" << node.aabb.min.x_ <<  "," << node.aabb.min.y_ <<  "," << node.aabb.min.z_ << ")";
        //std::cout << ",(" << node.aabb.max.x_ <<  "," << node.aabb.max.y_ <<  "," << node.aabb.max.z_ << ")";
        Vec aabbsize = node.aabb.getSize();
        std::cout << "size:(" << aabbsize.x_ <<  "," << aabbsize.y_ <<  "," << aabbsize.z_ << ")";
        std::cout << "v:" << (aabbsize.x_ * aabbsize.y_ * aabbsize.z_);
        std::cout << std::endl;
        //+++++
        */
    }
    return ret;
}

