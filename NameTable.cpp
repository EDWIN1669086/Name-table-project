#include "NameTable.h"
#include <string>
#include <list>
#include <functional>
#include <stack>
using namespace std;



struct Node
{
    Node(const string val, int line, int scopeNum):ID(val), lineNumber(line),scope(scopeNum){};
    const string ID;
    int lineNumber;
    int scope;
};

// This class does the real work of the implementation.

class NameTableImpl
{
public:
    NameTableImpl();
    ~NameTableImpl();
    void enterScope();
    bool exitScope();
    bool declare(const string& id, int line);
    int find(const string& id) const;
    int hashFunction(string ID) const
    {
        hash<string> hashObj;
        unsigned int h = hashObj(ID) % (HASH_TABLE_SIZE);
        return h;
    }
private:
    static const int HASH_TABLE_SIZE = 20000;
    list<Node> m_array[HASH_TABLE_SIZE];
    stack<int> hashNumbers;
    int scopeNumber = 0;
};

NameTableImpl::NameTableImpl()
{}

NameTableImpl::~NameTableImpl()
{}

void NameTableImpl::enterScope()
{
    scopeNumber++;
    hashNumbers.push(-1); //-1 is the identifying break betweeen scopes
}


bool NameTableImpl::exitScope()
{
    if(scopeNumber == 0)
        return false;
    
    int num = hashNumbers.top();
    while(num != -1)
    {
        if(!m_array[num].empty()) //if the list is not empty...
        {
            m_array[num].erase(m_array[num].begin());
        }
        hashNumbers.pop();
        num = hashNumbers.top();
    }
    hashNumbers.pop();
    scopeNumber--;
    return true;
}

bool NameTableImpl::declare(const string& id, int line)
{
    if(id == "")
        return false;
    
    int h = hashFunction(id);
    for (list<Node>::iterator itr = m_array[h].begin(); itr != m_array[h].end(); itr++)
    {
        if ((*itr).ID == id && (*itr).scope == scopeNumber)
        {
            return false;
        }
    }

    Node p(id, line, scopeNumber);
    m_array[h].push_front(p);
    hashNumbers.push(hashFunction(id));
    return true;
}

int NameTableImpl::find(const string& id) const
{
    if(id.empty())
        return -1;
    
    int h = hashFunction(id);
    
    if (m_array[h].empty())
    {
        return -1;
    }
    
    for (list<Node>::const_iterator itr = (m_array[h]).cbegin(); itr != (m_array[h]).cend(); itr++)
    {
        if ((*itr).ID == id)
        {
            return (*itr).lineNumber;
        }
    }
    return -1;
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

bool NameTable::declare(const string& id, int lineNumber)
{
    return m_impl->declare(id, lineNumber);
}

int NameTable::find(const string& id) const
{
    return m_impl->find(id);
}
