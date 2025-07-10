// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1
// TAC optimizer implementation file made by Nathan Alonso Guimarães (00334437)

#include "optimizer.hpp"
#include "tac.hpp"
#include "symbol.hpp"
#include "parser.tab.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// Helper function to check if a symbol represents a numeric literal
bool isNumericLiteral(Symbol* symbol) {
    if (!symbol) return false;
    
    // Check if it's a literal with numeric token type
    // Accept both LITERAL and SCALAR types as they both represent constant values
    return (symbol->getType() == LIT_INT || symbol->getType() == LIT_REAL) &&
           (symbol->getIdentifierType() == identifierType::LITERAL ||
            symbol->getIdentifierType() == identifierType::SCALAR);
}

// Helper function to get numeric value from a symbol
double getNumericValue(Symbol* symbol) {
    if (!symbol) return 0.0;
    
    std::string lexeme = symbol->getLexeme();
    
    if (symbol->getType() == LIT_INT) {
        return std::stod(lexeme);
    } else if (symbol->getType() == LIT_REAL) {
        // Handle real numbers in format "a/b"
        size_t slashPos = lexeme.find('/');
        if (slashPos != std::string::npos) {
            double numerator = std::stod(lexeme.substr(0, slashPos));
            double denominator = std::stod(lexeme.substr(slashPos + 1));
            if (denominator != 0.0) {
                return numerator / denominator;
            }
        }
        return std::stod(lexeme);
    }
    
    return 0.0;
}

// Helper function to create a new symbol with the computed value
Symbol* createConstantSymbol(double value, dataType type) {
    std::stringstream ss;
    
    if (type == dataType::INT) {
        // Create integer literal
        ss << static_cast<int>(value);
        Symbol* newSymbol = insertSymbol(ss.str(), LIT_INT);
        newSymbol->setDataType(dataType::INT);
        newSymbol->setIdentifierType(identifierType::LITERAL);
        return newSymbol;
    } else if (type == dataType::REAL) {
        // Create real literal - for simplicity, convert to decimal format
        ss << value;
        Symbol* newSymbol = insertSymbol(ss.str(), LIT_REAL);
        newSymbol->setDataType(dataType::REAL);
        newSymbol->setIdentifierType(identifierType::LITERAL);
        return newSymbol;
    }
    
    return nullptr;
}

// Helper function to determine the result data type for binary operations
dataType getResultDataType(Symbol* op1, Symbol* op2, TACType opType) {
    if (!op1 || !op2) return dataType::INT;
    
    // For comparison operations, result is always boolean/int
    switch (opType) {
        case TACType::LT:
        case TACType::GT:
        case TACType::LE:
        case TACType::GE:
        case TACType::EQ:
        case TACType::NE:
        case TACType::AND:
        case TACType::OR:
            return dataType::INT;
        default:
            break;
    }
    
    // For arithmetic operations, if either operand is real, result is real
    if (op1->getDataType() == dataType::REAL || op2->getDataType() == dataType::REAL ||
        op1->getType() == LIT_REAL || op2->getType() == LIT_REAL) {
        return dataType::REAL;
    }
    
    return dataType::INT;
}

// Helper function to perform constant folding computation
double computeConstantExpression(double val1, double val2, TACType opType) {
    switch (opType) {
        case TACType::ADD:
            return val1 + val2;
        case TACType::SUB:
            return val1 - val2;
        case TACType::MUL:
            return val1 * val2;
        case TACType::DIV:
            if (val2 != 0.0) return val1 / val2;
            return 0.0; // Avoid division by zero
        case TACType::MOD:
            if (val2 != 0.0) return static_cast<int>(val1) % static_cast<int>(val2);
            return 0.0;
        case TACType::LT:
            return (val1 < val2) ? 1.0 : 0.0;
        case TACType::GT:
            return (val1 > val2) ? 1.0 : 0.0;
        case TACType::LE:
            return (val1 <= val2) ? 1.0 : 0.0;
        case TACType::GE:
            return (val1 >= val2) ? 1.0 : 0.0;
        case TACType::EQ:
            return (val1 == val2) ? 1.0 : 0.0;
        case TACType::NE:
            return (val1 != val2) ? 1.0 : 0.0;
        case TACType::AND:
            return (val1 != 0.0 && val2 != 0.0) ? 1.0 : 0.0;
        case TACType::OR:
            return (val1 != 0.0 || val2 != 0.0) ? 1.0 : 0.0;
        default:
            return 0.0;
    }
}

// Helper function to perform unary constant folding
double computeUnaryConstantExpression(double val, TACType opType) {
    switch (opType) {
        case TACType::NOT:
            return (val == 0.0) ? 1.0 : 0.0;
        default:
            return val;
    }
}

// Helper function to find the constant value assigned to a temporary variable
Symbol* findConstantValue(TAC* tacHead, Symbol* tempVar) {
    if (!tempVar || !tacHead) return nullptr;
    
    // Look for a MOVE instruction that assigns a constant to this temporary
    TAC* current = tacHead;
    while (current) {
        if (current->getType() == TACType::MOVE && 
            current->getRes() == tempVar && 
            isNumericLiteral(current->getOp1())) {
            return current->getOp1();
        }
        current = current->getNext();
    }
    
    return nullptr;
}

// Helper function to check if a symbol is used anywhere in the TAC after a given point
bool isSymbolUsed(TAC* startPoint, Symbol* symbol) {
    if (!startPoint || !symbol) return false;
    
    TAC* current = startPoint;
    while (current) {
        // Check if symbol is used as operand in any instruction
        if (current->getOp1() == symbol || current->getOp2() == symbol) {
            return true;
        }
        current = current->getNext();
    }
    
    return false;
}

// Helper function to substitute all uses of a symbol with another symbol from a given point
void substituteSymbolUses(TAC* startPoint, Symbol* oldSymbol, Symbol* newSymbol) {
    if (!startPoint || !oldSymbol || !newSymbol) return;
    
    TAC* current = startPoint;
    while (current) {
        // Replace operand uses
        if (current->getOp1() == oldSymbol) {
            current->setOp1(newSymbol);
        }
        if (current->getOp2() == oldSymbol) {
            current->setOp2(newSymbol);
        }
        current = current->getNext();
    }
}

// Main optimization function for constant folding
TAC* optimizeConstantFolding(TAC* tacHead) {
    if (!tacHead) return nullptr;
    
    int totalOptimizations = 0;
    int pass = 1;
    bool hasOptimizations;
    
    // Apply optimization passes until no more optimizations are found
    do {
        hasOptimizations = false;
        TAC* current = tacHead;
        int passOptimizations = 0;
        
        while (current) {
            bool optimized = false;
            
            // First, check for dead code elimination: MOVE instructions to unused temporaries
            if (current->getType() == TACType::MOVE && 
                current->getRes() && 
                isNumericLiteral(current->getOp1())) {
                
                // Check if this temporary is used
                
                // Check if this temporary is used anywhere after this point
                if (!isSymbolUsed(current->getNext(), current->getRes())) {
                    // This is dead code - remove it
                    if (current->getPrev()) {
                        current->getPrev()->setNext(current->getNext());
                    } else {
                        tacHead = current->getNext(); // Update head if this was the first instruction
                    }
                    
                    if (current->getNext()) {
                        current->getNext()->setPrev(current->getPrev());
                    }
                    
                    TAC* toDelete = current;
                    current = current->getNext();
                    delete toDelete;
                    
                    optimized = true;
                    passOptimizations++;
                    hasOptimizations = true;
                    continue; // Skip the rest of the loop since we moved to next instruction
                }
            }
            
            // Second, check for constant propagation: replace uses of temporaries defined by MOVE with constants
            if (current->getType() == TACType::MOVE && 
                current->getRes() && 
                isNumericLiteral(current->getOp1())) {
                
                // This temporary is defined by a constant - propagate its use
                substituteSymbolUses(current->getNext(), current->getRes(), current->getOp1());
                
                // Now check if this MOVE is dead code after propagation
                if (!isSymbolUsed(current->getNext(), current->getRes())) {
                    // This is now dead code - remove it
                    if (current->getPrev()) {
                        current->getPrev()->setNext(current->getNext());
                    } else {
                        tacHead = current->getNext(); // Update head if this was the first instruction
                    }
                    
                    if (current->getNext()) {
                        current->getNext()->setPrev(current->getPrev());
                    }
                    
                    TAC* toDelete = current;
                    current = current->getNext();
                    delete toDelete;
                    
                    optimized = true;
                    passOptimizations++;
                    hasOptimizations = true;
                    continue; // Skip the rest of the loop since we moved to next instruction
                }
            }
            
            // Third, check for binary operations with constant operands (including constant propagation)
            switch (current->getType()) {
                case TACType::ADD:
                case TACType::SUB:
                case TACType::MUL:
                case TACType::DIV:
                case TACType::MOD:
                case TACType::LT:
                case TACType::GT:
                case TACType::LE:
                case TACType::GE:
                case TACType::EQ:
                case TACType::NE:
                case TACType::AND:
                case TACType::OR: {
                    Symbol* op1 = current->getOp1();
                    Symbol* op2 = current->getOp2();
                    
                    // Try to propagate constants for operands that are temporaries
                    Symbol* effectiveOp1 = op1;
                    Symbol* effectiveOp2 = op2;
                    
                    // Check if op1 is a temporary with a constant value
                    if (op1 && !isNumericLiteral(op1)) {
                        Symbol* constVal = findConstantValue(tacHead, op1);
                        if (constVal) {
                            effectiveOp1 = constVal;
                        }
                    }
                    
                    // Check if op2 is a temporary with a constant value
                    if (op2 && !isNumericLiteral(op2)) {
                        Symbol* constVal = findConstantValue(tacHead, op2);
                        if (constVal) {
                            effectiveOp2 = constVal;
                        }
                    }
                    
                    if (isNumericLiteral(effectiveOp1) && isNumericLiteral(effectiveOp2)) {
                        // Both operands are numeric literals (after propagation) - perform constant folding
                        double val1 = getNumericValue(effectiveOp1);
                        double val2 = getNumericValue(effectiveOp2);
                        double result = computeConstantExpression(val1, val2, current->getType());
                        
                        // Determine result data type
                        dataType resultType = getResultDataType(effectiveOp1, effectiveOp2, current->getType());
                        
                        // Create new constant symbol
                        Symbol* newConstant = createConstantSymbol(result, resultType);
                        
                        if (newConstant) {
                            // Replace the binary operation with a simple move
                            TAC* newTac = new TAC(TACType::MOVE, current->getRes(), newConstant);
                            
                            // Update links
                            newTac->setPrev(current->getPrev());
                            newTac->setNext(current->getNext());
                            
                            if (current->getPrev()) {
                                current->getPrev()->setNext(newTac);
                            } else {
                                tacHead = newTac; // Update head if this was the first instruction
                            }
                            
                            if (current->getNext()) {
                                current->getNext()->setPrev(newTac);
                            }
                            
                            // Clean up old instruction
                            TAC* toDelete = current;
                            current = newTac;
                            delete toDelete;
                            
                            optimized = true;
                            passOptimizations++;
                            hasOptimizations = true;
                        }
                    }
                    break;
                }
                
                case TACType::NOT: {
                    Symbol* op1 = current->getOp1();
                    
                    // Try to propagate constant for operand that is a temporary
                    Symbol* effectiveOp1 = op1;
                    
                    // Check if op1 is a temporary with a constant value
                    if (op1 && !isNumericLiteral(op1)) {
                        Symbol* constVal = findConstantValue(tacHead, op1);
                        if (constVal) {
                            effectiveOp1 = constVal;
                        }
                    }
                    
                    if (isNumericLiteral(effectiveOp1)) {
                        // Unary operation with constant operand (after propagation)
                        double val1 = getNumericValue(effectiveOp1);
                        double result = computeUnaryConstantExpression(val1, current->getType());
                        
                        // Create new constant symbol (NOT always returns int/boolean)
                        Symbol* newConstant = createConstantSymbol(result, dataType::INT);
                        
                        if (newConstant) {
                            // Replace the unary operation with a simple move
                            TAC* newTac = new TAC(TACType::MOVE, current->getRes(), newConstant);
                            
                            // Update links
                            newTac->setPrev(current->getPrev());
                            newTac->setNext(current->getNext());
                            
                            if (current->getPrev()) {
                                current->getPrev()->setNext(newTac);
                            } else {
                                tacHead = newTac; // Update head if this was the first instruction
                            }
                            
                            if (current->getNext()) {
                                current->getNext()->setPrev(newTac);
                            }
                            
                            // Clean up old instruction
                            TAC* toDelete = current;
                            current = newTac;
                            delete toDelete;
                            
                            optimized = true;
                            passOptimizations++;
                            hasOptimizations = true;
                        }
                    }
                    break;
                }
                
                default:
                    break;
            }
            
            if (!optimized) {
                current = current->getNext();
            }
        }
        
        totalOptimizations += passOptimizations;
        pass++;
        
        // Safety check to prevent infinite loops
        if (pass > 1000) {
            std::cout << "Warning: Maximum optimization passes reached (1000). Stopping to prevent infinite loop." << std::endl;
            break;
        }
        
    } while (hasOptimizations);
    
    std::cout << "Constant folding optimization completed: " << totalOptimizations 
              << " expressions optimized in " << (pass - 1) << " passes." << std::endl;
    
    return tacHead;
}

// Main optimization function
TAC* optimizeTAC(TAC* tacHead, const std::string& outputFileName) {
    if (!tacHead) {
        return nullptr;
    }  
    
    // Apply constant folding optimization
    TAC* optimizedTac = optimizeConstantFolding(tacHead);
    
    // Save optimized TAC to file
    if (!outputFileName.empty()) {
        std::ofstream outFile(outputFileName);
        if (outFile.is_open()) {
            outFile << "// Optimized TAC - Constant Folding Applied" << std::endl;
            outFile << "// Federal University of Rio Grande do Sul - Institute of Informatics - Compilers 2025/1" << std::endl;
            outFile << "// TAC optimizer made by Nathan Alonso Guimarães (00334437)" << std::endl;
            outFile << std::endl;
            outFile.close();
            
            // Use the existing TAC printing function to append to file
            printTACToFile(outputFileName, optimizedTac);
        } else {
            std::cerr << "Warning: Could not create optimization output file: " << outputFileName << std::endl;
        }
    }
    
    return optimizedTac;
}
