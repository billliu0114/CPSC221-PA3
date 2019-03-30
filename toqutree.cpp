
/**
 *
 * toqutree (pa3)
 * significant modification of a quadtree .
 * toqutree.cpp
 * This file will be used for grading.
 *
 */

#include "toqutree.h"

toqutree::Node::Node(pair<int,int> ctr, int dim, HSLAPixel a)
	:center(ctr),dimension(dim),avg(a),NW(NULL),NE(NULL),SE(NULL),SW(NULL)
	{}

toqutree::~toqutree(){
	clear(root);
}

toqutree::toqutree(const toqutree & other) {
	root = copy(other.root);
}


toqutree & toqutree::operator=(const toqutree & rhs){
	if (this != &rhs) {
		clear(root);
		root = copy(rhs.root);
	}
	return *this;
}

toqutree::toqutree(PNG & imIn, int k){ 

	/* This constructor grabs the 2^k x 2^k sub-image centered */
	/* in imIn and uses it to build a quadtree. It may assume  */
	/* that imIn is large enough to contain an image of that size. */

	/* your code here */
	int size=2;
	for(int i=1; i<k;i++){
		size=size*2;
	}
	PNG sub_image(size,size);
	for(int i=0;i<size;i++){
		for(int j=0;j<size;j++){
			*(sub_image.getPixel(i,j))=*(imIn.getPixel(i,j));
		}
	}
	root=buildTree(&sub_image, size);
}


int toqutree::size() {
	/* your code here */
	return sizeHelper(root);
	
}




toqutree::Node * toqutree::buildTree(PNG * im, int k) {

	/* your code here */

	// Note that you will want to practice careful memory use
	// In this function. We pass the dynamically allocated image
	// via pointer so that it may be released after it is used .
	// similarly, at each level of the tree you will want to 
	// declare a dynamically allocated stats object, and free it
	// once you've used it to choose a split point, and calculate
	// an average.

	if(k==2){
		stats* stat= new stats(*im);
		HSLAPixel avg= stat->getAvg(pair<int,int>(0,0),pair<int,int>(1,1));
		Node* curr= new Node(pair<int,int>(1,1), 2, avg);
		free(stat);
		curr->SE= new Node(pair<int,int>(0,0),1,*(im->getPixel(1,1)));
		curr->SW= new Node(pair<int,int>(0,0),1,*(im->getPixel(0,1)));
		curr->NE= new Node(pair<int,int>(0,0),1,*(im->getPixel(1,0)));
		curr->NW= new Node(pair<int,int>(0,0),1,*(im->getPixel(0,0)));
		return curr;
	}

	
	pair<int,int> pivot=getPivotPoint(k);
	int searchBound=k/2;
	int L= searchBound;

	//cout<<"before init"<<endl;
	stats* stat= new stats(*im);
	//cout<<"After init"<<endl;

	pair<int,int> center=getCtr(pivot, searchBound, stat, k);
	int x=center.first;
	int y=center.second;
	//pair<int,int> SE2= pair<int,int>((x+L-1)%k,(y+L-1)%k);
	pair<int,int> SW1= pair<int,int>((x+L)%k,y);
	//pair<int,int> SW2= pair<int,int>((x-1)%k,(y+L-1)%k);
	pair<int,int> NE1= pair<int,int>(x,(y+L)%k);
	//pair<int,int> NE2= pair<int,int>((x+L-1)%k,(y-1)%k);
	pair<int,int> NW1= pair<int,int>((x+L)%k,(y+L)%k);
	//pair<int,int> NW2= pair<int,int>((x-1)%k,(y-1)%k);

	//cout<<"2"<<endl;

	HSLAPixel avg= stat->getAvg(pair<int,int>(0,0),pair<int,int>(k-1,k-1));

	//cout<<"3"<<endl;
	Node * curr= new Node(center, k, avg);


	//4 new pics
	PNG* PicSE= new PNG(k/2, k/2);
	PNG* PicSW= new PNG(k/2, k/2);
	PNG* PicNE= new PNG(k/2, k/2);
	PNG* PicNW= new PNG(k/2, k/2);
	dividePic(PicSE, im, center);
	dividePic(PicSW, im, SW1);
	dividePic(PicNE, im, NE1);
	dividePic(PicNW, im, NW1);
	//recursion

	curr->SE=buildTree(PicSE, k/2);
	curr->SW=buildTree(PicSW, k/2);
	curr->NE=buildTree(PicNE, k/2);
	curr->NW=buildTree(PicNW, k/2);

	free(PicSE);
	free(PicSW);
	free(PicNE);
	free(PicNW);
	free(stat);


	return curr;

}

void toqutree::dividePic(PNG* PicPart, PNG* original, pair<int,int>one){
	int k=original->width();
	for(unsigned int i=0;i<PicPart->width();i++){
		for(unsigned int j=0;j<PicPart->height();j++){			
			*(PicPart->getPixel(i,j))=*(original->getPixel((one.first+i)%k,(one.second+j)%k));
		}
	}
}

pair<int,int>toqutree::getCtr(pair<int,int> pivot, int searchBound, stats* stat, int k){
	double entropy=getAvgEntropy(pivot.first,pivot.second,k,stat);

	for(int i=pivot.first; i<(pivot.first+searchBound);i++){
		for(int j=pivot.second;j<(pivot.second+searchBound);j++){
			double temp= getAvgEntropy(i,j,k,stat);
			entropy= (entropy>temp)?temp:entropy;
		}
	}
	//now we have the min Entropy
	for(int i=pivot.first; i<(pivot.first+searchBound);i++){
		for(int j=pivot.second;j<(pivot.second+searchBound);j++){
			double temp= getAvgEntropy(i,j,k,stat);
			if(entropy==temp){
				return pair<int,int>(i,j);
			}
		}
	}
	throw exception();

}

double toqutree::getAvgEntropy(int x, int y, int k, stats* stat){
	int L= k/2;
	pair<int,int> SE1= pair<int,int>(x,y);
	pair<int,int> SE2= pair<int,int>((x+L-1)%k,(y+L-1)%k);
	double EntropySE= stat->entropy(SE1,SE2);

	pair<int,int> SW1= pair<int,int>((x+L)%k,y);
	pair<int,int> SW2= pair<int,int>((x-1)%k,(y+L-1)%k);
	double EntropySW= stat->entropy(SW1,SW2);

	pair<int,int> NE1= pair<int,int>(x,(y+L)%k);
	pair<int,int> NE2= pair<int,int>((x+L-1)%k,(y-1)%k);
	double EntropyNE= stat->entropy(NE1,NE2);

	pair<int,int> NW1= pair<int,int>((x+L)%k,(y+L)%k);
	pair<int,int> NW2= pair<int,int>((x-1)%k,(y-1)%k);
	double EntropyNW= stat->entropy(NW1,NW2);

	return (EntropySE+EntropySW+EntropyNE+EntropyNW)/4.0;
}

pair<int,int> toqutree::getPivotPoint(int k){
	return pair<int,int>(k/4,k/4);
}

int toqutree::sizeHelper(Node * ro){
	if (ro==NULL){
		return 0;
	}
	else
	{
		return sizeHelper(ro->NE)+sizeHelper(ro->NW)+sizeHelper(ro->SE)+sizeHelper(ro->SW)+1;
	}
	
}


PNG toqutree::render(){

	// My algorithm for this problem included a helper function
	// that was analogous to Find in a BST, but it navigated the 
	// quadtree, instead.

	/* your code here */
	// int dim= root->dimension;
	// PNG result(dim,dim);

	// for(int i=0; i<dim; i++){
	// 	for(int j=0; j<dim; j++){
	// 		*(result.getPixel(i,j))=findPixel(root,i,j);
	// 	}
	// }

	return realRender(root);
}

HSLAPixel toqutree::findPixel(Node * node, int x, int y){
	int px= node->center.first;
	int py= node->center.second;
	int K= node->dimension;
	int L= K/2;
	// cout<<K<<endl;
	// cout<<"x: "<<x;
	// cout<<" y: "<<y<<endl;
	// cout<<"center "<<px<<" "<<py<<endl;
	if(K==2){
		//return SE's color
		if(x==1 && y==1){
			return node->SE->avg;
		}
		//return NE's color
		else if(x==1 && y==0){
			return node->NE->avg;
		}
		//return SW's color
		else if(x==0 && y==1){
			return node->SW->avg;
		}
		//return NW's color
		else{
			return node->NW->avg;
		}
	}

	//search in SE and NE
	for(int i=0; i<L;i++){
		//traverse X
		if((px+i)%K==x){
			//in SE?
			for(int j=0;j<L;j++){
				//traverse Y
				if((py+j)%K==y){
					//in SE
					return findPixel(node->SE,i,j);
				}
			}
			//in NE then...
			for(int j=0;j<L;j++){
				//traverse Y
				if(((py+L)%K+j)%K==y){
					//in NE
					return findPixel(node->NE,i,j);
				}
			}

		}
	}
	//not in SE or NE, search in SW and NW
	for(int i=0; i<L;i++){
		//traverse X
		if(((px+L)%K+i)%K==x){
			//in SW?
			for(int j=0;j<L;j++){
				//traverse Y
				if((py+j)%K==y){
					//in SW
					return findPixel(node->SW,i,j);
				}
			}
			//in NW then...
			for(int j=0;j<L;j++){
				//traverse Y
				if(((py+L)%K+j)%K==y){
					//in NW
					return findPixel(node->NW,i,j);
				}
			}

		}
	}

	throw "Nothing found!";
	
}

PNG toqutree::realRender(Node* node){

	if(node->SE==NULL||node->SW==NULL||node->NE==NULL||node->NW==NULL){
		int K= node->dimension;
		PNG Pic(K,K);
		for(int i=0;i<K;i++){
			for(int j=0;j<K;j++){
				*(Pic.getPixel(i,j))=node->avg;
			}
		}
		return Pic;
	}
	
	PNG PicSE=realRender(node->SE);
	PNG PicSW=realRender(node->SW);
	PNG PicNE=realRender(node->NE);
	PNG PicNW=realRender(node->NW);

	int cx= node->center.first;
	int cy= node->center.second;
	int K= node->dimension;
	int L= K/2;

	PNG result(K,K);

	for(int i=0; i<L; i++){
		for(int j=0;j<L;j++){
			*(result.getPixel((cx+i)%K,(cy+j)%K))=*(PicSE.getPixel(i,j));
			*(result.getPixel((cx+i)%K,((cy+L)%K+j)%K))=*(PicNE.getPixel(i,j));
			*(result.getPixel(((cx+L)%K+i)%K,(cy+j)%K))=*(PicSW.getPixel(i,j));
			*(result.getPixel(((cx+L)%K+i)%K,((cy+L)%K+j)%K))=*(PicNW.getPixel(i,j));
		}
	}
	return result;

}

/* oops, i left the implementation of this one in the file! */
void toqutree::prune(double tol){

	//prune(root,tol);

}

/* called by destructor and assignment operator*/
void toqutree::clear(Node * & curr){
/* your code here */

}

/* done */
/* called by assignment operator and copy constructor */
toqutree::Node * toqutree::copy(const Node * other) {

/* your code here */

}


