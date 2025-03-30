#pragma once

template <typename stack_type>
class TStack {
private:
	stack_type* stack_body;  // ���������� �����
	size_t body_size;        // ������������ ���������� ���������, ������� ����� ��������� � ������� ���������� ��� ���� ������
	size_t stack_size;       // ���������� ��������� � �����

public:
	TStack() {
		this->stack_body = new stack_type[1];
		this->stack_size = 0;
		this->body_size = 1;
	};

	// �������� ����� � ���������� ������� ��� n-���������
	TStack(size_t n) {
		this->stack_body = new stack_type[n];
		this->stack_size = 0;
		this->body_size = n;
	};

	~TStack() {
		delete this->stack_body;
	}

	// ��������� ���������� ������ ��� �����
	void resize(size_t n) {
		if (this->stack_size > n) { // ��� ������� ������� ���������� ������� ����������� ����������
			throw "Resize error: an attempt to reduce the size";
		}

		stack_type* temp_body = new stack_type[n];
		for (size_t i = 0; i < this->stack_size; ++i) {
			temp_body[i] = stack_body[i];
		}
		delete stack_body;       // �������� ������� �������, ������������ � ���� �����
		stack_body = temp_body;  // ������������ � ������, �� ������� ��������� ��������� ���������
		this->body_size = n;
	};

	// �������� �������� �������� �����
	stack_type pop() {
		if (this->stack_size == 0) throw "Size error: an attempt to delete an item for an empty stack";
		this->stack_size--;
		return this->stack_body[this->stack_size];
	}

	// ���������� �������� ������ �����
	void push(stack_type& new_el) {
		if (this->body_size < this->stack_size+1) {
			this->resize(this->stack_size * 2);
		}
		this->stack_body[stack_size++] = new_el;
	}

	// �������� �������� �������� �����
	stack_type top() const {
		if (this->stack_size == 0) throw "Size error: an attempt to get an item for an empty stack";
		return this->stack_body[this->stack_size-1];  // ����������� �������� �������� �����
	}

	// �������� �� �������
	bool empty() const  {
		return not((bool)this->stack_size);  // ���� ������ ����� �� ����, ������ �� �� ������
	}

	// ������ �������� ����� (���-�� ���������)
	size_t size() const  {
		return this->stack_size;
	}

	// �������� �� ���������� ��������
	bool contain(stack_type& el) const {
		for (size_t i = 0; i < this->stack_size; ++i) {
			if (el == this->stack_body[i]) return true;
		}
		return false;
	}
};