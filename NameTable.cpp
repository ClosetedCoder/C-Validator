// NameTable.cpp

#include "NameTable.h"
#include <list>
#include <functional>
#include <string>
#include <vector>
#include <stack>

using namespace std;

// This class does the real work of the implementation.
class Hash_Table
{
public:
    Hash_Table(int num_buckets)
    {
        m_buckets = num_buckets;
        buckets = new list <Node>[num_buckets]; //dynamically allocating array
    }
    int find(string id, int bucketNum);
    void remove(int bucketNum); // remove first element at bucketNum
    bool insert(int bucketNum, int line_num, int scope_num, string id); //go to bucket, create new node, and insert. Check if Id has already appeared in the scope, if so, return false. (2 of the same ids cannot be in the same scope)
    ~Hash_Table()
    {
        delete[] buckets;
    }
    struct Node
    {
        Node(string a, int b, int c)
        {
            name = a;
            line = b;
            scope = c;
        };
        string name;
        int line;
        int scope;
    };

private:
    int m_buckets;
    list <Node>* buckets; //dynamically allocated array
};

int Hash_Table :: find(string id, int bucketNum)
{
    list<Node> ::iterator p = buckets[bucketNum].begin(); 
    while ((p != buckets[bucketNum].end()) && (*p).name != id)
    {
        p++;
    }
    //check if we have reached end of list 
    if (p == buckets[bucketNum].end())
        return -1; 
    //return position 
    return (*p).line; 
}

void Hash_Table::remove(int bucketNum)
{
   // unsigned int bucketNum = m_hash_function(id) % m_buckets; 
    list<Node> ::iterator p = buckets[bucketNum].begin();
    buckets[bucketNum].erase(p);
}

bool Hash_Table::insert(int bucketNum, int line_num, int scope_num, string id)
{

    //check for another declaration in the same scope and return false if there is another 
    for (list<Node> :: iterator p = buckets[bucketNum].begin(); p != buckets[bucketNum].end(); p++) 
    {
        if ((*p).scope == scope_num)
        {
            if ((*p).name == id)
                return false;
        }
        else
            break; 
    }

    //proceed with insertion into hash map 
    buckets[bucketNum].push_front(Node(id, line_num, scope_num)); //keeps recent scope elements at the start of buckets 
    return true; 

}

class NameTableImpl
{
public:
    NameTableImpl();
    void enterScope();
    bool exitScope();
    bool declare(const string& id, int lineNum);
    int find(const string& id) const;
private:
    Hash_Table* m_hash;     
    int m_scope; 
    stack<int> m_ids; 
    hash<string> m_hash_function; //string s; m_hash_function(s) % m_buckets to obtain hash key
    int m_buckets; 
};

NameTableImpl::NameTableImpl() : m_scope(0), m_buckets(20000)
{
    m_hash = new Hash_Table(20000);
}

void NameTableImpl::enterScope()
{
    m_ids.push(-1);
    m_scope++;
}

bool NameTableImpl::exitScope()
{
    // Remove ids back to the last scope entry.

    while (!m_ids.empty() && m_ids.top() != -1)
    {
        m_hash->remove(m_ids.top());
        m_ids.pop();
    }
    if (m_ids.empty())
        return false;

    // Remove the scope entry marker itself.
    m_ids.pop();
    m_scope--;
    return true;
}

bool NameTableImpl::declare(const string& id, int lineNum)
{
    //if (id.empty())
    //   return false;

    //insert fails then return false
    unsigned int bucketNum = m_hash_function(id) % m_buckets;
    if(!(m_hash->insert(bucketNum, lineNum, m_scope, id)))
        return false; 
    
    //insert success
    m_ids.push(bucketNum);
    return true; 
}

int NameTableImpl::find(const string& id) const
{
    //if (id.empty())
    //    return -1;

    // Search back for the most recent declaration still
    // available.
    unsigned int bucketNum = m_hash_function(id) % m_buckets;
    return m_hash->find(id, bucketNum); 
}

//*********** NameTable functions **************

// For the most part, these functions simply delegate to NameTableImpl's
// functions.

NameTable::NameTable()
{
    m_impl = new NameTableImpl;
}

NameTable::~NameTable()
{
    delete m_impl;
}

void NameTable::enterScope()
{
    m_impl->enterScope();
}

bool NameTable::exitScope()
{
    return m_impl->exitScope();
}

bool NameTable::declare(const string& id, int lineNum)
{
    return m_impl->declare(id, lineNum);
}

int NameTable::find(const string& id) const
{
    return m_impl->find(id);
}
