/**
   Project: Implementation of a Queue in C++.
   Programmer: Karim Naqvi
   Course: enel452
*/

#include "queue.h"
#include <iostream>
#include <cstdlib>              // for exit

using namespace std;

Queue::Queue()
{
    head = 0;
    tail = 0;
    nelements = 0;
    verbose = false;
}

Queue::~Queue()
{
    for (QElement* qe = head; qe != 0;)
    {
	QElement* temp = qe;
	qe = qe->next;
	delete(temp);
    }
}

void Queue::remove(Data* d)
{
    if (size() > 0)
    {
        QElement* qe = head;
        head = head->next;
        nelements--;
        *d = qe->data;
	delete qe;
    }
    else
    {
        cerr << "Error: Queue is empty.\n";
        exit(1);
    }
}

void Queue::insert(Data d)
{
    if (verbose) std::cout << "insert(d)\n";
    QElement* el = new QElement(d);
    if (size() > 0)
    {
        tail->next = el;
    }
    else
    {
        head = el;
    }
    tail = el;
    nelements++;
}

// Assignment 2 Addition
void Queue::insert(Data d, unsigned position)
{
    if (verbose) std::cout << "insert at position(d)\n";
    QElement* el = new QElement(d);
    // Insert at head
    if (position == 0) {
        if (size() != 0) { el->next = head; }
        head = el;
    // Insert between head and tail
    } else if (position < size()) {
        QElement* currElement = head;
        unsigned currPosition = 0;
        for (unsigned i = 0; i < size(); i++) {
            if (currPosition == position - 1) {
                el->next = currElement->next;
                currElement->next = el;
            }
            currElement = currElement->next;
            currPosition += 1;
        }
    // Insert at tail
    } else if (position == size()) {
        tail->next = el;
        tail = el;
    }
    else
    {
        cerr << "insert: range error.\n";
        exit(3);
    }
    nelements++;
}

bool Queue::search(Data otherData) const
{
    QElement* insideEl = head;
    for (int i = 0; i < nelements; i++)
    {
        if (insideEl->data.equals(otherData))
            return true;
        insideEl = insideEl->next;
    }
    return false;
}

void Queue::print() const
{
    QElement* qe = head;
    if (size() > 0)
    {
        for (unsigned i = 0; i < size(); i++)
        {
            cout << i << ":(" << qe->data.x << "," << qe->data.y << ") ";
            qe = qe->next;
        }
    }
    cout << "\n";
}

unsigned Queue::size() const
{
    return nelements;
}
