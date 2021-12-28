#include <iostream>
#include <mutex>

using namespace std;
std::mutex mtx;

struct State {
	int id;
	int size;
	int index;
	struct State *next;
};

void display(struct State* node) {
	if (node == NULL)
		return;
	cout << "[" << node->id << "]" << "[" << node->size << "]" << "[" << node->index << "]";
}

class HeapManager {
	struct State* head;
public:
	HeapManager() {
		head = NULL;
	}
	int initMemory(int size) {
		head = (struct State*) malloc(sizeof(struct State));
		head->id = -1;
		head->size = size;
		head->index = 0;
		head->next = NULL;
		return 1;
	}
	int myMalloc(int id, int size) {
		mtx.lock();
		struct State* ptr = head;
		while (ptr != NULL) {
			if (ptr->id == -1 && ptr->size >= size) {
				break;
			}
			ptr = ptr->next;
		}
		if (ptr == NULL) {
			cout << "Can not allocate, requested size " << size << " for thread " << id << " is bigger than remaining size" << endl;
			mtx.unlock();
			return -1;
		}
		int temp_size = ptr->size;
		int temp_index = ptr->index;
		struct State* temp_next = NULL;
		if (ptr->next)
			temp_next = ptr->next;
		ptr->id = id;
		ptr->size = size;
		ptr->index = temp_index;
		if (temp_size != size) {
			struct State* new_node = (struct State*) malloc(sizeof(struct State));
			new_node->id = -1;
			new_node->size = temp_size - size;
			new_node->index = temp_index + size;
			new_node->next = temp_next;
			ptr->next = new_node;
		} else {
			ptr->next = temp_next;
		}
		cout << "Allocated for thread " << id << endl;
		mtx.unlock();
		print();
		return temp_index;
	}
	int myFree(int id, int index) {
		mtx.lock();
		struct State* ptr = head;
		while (ptr != NULL) {
			if (ptr->id == id && ptr->index == index) {
				ptr->id = -1;
				break;
			}
			ptr = ptr->next;
		}
		if (ptr == NULL) {
			mtx.unlock();
			return -1;
		}
		ptr = head;
		while (ptr != NULL) {
			if (ptr->id == -1 && ptr->next != NULL && ptr->next->id == -1) {
				ptr->size += ptr->next->size;
				ptr->next = ptr->next->next;
				continue;
			}
			ptr = ptr->next;
		}
		cout << "Freed for thread " << id << endl;
		print();
		mtx.unlock();
		return 1;
	}
	void print() {
		struct State* ptr;
		ptr = head;
		if (ptr == NULL)
			return;
		display(ptr);
		ptr = ptr->next;
		while (ptr != NULL) {
			cout << "---";
			display(ptr);
			ptr = ptr->next;
		}
		cout << endl;
	}
};
