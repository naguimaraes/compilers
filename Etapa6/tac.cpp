// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Three Address Code (TAC) implementation file made by Nathan Alonso Guimarães (00334437)

#include "tac.hpp"
#include "parser.tab.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>

// Forward declarations for internal functions
TAC* tacCreate(TACType type, Symbol* res = nullptr, Symbol* op1 = nullptr, Symbol* op2 = nullptr);
TAC* tacJoin(TAC* tac1, TAC* tac2);
TAC* tacReverse(TAC* tac);
void tacPrintBackwards(TAC* tac);
Symbol* makeTemp(dataType type);
Symbol* makeLabel();
Symbol* getResultSymbol(TAC* tacSequence, ASTNode* node);
std::string truncateString(const std::string& str, size_t maxWidth);
std::string tacTypeToString(TACType type);

// Code generation functions - internal use only
TAC* generateBinaryOp(ASTNode* node);
TAC* generateUnaryOp(ASTNode* node);
TAC* generateAssignment(ASTNode* node);
TAC* generateVariableDeclaration(ASTNode* node);
TAC* generateVectorDeclaration(ASTNode* node);
TAC* generateVectorInitialization(ASTNode* node);
TAC* generateIf(ASTNode* node);
TAC* generateWhile(ASTNode* node);
TAC* generateDoWhile(ASTNode* node);
TAC* generateFunctionCall(ASTNode* node);
TAC* generateFunctionDeclaration(ASTNode* node);
TAC* generateReturn(ASTNode* node);
TAC* generatePrint(ASTNode* node);
TAC* generateRead(ASTNode* node);
TAC* generateVectorAccess(ASTNode* node);
TAC* generateVectorAssignment(ASTNode* node);

// Global counters for temporary variables and labels
static int tempCounter = 0;
static int labelCounter = 0;

// TAC class implementation
TAC::TAC(TACType type, Symbol* res, Symbol* op1, Symbol* op2) 
    : type(type), result(res), operand1(op1), operand2(op2), previous(nullptr), next(nullptr) {
}

TAC::~TAC() {
    // Don't delete symbols as they are managed by the symbol table
}

TACType TAC::getType() const {
    return type;
}

Symbol* TAC::getRes() const {
    return result;
}

Symbol* TAC::getOp1() const {
    return operand1;
}

Symbol* TAC::getOp2() const {
    return operand2;
}

TAC* TAC::getPrev() const {
    return previous;
}

TAC* TAC::getNext() const {
    return next;
}

void TAC::setPrev(TAC* prev) {
    this->previous = prev;
}

void TAC::setNext(TAC* next) {
    this->next = next;
}

// Helper function to truncate strings that are too long for table columns
std::string truncateString(const std::string& str, size_t maxWidth) {
    if (str.length() <= maxWidth) {
        return str;
    }
    
    // If it's too long, truncate and add "..." if there's space
    if (maxWidth <= 3) {
        return str.substr(0, maxWidth);
    } else {
        return str.substr(0, maxWidth - 3) + "...";
    }
}

void TAC::print() const {
    // Don't print SYMBOL as they are utility instructions
    if (type == TACType::SYMBOL) {
        return;
    }
    
    // Get strings and truncate if necessary
    std::string typeStr = tacTypeToString(type);
    std::string resStr = result ? result->getLexeme() : "";
    std::string op1Str = operand1 ? operand1->getLexeme() : "";
    std::string op2Str = operand2 ? operand2->getLexeme() : "";
    
    // Truncate strings to fit in 12-character columns
    typeStr = truncateString(typeStr, 11); // 11 to leave space for proper alignment
    resStr = truncateString(resStr, 11);
    op1Str = truncateString(op1Str, 11);
    op2Str = truncateString(op2Str, 11);
    
    // Print in table format with fixed-width columns
    std::cout << "| " << std::left << std::setw(12) << typeStr;
    std::cout << "| " << std::setw(12) << resStr;
    std::cout << "| " << std::setw(12) << op1Str;
    std::cout << "| " << std::setw(12) << op2Str;
    std::cout << "|" << std::endl;
}

// TAC list management functions
TAC* tacCreate(TACType type, Symbol* res, Symbol* op1, Symbol* op2) {
    return new TAC(type, res, op1, op2);
}

TAC* tacJoin(TAC* tac1, TAC* tac2) {
    if (!tac1) return tac2;
    if (!tac2) return tac1;
    
    // Find the last instruction in tac1 by going forward
    TAC* last = tac1;
    while (last->getNext()) {
        last = last->getNext();
    }
    
    // Connect tac1 and tac2
    last->setNext(tac2);
    if (tac2) {
        tac2->setPrev(last);
    }
    
    return tac1;
}

TAC* tacReverse(TAC* tac) {
    if (!tac) return nullptr;
    
    TAC* current = tac;
    TAC* newHead = nullptr;
    
    // Find the beginning of the list (last instruction in inverted list)
    while (current->getPrev()) {
        current = current->getPrev();
    }
    
    // Reverse the list
    while (current) {
        TAC* next = current->getNext();
        current->setNext(current->getPrev());
        current->setPrev(next);
        newHead = current;
        current = next;
    }
    
    return newHead;
}

void printTAC(TAC* tac) {
    if (!tac) return;

    // Print table header
    std::cout << "+-------------------------------------------------------+" << std::endl;
    std::cout << "|       TAC list made by Nathan Guimaraes (334437)      |" << std::endl;
    std::cout << "+-------------+-------------+-------------+-------------+" << std::endl;
    std::cout << "| " << std::left << std::setw(12) << "OPERATION";
    std::cout << "| " << std::setw(12) << "RESULT";
    std::cout << "| " << std::setw(12) << "OPERAND1";
    std::cout << "| " << std::setw(12) << "OPERAND2";
    std::cout << "|" << std::endl;
    std::cout << "+-------------+-------------+-------------+-------------+" << std::endl;

    // Print from current position backwards (reverse order)
    TAC* current = tac;
    while (current) {
        current->print();
        current = current->getPrev();
    }
    
    // Print table footer
    std::cout << "+-------------+-------------+-------------+-------------+" << std::endl;
}

void printTACToFile(const std::string& filename, TAC* tac) {
    if (!tac) return;
    
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        fprintf(stderr, "Error: Could not open file %s for writing TAC.\n", filename.c_str());
        return;
    }
    
    // Print table header
    outFile << "+-------------------------------------------------------+" << std::endl;
    outFile << "|       TAC LIST MADE BY NATHAN GUIMARAES (334437)      |" << std::endl;
    outFile << "+-------------+-------------+-------------+-------------+" << std::endl;
    outFile << "| " << std::left << std::setw(12) << "OPERATION";
    outFile << "| " << std::setw(12) << "RESULT";
    outFile << "| " << std::setw(12) << "OPERAND1";
    outFile << "| " << std::setw(12) << "OPERAND2";
    outFile << "|" << std::endl;
    outFile << "+-------------+-------------+-------------+-------------+" << std::endl;
    
    // Print from current position forwards (normal order)
    TAC* current = tac;
    while (current) {
        // Don't print SYMBOL as they are utility instructions
        if (current->getType() != TACType::SYMBOL) {
            // Get strings and truncate if necessary
            std::string typeStr = tacTypeToString(current->getType());
            std::string resStr = current->getRes() ? current->getRes()->getLexeme() : "";
            std::string op1Str = current->getOp1() ? current->getOp1()->getLexeme() : "";
            std::string op2Str = current->getOp2() ? current->getOp2()->getLexeme() : "";
            
            // Truncate strings to fit in 12-character columns
            typeStr = truncateString(typeStr, 11);
            resStr = truncateString(resStr, 11);
            op1Str = truncateString(op1Str, 11);
            op2Str = truncateString(op2Str, 11);
            
            // Print in table format with fixed-width columns
            outFile << "| " << std::left << std::setw(12) << typeStr;
            outFile << "| " << std::setw(12) << resStr;
            outFile << "| " << std::setw(12) << op1Str;
            outFile << "| " << std::setw(12) << op2Str;
            outFile << "|" << std::endl;
        }
        current = current->getNext();
    }
    
    // Print table footer
    outFile << "+-------------+-------------+-------------+-------------+" << std::endl;
    
    outFile.close();
}

void tacPrintBackwards(TAC* tac) {
    TAC* current = tac;
    
    while (current) {
        current->print();
        current = current->getPrev();
    }
}

// Helper function to normalize numeric symbols for TAC generation
Symbol* normalizeNumericSymbol(Symbol* symbol) {
    if (!symbol) return symbol;
    
    // Check if it's a numeric literal (integer)
    if (symbol->getType() == LIT_INT) {
        std::string lexeme = symbol->getLexeme();
        std::string normalized = removeZeros(lexeme);
        
        // If the normalized value is different, create a new symbol
        if (normalized != lexeme) {
            return insertSymbol(normalized, LIT_INT, symbol->getLineNumber());
        }
    }
    
    // Check if it's a real number literal (format a/b)
    if (symbol->getType() == LIT_REAL) {
        std::string lexeme = symbol->getLexeme();
        size_t slashPos = lexeme.find('/');
        if (slashPos != std::string::npos) {
            std::string numeratorStr = lexeme.substr(0, slashPos);
            std::string denominatorStr = lexeme.substr(slashPos + 1);
            
            // Remove leading zeros from both parts
            std::string normalizedNumerator = removeZeros(numeratorStr);
            std::string normalizedDenominator = removeZeros(denominatorStr);
            
            std::string normalized = normalizedNumerator + "/" + normalizedDenominator;
            
            // If the normalized value is different, create a new symbol
            if (normalized != lexeme) {
                return insertSymbol(normalized, LIT_REAL, symbol->getLineNumber());
            }
        }
    }
    
    return symbol;
}

// Helper function to get the result symbol from a TAC sequence
Symbol* getResultSymbol(TAC* tacSequence, ASTNode* node) {
    if (!tacSequence && !node) return nullptr;
    
    // If the node is a direct symbol, return it
    if (node && node->getType() == ASTNodeType::SYMBOL) {
        return normalizeNumericSymbol(node->getSymbol());
    }
    
    // If the node is a literal (numbers, characters, etc), return its symbol
    if (node && (node->getType() == ASTNodeType::LITERAL || 
                 node->getType() == ASTNodeType::BYTE ||
                 node->getType() == ASTNodeType::INT ||
                 node->getType() == ASTNodeType::REAL ||
                 node->getType() == ASTNodeType::CHAR)) {
        return normalizeNumericSymbol(node->getSymbol());
    }
    
    // Otherwise, find the last TAC instruction with a result
    if (tacSequence) {
        TAC* current = tacSequence;
        TAC* lastWithResult = nullptr;
        
        // Find the last instruction with a result by going forward
        while (current) {
            if (current->getRes()) {
                lastWithResult = current;
            }
            current = current->getNext();
        }
        
        if (lastWithResult) {
            return lastWithResult->getRes();
        }
    }
    
    return nullptr;
}

// Symbol creation functions
Symbol* makeTemp(dataType type = dataType::INT) {
    std::stringstream ss;
    ss << "__temp" << tempCounter++;
    Symbol* temp = insertSymbol(ss.str(), INTERNAL, 1); // Using INTERNAL type and line number 1 as default
    temp->setIdentifierType(identifierType::TEMP); // Set as TEMP for temporary variables
    temp->setDataType(type); // Set the inferred data type
    return temp;
}

Symbol* makeLabel() {
    std::stringstream ss;
    ss << "__label" << labelCounter++;
    Symbol* label = insertSymbol(ss.str(), INTERNAL, 1); // Using INTERNAL type and line number 1 as default
    label->setIdentifierType(identifierType::LABEL); // Set as LABEL for labels
    label->setDataType(dataType::ADDRESS); // Set as ADDRESS for labels
    return label;
}

// Helper function to infer data type from operands
dataType inferDataType(Symbol* leftOperand, Symbol* rightOperand = nullptr) {
    if (!leftOperand) {
        return dataType::INT; // Default to INT
    }
    
    // If left operand has a defined type, use it
    if (leftOperand->getDataType() != dataType::UNDEFINED) {
        return leftOperand->getDataType();
    }
    
    // If right operand exists and has a defined type, use it
    if (rightOperand && rightOperand->getDataType() != dataType::UNDEFINED) {
        return rightOperand->getDataType();
    }
    
    // Check token types to infer
    if (leftOperand->getType() == LIT_INT) {
        return dataType::INT;
    }
    if (leftOperand->getType() == LIT_REAL) {
        return dataType::REAL;
    }
    if (leftOperand->getType() == LIT_CHAR) {
        return dataType::CHAR;
    }
    if (leftOperand->getType() == LIT_STRING) {
        return dataType::STRING;
    }
    
    // Default to INT for unknown types
    return dataType::INT;
}

// Helper function to infer data type for comparison operations (always return INT for boolean results)
dataType inferComparisonDataType(Symbol* leftOperand, Symbol* rightOperand = nullptr) {
    // Comparison operations always return boolean (represented as INT in our system)
    return dataType::INT;
}

// Main code generation function
TAC* generateTAC(ASTNode* node) {
    if (!node) return nullptr;
    
    switch (node->getType()) {
        case ASTNodeType::SYMBOL:
            // For symbols, we don't generate TAC, just return nullptr
            // The symbol will be used directly by parent operations
            return nullptr;
            
        case ASTNodeType::LITERAL:
        case ASTNodeType::BYTE:
        case ASTNodeType::INT:
        case ASTNodeType::REAL:
        case ASTNodeType::CHAR:
            // For literals, we don't generate TAC, just return nullptr
            // The symbol will be used directly by parent operations
            return nullptr;
            
        case ASTNodeType::ADD:
        case ASTNodeType::SUBTRACT:
        case ASTNodeType::MULTIPLY:
        case ASTNodeType::DIVIDE:
        case ASTNodeType::MODULO:
        case ASTNodeType::AND:
        case ASTNodeType::OR:
        case ASTNodeType::LESS_THAN:
        case ASTNodeType::GREATER_THAN:
        case ASTNodeType::LESS_EQUAL:
        case ASTNodeType::GREATER_EQUAL:
        case ASTNodeType::EQUAL:
        case ASTNodeType::DIFFERENT:
            return generateBinaryOp(node);
            
        case ASTNodeType::NOT:
            return generateUnaryOp(node);
            
        case ASTNodeType::ASSIGNMENT:
            return generateAssignment(node);
            
        case ASTNodeType::VARIABLE_DECLARATION:
            return generateVariableDeclaration(node);
            
        case ASTNodeType::VECTOR_DECLARATION:
            return generateVectorDeclaration(node);
            
        case ASTNodeType::VECTOR_INITIALIZATION:
            return generateVectorInitialization(node);
            
        case ASTNodeType::IF:
            return generateIf(node);
            
        case ASTNodeType::WHILE_DO:
            return generateWhile(node);
            
        case ASTNodeType::DO_WHILE:
            return generateDoWhile(node);
            
        case ASTNodeType::FUNCTION_CALL:
            return generateFunctionCall(node);
            
        case ASTNodeType::FUNCTION_DECLARATION:
            return generateFunctionDeclaration(node);
            
        case ASTNodeType::RETURN:
            return generateReturn(node);
            
        case ASTNodeType::PRINT:
            return generatePrint(node);
            
        case ASTNodeType::READ:
            return generateRead(node);
            
        case ASTNodeType::VECTOR_ACCESS:
            return generateVectorAccess(node);
            
        case ASTNodeType::COMMAND_LIST:
        case ASTNodeType::DECLARATION_LIST:
        case ASTNodeType::EXPRESSION_LIST: {
            TAC* result = nullptr;
            for (ASTNode* child : node->getChildren()) {
                TAC* childCode = generateTAC(child);
                result = tacJoin(result, childCode);
            }
            return result;
        }
        
        default:
            // For other node types, process children
            TAC* result = nullptr;
            for (ASTNode* child : node->getChildren()) {
                TAC* childCode = generateTAC(child);
                result = tacJoin(result, childCode);
            }
            return result;
    }
}

TAC* generateBinaryOp(ASTNode* node) {
    if (node->getChildren().size() < 2) return nullptr;
    
    TAC* leftCode = generateTAC(node->getChildren()[0]);
    TAC* rightCode = generateTAC(node->getChildren()[1]);
    
    // Extract symbols from child nodes or their generated code
    Symbol* leftSymbol = getResultSymbol(leftCode, node->getChildren()[0]);
    Symbol* rightSymbol = getResultSymbol(rightCode, node->getChildren()[1]);
    
    // Infer data type for the temporary variable
    dataType tempType;
    TACType tacType;
    
    switch (node->getType()) {
        case ASTNodeType::ADD: 
            tacType = TACType::ADD;
            tempType = inferDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::SUBTRACT: 
            tacType = TACType::SUB;
            tempType = inferDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::MULTIPLY: 
            tacType = TACType::MUL;
            tempType = inferDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::DIVIDE: 
            tacType = TACType::DIV;
            tempType = inferDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::MODULO: 
            tacType = TACType::MOD;
            tempType = inferDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::AND: 
            tacType = TACType::AND;
            tempType = inferComparisonDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::OR: 
            tacType = TACType::OR;
            tempType = inferComparisonDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::LESS_THAN: 
            tacType = TACType::LT;
            tempType = inferComparisonDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::GREATER_THAN: 
            tacType = TACType::GT;
            tempType = inferComparisonDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::LESS_EQUAL: 
            tacType = TACType::LE;
            tempType = inferComparisonDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::GREATER_EQUAL: 
            tacType = TACType::GE;
            tempType = inferComparisonDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::EQUAL: 
            tacType = TACType::EQ;
            tempType = inferComparisonDataType(leftSymbol, rightSymbol);
            break;
        case ASTNodeType::DIFFERENT: 
            tacType = TACType::NE;
            tempType = inferComparisonDataType(leftSymbol, rightSymbol);
            break;
        default: 
            tacType = TACType::MOVE;
            tempType = inferDataType(leftSymbol, rightSymbol);
            break;
    }
    
    Symbol* temp = makeTemp(tempType);
    TAC* opTac = tacCreate(tacType, temp, leftSymbol, rightSymbol);
    
    TAC* result = tacJoin(leftCode, rightCode);
    result = tacJoin(result, opTac);
    
    return result;
}

TAC* generateUnaryOp(ASTNode* node) {
    if (node->getChildren().empty()) return nullptr;
    
    TAC* operandCode = generateTAC(node->getChildren()[0]);
    Symbol* operandSymbol = getResultSymbol(operandCode, node->getChildren()[0]);
    
    TACType tacType;
    dataType tempType;
    
    switch (node->getType()) {
        case ASTNodeType::NOT: 
            tacType = TACType::NOT;
            tempType = inferComparisonDataType(operandSymbol); // NOT returns boolean (INT)
            break;
        default: 
            tacType = TACType::MOVE;
            tempType = inferDataType(operandSymbol);
            break;
    }
    
    Symbol* temp = makeTemp(tempType);
    TAC* opTac = tacCreate(tacType, temp, operandSymbol, nullptr);
    
    return tacJoin(operandCode, opTac);
}

TAC* generateAssignment(ASTNode* node) {
    if (node->getChildren().size() < 2) return nullptr;
    
    // Check if this is a vector assignment (left side is vector access)
    if (node->getChildren()[0]->getType() == ASTNodeType::VECTOR_ACCESS) {
        return generateVectorAssignment(node);
    }
    
    TAC* valueCode = generateTAC(node->getChildren()[1]);
    Symbol* target = node->getChildren()[0]->getSymbol();
    Symbol* valueSymbol = getResultSymbol(valueCode, node->getChildren()[1]);
    
    TAC* assignTac = tacCreate(TACType::MOVE, target, valueSymbol, nullptr);
    
    return tacJoin(valueCode, assignTac);
}

TAC* generateIf(ASTNode* node) {
    if (node->getChildren().size() < 2) return nullptr;
    
    TAC* conditionCode = generateTAC(node->getChildren()[0]);
    TAC* thenCode = generateTAC(node->getChildren()[1]);
    TAC* elseCode = nullptr;
    
    if (node->getChildren().size() > 2) {
        elseCode = generateTAC(node->getChildren()[2]);
    }
    
    Symbol* conditionSymbol = getResultSymbol(conditionCode, node->getChildren()[0]);
    
    Symbol* elseLabel = makeLabel();
    Symbol* endLabel = makeLabel();
    
    TAC* ifzTac = tacCreate(TACType::IFZ, nullptr, conditionSymbol, elseLabel);
    TAC* elseLabelTac = tacCreate(TACType::LABEL, nullptr, elseLabel, nullptr);
    TAC* endLabelTac = tacCreate(TACType::LABEL, nullptr, endLabel, nullptr);
    
    TAC* result = tacJoin(conditionCode, ifzTac);
    result = tacJoin(result, thenCode);
    
    if (elseCode) {
        TAC* jumpTac = tacCreate(TACType::JUMP, nullptr, endLabel, nullptr);
        result = tacJoin(result, jumpTac);
        result = tacJoin(result, elseLabelTac);
        result = tacJoin(result, elseCode);
        result = tacJoin(result, endLabelTac);
    } else {
        result = tacJoin(result, elseLabelTac);
    }
    
    return result;
}

TAC* generateWhile(ASTNode* node) {
    if (node->getChildren().size() < 2) return nullptr;
    
    Symbol* beginLabel = makeLabel();
    Symbol* endLabel = makeLabel();
    
    TAC* beginLabelTac = tacCreate(TACType::LABEL, nullptr, beginLabel, nullptr);
    TAC* conditionCode = generateTAC(node->getChildren()[0]);
    TAC* bodyCode = generateTAC(node->getChildren()[1]);
    
    Symbol* conditionSymbol = getResultSymbol(conditionCode, node->getChildren()[0]);
    
    TAC* ifzTac = tacCreate(TACType::IFZ, nullptr, conditionSymbol, endLabel);
    TAC* jumpTac = tacCreate(TACType::JUMP, nullptr, beginLabel, nullptr);
    TAC* endLabelTac = tacCreate(TACType::LABEL, nullptr, endLabel, nullptr);
    
    TAC* result = tacJoin(beginLabelTac, conditionCode);
    result = tacJoin(result, ifzTac);
    result = tacJoin(result, bodyCode);
    result = tacJoin(result, jumpTac);
    result = tacJoin(result, endLabelTac);
    
    return result;
}

// Helper function to process expression list recursively (collects all arguments in order)
void collectExpressionListArgs(ASTNode* node, std::vector<ASTNode*>& args) {
    if (node == nullptr) return;
    
    if (node->getType() == ASTNodeType::EXPRESSION_LIST) {
        // For EXPRESSION_LIST, the first child is an expression, 
        // and the remaining children (if any) are more EXPRESSION_LISTs
        if (!node->getChildren().empty()) {
            // Add the first child (the actual expression)
            args.push_back(node->getChildren()[0]);
            
            // Recursively process remaining children
            for (size_t i = 1; i < node->getChildren().size(); i++) {
                collectExpressionListArgs(node->getChildren()[i], args);
            }
        }
    } else {
        // If it's not an EXPRESSION_LIST, it's a direct expression
        args.push_back(node);
    }
}

TAC* generateFunctionCall(ASTNode* node) {
    if (node->getChildren().empty()) return nullptr;
    
    // The function symbol might be in the node itself or in the first child
    Symbol* functionSymbol = node->getSymbol();
    if (!functionSymbol && !node->getChildren().empty()) {
        functionSymbol = node->getChildren()[0]->getSymbol();
    }
    
    TAC* result = nullptr;
    
    // Generate code for arguments (skip first child if it's the function name)
    // If function symbol is in the node itself, all children are arguments
    // If function symbol is in first child, skip the first child
    size_t startIndex = (node->getSymbol() != nullptr) ? 0 : 1;
    
    if (node->getChildren().size() > startIndex) {
        for (size_t i = node->getChildren().size() - 1; i >= startIndex; i--) {
            ASTNode* child = node->getChildren()[i];
            
            // If this child is an arguments list, process its children
            if (child->getType() == ASTNodeType::ARGUMENTS_LIST) {
                // Collect all arguments from the nested EXPRESSION_LIST structure
                std::vector<ASTNode*> args;
                
                for (size_t j = 0; j < child->getChildren().size(); j++) {
                    collectExpressionListArgs(child->getChildren()[j], args);
                }
                
                // Generate ARG TACs in correct order (not reversed)
                for (size_t k = 0; k < args.size(); k++) {
                    ASTNode* arg = args[k];
                    TAC* argCode = generateTAC(arg);
                    Symbol* argSymbol = getResultSymbol(argCode, arg);
                    
                    TAC* argTac = tacCreate(TACType::ARG, nullptr, argSymbol, nullptr);
                    result = tacJoin(result, argCode);
                    result = tacJoin(result, argTac);
                }
            } else {
                // Process single argument
                TAC* argCode = generateTAC(child);
                Symbol* argSymbol = getResultSymbol(argCode, child);
                
                TAC* argTac = tacCreate(TACType::ARG, nullptr, argSymbol, nullptr);
                result = tacJoin(result, argCode);
                result = tacJoin(result, argTac);
            }
            
            if (i == startIndex) break; // Avoid underflow in unsigned arithmetic
        }
    }
    
    Symbol* temp = makeTemp(dataType::INT); // Function calls typically return INT, could be improved with function signature analysis
    TAC* callTac = tacCreate(TACType::CALL, temp, functionSymbol, nullptr);
    result = tacJoin(result, callTac);
    
    return result;
}

TAC* generateFunctionDeclaration(ASTNode* node) {
    if (node->getChildren().empty()) return nullptr;
    
    Symbol* functionSymbol = node->getSymbol();
    
    // Check children for function symbol (usually in second child - index 1)
    for (size_t i = 0; i < node->getChildren().size() && !functionSymbol; i++) {
        ASTNode* child = node->getChildren()[i];
        if (child->getType() == ASTNodeType::SYMBOL && child->getSymbol()) {
            functionSymbol = child->getSymbol();
            break;
        }
    }
    
    TAC* beginTac = tacCreate(TACType::BEGINFUN, nullptr, functionSymbol, nullptr);
    
    TAC* bodyCode = nullptr;
    for (size_t i = 1; i < node->getChildren().size(); i++) {
        TAC* childCode = generateTAC(node->getChildren()[i]);
        bodyCode = tacJoin(bodyCode, childCode);
    }
    
    TAC* endTac = tacCreate(TACType::ENDFUN, nullptr, functionSymbol, nullptr);
    
    TAC* result = tacJoin(beginTac, bodyCode);
    result = tacJoin(result, endTac);
    
    return result;
}

TAC* generateReturn(ASTNode* node) {
    TAC* valueCode = nullptr;
    Symbol* valueSymbol = nullptr;
    
    if (!node->getChildren().empty()) {
        valueCode = generateTAC(node->getChildren()[0]);
        valueSymbol = getResultSymbol(valueCode, node->getChildren()[0]);
    }
    
    TAC* retTac = tacCreate(TACType::RET, nullptr, valueSymbol, nullptr);
    
    return tacJoin(valueCode, retTac);
}

// Helper function to process print list recursively
TAC* processPrintList(ASTNode* node) {
    if (node == nullptr) return nullptr;
    
    TAC* result = nullptr;
    
    // Process current node based on its type
    if (node->getType() == ASTNodeType::LITERAL || 
        node->getType() == ASTNodeType::SYMBOL ||
        node->getType() == ASTNodeType::VECTOR_ACCESS ||
        node->getType() == ASTNodeType::FUNCTION_CALL ||
        node->getType() == ASTNodeType::ADD ||
        node->getType() == ASTNodeType::SUBTRACT ||
        node->getType() == ASTNodeType::MULTIPLY ||
        node->getType() == ASTNodeType::DIVIDE ||
        node->getType() == ASTNodeType::MODULO ||
        node->getType() == ASTNodeType::LESS_THAN ||
        node->getType() == ASTNodeType::GREATER_THAN ||
        node->getType() == ASTNodeType::LESS_EQUAL ||
        node->getType() == ASTNodeType::GREATER_EQUAL ||
        node->getType() == ASTNodeType::EQUAL ||
        node->getType() == ASTNodeType::DIFFERENT ||
        node->getType() == ASTNodeType::AND ||
        node->getType() == ASTNodeType::OR ||
        node->getType() == ASTNodeType::NOT) {
        
        // For VECTOR_ACCESS, we need to handle it specially
        if (node->getType() == ASTNodeType::VECTOR_ACCESS && node->getChildren().size() >= 2) {
            // Create a temporary VECTOR_ACCESS node with only vector and index (first 2 children)
            ASTNode tempVectorAccess(ASTNodeType::VECTOR_ACCESS, nullptr, {}, node->getLineNumber());
            tempVectorAccess.addChild(node->getChildren()[0]); // vector symbol
            tempVectorAccess.addChild(node->getChildren()[1]); // index
            
            // Generate TAC for vector access
            TAC* valueCode = generateTAC(&tempVectorAccess);
            Symbol* valueSymbol = getResultSymbol(valueCode, &tempVectorAccess);
            
            TAC* printTac = tacCreate(TACType::PRINT, nullptr, valueSymbol, nullptr);
            result = tacJoin(valueCode, printTac);
            
            // Process the remaining children as the next items in the print list
            if (node->getChildren().size() > 2) {
                TAC* nextTac = processPrintList(node->getChildren()[2]);
                result = tacJoin(result, nextTac);
            }
        } else {
            // For arithmetic operations and other expressions, don't process children as separate print items
            if (node->getType() == ASTNodeType::ADD ||
                node->getType() == ASTNodeType::SUBTRACT ||
                node->getType() == ASTNodeType::MULTIPLY ||
                node->getType() == ASTNodeType::DIVIDE ||
                node->getType() == ASTNodeType::MODULO ||
                node->getType() == ASTNodeType::LESS_THAN ||
                node->getType() == ASTNodeType::GREATER_THAN ||
                node->getType() == ASTNodeType::LESS_EQUAL ||
                node->getType() == ASTNodeType::GREATER_EQUAL ||
                node->getType() == ASTNodeType::EQUAL ||
                node->getType() == ASTNodeType::DIFFERENT ||
                node->getType() == ASTNodeType::AND ||
                node->getType() == ASTNodeType::OR ||
                node->getType() == ASTNodeType::NOT ||
                node->getType() == ASTNodeType::FUNCTION_CALL) {
                
                // For expressions, just generate TAC and print the result
                TAC* valueCode = generateTAC(node);
                Symbol* valueSymbol = getResultSymbol(valueCode, node);
                
                TAC* printTac = tacCreate(TACType::PRINT, nullptr, valueSymbol, nullptr);
                result = tacJoin(valueCode, printTac);
                
                // Special case: check if there's a third child that's a literal (like "\n")
                // This happens when the parser includes "\n" as part of the expression
                if (node->getChildren().size() > 2 && 
                    node->getChildren()[2]->getType() == ASTNodeType::LITERAL) {
                    TAC* nextTac = processPrintList(node->getChildren()[2]);
                    result = tacJoin(result, nextTac);
                }
                
                // Don't process first two children as separate print items for expressions
            } else {
                // For other types (literals, symbols), process normally
                TAC* valueCode = generateTAC(node);
                Symbol* valueSymbol = getResultSymbol(valueCode, node);
                
                TAC* printTac = tacCreate(TACType::PRINT, nullptr, valueSymbol, nullptr);
                result = tacJoin(valueCode, printTac);
                
                // Process the next item in the list (first child) only for non-expression nodes
                if (!node->getChildren().empty() && 
                    node->getType() != ASTNodeType::ADD &&
                    node->getType() != ASTNodeType::SUBTRACT &&
                    node->getType() != ASTNodeType::MULTIPLY &&
                    node->getType() != ASTNodeType::DIVIDE &&
                    node->getType() != ASTNodeType::MODULO &&
                    node->getType() != ASTNodeType::LESS_THAN &&
                    node->getType() != ASTNodeType::GREATER_THAN &&
                    node->getType() != ASTNodeType::LESS_EQUAL &&
                    node->getType() != ASTNodeType::GREATER_EQUAL &&
                    node->getType() != ASTNodeType::EQUAL &&
                    node->getType() != ASTNodeType::DIFFERENT &&
                    node->getType() != ASTNodeType::AND &&
                    node->getType() != ASTNodeType::OR &&
                    node->getType() != ASTNodeType::NOT &&
                    node->getType() != ASTNodeType::FUNCTION_CALL) {
                    TAC* nextTac = processPrintList(node->getChildren()[0]);
                    result = tacJoin(result, nextTac);
                }
            }
        }
    }
    
    return result;
}

TAC* generatePrint(ASTNode* node) {
    if (node->getChildren().empty()) return nullptr;
    
    return processPrintList(node->getChildren()[0]);
}

TAC* generateRead(ASTNode* node) {
    if (node->getChildren().empty()) return nullptr;
    
    Symbol* targetSymbol = node->getChildren()[0]->getSymbol();
    TAC* readTac = tacCreate(TACType::READ, targetSymbol, nullptr, nullptr);
    
    return readTac;
}

TAC* generateVectorAccess(ASTNode* node) {
    if (node->getChildren().size() < 2) return nullptr;
    
    TAC* indexCode = generateTAC(node->getChildren()[1]);
    Symbol* vectorSymbol = node->getChildren()[0]->getSymbol();
    Symbol* indexSymbol = getResultSymbol(indexCode, node->getChildren()[1]);
    
    // Infer type from vector symbol (vector access returns the element type)
    dataType tempType = inferDataType(vectorSymbol);
    
    Symbol* temp = makeTemp(tempType);
    TAC* vecreadTac = tacCreate(TACType::VECREAD, temp, vectorSymbol, indexSymbol);
    
    return tacJoin(indexCode, vecreadTac);
}

TAC* generateVectorAssignment(ASTNode* node) {
    if (node->getChildren().size() < 2) return nullptr;
    
    // First child is the vector access (v[index])
    ASTNode* vectorAccessNode = node->getChildren()[0];
    if (vectorAccessNode->getChildren().size() < 2) return nullptr;
    
    // Generate code for index and value
    TAC* indexCode = generateTAC(vectorAccessNode->getChildren()[1]);
    TAC* valueCode = generateTAC(node->getChildren()[1]);
    
    Symbol* vectorSymbol = vectorAccessNode->getChildren()[0]->getSymbol();
    Symbol* indexSymbol = getResultSymbol(indexCode, vectorAccessNode->getChildren()[1]);
    Symbol* valueSymbol = getResultSymbol(valueCode, node->getChildren()[1]);
    
    TAC* vecwriteTac = tacCreate(TACType::VECWRITE, vectorSymbol, indexSymbol, valueSymbol);
    
    TAC* result = tacJoin(indexCode, valueCode);
    result = tacJoin(result, vecwriteTac);
    
    return result;
}

// Generation functions for declarations
TAC* generateVariableDeclaration(ASTNode* node) {
    // Variable declarations with initialization generate MOVE instructions
    if (node->getChildren().size() >= 2) {
        // Structure might be: VARIABLE_DECLARATION -> [SYMBOL, INIT_VALUE]
        // or: VARIABLE_DECLARATION -> [TYPE, SYMBOL, INIT_VALUE]
        
        Symbol* varSymbol = nullptr;
        ASTNode* valueNode = nullptr;
        
        // Try to find the variable symbol and value
        if (node->getSymbol()) {
            // Variable symbol is in the main node
            varSymbol = node->getSymbol();
            valueNode = node->getChildren()[node->getChildren().size() - 1]; // Last child is value
        } else if (node->getChildren().size() >= 2) {
            // Find symbol in children
            for (size_t i = 0; i < node->getChildren().size(); i++) {
                if (node->getChildren()[i]->getType() == ASTNodeType::SYMBOL) {
                    varSymbol = node->getChildren()[i]->getSymbol();
                    if (i + 1 < node->getChildren().size()) {
                        valueNode = node->getChildren()[i + 1];
                    }
                    break;
                }
            }
        }
        
        if (varSymbol && valueNode) {
            TAC* valueCode = generateTAC(valueNode);
            Symbol* valueSymbol = getResultSymbol(valueCode, valueNode);
            
            TAC* assignTac = tacCreate(TACType::INIT, varSymbol, valueSymbol, nullptr);
            return tacJoin(valueCode, assignTac);
        }
    }
    
    // Variable declaration without initialization - no TAC needed
    return nullptr;
}

// Helper function to extract initialization values from nested VECTOR_INITIALIZATION nodes
void extractInitValues(ASTNode* initNode, std::vector<Symbol*>& values) {
    if (!initNode) return;
    
    // If this node has children, recursively process them
    for (ASTNode* child : initNode->getChildren()) {
        if (child->getType() == ASTNodeType::LITERAL ||
            child->getType() == ASTNodeType::INT ||
            child->getType() == ASTNodeType::REAL ||
            child->getType() == ASTNodeType::CHAR ||
            child->getType() == ASTNodeType::BYTE) {
            values.push_back(child->getSymbol());
        } else if (child->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
            extractInitValues(child, values);
        }
    }
}

TAC* generateVectorDeclaration(ASTNode* node) {
    if (!node || node->getChildren().empty()) return nullptr;
    
    // Get vector symbol, size, and initialization values
    Symbol* vectorSymbol = nullptr;
    Symbol* sizeSymbol = nullptr;
    ASTNode* initNode = nullptr;
    
    // Find vector symbol, size, and initialization node in children
    for (size_t i = 0; i < node->getChildren().size(); i++) {
        ASTNode* child = node->getChildren()[i];
        if (child->getType() == ASTNodeType::SYMBOL && !vectorSymbol) {
            vectorSymbol = child->getSymbol();
        } else if ((child->getType() == ASTNodeType::LITERAL || 
                   child->getType() == ASTNodeType::INT || 
                   child->getType() == ASTNodeType::REAL || 
                   child->getType() == ASTNodeType::CHAR ||
                   child->getType() == ASTNodeType::BYTE) && !sizeSymbol) {
            sizeSymbol = child->getSymbol(); // First literal is size
        } else if (child->getType() == ASTNodeType::VECTOR_INITIALIZATION) {
            initNode = child; // Initialization values
        }
    }
    
    if (!vectorSymbol || !sizeSymbol) return nullptr;
    
    TAC* result = nullptr;
    
    // Begin vector initialization
    TAC* beginVecTac = tacCreate(TACType::BEGINVEC, nullptr, vectorSymbol, sizeSymbol);
    result = tacJoin(result, beginVecTac);
    
    // Get vector size
    std::string sizeStr = sizeSymbol->getLexeme();
    int vectorSize = std::stoi(sizeStr);
    
    // If there's an initialization node, extract values from it
    if (initNode) {
        std::vector<Symbol*> initValues;
        extractInitValues(initNode, initValues);
        
        // Initialize with provided values
        for (size_t i = 0; i < initValues.size() && (int)i < vectorSize; i++) {
            std::stringstream ss;
            ss << i;
            Symbol* indexSymbol = insertSymbol(ss.str(), 0, 1);
            
            TAC* vecwriteTac = tacCreate(TACType::VECWRITE, vectorSymbol, indexSymbol, initValues[i]);
            result = tacJoin(result, vecwriteTac);
        }
        
        // Fill remaining positions with zeros if vector is larger than init values
        Symbol* zeroSymbol = insertSymbol("0", 0, 1);
        for (size_t i = initValues.size(); (int)i < vectorSize; i++) {
            std::stringstream ss;
            ss << i;
            Symbol* indexSymbol = insertSymbol(ss.str(), 0, 1);
            
            TAC* vecwriteTac = tacCreate(TACType::VECWRITE, vectorSymbol, indexSymbol, zeroSymbol);
            result = tacJoin(result, vecwriteTac);
        }
    } else {
        // Initialize all positions with zero (no explicit initialization)
        Symbol* zeroSymbol = insertSymbol("0", 0, 1);
        for (int i = 0; i < vectorSize; i++) {
            std::stringstream ss;
            ss << i;
            Symbol* indexSymbol = insertSymbol(ss.str(), 0, 1);
            
            TAC* vecwriteTac = tacCreate(TACType::VECWRITE, vectorSymbol, indexSymbol, zeroSymbol);
            result = tacJoin(result, vecwriteTac);
        }
    }
    
    // End vector initialization
    TAC* endVecTac = tacCreate(TACType::ENDVEC, nullptr, vectorSymbol, nullptr);
    result = tacJoin(result, endVecTac);
    
    return result;
}

TAC* generateVectorInitialization(ASTNode* node) {
    // Vector initialization: vec[size] = {val1, val2, ...}
    TAC* result = nullptr;
    
    if (node->getChildren().size() > 1) {
        Symbol* vectorSymbol = node->getChildren()[0]->getSymbol();
        
        // Begin vector initialization
        TAC* beginVecTac = tacCreate(TACType::BEGINVEC, nullptr, vectorSymbol, nullptr);
        result = tacJoin(result, beginVecTac);
        
        // Generate VECWRITE for each initialization value
        for (size_t i = 1; i < node->getChildren().size(); i++) {
            TAC* valueCode = generateTAC(node->getChildren()[i]);
            Symbol* valueSymbol = getResultSymbol(valueCode, node->getChildren()[i]);
            
            // Create index symbol for position (i-1)
            std::stringstream ss;
            ss << (i - 1);
            Symbol* indexSymbol = insertSymbol(ss.str(), 0, 1);
            
            TAC* vecwriteTac = tacCreate(TACType::VECWRITE, vectorSymbol, indexSymbol, valueSymbol);
            result = tacJoin(result, valueCode);
            result = tacJoin(result, vecwriteTac);
        }
        
        // End vector initialization
        TAC* endVecTac = tacCreate(TACType::ENDVEC, nullptr, vectorSymbol, nullptr);
        result = tacJoin(result, endVecTac);
    }
    
    return result;
}

TAC* generateDoWhile(ASTNode* node) {
    if (node->getChildren().size() < 2) return nullptr;
    
    Symbol* beginLabel = makeLabel();
    Symbol* endLabel = makeLabel();
    
    TAC* beginLabelTac = tacCreate(TACType::LABEL, nullptr, beginLabel, nullptr);
    TAC* bodyCode = generateTAC(node->getChildren()[0]); // body comes first in do-while
    TAC* conditionCode = generateTAC(node->getChildren()[1]); // condition comes second
    
    Symbol* conditionSymbol = getResultSymbol(conditionCode, node->getChildren()[1]);
    
    // For do-while: jump back to beginning if condition is true (non-zero)
    // IFZ jumps if the condition is zero (false), so we jump to end when condition is false
    TAC* ifzTac = tacCreate(TACType::IFZ, nullptr, conditionSymbol, endLabel);
    TAC* jumpTac = tacCreate(TACType::JUMP, nullptr, beginLabel, nullptr);
    TAC* endLabelTac = tacCreate(TACType::LABEL, nullptr, endLabel, nullptr);
    
    TAC* result = tacJoin(beginLabelTac, bodyCode);
    result = tacJoin(result, conditionCode);
    result = tacJoin(result, ifzTac);
    result = tacJoin(result, jumpTac);
    result = tacJoin(result, endLabelTac);
    
    return result;
}

// Helper function to get operator symbol for printing
// Utility functions
std::string tacTypeToString(TACType type) {
    switch (type) {
        case TACType::SYMBOL: return "SYMBOL";
        case TACType::MOVE: return "MOVE";
        case TACType::INIT: return "INIT";
        case TACType::ADD: return "ADD";
        case TACType::SUB: return "SUB";
        case TACType::MUL: return "MUL";
        case TACType::DIV: return "DIV";
        case TACType::MOD: return "MOD";
        case TACType::AND: return "AND";
        case TACType::OR: return "OR";
        case TACType::NOT: return "NOT";
        case TACType::LT: return "LT";
        case TACType::GT: return "GT";
        case TACType::LE: return "LE";
        case TACType::GE: return "GE";
        case TACType::EQ: return "EQ";
        case TACType::NE: return "NE";
        case TACType::LABEL: return "LABEL";
        case TACType::BEGINFUN: return "BEGINFUN";
        case TACType::ENDFUN: return "ENDFUN";
        case TACType::IFZ: return "IFZ";
        case TACType::JUMP: return "JUMP";
        case TACType::CALL: return "CALL";
        case TACType::ARG: return "ARG";
        case TACType::RET: return "RET";
        case TACType::PRINT: return "PRINT";
        case TACType::READ: return "READ";
        case TACType::VECWRITE: return "VECWRITE";
        case TACType::VECREAD: return "VECREAD";
        case TACType::BEGINVEC: return "BEGINVEC";
        case TACType::ENDVEC: return "ENDVEC";
        default: return "UNKNOWN";
    }
}

void initTAC() {
    tempCounter = 0;
    labelCounter = 0;
}
