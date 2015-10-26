//
//  utils.hpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 26.10.15.
//  Copyright © 2015 Alexander Bochkarev. All rights reserved.
//

#pragma once

template<typename T>
int hamming(T p1, T p2) {
    T diff = p1 & p2;
    
    std::bitset<sizeof(T) * 8> d(diff);
    
    int hammingDistance = 0;
    for (int i = 0; i < d.size(); i++) {
        hammingDistance += d[i];
    }
    
    return hammingDistance;
}
