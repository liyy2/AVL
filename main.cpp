#include <bits/stdc++.h>
#include <algorithm>

using namespace std;

int abs(int x) {
    return x > 0 ? x : -x;
}

struct treeNode{
    int label;
    int height;
    size_t size;
    treeNode(int lab):label(lab),height(0),size(1), leftChild(nullptr), rightChild(nullptr),parent(nullptr){};
    treeNode* leftChild;
    treeNode* rightChild;
    treeNode* parent;
    bool myParentRightChild;
    int retreive() const{
        return label;
    }

    int updateHeight() {
        if((this->leftChild == nullptr) && (this->rightChild== nullptr)){
            this -> height = 0;
            return 0;
        }
        else if (this -> leftChild == nullptr ){
            this-> height = 1 + this->rightChild->height;
            return this->rightChild->height+1;
        } else if (this ->rightChild == nullptr){
            this -> height = 1 + this ->leftChild ->height;
            return this ->leftChild ->height+1;
        }
        else{
            this -> height = 1 + std::max(this->leftChild->height, this->rightChild->height);
            return abs(this->leftChild->height - this->rightChild->height);
        }
    }
    void updateSize() {
        if((this->leftChild == nullptr) && (this->rightChild== nullptr)){
            this -> size = 1;
        }
        else if (this -> leftChild == nullptr ){
            this-> size = 1 + this->rightChild->size;
        } else if (this ->rightChild == nullptr){
            this-> size = 1 + this->leftChild->size;
        }
        else{
            this -> size = 1 + this->leftChild->size + this->rightChild->size;
        }
    }

    bool is_leaf() const{
        return (leftChild== nullptr) && (rightChild == nullptr);
    }

};


treeNode* getfront(treeNode* root){
    treeNode* curr = root;
    while (curr->leftChild!= nullptr){
        curr = curr->leftChild;
    }
    return curr;
}

struct AVL {
    AVL();
    treeNode* setinelNode;
    treeNode* getRoot();
    bool is_empty();
    void insert(int v);
    void remove(int v);
    treeNode* find(int val, treeNode* startingPoint);
    int kth(int k);
    int rank(int v);
    long long size();
};
std::pair<treeNode*, treeNode*> insertHelper(treeNode* curr, int v);
void rotateLeft(treeNode * curr);
void rotateRight(treeNode * curr);
int patternRecognizer(treeNode * tree);
treeNode* removeHelper(treeNode* toBeDeletedNode);
void remainBalance(treeNode* curr);
void remainBalanceToRoot(treeNode* curr);
int kth_helper(treeNode* curr, int k);
int rank_helper(treeNode* curr, int v);

void AVL::insert(int v) {
    if(this -> getRoot() == nullptr){
        auto insertedNode = new treeNode(v);
        this->setinelNode->rightChild = insertedNode;
        insertedNode -> parent = this->setinelNode;
        insertedNode -> myParentRightChild = true;
        return;
    }
    auto TreeViolatorPair = insertHelper(this->getRoot(), v);
    auto violator =  TreeViolatorPair.second;
    if (violator == nullptr){
        return;
    }
    else{
        remainBalance(violator);
    }
}




// return left height - right height
int diffHeight(treeNode* tree){
    if((tree->leftChild == nullptr) && (tree->rightChild== nullptr)){
        return 0;
    }
    else if (tree -> leftChild == nullptr ){
        return -(tree->rightChild->height+1);
    } else if (tree ->rightChild == nullptr){
        return tree ->leftChild ->height+1;
    }
    else{
        return tree->leftChild->height - tree->rightChild->height;
    }
}



void remainBalance(treeNode* curr){
    switch (patternRecognizer(curr)) {
        case 0:{
            rotateRight(curr);
            break;
        }
        case 1:{
            rotateLeft(curr->leftChild);
            rotateRight(curr);
            break;
        }
        case 2:{
            rotateRight(curr->rightChild);
            rotateLeft(curr);
            break;
        }
        case 3:{
            rotateLeft(curr);
            break;
        }

    }
}

int patternRecognizer(treeNode* tree){
    if(diffHeight(tree)>0 ){
        auto subtreeWithLargerHeight = tree ->leftChild;
        if (diffHeight(subtreeWithLargerHeight) >= 0){
            // left-left case
            return 0;
        }
        else{
            // left - right case
            return 1;
        }
    }
    else{
        auto subtreeWithLargerHeight = tree ->rightChild;
        if (diffHeight(subtreeWithLargerHeight) > 0){
            // right-left case
            return 2;
        }
        else{
            // right - right case
            return 3;
        }
    }
}


std::pair<treeNode*, treeNode*> insertHelper(treeNode* curr, int v) {
    if (curr == nullptr){
        auto toBeInsertedNode = new treeNode(v);
        // the first is return the left/right sub tree for link adjustments
        // the second is to return the first node that violates the AVL property
        return std::pair<treeNode*, treeNode*>{toBeInsertedNode, nullptr};
    }
    std::pair<treeNode*, treeNode*> childrenPair;
    if (v >= curr->retreive()){
        childrenPair = insertHelper(curr->rightChild, v);
        curr-> rightChild = childrenPair.first;
        childrenPair.first->myParentRightChild = true;
    }
    else if(v < curr->retreive()){
        childrenPair = insertHelper(curr->leftChild, v);
        curr-> leftChild = childrenPair.first;
        childrenPair.first->myParentRightChild = false;
    }
    // updating the children's parental tree
    childrenPair.first->parent = curr;
    // updating the currNode's height and size
    curr->updateSize();
    // if it is the first node that has violation return it to the root
    if (childrenPair.second == nullptr){
        // all of the predecessors will remain height invariants
        // thus we don't need to update them when we find the first violator of AVL property
        int diffHeights = curr->updateHeight();
        if (diffHeights >= 2){
            childrenPair.second = curr;
        }
    }
    return std::pair<treeNode*, treeNode*>{curr, childrenPair.second};
}



bool AVL::is_empty()  {
    return this->getRoot() == nullptr;
}

AVL::AVL() {
    // make sentinel node's value small enough so that root is always sentinel's right child
    this->setinelNode = new treeNode(INT32_MIN);
}

treeNode *AVL::getRoot() {
    return this->setinelNode->rightChild;
}

void AVL::remove(int v) {
    treeNode* toBeDeletedNode = find(v, this->getRoot());
    if(toBeDeletedNode == nullptr){
        return;
    }
    auto candidate = removeHelper(toBeDeletedNode);
    remainBalanceToRoot(candidate);
}


void remainBalanceToRoot(treeNode* curr) {
    // first we need to update the height and size
    curr->updateSize();
    int diffHeights = curr->updateHeight();
    treeNode *parent = curr->parent;
    if (parent == nullptr) {
        return; // remember that we use sentinel nodes
    }
    if (diffHeights >= 2) {
        remainBalance(curr);
    }
    remainBalanceToRoot(parent);
}



treeNode* removeHelper(treeNode* toBeDeletedNode){
    // return the first node that might violates the AVL property
        treeNode* firstCandidateViolator = toBeDeletedNode->parent;
        treeNode* childrenLink = nullptr;
        if (toBeDeletedNode->is_leaf()) {
            childrenLink = nullptr;
        }
        else if(toBeDeletedNode -> rightChild == nullptr){
            childrenLink = toBeDeletedNode->leftChild;
        }
        else if (toBeDeletedNode -> leftChild == nullptr){
            childrenLink = toBeDeletedNode->rightChild;
        }
        else{
            auto rightFront = getfront(toBeDeletedNode->rightChild);
            toBeDeletedNode->label = rightFront->label;
            return removeHelper(rightFront);
        }
        // to check the relation of toBeDeletedNode and its parent
        if (!toBeDeletedNode->myParentRightChild){
            firstCandidateViolator->leftChild = childrenLink;
        }
        else{
            firstCandidateViolator->rightChild = childrenLink;
        }
        if (childrenLink != nullptr){
            childrenLink -> parent = firstCandidateViolator;
            childrenLink ->myParentRightChild = toBeDeletedNode->myParentRightChild;
        }
    delete toBeDeletedNode;
    return firstCandidateViolator;
}


treeNode *AVL::find(int val, treeNode *startingPoint) {
    if (startingPoint== nullptr){
        return startingPoint;
    }
    if ( val == startingPoint->retreive()){
        return startingPoint;
    }
    else if (val < startingPoint->retreive()){
        return find(val, startingPoint->leftChild);
    }
    else{
        return find(val, startingPoint->rightChild);
    }
}

int AVL::kth(int k) {
    return kth_helper(this->getRoot(),k);
}

int AVL::rank(int v) {
    return rank_helper(this->getRoot(),v);
}

long long AVL::size() {
    return this->getRoot()->size;
}

int kth_helper(treeNode* curr, int k){
    int leftRank;
    if (!curr->leftChild){
        leftRank = 0;
    }
    else{
        leftRank = curr->leftChild->size;
    }
    if (leftRank == k){
        return curr->retreive();
    }
    else if (leftRank<k){
        return kth_helper(curr->rightChild, k - leftRank - 1);
    }
    else{
        return kth_helper(curr->leftChild, k );
    }
}

int rank_helper(treeNode* curr, int v){
    if (curr == nullptr){
        return 0;
    }
    if ( v <= curr->retreive()){
        return rank_helper(curr->leftChild, v);
    }
    else if (v> curr->retreive()){
        int leftsize;
        if(curr->leftChild == nullptr){
             leftsize = 0;
        } else{
            leftsize = curr->leftChild->size;
        }
        return 1 + leftsize + rank_helper(curr->rightChild,v) ;
    }
}




// update the height and return the difference of the left tree and right tree


void rotateLeft(treeNode * curr) {
    auto rightTree = curr->rightChild;
    auto parentalTree = curr->parent;
    auto rightLeftTree = rightTree->leftChild;
    if (rightTree == nullptr){
        throw "Error, can't do rotateLeft when right tree is empty";
    }
    // consider edge case where my parent is empty
        // I am my parent's left subtree
    if (!curr->myParentRightChild){
        parentalTree -> leftChild = rightTree;
    }
    else{
        parentalTree -> rightChild = rightTree;
    }
    rightTree->parent = parentalTree;
    rightTree->myParentRightChild = curr->myParentRightChild;
    rightTree->leftChild = curr;
    curr -> rightChild = rightLeftTree;
    curr->myParentRightChild = false;
    curr -> parent = rightTree;
    if (rightLeftTree){
        rightLeftTree->parent = curr;
        rightLeftTree->myParentRightChild = true;
    }
    // updating size and height
    curr->updateHeight();
    curr->updateSize();
    rightTree->updateHeight();
    rightTree->updateSize();
}

void rotateRight(treeNode * curr) {
    auto leftTree = curr->leftChild;
    auto parentalTree = curr->parent;
    auto leftRightTree = leftTree->rightChild;
    if (leftTree == nullptr){
        throw "Error, can't do rotateRight when left tree is empty";
    }
    // consider edge case where my parent is empty
    if (!curr->myParentRightChild){
        parentalTree -> leftChild = leftTree;
    }
    else{
        parentalTree -> rightChild = leftTree;
    }
    leftTree->parent = parentalTree;
    leftTree->myParentRightChild = curr->myParentRightChild;
    leftTree->rightChild = curr;
    curr -> leftChild = leftRightTree;
    curr -> myParentRightChild = true;
    curr -> parent = leftTree;
    if (leftRightTree){
        leftRightTree -> parent = curr;
        leftRightTree -> myParentRightChild = false;
    }
    curr->updateHeight();
    curr->updateSize();
    leftTree->updateHeight();
    leftTree->updateSize();
}

int A, B, C, lfsr;
double P[4][4];
int lfsr_generator() {
    auto ret = lfsr;
    return (lfsr ^= lfsr << 13, lfsr ^= lfsr >> 17, lfsr ^= lfsr << 5, ret);
}
tuple<int, int> command() {
    auto imm = lfsr_generator();
    static int state = 0;
    auto p = double(lfsr_generator() & 0x7fffffff) / INT32_MAX;
    for (int i = 0; i < 4; i++)
        if ((p -= P[state % 4][i]) < 0) {
            state += 4 - state % 4 + i;
            break;
        }
    return {state % 4, (imm * A + state * B + C) & 0x7fffffff};
}
/* PLEASE DO NOT CHANGE ABOVE*/
int main() {
    AVL tree;
    int m;
    freopen("example","r", stdin);
    cin >> m >> lfsr >> A >> B >> C;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) cin >> P[i][j];
    int tans = 0;
    for(int i=1;i<=m;i++) {
        int op, imm;
        tie(op, imm) = command();
        if (op == 0) tree.insert(imm);
        if (op == 1) tree.remove(tree.kth(imm % tree.size()));
        if (op == 2) tans ^= tree.rank(imm);
        if (op == 3) tans ^= tree.kth(imm % tree.size());
    }
    cout << tans << "\n";

}
