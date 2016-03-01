//
//  CConfig.cpp
//  ImgReconstruction
//
//  Created by Alexander Bochkarev on 28.02.16.
//  Copyright © 2016 Alexander Bochkarev. All rights reserved.
//

#include "CConfig.hpp"
#include <fstream>
#include <iostream>

void CConfig::ReadFromFile(const std::string &path)
{
    
}

void CConfig::WriteToFile(const std::string &path) const
{
    std::ofstream out;
    out.open(path);

    for(auto& it: _configMap) {
        out << it.first << " = " << it.second << std::endl;
    }
    
    out.close();
}