#pragma once

#include <string>
#include "global.h" //for DWORD 

std::string LocalLanguageMessageFromErrorCode(DWORD dwErrorCode) ;

std::string PossibleSolution(DWORD dwErrorCode) ;
