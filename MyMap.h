#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED

#include <iostream>
#include <queue>

template <class KeyType, class ValueType>
class MyMap
{
public:
    MyMap()
		: m_root(NULL), m_size(0)
	{}

    ~MyMap()
    {
		freeTree(m_root);
		m_root = NULL;
    }

    void clear()
    {
		// clear tree and set all private member variables back to their default values
		freeTree(m_root);
		m_root = NULL;
		m_size = 0;
		while (!levelOrder.empty())
			levelOrder.pop();
    }

    int size() const
    {
        return m_size;
    }

    void associate(const KeyType& key, const ValueType& value)
    {
		ValueType* temp = find(key);
		if (temp != NULL)	// found
		{
			// only adjust value if key is already in the map
			*temp = value;
			return;
		}
		if (m_root == NULL)
		{
			// create a new node, remake the queue to fit in the new node, and increase m_size;
			// set all children to NULL because this was an empty tree
			Node* create = new Node;
			create->key = key;
			create->value = value;
			create->left = NULL;
			create->right = NULL;
			m_root = create;
			traverse();
			m_size++;
			return;
		}
		// create a new node, remake the queue to fit in the new node, and increase m_size;
		// children will either be connected through left node or right node depending on its value
		Node* insert = m_root;
		for (;;)
		{
			if (key < insert->key)
			{
				if (insert->left != NULL)
					insert = insert->left;
				else
				{
					Node* newNode = new Node;
					newNode->key = key;
					newNode->value = value;
					newNode->left = NULL;
					newNode->right = NULL;
					insert->left = newNode;
					traverse();
					m_size++;
					return;
				}
			}
			else
			{
				if (insert->right != NULL)
					insert = insert->right;
				else
				{
					Node* newNode = new Node;
					newNode->key = key;
					newNode->value = value;
					newNode->left = NULL;
					newNode->right = NULL;
					insert->right = newNode;
					traverse();
					m_size++;
					return;
				}
			}
		}
    }

    const ValueType* find(const KeyType& key) const
    {
		Node* ptr = m_root;
		while (ptr != NULL)
		{
			if (ptr->key == key)
				return &(ptr->value);
			if (key < ptr->key)
			{
				ptr = ptr->left;
				continue;
			}
			if (key > ptr->key)
			{
				ptr = ptr->right;
				continue;
			}
		}
        return NULL;  
    }
        
    ValueType* find(const KeyType& key)
    {
          // Do not change the implementation of this overload of find
        const MyMap<KeyType,ValueType>* constThis = this;
        return const_cast<ValueType*>(constThis->find(key));
    }

    ValueType* getFirst(KeyType& key)
    {
		// traverse in level order
		// just need to pop from queue because queue always traverse in level order
		if (levelOrder.empty())
			return NULL;
		key = m_root->key;
		levelOrder.pop();
		return &(m_root->value);
    }

    ValueType* getNext(KeyType& key)
    {
		// traverse in level order
		// just need to pop from queue because queue always traverse in level order
		if (levelOrder.empty())
			return NULL;
		Node* temp = levelOrder.front();
		levelOrder.pop();
		key = temp->key;
		return &(temp->value);
    }

private:
    MyMap(const MyMap &other);
    MyMap &operator=(const MyMap &other);

    // Add needed members
	struct Node
	{
		KeyType key;
		ValueType value;
		Node* left;
		Node* right;
	};
	int m_size;
	Node* m_root;
	std::queue<Node*> levelOrder;
	void traverse()
	{
		// reorganize the queue everytime a new node is inserted;
		// this way, the map will always be returned in level order
		while (!levelOrder.empty())
			levelOrder.pop();
		Node* temp;
		std::queue<Node*> temporary;
		levelOrder.push(m_root);
		temporary.push(m_root);
		while (!temporary.empty())
		{
			temp = temporary.front();
			temporary.pop();
			if (temp->left != NULL)
			{
				levelOrder.push(temp->left);
				temporary.push(temp->left);
			}
			if (temp->right != NULL)
			{
				levelOrder.push(temp->right);
				temporary.push(temp->right);
			}
		}
	}
	void freeTree(Node* cur)
	{
		if (cur == NULL)
			return;
		freeTree(cur->left);
		freeTree(cur->right);
		delete cur;
	}
};

#endif // MYMAP_INCLUDED