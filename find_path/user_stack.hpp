#pragma once

template <typename dyn_array_type>
class TDynamic_array {
public:
	dyn_array_type* body;  // Dynamic array contents
	size_t body_size;      // The maximum number of elements that can be in the current memory allocated for the dynamic array
	size_t body_len;       // Number of items in the dynamic array

	// Creating a dynamic array with allocated memory for 1 element
	TDynamic_array() { 
		this->body = new dyn_array_type[1];
		this->body_len = 0;
		this->body_size = 1;
	};

	// Creating a dynamic array with allocated memory for n-elements
	TDynamic_array(size_t n) {
		this->body = new dyn_array_type[n];
		this->body_len = 0;
		this->body_size = n;
	};

	~TDynamic_array() {
		delete this->body;
	}

	// Changing the allocated memory for the stack
	void resize(size_t n) {
		if (this->body_len > n) { // When trying to make a size reduction, we throw an exception
			throw "Resize error: an attempt to reduce the size";
		}

		dyn_array_type* temp_body = new dyn_array_type[n];
		for (size_t i = 0; i < this->body_len; ++i) {
			temp_body[i] = body[i];
		}
		delete body;       // Deleting the old array bound to the stack body
		body = temp_body;  // Reconnecting to the memory that the temporary pointer points to
		this->body_size = n;
	};

	virtual bool empty() const = 0;
};

template <typename stack_type>
class TStack : TDynamic_array<stack_type> {
public:
	TStack() {
		this->body = new stack_type[1];
		this->body_len = 0;
		this->body_size = 1;
	};

	// Creating a stack with allocated memory for n-elements
	TStack(size_t n) {
		this->body = new stack_type[n];
		this->body_len = 0;
		this->body_size = n;
	};

	// Removing the top element of the stack
	stack_type pop() {
		if (this->body_len == 0) throw "Size error: an attempt to delete an item for an empty stack";
		this->body_len--;
		return this->body[this->body_len];
	}

	// Adding an item to the top of the stack
	void push(stack_type& new_el) {
		if (this->body_size < this->body_len+1) {
			this->resize(this->body_len * 2);
		}
		this->body[this->body_len++] = new_el;
	}

	// Viewing the top element of the stack
	stack_type top() const {
		if (this->body_len == 0) throw "Size error: an attempt to get an item for an empty stack";
		return this->body[this->body_len-1];  // Return of the top element of the stack
	}

	// Checking for emptiness
	bool empty() const override {
		return not((bool)this->body_len);  // If the stack size is not zero, then it is not empty
	}

	// Current stack size (number of items)
	size_t size() const {
		return this->body_len;
	}

	// Checking for the content of an element
	bool contain(stack_type& el) const {
		for (size_t i = 0; i < this->body_len; ++i) {
			if (el == this->body[i]) return true;
		}
		return false;
	}
};