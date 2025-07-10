// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Abstract Syntax Tree (AST) header file made by Nathan Alonso Guimarães (00334437)

#pragma once

#include <vector>
#include <string>
#include <fstream>

// Forward declaration
class Symbol;

// AST node types for different language constructs
enum class ASTNodeType {
    UNKNOWN,                // Unknown/uninitialized node type
    SYMBOL,                 // Symbol reference
    
    // Declaration types
    PROGRAM,                // Always the first node
    DECLARATION_LIST,       // List of declarations
    VARIABLE_DECLARATION,   // Variable declaration: type var = value
    VECTOR_DECLARATION,     // Vector declaration: type vec[size]

    // Vector-related types
    VECTOR_INITIALIZATION,  // Vector initialization: type vec[size] = {values}
    VECTOR_ACCESS,          // Vector access: vec[index]
    
    // Function-related types
    EXPRESSION_LIST,        // List of expressions
    ARGUMENTS_LIST,         // Function call arguments
    FUNCTION_DECLARATION,   // Function declaration
    PARAMETERS_LIST,        // Function parameter list
    FUNCTION_CALL,          // Function call
    FORMAL_PARAMETERS,      // Formal parameters in function definition
    
    // Command types
    COMMAND,            // Single command
    COMMAND_LIST,       // List of commands
    ASSIGNMENT,         // Assignment operation: var = expr
    
    // Arithmetic operations
    ADD,                // Addition: a + b
    SUBTRACT,           // Subtraction: a - b
    MULTIPLY,           // Multiplication: a * b
    DIVIDE,             // Division: a / b
    MODULO,             // Modulo: a % b
    
    // Logical operations
    AND,                // Logical AND: a && b
    OR,                 // Logical OR: a || b
    NOT,                // Logical NOT: !a
    
    // Comparison operations
    LESS_THAN,          // Less than: a < b
    GREATER_THAN,       // Greater than: a > b
    LESS_EQUAL,         // Less or equal: a <= b
    GREATER_EQUAL,      // Greater or equal: a >= b
    EQUAL,              // Equal: a == b
    DIFFERENT,          // Different: a != b
    
    // Literal types
    BYTE,               // Byte literal
    INT,                // Integer literal
    REAL,               // Real number literal
    CHAR,               // Character literal
    LITERAL,            // General literal
    
    // Control flow
    IF,                 // If statement
    ELSE,               // Else clause
    WHILE_DO,           // While loop
    DO_WHILE,           // Do-while loop
    
    // I/O operations
    READ,               // Read operation
    PRINT,              // Print operation
    RETURN,             // Return statement
};

extern std::vector<std::string> ASTTypeNames;

// AST node structure - represents a node in the abstract syntax tree
class ASTNode {
private:
    ASTNodeType type;               // Node type from ASTNodeType enum above
    std::vector<ASTNode*> children; // Child nodes
    Symbol* symbol;                 // Associated symbol (if any)
    int lineNumber;                 // Line number in source code

public:
    ASTNode(ASTNodeType type = ASTNodeType::UNKNOWN, Symbol* symbol = nullptr, std::vector<ASTNode*> children = {}, int lineNumber = 0);
    
    // Tree manipulation
    void addChild(ASTNode* child);
    
    // Getters
    std::vector<ASTNode*> getChildren();
    Symbol* getSymbol();
    ASTNodeType getType();
    int getLineNumber() const;
    
    // Setters
    void setLineNumber(int line);
};

// Public interface functions - AST manipulation and output
void printAST(int level, ASTNode* node);                               // Print AST structure to stderr
void printASTToFile(const std::string& filename, ASTNode* node);       // Print AST structure to file
void decompileAST(std::ofstream& outFile, ASTNode* node, int depth);   // Decompile AST back to source code