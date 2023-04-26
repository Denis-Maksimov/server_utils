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


/// @brief десериализация http
/// request {mehod{},url{},version{},headers{...},body{}}
/// @param input 
/// @return 
ASTNode parseHTTPRequest(std::string input)
{
    //Проверка строки запроса на пустоту.
    if (input.empty())
    {
        throw std::invalid_argument("HTTP Parse ERROR: Request is empty");
    }

    
    ASTNode request("request");

    //Проверка наличия первой строки запроса, содержащей заголовок запроса.
    std::size_t pos = input.find("\r\n");
    if (pos == std::string::npos)
    {
        throw std::invalid_argument("HTTP Parse ERROR: Expected '\\r\\n' in request");   
    }


    // Парсинг первой строки запроса (заголовок запроса)
    std::string firstLine = input.substr(0, pos);
    input = input.substr(pos + 2);
    pos = firstLine.find(' ');

    if (pos == std::string::npos)
    {
        throw std::invalid_argument("HTTP Parse ERROR: Expected separator 'space' at first line after METHOD on request");
    }
    // Метод запроса
    std::string method = firstLine.substr(0, pos);
    if (method.empty() || method.size() > 10 || (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE"))
    {
        throw std::runtime_error("HTTP Parse ERROR: Invalid HTTP method");
    }
    ASTNode methodNode("method");
    methodNode.addChild(ASTNode(method));
    request.addChild(methodNode);

    firstLine = firstLine.substr(pos + 1);
    pos = firstLine.find(' ');
    if (pos == std::string::npos)
    {
        throw std::invalid_argument("HTTP Parse ERROR: Expected separator 'space' at first line after URL on request");
    }
    // URL запроса
    std::string url = firstLine.substr(0, pos);
    if (url.empty())
    {
        throw std::invalid_argument("HTTP Parse ERROR: Empty URL");    
    }
    ASTNode urlNode("url");
    urlNode.addChild(ASTNode(url));
    request.addChild(urlNode);
    firstLine = firstLine.substr(pos + 1);


    // Версия протокола (HTTP/1.1, HTTP/1.0 и т.д.)
    if (!firstLine.empty() && firstLine[0] != 'H')
    {
        throw std::runtime_error("HTTP Parse ERROR: invalid field of HTTP version");         
    }
    ASTNode versionNode("version");
    versionNode.addChild(ASTNode(firstLine));
    request.addChild(versionNode);

    // Парсинг заголовков запроса
    pos = input.find("\r\n\r\n");
    if (pos == std::string::npos)
    {
        throw std::invalid_argument("HTTP Parse ERROR: Expected '\\r\\n\\r\\n' after field of HTTP-headers in request");         
    }
    std::string headers = input.substr(0, pos);
    input = input.substr(pos + 4);
    ASTNode headersNode ("headers");
    while (!headers.empty())
    {
        pos = headers.find("\r\n");
        if (pos == std::string::npos)
        {
            break;
        }
        std::string headerLine = headers.substr(0, pos);
        headers = headers.substr(pos + 2);
        pos = headerLine.find(':');
        if (pos == std::string::npos)
        {
            continue;
        }
        std::string headerKey = headerLine.substr(0, pos);
        std::string headerValue = headerLine.substr(pos + 2);
        ASTNode headerNode(headerKey);
        headerNode.addChild(ASTNode(headerValue));
        headersNode.addChild(headerNode);
    }
    request.addChild(headersNode);
    // Полезная нагрузка запроса (тело запроса)
    pos = 0;

    // if (headersNode)
    {
        std::optional<ASTNode> contentLengthHeader={};
        std::vector<ASTNode> headersChildren = headersNode.getChildren();
        for (ASTNode &node : headersChildren)
        {
            if (node.getType() == "Content-Length")
            {
                contentLengthHeader = node;
                break;
            }
        }
        if (contentLengthHeader)
        {
            std::string contentLengthStr = (*contentLengthHeader).getChildren()[0].getType();
            int contentLength = std::stoi(contentLengthStr);
            if (input.size() != contentLength)
            {
                throw std::runtime_error("HTTP Parse ERROR: Content-Length");     
            }
            pos = contentLength;
        }
    }
    ASTNode bodyNode("body");
    bodyNode.addChild(ASTNode(input/*.substr(0, pos)*/));
    request.addChild(bodyNode);
    return request;
}


/// @brief провверка структуры AST для http request
/// @param node 
/// @return true if is valid
bool isASTValid(ASTNode node)
{
    try
    {         
        // Проверяем корневой узел
        if (node.getType() != "request")
        {
            // std::cerr<<"root err\n";
            throw std::runtime_error("ASyntaxTree check ERROR: root node isnt named 'request'");   
            // return false;
        }

        // Проверяем наличие необходимых дочерних узлов
        std::vector<std::string> requiredChildren = {"method", "url", "version", "headers", "body"};
        for (std::string& child : requiredChildren)
        {
            bool found = false;
            for (ASTNode& nodeChild : node.getChildren())
            {
                if (nodeChild.getType() == child)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                std::cerr<<"childs err\n";
                // return false;
                throw std::runtime_error("ASyntaxTree check ERROR: structure of childs isnt valid");
            }
        }
        
    //     // Дополнительные проверки, если необходимо
        
    //     // Рекурсивно проверяем дочерние узлы
    //     // for (ASTNode* child : node->getChildren())
    //     // {
    //     //     if (!isASTValid(child))
    //     //     {
    //     //         std::cerr<<"rchilds err\n";
    //     //         return false;
    //     //     }
    //     // }

        

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}













