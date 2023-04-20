#ifndef AST
#define AST

#include <iostream>
#include <string>
#include <vector>
#include <optional>

/// @brief 
class ASTNode
{
public:
    ASTNode(std::string type);
    virtual ~ASTNode();
    void addChild(const ASTNode& child);
    void addChild(ASTNode&& child);
    std::string getType() const;
    std::vector<ASTNode> getChildren() const;
    virtual std::string toString() const;
    std::optional<ASTNode> operator[] (std::string node);

private:
    std::string type;
    std::vector<ASTNode> children;
};






#endif /* AST */
