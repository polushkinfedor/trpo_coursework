#pragma once

template <typename stack_type>
class TStack {
private:
	stack_type* stack_body;  // Stack Contents
	size_t body_size;        // The maximum number of elements that can be in the current memory allocated for the stack
	size_t stack_size;       // Number of items in the stack

public:
	TStack() {
		this->stack_body = new stack_type[1];
		this->stack_size = 0;
		this->body_size = 1;
	};

	// Creating a stack with allocated memory for n-elements
	TStack(size_t n) {
		this->stack_body = new stack_type[n];
		this->stack_size = 0;
		this->body_size = n;
	};

	~TStack() {
		delete this->stack_body;
	}

	// Changing the allocated memory for the stack
	void resize(size_t n) {
		if (this->stack_size > n) { // When trying to make a size reduction, we throw an exception
			throw "Resize error: an attempt to reduce the size";
		}

		stack_type* temp_body = new stack_type[n];
		for (size_t i = 0; i < this->stack_size; ++i) {
			temp_body[i] = stack_body[i];
		}
		delete stack_body;       // Deleting the old array bound to the stack body
		stack_body = temp_body;  // Reconnecting to the memory that the temporary pointer points to
		this->body_size = n;
	};

	// Removing the top element of the stack
	stack_type pop() {
		if (this->stack_size == 0) throw "Size error: an attempt to delete an item for an empty stack";
		this->stack_size--;
		return this->stack_body[this->stack_size];
	}

	// Adding an item to the top of the stack
	void push(stack_type& new_el) {
		if (this->body_size < this->stack_size+1) {
			this->resize(this->stack_size * 2);
		}
		this->stack_body[stack_size++] = new_el;
	}

	// Viewing the top element of the stack
	stack_type top() const {
		if (this->stack_size == 0) throw "Size error: an attempt to get an item for an empty stack";
		return this->stack_body[this->stack_size-1];  // Return of the top element of the stack
	}

	// Checking for emptiness
	bool empty() const  {
		return not((bool)this->stack_size);  // If the stack size is not zero, then it is not empty
	}

	// Current stack size (number of items)
	size_t size() const  {
		return this->stack_size;
	}

	// Checking for the content of an element
	bool contain(stack_type& el) const {
		for (size_t i = 0; i < this->stack_size; ++i) {
			if (el == this->stack_body[i]) return true;
		}
		return false;
	}
};