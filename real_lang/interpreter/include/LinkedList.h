/**
 * @file LinkedList.h
 * @author Bradley Marshall (bradley499)
 * @version 0.1
 * @date 2020-11-09
 * 
 * @copyright Copyright (c) 2020 - Bradley Marshall
 * 
 */

#ifndef LinkedList_hpp
#define LinkedList_hpp

template <class T>
class ListNode {
public:
	T element;
	ListNode* next;
	ListNode* prev;

	/**
	 * @brief Construct a new List Node object
	 * 
	 * @param element the current element
	 * @param prev the previous element
	 * @param next the next element
	 */
	ListNode(T element, ListNode* prev, ListNode* next) : element(element)
	{
	  this->next = next;
	  this->prev = prev;
	};
};

template <class T>
class LinkedList
{
private:
	int length;
	ListNode<T>* head;
	ListNode<T>* tail;
	ListNode<T>* curr;
public:
    /**
     * @brief Construct a new Linked List object
     * 
     */
	LinkedList();

    /**
     * @brief Construct a new Linked List object from another list
     * 
     */
	LinkedList(const LinkedList<T>&);

    /**
     * @brief Destroy the Linked List object
     * 
     */
	~LinkedList();

    /**
     * @brief Get the current value from the list
     * 
     * @return the current value
     */
	T& getCurrent();

    /**
     * @brief Get the first value from the list
     * 
     * @return the first value
     */
	T& First() const;

    /**
     * @brief Get the last value from the list
     * 
     * @return the last value
     */
	T& Last() const;

    /**
     * @brief Get the the length of the list
     * 
     * @return length of the list
     */
	unsigned int getLength();

    /**
     * @brief Add object to the list
     * 
     */
	void Append(T);

    /**
     * @brief Delete the last object from the list
     * 
     */
	void DeleteLast();

    /**
     * @brief Delete the first object from the list
     * 
     */
	void DeleteFirst();

    /**
     * @brief Delete the current object from the list
     * 
     */
	void DeleteCurrent();

    /**
     * @brief Moves to the next object in the list
     * 
     * @return if there is another object in the list `true` is returned otherwise `false` 
     */
	bool next();

    /**
     * @brief Moves the pointer of the list to the beginning
     * 
     * @return if there is another object in the list `true` is returned otherwise `false`   
     */
	bool moveToStart();

    /**
     * @brief Moves to the previous object in the list
     * 
     * @return if there is another object in the list `true` is returned otherwise `false`  
     */
	bool prev();

    /**
     * @brief Get the value of the object at the defined position
     * 
     * @param position the position of the object in the list
     * @return the value of the object in the list
     */
	T& At(unsigned int position);

    /**
     * @brief Deletes all objects in the list
     * 
     */
	void Clear();
	LinkedList& operator = (const LinkedList<T>&);
};

template <class T>
LinkedList<T>::LinkedList() {
	length = 0;
	head = nullptr;
	tail = nullptr;
	curr = nullptr;
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T> & list) {
	length = 0;
	head = nullptr;
	tail = nullptr;
	curr = nullptr;

	ListNode<T> * temp = list.head;

	while(temp != nullptr)
	{
		Append(temp->element);
		temp = temp->next;
	}
}

template <class T>
LinkedList<T> & LinkedList<T>::operator=(const LinkedList<T> & list)
{
	Clear();

	ListNode<T> * temp = list.head;

	while(temp != nullptr)
	{
		Append(temp->element);
		temp = temp->next;
	}

	return *this;
}

template <class T>
LinkedList<T>::~LinkedList() {
	Clear();
}

template<class T>
T& LinkedList<T>::getCurrent()
{
  return curr->element;
}

template<class T>
T& LinkedList<T>::First() const
{
  return head->element;
}

template<class T>
T& LinkedList<T>::Last() const
{
  return tail->element;
}

template<class T>
unsigned int LinkedList<T>::getLength()
{
  return length;
}

template <class T>
void LinkedList<T>::Append(T element)
{
	ListNode<T> * node = new ListNode<T>(element, tail, nullptr);
	if(length == 0)
		curr = tail = head = node;
	else {
		tail->next = node;
		tail = node;
	}

	length++;

}

template <class T>
void LinkedList<T>::DeleteLast()
{
	if(length == 0)
	  return;
	curr = tail;
	DeleteCurrent();
}

template <class T>
void LinkedList<T>::DeleteFirst()
{
	if(length == 0)
	  return;
	curr = head;
	DeleteCurrent();
}

template <class T>
bool LinkedList<T>::next()
{
	if(length == 0)
		return false;

	if(curr->next == nullptr)
		return false;

	curr = curr->next;
	return true;
}

template <class T>
bool LinkedList<T>::moveToStart()
{
	curr = head;
	return length != 0;
}

template<class T>
bool LinkedList<T>::prev()
{
	if(length == 0)
		return false;

	if(curr->prev != nullptr)
		return false;

	curr = curr->prev;
	return true;
}

template <class T>
void LinkedList<T>::DeleteCurrent()
{
	if(length == 0)
		return;
	length--;
	ListNode<T> * temp = curr;

	if(temp->prev != nullptr)
		temp->prev->next = temp->next;
	if(temp->next != nullptr)
		temp->next->prev = temp->prev;

	if(length == 0)
		head = curr = tail = nullptr;
	else if(curr == head)
		curr = head = head->next;
	else if(curr == tail)
		curr = tail = tail->prev;
	else
		curr = curr->prev;

	 delete temp;
}

template <class T>
T& LinkedList<T>::At(unsigned int position)
{
	moveToStart();
	if (position > 0)
	{
		do {
			position--;
		} while (next() && position > 0);
	}
	return curr->element;
}

template <class T>
void LinkedList<T>::Clear()
{
	if(length == 0)
		return;
	ListNode<T> * temp = head;

	while(temp != nullptr)
	{
		head = head->next;
		delete temp;
		temp = head;
	}

	head = curr = tail = nullptr;

	length = 0;

}

#endif