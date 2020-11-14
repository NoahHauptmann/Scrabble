#include <iostream>
#include <map>
#include "bst.h"
#include "avlbst.h"

using namespace std;

int main(int argc, char *argv[])
{
    // // Binary Search Tree tests
    // BinarySearchTree<char,int> bt;
    // bt.insert(std::make_pair('d',10));
    // bt.insert(std::make_pair('b',5));
    // bt.insert(std::make_pair('a',20));
    // bt.insert(std::make_pair('c',15));
    // bt.print();
    // cout << "Binary Search Tree contents:" << endl;
    // for(BinarySearchTree<char,int>::iterator it = bt.begin(); it != bt.end(); ++it) {
    //     cout << it->first << " " << it->second << endl;
    // }
    // if(bt.find('b') != bt.end()) {
    //     cout << "Found b" << endl;
    // }
    // else {
    //     cout << "Did not find b" << endl;
    // }
    // cout << "Erasing b" << endl;
    // bt.remove('b');
    // if(bt.find('b') != bt.end()) {
    //     cout << "Found b" << endl;
    // }
    // else {
    //     cout << "Did not find b" << endl;
    // }

    // //AVL Tree Tests
    // AVLTree<char,int> at;
    // at.insert(std::make_pair('a',1));
    // at.insert(std::make_pair('b',2));
    // at.insert(std::make_pair('c',3));
    // at.insert(std::make_pair('d',1));
    // at.insert(std::make_pair('e',10));

    // cout << endl;

    // cout << "AVLTree contents:" << endl;
    // for(AVLTree<char,int>::iterator it = at.begin(); it != at.end(); ++it) {
    //     cout << it->first << " " << it->second << endl;
    // }
    // if(at.find('b') != at.end()) {
    //     cout << "Found b" << endl;
    // }
    // else {
    //     cout << "Did not find b" << endl;
    // }
    // cout << "Erasing a" << endl;
    // at.remove('a');
    

    // return 0;
    const RandomSeed masterSeed = 1778;
	const size_t numElements = 30;
	const size_t numTrials = 50;

	AVLTree<int, int> testTree;

	// generate 3x as many seeds as trials because we need 3 for each trial
	std::vector<RandomSeed> seeds = makeRandomSeedVector(numTrials * 3, masterSeed);

	for(size_t counter = 0; counter < numTrials*3; counter+= 3)
	{

		// create tree of random data
		std::set<int> randomData = makeRandomIntSet(numElements, seeds.at(counter));
		fillTree(testTree, randomData, seeds.at(counter + 1));
		
		ASSERT_TRUE(verifyAVL(testTree, randomData));
        testTree.print()
		// now remove elements at random
		std::vector<int> randomDataVector(randomData.begin(), randomData.end());
		std::default_random_engine shuffleRandEngine;
		shuffleRandEngine.seed(seeds.at(counter + 2));
		std::shuffle(randomDataVector.begin(), randomDataVector.end(), shuffleRandEngine);

		for(size_t elementIndex = 0; elementIndex < randomDataVector.size(); ++elementIndex)
		{
			testTree.remove(randomDataVector[elementIndex]);
			randomData.erase(randomData.find(randomDataVector[elementIndex]));

			ASSERT_TRUE(verifyAVL(testTree, randomData));

		}
	}
    return 0;

}
