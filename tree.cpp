#include "tree.h"

bool CompareSymbol::operator()(const Symbol& s1, const Symbol& s2) const{
	return s1 < s2;
}

Tree* Tree::addPath(const Context& context){
	
	Tree* child_node = this;
	Tree* parent_node;

	for(uint k = 0; k < context.size(); k++){
        parent_node = child_node;
		child_node = parent_node->findChild(context[k]);
		if(!child_node) child_node = parent_node->addChild(context[k]);
	}

	return child_node;

}

Tree* Tree::addPath(const Symbol& symbol){
    
    contexts_count++;

	Tree* node = this->findChild(symbol);
	if(!node) node = this->addChild(symbol);
    node->num_ocurrences_++;
    
	return node;

}

Tree* Tree::addPath(const Context& context, const Symbol& symbol){
	
    return addPath(context)->addPath(symbol);

}

Tree* Tree::findPath(const Context& context){

	Tree* node = this;

	for(uint k = 0; k < context.size(); k++){
		node = node->findChild(context[k]);
		if(!node) return nullptr;
	}

	return node;

}

Tree* Tree::findPath(const Symbol& symbol){

	return this->findChild(symbol);

}

Tree* Tree::findPath(const Context& context, const Symbol& symbol){
	
    Tree* node = findPath(context);
    
    return node ? node->findPath(symbol) : node;

}

Tree* Tree::addChild(const Symbol& symbol){

    return children[symbol] = new Tree;
    
}

Tree* Tree::findChild(const Symbol& symbol){

    return children.count(symbol) ? children[symbol] : nullptr;
    
}