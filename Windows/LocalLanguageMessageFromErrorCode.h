#pragma once

#include <string>
#include "global.h" //for DWORD 

std::string LocalLanguageMessageFromErrorCodeA(DWORD dwErrorCode) ;
std::string LocalLanguageMessageAndErrorCodeA(DWORD dwErrorCode) ;

std::string PossibleSolution(DWORD dwErrorCode) ;
