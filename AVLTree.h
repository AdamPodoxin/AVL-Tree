#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

using std::max;
using std::vector;

template <class K, class V>
class AVLTreeNode
{
public:
	AVLTreeNode<K, V> *parent, *left, *right;

	K key;
	V value;

	unsigned height;

	AVLTreeNode(K key, V value);
	~AVLTreeNode();
};

template <class K, class V>
AVLTreeNode<K, V>::AVLTreeNode(K k, V v)
{
	parent = nullptr;
	left = nullptr;
	right = nullptr;

	key = k;
	value = v;

	height = 0;
}

template <class K, class V>
AVLTreeNode<K, V>::~AVLTreeNode()
{
	parent = nullptr;
	left = nullptr;
	right = nullptr;

	height = 0;
}

template <class K, class V>
class AVLTree
{
private:
	AVLTreeNode<K, V> *root;

	unsigned m_size;

	AVLTreeNode<K, V> *get_node_with_key(const K key) const;

	AVLTreeNode<K, V> *insert_internal(AVLTreeNode<K, V> *root_node, AVLTreeNode<K, V> *node_to_insert);

	void remove_internal(AVLTreeNode<K, V> *root_node);

	void values_internal(const AVLTreeNode<K, V> *root_node, vector<V> &vec) const;
	void keys_internal(const AVLTreeNode<K, V> *root_node, vector<K> &vec) const;

	int node_height(const AVLTreeNode<K, V> *node) const;
	void delete_all_nodes(AVLTreeNode<K, V> *node);
	AVLTreeNode<K, V> *deep_copy_node(const AVLTreeNode<K, V> *root_node) const;
	AVLTreeNode<K, V> *get_successor_node(AVLTreeNode<K, V> *node) const;
	bool is_balanced(const AVLTreeNode<K, V> *node) const;

	void calculate_all_heights(AVLTreeNode<K, V> *root);

	void left_rotate(AVLTreeNode<K, V> *A);
	void right_rotate(AVLTreeNode<K, V> *A);
	void rebalance_on_insert(AVLTreeNode<K, V> *node);
	void rebalance_on_delete(AVLTreeNode<K, V> *node);

public:
	AVLTree();

	AVLTree(const AVLTree<K, V> &rhs);

	AVLTree<K, V> &operator=(const AVLTree<K, V> &rhs);

	~AVLTree();

	void *getRoot() const { return root; }; // DO NOT REMOVE

	bool insert(const K key, const V value);

	bool remove(const K key);

	V search(const K key);

	vector<V> values() const;
	vector<K> keys() const;

	unsigned size() const;
};

/**************** PRIVATE METHODS ****************/

template <class K, class V>
AVLTreeNode<K, V> *AVLTree<K, V>::get_node_with_key(const K key) const
{
	AVLTreeNode<K, V> *curr = root;
	while (curr)
	{
		K currKey = curr->key;

		if (key == currKey)
		{
			return curr;
		}
		else if (key < currKey)
		{
			curr = curr->left;
		}
		else
		{
			curr = curr->right;
		}
	}

	return curr;
}

template <class K, class V>
AVLTreeNode<K, V> *AVLTree<K, V>::insert_internal(AVLTreeNode<K, V> *root_node, AVLTreeNode<K, V> *node_to_insert)
{
	if (!root_node)
	{
		return node_to_insert;
	}

	K key = node_to_insert->key;
	if (key <= root_node->key)
	{
		root_node->left = insert_internal(root_node->left, node_to_insert);
		root_node->left->parent = root_node;
	}
	else
	{
		root_node->right = insert_internal(root_node->right, node_to_insert);
		root_node->right->parent = root_node;
	}

	return root_node;
}

template <class K, class V>
void AVLTree<K, V>::remove_internal(AVLTreeNode<K, V> *node)
{
	// Case 3
	if (node->left && node->right)
	{
		AVLTreeNode<K, V> *successor_node = get_successor_node(node);
		node->key = successor_node->key;
		node->value = successor_node->value;

		remove_internal(successor_node);
	}
	// Case 2
	else if (node->left || node->right)
	{
		AVLTreeNode<K, V> *child_node = (node->left ? node->left : node->right);
		AVLTreeNode<K, V> *parent_node = node->parent;

		if (parent_node)
		{
			if (node == parent_node->left)
			{
				parent_node->left = child_node;
			}
			else if (node == parent_node->right)
			{
				parent_node->right = child_node;
			}

			node->parent = nullptr;
		}

		child_node->parent = parent_node;

		if (node == root)
		{
			root = child_node;
		}

		node->parent = nullptr;
		node->left = nullptr;
		node->right = nullptr;
		delete node;
		node = nullptr;
	}
	// Case 1
	else
	{
		AVLTreeNode<K, V> *parent_node = node->parent;

		if (parent_node)
		{
			if (node == parent_node->left)
			{
				parent_node->left = nullptr;
			}
			else if (node == parent_node->right)
			{
				parent_node->right = nullptr;
			}

			node->parent = nullptr;
		}

		delete node;

		if (node == root)
		{
			root = nullptr;
		}
	}
}

template <class K, class V>
void AVLTree<K, V>::values_internal(const AVLTreeNode<K, V> *root_node, vector<V> &vec) const
{
	if (!root_node)
	{
		return;
	}

	// In-order traversal
	values_internal(root_node->left, vec);
	vec.push_back(root_node->value);
	values_internal(root_node->right, vec);
}

template <class K, class V>
void AVLTree<K, V>::keys_internal(const AVLTreeNode<K, V> *root_node, vector<K> &vec) const
{
	if (!root_node)
	{
		return;
	}

	// In-order traversal
	keys_internal(root_node->left, vec);
	vec.push_back(root_node->key);
	keys_internal(root_node->right, vec);
}

template <class K, class V>
int AVLTree<K, V>::node_height(const AVLTreeNode<K, V> *node) const
{
	return (node ? node->height : -1);
}

template <class K, class V>
void AVLTree<K, V>::delete_all_nodes(AVLTreeNode<K, V> *node)
{
	if (node == nullptr)
	{
		return;
	}

	delete_all_nodes(node->left);
	delete_all_nodes(node->right);
	delete node;
	node = nullptr;
}

template <class K, class V>
AVLTreeNode<K, V> *AVLTree<K, V>::deep_copy_node(const AVLTreeNode<K, V> *root_node) const
{
	if (!root_node)
	{
		return nullptr;
	}

	AVLTreeNode<K, V> *new_node = new AVLTreeNode<K, V>(root_node->key, root_node->value);

	new_node->left = deep_copy_node(root_node->left);
	new_node->right = deep_copy_node(root_node->right);

	if (new_node->left)
	{
		new_node->left->parent = new_node;
	}
	if (new_node->right)
	{
		new_node->right->parent = new_node;
	}

	new_node->height = node_height(root_node);

	return new_node;
}

template <class K, class V>
AVLTreeNode<K, V> *AVLTree<K, V>::get_successor_node(AVLTreeNode<K, V> *node) const
{
	if (!node)
	{
		return nullptr;
	}

	if (node->right)
	{
		node = node->right;
		for (; node->left; node = node->left)
		{
			// This loop traverses to the minimum of the right subtree
		}

		return node;
	}
	else
	{
		for (; node->parent; node = node->parent)
		{
			AVLTreeNode<K, V> *parent = node->parent;

			if (node == parent->left)
			{
				return parent;
			}
		}

		return nullptr;
	}
}

template <class K, class V>
bool AVLTree<K, V>::is_balanced(const AVLTreeNode<K, V> *node) const
{
	if (!node)
	{
		return true;
	}

	int left_height = node_height(node->left), right_height = node_height(node->right);

	int diff = left_height - right_height;
	return (diff >= -1 && diff <= 1);
}

template <class K, class V>
void AVLTree<K, V>::calculate_all_heights(AVLTreeNode<K, V> *root)
{
	if (!root)
	{
		return;
	}

	AVLTreeNode<K, V> *left = root->left, *right = root->right;

	unsigned left_height = 0, right_height = 0;

	if (left)
	{
		calculate_all_heights(left);
		left_height = node_height(left) + 1;
	}
	if (right)
	{
		calculate_all_heights(right);
		right_height = node_height(right) + 1;
	}

	root->height = max(left_height, right_height);
}

template <class K, class V>
void AVLTree<K, V>::left_rotate(AVLTreeNode<K, V> *A)
{
	if (!A)
	{
		return;
	}

	AVLTreeNode<K, V> *C = A->right;
	AVLTreeNode<K, V> *T2 = C->left;
	AVLTreeNode<K, V> *A_parent = A->parent;

	if (A_parent)
	{
		if (A_parent->left == A)
		{
			A_parent->left = C;
		}
		else
		{
			A_parent->right = C;
		}
	}

	C->parent = A_parent;

	A->right = T2;
	if (T2)
	{
		T2->parent = A;
	}

	C->left = A;
	A->parent = C;

	if (A == root)
	{
		root = C;
	}
}

template <class K, class V>
void AVLTree<K, V>::right_rotate(AVLTreeNode<K, V> *A)
{
	if (!A)
	{
		return;
	}

	AVLTreeNode<K, V> *C = A->left;
	AVLTreeNode<K, V> *T2 = C->right;
	AVLTreeNode<K, V> *A_parent = A->parent;

	if (A_parent)
	{
		if (A_parent->left == A)
		{
			A_parent->left = C;
		}
		else
		{
			A_parent->right = C;
		}
	}

	C->parent = A_parent;

	A->left = T2;
	if (T2)
	{
		T2->parent = A;
	}

	C->right = A;
	A->parent = C;

	if (A == root)
	{
		root = C;
	}
}

template <class K, class V>
void AVLTree<K, V>::rebalance_on_insert(AVLTreeNode<K, V> *node)
{
	if (!node)
	{
		return;
	}

	AVLTreeNode<K, V> *n1 = node;
	AVLTreeNode<K, V> *n2 = n1->parent;
	if (!n2)
	{
		return;
	}

	AVLTreeNode<K, V> *n3 = n2->parent;
	if (!n3)
	{
		return;
	}

	while (n3->parent && is_balanced(n3))
	{
		n3 = n3->parent;
		n2 = n2->parent;
		n1 = n1->parent;
	}
	if (is_balanced(n3))
	{
		return;
	}

	// CASE 1
	// Right-right
	if (n2 == n3->right && n1 == n2->right)
	{
		left_rotate(n3);
	}
	// Left-left
	else if (n2 == n3->left && n1 == n2->left)
	{
		right_rotate(n3);
	}

	// CASE 2
	// Right-left
	else if (n2 == n3->right && n1 == n2->left)
	{
		right_rotate(n2);
		left_rotate(n3);
	}
	// Left-right
	else if (n2 == n3->left && n1 == n2->right)
	{
		left_rotate(n2);
		right_rotate(n3);
	}

	calculate_all_heights(root);
}

template <class K, class V>
void AVLTree<K, V>::rebalance_on_delete(AVLTreeNode<K, V> *node)
{
	if (!node)
	{
		return;
	}

	AVLTreeNode<K, V> *node_parent = node->parent;

	if (!is_balanced(node))
	{
		const AVLTreeNode<K, V> *left = node->left, *right = node->right;
		int left_height = node_height(left), right_height = node_height(right);

		bool is_right_unbalanced = right_height > left_height;

		int h = is_right_unbalanced ? left_height : right_height;

		// Right-right grandchild
		if (is_right_unbalanced)
		{
			AVLTreeNode<K, V> *child = node->right;
			if (!child)
			{
				return;
			}

			AVLTreeNode<K, V> *grand_child = child->right;
			int grand_child_height = node_height(grand_child);

			// CASE 1
			if (grand_child_height == h + 1)
			{
				left_rotate(node);
			}
			// CASE 2
			else if (grand_child_height == h)
			{
				right_rotate(child);
				left_rotate(node);
			}
		}
		// Left-left grandchild
		else
		{
			AVLTreeNode<K, V> *child = node->left;
			if (!child)
			{
				return;
			}

			AVLTreeNode<K, V> *grand_child = child->left;
			int grand_child_height = node_height(grand_child);

			// CASE 1
			if (grand_child_height == h + 1)
			{
				right_rotate(node);
			}
			// CASE 2
			else if (grand_child_height == h)
			{
				left_rotate(child);
				right_rotate(node);
			}
		}

		calculate_all_heights(root);
	}

	// Check for cascading rotations
	rebalance_on_delete(node_parent);
}

/**************** PUBLIC METHODS ****************/

template <class K, class V>
AVLTree<K, V>::AVLTree()
{
	root = nullptr;
	m_size = 0;
}

template <class K, class V>
AVLTree<K, V>::AVLTree(const AVLTree<K, V> &rhs)
{
	if (this != &rhs)
	{
		root = deep_copy_node(rhs.root);
		m_size = rhs.m_size;
	}
}

template <class K, class V>
AVLTree<K, V> &AVLTree<K, V>::operator=(const AVLTree<K, V> &rhs)
{
	if (this != &rhs)
	{
		root = deep_copy_node(rhs.root);
		m_size = rhs.m_size;
	}

	return *this;
}

template <class K, class V>
AVLTree<K, V>::~AVLTree()
{
	delete_all_nodes(root);
}

template <class K, class V>
bool AVLTree<K, V>::insert(const K key, const V value)
{
	try
	{
		// Key exists => exit
		search(key);
		return false;
	}
	catch (const std::exception &e)
	{
		// Key doesn't exist => insert it
		AVLTreeNode<K, V> *node_to_insert = new AVLTreeNode<K, V>(key, value);
		root = insert_internal(root, node_to_insert);
		calculate_all_heights(root);
		m_size++;

		rebalance_on_insert(node_to_insert);

		return true;
	}
}

template <class K, class V>
bool AVLTree<K, V>::remove(const K key)
{
	AVLTreeNode<K, V> *node_to_remove = get_node_with_key(key);

	if (!node_to_remove)
	{
		return false;
	}

	AVLTreeNode<K, V> *node_parent = node_to_remove->parent;
	remove_internal(node_to_remove);
	calculate_all_heights(root);
	m_size--;

	rebalance_on_delete(node_parent);

	return true;
}

template <class K, class V>
V AVLTree<K, V>::search(K key)
{
	AVLTreeNode<K, V> *curr = root;
	while (curr)
	{
		K currKey = curr->key;

		if (key == currKey)
		{
			break;
		}
		else if (key < currKey)
		{
			curr = curr->left;
		}
		else
		{
			curr = curr->right;
		}
	}

	// Not found
	if (!curr || curr->key != key)
	{
		throw std::runtime_error("search(key) did not find the specified key");
	}
	else
	{
		return curr->value;
	}
}

template <class K, class V>
vector<V> AVLTree<K, V>::values() const
{
	vector<V> values;
	values_internal(root, values);
	return values;
}

template <class K, class V>
vector<K> AVLTree<K, V>::keys() const
{
	vector<K> keys;
	keys_internal(root, keys);
	return keys;
}

template <class K, class V>
unsigned AVLTree<K, V>::size() const
{
	return m_size;
}
