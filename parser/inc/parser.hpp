#ifndef PARSER
#define PARSER

#include "AST.hpp"



/// @brief десериализация http
/// request {mehod{},url{},version{},headers{...},body{}}
/// @param input 
/// @return 
ASTNode parseHTTPRequest(std::string input);

/// @brief провверка структуры AST для http request
/// @param node 
/// @return true if is valid
bool isASTValid(ASTNode node);



#endif /* PARSER */
