#include <fstream>
#include <iostream> 
#include <string>
#include <stdlib.h>
#include <unordered_map>
using namespace std;

class Entry {
public:
	string keyword;		// unique keyword of entry
	int count;			// number of accesses of keyword
	int degree;			// number of children of entry
	Entry* parent;		// pointer to parent in heap
	Entry* left;		// pointer to left sibling if any
	Entry* right;		// pointer to right sibling if any
	Entry* child;		// pointer to first child of entry
	bool childCut;		// true if entry has lost a child since becoming the child of its parent

	Entry(string k, int c);	// constructor for Entry type
	~Entry();			// destructor 
};

Entry::Entry(string k, int c) {
	// create and initialize an Entry object 
	keyword = k;
	count = c;
	degree = 0;			// 0 children initially
	parent = nullptr;	// no initial parent
	left = nullptr;			// no initial siblings; set pointer to itself
	right = nullptr;		// no initial siblings; set pointer to itself
	child = nullptr;	// no initial children
	childCut = false;	// undefined without parent or children
}

Entry::~Entry(void) {

}

Entry meld(Entry *root1, Entry *root2) {
	// Merge two fibonacci heaps into one and return the new root

	// Check if roots have siblings; if not, assign left and right pointers to themself
	if (!root1->left){
		cout << "r1 no sibs.\n";
		root1->left = root1;
		root1->right = root1;
	}
	if (!root2->left){
		cout << "r2 no sibs.\n";
		root2->left = root2;
		root2->right = root2;
	}
	cout << "Meld: r1.L: " << root1->left->keyword << " r1.R: " << root1->right->keyword << " r2.L: " << root2->left->keyword << " r2.R: " << root2->right->keyword << "\n";

	// Split top level circular doubly linked lists and combine 
	// Example for root1 and root2 with 1 sibling each: 
	/*	1       |		2  |
	*	,-L0R--L1R-,	,-L2R--L0R-,  -->	,-L0R--L1R--L2R--L0R-,
	*	'----------'	'----------'		'--------------------'
	*/
	root1->right->left = root2->left;
	root2->left->right = root1->right;
	root1->right = root2;
	root2->left = root1;
	cout << "Meld2: r1.L: " << root1->left->keyword << " r1.R: " << root1->right->keyword << " r2.L: " << root2->left->keyword << " r2.R: " << root2->right->keyword << "\n";

	// determine new max and return as the root; tie goes to root1
	cout << "r1.c: " << root1->count << " r2.c: " << root2->count << "\n";
	if (root2->count > root1->count) {
		return *root2;
	}
	else return *root1;
}

Entry* insertEntry(Entry *root, Entry *ent, string hashTag) {
	// Insert Entry ent into fibonacci heap rooted at root 

	cout << "Insert. root: " << root->keyword << " entry: " << ent->keyword << "\n";
	if(root->right == nullptr){
		root->left = ent;
		ent->right = root;
	} else {
		root->right->left = ent;
		ent->right = root->right;
	}
	root->right = ent;
	ent->left = root;

	cout << "insert. r.L: " << root->left->keyword << " r.R: " << root->right->keyword << " e.L: " << ent->left->keyword << " e.R: " << ent->right->keyword << "\n";

	if (ent->count > root->count) {
		return ent;
	}
	else return root;
	//return meld(root, ent);
}

Entry *heap; 	// pointer for root of max fib heap

int main () {
	// open input file
	ifstream infile;
	infile.open("test1.txt");

	// initialiaze hash map
	unordered_map<string, Entry*> umap;

	Entry heap1("initial_root",0);	// fake entry for intial root
	heap = &heap1; 
	string line;
	int num = 0;	// number of keywords
	while (getline(infile, line)) {
		if (line[0] == '$') {
			line = line.substr(1, string::npos);	// discard first character
			string keyword = line.substr(0, line.find(' '));	// extract keyword before space
			int amount = stoi(line.substr(line.find(' '), string::npos),nullptr,10); 	// extract number after space 
			cout << "Keyword: '" << keyword << "' was accessed " << amount << " times.\n";
			num++; 

			// check if keyword has already been mapped
			if (umap.find(keyword) == umap.end()) {
				// keyword not found in hash map; add new entry to heap
				Entry new_entry(keyword, amount);	// create Entry for given input 
				heap = insertEntry(heap, &new_entry, keyword);	// add entry to heap

				umap[keyword] = &new_entry;	// map keyword to new entry in hash map
				Entry *temp;
				temp = umap.at(keyword);	// check to make sure mapped item is accurate
				cout << "kw from entry: " << new_entry.keyword << " kw from map: " << temp->keyword << endl;
			} else {
				// keyword exists in hash map; increase count of accesses

			}

			//cout << "Root: " << heap.keyword << ", " << heap.count << " left: " << heap.left->keyword << heap.left->count << " right: " << heap.right->keyword << heap.right->count << "\n";
			cout << "Root: " << heap->keyword << ", " << heap->count << " left: " << heap->left->keyword << heap->left->count << " right: " << heap->right->keyword << heap->right->count << "\n";
		}
		else if ((line[0]>'0') && (line[0]<='9')) {
			int topn = stoi(line,nullptr,10);	// extract number of top keywords to report 
			//cout << "Return top " << topn << " keywords.\n";
		}
		else if (line == "stop") {
			//cout << "STOP command.\n";
			break;	// terminate loop
		}
		//cout << "num: " << num << endl;
	}

	Entry *temp;
	temp = heap;
	for (int i=0; i<num; i++) {
		cout << i << ":" << temp->keyword << temp->count << " ";
		temp = temp->right;
	}
	cout << endl; 

	return 0;
}