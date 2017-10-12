#include "model.h"

Model::Model(){
    for(Symbol i = 0; i < 256; i++)
        context_minus_1.insert(i);
    k = 2;
}

void Model::updateModel(const Context& context, const Symbol& symbol){

    Context aux_ctx = context;
    Tree* node;
    
    while( true ){
        node = &tree;
        
        node = node->findPath(aux_ctx);

        if(!node){
            node = node->addPath(aux_ctx);
            node->addPath(ESC);
            node->addPath(symbol);
        }
        else{
            if( !node->findPath(symbol) ) node->addPath(ESC);
            node->addPath(symbol);
        }

        if( node->child_count() == 257 ) node->erasePath(ESC);

        if(aux_ctx.empty()) break;
        aux_ctx.pop_front();
    }

    if( context_minus_1.count(symbol) )
        context_minus_1.erase(symbol);

}

ProbabilitiesSet Model::getSymbolProbability(const Context& context, const Symbol& symbol){

    Context aux_ctx = context;
    Tree* node, *aux_node;
    ProbabilitiesSet out;
    uint low, high, den;
    std::unordered_set<Symbol> exc_set;
    
    while( true ){
        node = &tree;
        
        node = node->findPath(aux_ctx);

        if(!node){
            if( aux_ctx.empty() ) break;
            aux_ctx.pop_front();
            continue;
        }
        
        if( aux_node = node->findPath(symbol) ){
            low = node->getOcurrencesFromPreviousSimblings(symbol);
            high = low + aux_node->ocurrences();
            den = node->contexts();
            out.push_back( {low, high, den } );
            break;
        }
        
        aux_node = node->findPath(ESC);
        if(!aux_node){
            if(aux_ctx.empty()) break;
            aux_ctx.pop_front();
            continue;
        }

        low = node->getOcurrencesFromPreviousSimblings(ESC);
        high = low + aux_node->ocurrences();
        den = node->contexts();

        for( auto element : exc_set )
            if(aux_node = node->findPath(element)){
                den -= aux_node->ocurrences();
                low -= aux_node->ocurrences();
                high -= aux_node->ocurrences();
            }

        node->getChildrenSet(symbol, exc_set);

        out.push_back( {low, high, den } );
    
        if(aux_ctx.empty()) break;
        aux_ctx.pop_front();
    }

    if( context_minus_1.count(symbol) ){ // context -1
        low = 0;
        for( auto k = context_minus_1.begin(); k != context_minus_1.end(); k++ ){
            if(*k == symbol) break;
            low++;
        }
        high = low + 1;
        den = context_minus_1.size();
        out.push_back( {low, high, den } );
    }

    return out;

}

ProbabilitiesSet Model::getSymbolProbability(int x, const Symbol& symbol){

    ProbabilitiesSet out;
    uint low, high, den;

    if( context_minus_1.count(symbol) ){ // context -1
        low = 0;
        for( auto k = context_minus_1.begin(); k != context_minus_1.end(); k++ ){
            if(*k == symbol) break;
            low++;
        }
        high = low + 1;
        den = context_minus_1.size();
        out.push_back( {low, high, den } );
    }

    return out;

}

Symbol Model::getSymbol(const Context& context, uint count){

    Tree* node;
    
    node = &tree;
    node = node->findPath(context);

    if(node) return node->getSymbolOnCount(count);
    return ESC;

}

Symbol Model::getSymbol(int x, uint count){

    uint aux = 0;
    
    for( auto k = context_minus_1.begin(); k != context_minus_1.end(); k++){
        aux++;
        if( aux > count) return *k;
    }

    return *context_minus_1.rbegin();
}

uchar Model::getK() const{
    return k;
}

uint Model::getCount(const Context& context){

    Tree* node = &tree;

    node = node->findPath(context);

    if(node) return node->contexts();
    else return 0;

}

uint Model::getCount(int x){
    
    return context_minus_1.size();

}

void Model::clearModel(){
    context_minus_1.clear();

    for(Symbol i = 0; i < 256; i++)
        context_minus_1.insert(i);
    k = 2;

    tree.clear();
}