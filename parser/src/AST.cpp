#include "AST.hpp"


    ASTNode::ASTNode(std::string type) : type(type) {}
    ASTNode::~ASTNode()
    {
    }
    void ASTNode::addChild(const ASTNode& child)
    {
        children.emplace_back(child);
    }
    void ASTNode::addChild(ASTNode&& child)
    {
        children.emplace_back(child);
    }
    std::string ASTNode::getType() const
    {
        return type;
    }
    std::vector<ASTNode> ASTNode::getChildren() const
    {
        return children;
    }
    std::string ASTNode::toString() const
    {
        std::string result = type;
        if (!children.empty())
        {
            result += " [";
            for (auto &child : children)
            {
                result += " " + child.toString();
            }
            result += " ]";
            
        }
        return result;
    }
    std::optional<ASTNode> ASTNode::operator[] (std::string node) 
    {
        for (ASTNode &child : children)
        {
            if (child.getType() == node)
            {
                return child;
            }
        }
        return std::nullopt;
    }





//===========================================
