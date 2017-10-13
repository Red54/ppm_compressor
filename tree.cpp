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
    
    contexts_count_++;

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

void Tree::erasePath(const Symbol& symbol){
	
	contexts_count_ -= children[symbol]->num_ocurrences_;
	delete children[symbol];
	children.erase(symbol);
		
}

Tree* Tree::addChild(const Symbol& symbol){

    return (children[symbol] = new Tree);
    
}

Tree* Tree::findChild(const Symbol& symbol){

    return children.count(symbol) ? children[symbol] : nullptr;
    
}

uint Tree::getOcurrencesFromPreviousSimblings(const Symbol& symbol){
	
	uint count = 0;
	auto k = children.begin();
	auto limit = children.lower_bound(symbol);

	for( ; k != limit; k++)
	  	count += k->second->num_ocurrences_;

	return count;
}

uint Tree::ocurrences() const{
	return num_ocurrences_;
}

uint Tree::contexts() const{
	return contexts_count_;
}

uint Tree::child_count() const{
	return children.size();
}

Symbol Tree::getSymbolOnCount(uint count, const std::unordered_set<Symbol> exc_mec) const{

	uint aux = 0;

	for( auto k = children.begin(); k != children.end(); k++){
		if( !exc_mec.count(k->first) ) aux += k->second->ocurrences();
		if( aux > count) return k->first;
	}

	if( !children.empty() ) return children.rbegin()->first;
	return ESC;

}

void Tree::clear(){

	num_ocurrences_ = 0;
	contexts_count_ = 0;

	for( auto element : children ){
		element.second->clear();
		delete element.second;
	}
	children.clear();
}

void Tree::getChildrenSet(std::unordered_set<Symbol>& exc_set){
	for(auto it = children.begin(); it != children.end(); it++){
		if(it->first != ESC) exc_set.insert(it->first);
	}
  }