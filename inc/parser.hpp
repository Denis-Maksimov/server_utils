#ifndef PARSER
#define PARSER

#include "AST.hpp"


/**
 * @brief 
 *  Проверки входных параметров:
 * - Проверка строки запроса на пустоту.
 * - Проверка наличия первой строки запроса, содержащей заголовок запроса.
 * - Проверка корректности метода запроса (допустимые значения: GET, POST, PUT, DELETE и т.д.).
 * - Проверка наличия URL запроса.
 * - Проверка наличия версии протокола (HTTP/1.1, HTTP/1.0 и т.д.).
 * - Проверка наличия заголовков запроса.
 * - Проверка корректности длины тела запроса (если задан Content-Length заголовок).
 * @param input 
 * @return ASTNode* 
 */
ASTNode parseHTTPRequest(std::string input);
bool isASTValid(ASTNode node);



#endif /* PARSER */
