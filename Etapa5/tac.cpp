// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// Three Address Code (TAC) implementation file made by Nathan Alonso Guimarães (00334437)

#include "tac.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

// Forward declarations for internal functions
TAC* tacCreate(TACType type, Symbol* res = nullptr, Symbol* op1 = nullptr, Symbol* op2 = nullptr);
TAC* tacJoin(TAC* tac1, TAC* tac2);
TAC* tacReverse(TAC* tac);
void tacPrintBackwards(TAC* tac);
Symbol* makeTemp();
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
    
    // Find the last instruction in tac1 (assuming inverted list)
    TAC* last = tac1;
    while (last->getPrev()) {
        last = last->getPrev();
    }
    
    // Connect tac1 and tac2
    last->setPrev(tac2);
    if (tac2) {
        tac2->setNext(last);
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
    std::cout << "+-------------+-------------+-------------+-------------+" << std::endl;
    std::cout << "| " << std::left << std::setw(12) << "OPERATION";
    std::cout << "| " << std::setw(12) << "RESULT";
    std::cout << "| " << std::setw(12) << "OPERAND1";
    std::cout << "| " << std::setw(12) << "OPERAND2";
    std::cout << "|" << std::endl;
    std::cout << "+-------------+-------------+-------------+-------------+" << std::endl;
    
    // Find the beginning of the list (last instruction in inverted list)
    TAC* current = tac;
    while (current->getPrev()) {
        current = current->getPrev();
    }
    
    // Print from beginning to end
    while (current) {
        current->print();
        current = current->getNext();
    }
    
    // Print table footer
    std::cout << "+-------------+-------------+-------------+-------------+" << std::endl;
}

void tacPrintBackwards(TAC* tac) {
    TAC* current = tac;
    
    while (current) {
        current->print();
        current = current->getPrev();
    }
}

// Helper function to get the result symbol from a TAC sequence
Symbol* getResultSymbol(TAC* tacSequence, ASTNode* node) {
    if (!tacSequence && !node) return nullptr;
    
    // If the node is a direct symbol, return it
    if (node && node->getType() == ASTNodeType::SYMBOL) {
        return node->getSymbol();
    }
    
    // If the node is a literal (numbers, characters, etc), return its symbol
    if (node && (node->getType() == ASTNodeType::LITERAL || 
                 node->getType() == ASTNodeType::BYTE ||
                 node->getType() == ASTNodeType::INT ||
                 node->getType() == ASTNodeType::REAL ||
                 node->getType() == ASTNodeType::CHAR)) {
        return node->getSymbol();
    }
    
    // Otherwise, find the last TAC instruction with a result
    if (tacSequence) {
        TAC* current = tacSequence;
        while (current && !current->getRes()) {
            current = current->getPrev();
        }
        if (current) {
            return current->getRes();
        }
    }
    
    return nullptr;
}

// Symbol creation functions
Symbol* makeTemp() {
    std::stringstream ss;
    ss << "__temp" << tempCounter++;
    return insertSymbol(ss.str(), 0, 1); // Using line number 1 as default
}

Symbol* makeLabel() {
    std::stringstream ss;
    ss << "__label" << labelCounter++;
    return insertSymbol(ss.str(), 0, 1); // Using line number 1 as default
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
    
    Symbol* temp = makeTemp();
    Symbol* leftSymbol = nullptr;
    Symbol* rightSymbol = nullptr;
    
    // Extract symbols from child nodes or their generated code
    leftSymbol = getResultSymbol(leftCode, node->getChildren()[0]);
    rightSymbol = getResultSymbol(rightCode, node->getChildren()[1]);
    
    TACType tacType;
    switch (node->getType()) {
        case ASTNodeType::ADD: tacType = TACType::ADD; break;
        case ASTNodeType::SUBTRACT: tacType = TACType::SUB; break;
        case ASTNodeType::MULTIPLY: tacType = TACType::MUL; break;
        case ASTNodeType::DIVIDE: tacType = TACType::DIV; break;
        case ASTNodeType::MODULO: tacType = TACType::MOD; break;
        case ASTNodeType::AND: tacType = TACType::AND; break;
        case ASTNodeType::OR: tacType = TACType::OR; break;
        case ASTNodeType::LESS_THAN: tacType = TACType::LT; break;
        case ASTNodeType::GREATER_THAN: tacType = TACType::GT; break;
        case ASTNodeType::LESS_EQUAL: tacType = TACType::LE; break;
        case ASTNodeType::GREATER_EQUAL: tacType = TACType::GE; break;
        case ASTNodeType::EQUAL: tacType = TACType::EQ; break;
        case ASTNodeType::DIFFERENT: tacType = TACType::NE; break;
        default: tacType = TACType::MOVE; break;
    }
    
    TAC* opTac = tacCreate(tacType, temp, leftSymbol, rightSymbol);
    
    TAC* result = tacJoin(leftCode, rightCode);
    result = tacJoin(result, opTac);
    
    return result;
}

TAC* generateUnaryOp(ASTNode* node) {
    if (node->getChildren().empty()) return nullptr;
    
    TAC* operandCode = generateTAC(node->getChildren()[0]);
    Symbol* temp = makeTemp();
    Symbol* operandSymbol = getResultSymbol(operandCode, node->getChildren()[0]);
    
    TACType tacType;
    switch (node->getType()) {
        case ASTNodeType::NOT: tacType = TACType::NOT; break;
        default: tacType = TACType::MOVE; break;
    }
    
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
                // Process arguments - they might be in an EXPRESSION_LIST
                for (size_t j = 0; j < child->getChildren().size(); j++) {
                    ASTNode* argChild = child->getChildren()[j];
                    
                    if (argChild->getType() == ASTNodeType::EXPRESSION_LIST) {
                        // Process each expression in the list in reverse order for stack
                        for (size_t k = argChild->getChildren().size(); k > 0; k--) {
                            ASTNode* arg = argChild->getChildren()[k-1];
                            TAC* argCode = generateTAC(arg);
                            Symbol* argSymbol = getResultSymbol(argCode, arg);
                            
                            TAC* argTac = tacCreate(TACType::ARG, nullptr, argSymbol, nullptr);
                            result = tacJoin(result, argCode);
                            result = tacJoin(result, argTac);
                        }
                    } else {
                        // Direct argument
                        TAC* argCode = generateTAC(argChild);
                        Symbol* argSymbol = getResultSymbol(argCode, argChild);
                        
                        TAC* argTac = tacCreate(TACType::ARG, nullptr, argSymbol, nullptr);
                        result = tacJoin(result, argCode);
                        result = tacJoin(result, argTac);
                    }
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
    
    Symbol* temp = makeTemp();
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

TAC* generatePrint(ASTNode* node) {
    if (node->getChildren().empty()) return nullptr;
    
    TAC* valueCode = generateTAC(node->getChildren()[0]);
    Symbol* valueSymbol = getResultSymbol(valueCode, node->getChildren()[0]);
    
    TAC* printTac = tacCreate(TACType::PRINT, nullptr, valueSymbol, nullptr);
    
    return tacJoin(valueCode, printTac);
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
    
    Symbol* temp = makeTemp();
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
            
            TAC* assignTac = tacCreate(TACType::MOVE, varSymbol, valueSymbol, nullptr);
            return tacJoin(valueCode, assignTac);
        }
    }
    
    // Variable declaration without initialization - no TAC needed
    return nullptr;
}

TAC* generateVectorDeclaration(ASTNode* node) {
    if (!node || node->getChildren().empty()) return nullptr;
    
    // Get vector symbol and size
    Symbol* vectorSymbol = nullptr;
    Symbol* sizeSymbol = nullptr;
    
    // Find vector symbol and size in children
    for (size_t i = 0; i < node->getChildren().size(); i++) {
        ASTNode* child = node->getChildren()[i];
        if (child->getType() == ASTNodeType::SYMBOL && !vectorSymbol) {
            vectorSymbol = child->getSymbol();
        } else if (!sizeSymbol) {
            sizeSymbol = child->getSymbol();
        }
    }
    
    if (!vectorSymbol || !sizeSymbol) return nullptr;
    
    TAC* result = nullptr;
    
    // Begin vector initialization
    TAC* beginVecTac = tacCreate(TACType::BEGINVEC, nullptr, vectorSymbol, sizeSymbol);
    result = tacJoin(result, beginVecTac);
    
    // Initialize all positions with zero
    // We need to get the size from the symbol's lexeme and convert to integer
    std::string sizeStr = sizeSymbol->getLexeme();
    int vectorSize = std::stoi(sizeStr);
    
    // Create zero symbol
    Symbol* zeroSymbol = insertSymbol("0", 0, 1);
    
    // Initialize each position with zero
    for (int i = 0; i < vectorSize; i++) {
        std::stringstream ss;
        ss << i;
        Symbol* indexSymbol = insertSymbol(ss.str(), 0, 1);
        
        TAC* vecwriteTac = tacCreate(TACType::VECWRITE, vectorSymbol, indexSymbol, zeroSymbol);
        result = tacJoin(result, vecwriteTac);
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
    // We need to negate the condition for IFZ (jump if zero)
    Symbol* temp = makeTemp();
    TAC* notTac = tacCreate(TACType::NOT, temp, conditionSymbol, nullptr);
    TAC* ifzTac = tacCreate(TACType::IFZ, nullptr, temp, endLabel);
    TAC* jumpTac = tacCreate(TACType::JUMP, nullptr, beginLabel, nullptr);
    TAC* endLabelTac = tacCreate(TACType::LABEL, nullptr, endLabel, nullptr);
    
    TAC* result = tacJoin(beginLabelTac, bodyCode);
    result = tacJoin(result, conditionCode);
    result = tacJoin(result, notTac);
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
