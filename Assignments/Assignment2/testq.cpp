/**
   Project: Implementation of a Queue in C++.
   Programmer: Karim Naqvi
   Course: enel452
   Description: test code
*/

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <vector>

#include "queue.h"

/**
   Compare the given queue to the given array of data elements.  Return
   true if they are all equal.
 */
bool testQueueEquality( Queue & q, std::vector<Data> const & array)
{
    if (q.size() != array.size())
	return false;

    for (unsigned i = 0; i < q.size(); ++i)
    {
        Data d(0,0);
	q.remove(&d);
	if ( !d.equals(array[i]) )
	    return false;
	q.insert(d);
    }
    return true;
}


int main()
{
    using namespace std;

    // Assignment 2 example test from assignment document, tests inserting elements between the head and tail.
    cout << "Testing queue.\n";
    Queue q;
    q.insert(Data(1, 2));
    q.insert(Data(3, 4));
    q.insert(Data(5, 6));
    q.insert(Data(-2, -3), 1);
    q.insert(Data(-4, -5), 3);
    q.print();

    vector<Data> qDataVec;
    qDataVec.push_back(Data(1, 2));
    qDataVec.push_back(Data(-2, -3));
    qDataVec.push_back(Data(3, 4));
    qDataVec.push_back(Data(-4, -5));
    qDataVec.push_back(Data(5, 6));
    assert(testQueueEquality(q, qDataVec) == true);

    // Inserting at tail position
    cout << "Inserting (25, 25) at tail position.\n";
    qDataVec.push_back(Data(25, 25));
    q.insert(Data(25, 25), q.size());
    q.print();
    assert(testQueueEquality(q, qDataVec) == true);

    // Inserting at head position
    cout << "Inserting (50, 50) at head position.\n";
    qDataVec.insert(qDataVec.begin(), Data(50, 50));
    q.insert(Data(50, 50), 0);
    q.print();
    assert(testQueueEquality(q, qDataVec) == true);

    // Inserting at invalid position
    cout << "Inserting (400, 400) at invalid position.\n";
    q.insert(Data(400, 400), q.size() + 1);
}
